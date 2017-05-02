#include "Photogate.h"
#include "application.h"

Photogate::Photogate(unsigned int dc_laser_port, unsigned int ac_photores_port) {
	laser = dc_laser_port;
	photores = ac_photores_port;
	
	pinMode(laser, OUTPUT);
	pinMode(laser, OUTPUT);			
}

void Photogate::enable() {
	status_on = true;
	RGB.color(RGB_GREEN);
	digitalWrite(laser, HIGH);
}

void Photogate::disable() {
	status_on = false;
	digitalWrite(laser, LOW);
	RGB.color(RGB_RED);
}

void Photogate::toggle() {
	if(status_on) {
		disable();
	} else {
		enable();	
	}
}

bool Photogate::isEnabled() {
	return status_on;
}

void Photogate::callibrate() {
	normal_lighting = (analogRead(photores)+99)/100*100;	
}

bool Photogate::isBroken() {
	if(analogRead(photores) > normal_lighting) {
		return false;
	}
	return true;
}

int Photogate::getPhotoAnalog() {
	return analogRead(photores);
//	return normal_lighting;
}
