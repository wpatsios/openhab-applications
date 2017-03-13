#include "MQTT.h"
#include "WeatherShield.h"
#include "application.h"

SYSTEM_MODE(SEMI_AUTOMATIC);

void callback(char* topic, byte* payload, unsigned int length);

/**
 * if want to use IP address,
 * byte server[] = { XXX,XXX,XXX,XXX };
 * MQTT client(server, 1883, callback);
 * want to use domain name,
 * MQTT client(www.sample.com, 1883, callback);
 **/
byte server[] = { 192,168,1,131 };
MQTT client(server, 1883, callback);

Weather sensor;

// recieve message
void callback(char* topic, byte* payload, unsigned int length) {
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = '\0';
    String message(p);

    if (message.equals("RED")) { 
        RGB.color(255, 0, 0);
	client.publish("outTopic/message", "00");
    } else if (message.equals("GREEN")) {   
        RGB.color(0, 255, 0);
	client.publish("outTopic/message", "01");
    } else if (message.equals("BLUE")) {
        RGB.color(0, 0, 255);
    } else {    
        RGB.color(255, 255, 255);
    }
    delay(1000);
}


void setup() {
    pinMode(D7, OUTPUT);
    RGB.control(true);
    
    WiFi.on();
    WiFi.connect();

    // connect to the server
    client.connect("sparkclient");

    // publish/subscribe
    if (client.isConnected()) {
        client.subscribe("inTopic/message");
    }

    sensor.begin();
    sensor.setModeBarometer();
    sensor.setOversampleRate(7);
    sensor.enableEventFlags();
}

void loop() {
    if (client.isConnected()) {
        digitalWrite(D7, LOW);
        client.loop();

	char humid_buff[64];
	snprintf(humid_buff, sizeof humid_buff, "%f", sensor.getRH());
	client.publish("outTopic/humidity", humid_buff);

	char tempf_buff[64];
	snprintf(tempf_buff, sizeof tempf_buff, "%f", sensor.getTempF());
	client.publish("outTopic/tempf", tempf_buff);
    } else {
        digitalWrite(D7, HIGH);
	client.connect("sparkclient");
    }
}
