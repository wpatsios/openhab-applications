#include "applications/lib/MQTT.h"
#include "Photogate.h"
#include "application.h"

void callback(char* topic, byte* payload, unsigned int length);

/**
 * if want to use IP address,
 * byte server[] = { XXX,XXX,XXX,XXX };
 * MQTT client(server, 1883, callback);
 * want to use domain name,
 * MQTT client(www.sample.com, 1883, callback);
 **/
byte server[] = { 192,168,1,107 };
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
	client.publish("outTopic/message", "00");
	enabled=false;
    } else if (message.equals("GREEN")) {   
	client.publish("outTopic/message", "01");
	enabled=true;
    }
    delay(1000);
}

void connect() {
    if(!client.isConnected()) {
	digitalWrite(D7, HIGH);
	client.connect("sparkclient");
	client.subscribe("inTopic/message");
    }
}

void setup() {
    RGB.control(true);
    	
    connect();

    pg1.enable();
    pg2.enable();
	
    pg1.callibrate();
    pg2.callibrate();

    start = -0x80000000;

    client.publish("outTopic/message", "01");
    enabled=true;
}

bool pg1_prev = true;
bool pg2_prev = true;

#define PHTO_ID_1 0x01
#define PHTO_ID_2 0x02

int waitFor = 0;

void loop() {
    if (client.isConnected()) {
        digitalWrite(D7, LOW);
        client.loop();
	
	end = millis();
	if(end - start > 100  && enabled) {
	    start = millis();
	    
	    if(!pg1.isEnabled()) {
		pg1.enable();
		pg2.enable();
	    }
		
	    char analog0_buff[64];
	    snprintf(analog0_buff, sizeof analog0_buff, "%i", pg2.getPhotoAnalog());

	 
	    
	    client.publish("outTopic/d", analog0_buff);

	    if(waitFor == PHTO_ID_2 && pg2.isBroken()) {
		peops++;
		waitFor = 0;
	    } else if(waitFor == PHTO_ID_1 && pg1.isBroken()) {
		peops--;
		waitFor = 0;
	    } else if(waitFor == 0) {
		if(!pg1_prev && pg1.isBroken()) {
	       	    RGB.color(RGB_MAGENTA);
		    waitFor = PHTO_ID_2;
		} else if(!pg2_prev && pg2.isBroken()) {
		    RGB.color(RGB_YELLOW);
	     	    waitFor = PHTO_ID_1;
		} 
	    }
	    
	    char peop_buff[64];
	    snprintf(peop_buff, sizeof peop_buff, "%i", peops);
	    client.publish("outTopic/analog0", peop_buff);
	    
	    pg1_prev = pg1.isBroken();
	    pg2_prev = pg2.isBroken();
	} else if(end - start < 0) {
	    start=millis();
	} else {
	    //pg1.disable();
	    //pg2.disable();
	}
    } else {
	connect();
    }
}
