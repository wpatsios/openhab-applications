#include "stopwatch.h"
#include "application.h"

Stopwatch::Stopwatch() {
	reset();
	status = false;
}

long Stopwatch::getTimeElapsed() {
	return millis() - start_time;
}

void Stopwatch::start() {
	status = true;
}

void Stopwatch::stop() {
	status = false;
}

void Stopwatch::reset() {
	start_time = millis();
}

bool Stopwatch::isRunning() {
	return status;
}
