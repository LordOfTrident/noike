#ifndef TIMER_H_HEADER_GUARD
#define TIMER_H_HEADER_GUARD

#include <stdbool.h> /* bool, true, false */

#include "utils/common.h"

typedef struct {
	size_t time;
	double now;
	bool   paused;
	void (*callback)(void);
} Timer;

void  Timer_setup   (Timer *this, size_t time, void (*callback)(void));
void  Timer_start   (Timer *this);
void  Timer_update  (Timer *this, double dt);
void  Timer_stop    (Timer *this);
float Timer_progress(Timer *this);
bool  Timer_isActive(Timer *this);
void  Timer_pause   (Timer *this);
void  Timer_resume  (Timer *this);

#endif
