#include "../lib/MQTT.h"
#include "../lib/WeatherShield.h"
#include "application.h"

//SYSTEM_MODE(SEMI_AUTOMATIC);

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

Weather sensor;
unsigned long start, end;
bool enabled;

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
        RGB.color(0, 255, 0);
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
    RGB.control(true);
    
    //WiFi.on();
    //WiFi.connect();
	
    connect();

    sensor.begin();
    sensor.setModeBarometer();
    sensor.setOversampleRate(7);
    sensor.enableEventFlags();

    start = -0x80000000;

    RGB.color(0, 255, 0);
    client.publish("outTopic/message", "01");
    enabled=true;
}

void loop() {
    if (client.isConnected()) {
        digitalWrite(D7, LOW);
        client.loop();
	
	end = millis();
	if(end - start > 60000 && enabled) {
		start = millis();		

		char humid_buff[64];
		snprintf(humid_buff, sizeof humid_buff, "%f", sensor.getRH());
		client.publish("outTopic/humidity", humid_buff);

		char tempf_buff[64];
		snprintf(tempf_buff, sizeof tempf_buff, "%f", sensor.getTempF());
		client.publish("outTopic/tempf", tempf_buff);
	//fix buffer overflow after 1 month
	} else if(end - start < 0) {
		start=millis();
	}
    } else {
	connect();
    }
}
