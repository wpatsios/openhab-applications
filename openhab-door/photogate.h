#ifndef PHOTOGATE_H
#define PHOTOGATE_H

#include "applications/lib/RGBColor.h"

class Photogate {
public:
	Photogate(unsigned int dc_laser_port, unsigned int ac_photores_port);
	
	void enable();
	void disable();
	void toggle();
	bool isEnabled();

	void callibrate();
	bool isBroken();
	int getPhotoAnalog();
		
private:
	unsigned int laser;
	unsigned int photores;
	
	bool status_on = false;

	int normal_lighting = 100;
}; 
#endif
