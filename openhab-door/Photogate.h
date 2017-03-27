
#ifndef PHOTOGATE_H
#define PHOTOGATE_H

#define RGB_RED 255,0,0
#define RGB_GREEN 0,255,0
#define RGB_BLUE 0,0,255
#define RGB_CYAN 255,255,0
#define RGB_MAGENTA 255,0,255
#define RGB_YELLOW 0,255,255
#define RGB_BLACK 0,0,0

class Photogate {
public:
	Photogate(unsigned int dc_laser_port, unsigned int ac_photores_port);
	
	void enable();
	void disable();
	void toggle();

	void callibrate();
	bool isBroken();
		
private:
	unsigned int laser;
	unsigned int photores;
	
	bool status_on = false;

	int normal_lighting;
}; 
#endif
