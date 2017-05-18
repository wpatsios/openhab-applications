#include "applications/lib/MQTT.h"
#include "photogate.h"
#include "stopwatch.h"
#include "application.h"

void callback(char* topic, byte* payload, unsigned int length);
void publish(const char* topic, int num);
/**
 * if want to use IP address,
 * byte server[] = { XXX,XXX,XXX,XXX };
 * MQTT client(server, 1883, callback);
 * want to use domain name,
 * MQTT client(www.sample.com, 1883, callback);
 **/
byte server[] = { 192,168,1,133 };
MQTT client(server, 1883, callback);

bool enabled;
int peops = 0;
unsigned long start, end;

Photogate pg1(D6, 1);
Photogate pg2(D4, 0);

// recieve message
void callback(char* topic, byte* payload, unsigned int length) {
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = '\0';
    String message(p);

    if (message.equals("RED")) { 
	pg1.disable();
	pg2.disable();
	client.publish("outTopic/doorStat", "00");
	enabled=false;
    } else if (message.equals("GREEN")) {   
	client.publish("outTopic/doorStat", "01");
	enabled=true;
    }
    delay(1000);
}

void connect() {
    if(!client.isConnected()) {
	digitalWrite(D7, HIGH);
	client.connect("sparkclient");
	client.subscribe("inTopic/doorStat");
    }
}

void setup() {
    RGB.control(true);
    	
    connect();

    //pg1.enable();
    //pg2.enable();

    start = -0x80000000;

    client.publish("outTopic/doorStat", "01");
    enabled=true;
}

bool pg1_prev = true;
bool pg2_prev = true;

#define PHTO_ID_1 1
#define PHTO_ID_2 2

int waitFor = 0;
Stopwatch timeout;

void loop() {
    if (client.isConnected()) {
        digitalWrite(D7, LOW);
        client.loop();
	
	end = millis();
	if(end - start > 100  && enabled) {
	    start = millis();
	    
	    if(!pg1.isEnabled()) {
    		pg1.callibrate();
    		pg2.callibrate();
		pg1.enable();
		pg2.enable();
	    }
		
	    //publish("outTopic/d1", pg2.isBroken());
	    //publish("outTopic/d2", pg1.isBroken());

	    if(waitFor == PHTO_ID_2 && pg2.isBroken()) {
		peops++;
		waitFor = 0;
		RGB.color(RGB_RED);
		timeout.stop();
		publish("outTopic/peops", peops);
	    } else if(waitFor == PHTO_ID_1 && pg1.isBroken()) {
		peops--;
		waitFor = 0;
		RGB.color(RGB_BLUE);
		timeout.stop();
		publish("outTopic/peops", peops);
	    } else if(waitFor == 0) {
		if(!pg1_prev && pg1.isBroken()) {
	       	    RGB.color(RGB_MAGENTA);
		    waitFor = PHTO_ID_2;
		} else if(!pg2_prev && pg2.isBroken()) {
		    RGB.color(RGB_CYAN);
	     	    waitFor = PHTO_ID_1;
		} 
	    } else if(waitFor != 0 && !timeout.isRunning()) {
		timeout.start();
		timeout.reset();
	    } else if(timeout.isRunning() && timeout.getTimeElapsed() > 500) {
		waitFor = 0;
		RGB.color(RGB_GREEN);
		timeout.stop();
	    }
	    
	    pg1_prev = pg1.isBroken();
	    pg2_prev = pg2.isBroken();
	} else if(end - start < 0) {
	    start=millis();
	}
    } else {
	connect();
    }
}

void publish(const char* topic, int num) {
    char buff[64];
    snprintf(buff, sizeof buff, "%i", num);
    client.publish(topic, buff);
}
