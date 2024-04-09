#include "timer.h"

void Timer_setup(Timer *this, size_t time, void (*callback)(void)) {
	zeroStruct(this);
	this->time     = time;
	this->callback = callback;
}

void Timer_start(Timer *this) {
	this->now = this->time;
}

void Timer_update(Timer *this, double dt) {
	if (this->now <= 0 || this->paused)
		return;

	this->now -= dt;
	if (this->now <= 0) {
		this->now = 0;

		if (this->callback != NULL)
			this->callback();
	}
}

void Timer_stop(Timer *this) {
	this->now = 0;
}

float Timer_progress(Timer *this) {
	return 1 - (double)this->now / this->time;
}

bool Timer_isActive(Timer *this) {
	return this->now > 0;
}

void Timer_pause(Timer *this) {
	this->paused = true;
}

void Timer_resume(Timer *this) {
	this->paused = false;
}
