#include "applications/lib/MQTT.h"
#include "application.h"

void callback(char* topic, byte* payload, unsigned int length);

/**
 * if want to use IP address,
 * byte server[] = { XXX,XXX,XXX,XXX };
 * MQTT client(server, 1883, callback);
 * want to use domain name,
 * MQTT client(www.sample.com, 1883, callback);
 **/
byte server[] = { 192,168,1,125 };
MQTT client(server, 1883, callback);

bool enabled;

int laser = D6;

// recieve message
void callback(char* topic, byte* payload, unsigned int length) {
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = '\0';
    String message(p);

    if (message.equals("RED")) { 
        RGB.color(255, 0, 0);
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
    pinMode(D7, OUTPUT);
    pinMode(laser, OUTPUT);	

    RGB.control(true);
    	
    connect();

    RGB.color(0, 255, 0);
    client.publish("outTopic/message", "01");
    enabled=true;
}

void loop() {
    if (client.isConnected()) {
        digitalWrite(D7, LOW);
        client.loop();
	
	if(enabled) {
	    digitalWrite(laser, HIGH);
		
	    int analog = analogRead(0);
	    char analog0_buff[64];
	    snprintf(analog0_buff, sizeof analog0_buff, "%i", analog);

	    client.publish("outTopic/analog0", analog0_buff);
	    if(analog > 1400) {
		RGB.color(255,255,0);
	    } else {
	    	RGB.color(0,255,0);
	    }
	} else { 
	    digitalWrite(laser, LOW);
	}
    } else {
	connect();
    }
}
