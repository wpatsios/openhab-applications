#ifndef STOPWATCH_H
#define STOPWATCH_H

class Stopwatch {
public:
	Stopwatch();
	long getTimeElapsed();
	void start();
	void stop();
	void reset();
	bool isRunning();
private:
	long start_time;
	bool status;
};

#endif
