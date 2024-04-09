#ifndef TWEENER_H_HEADER_GUARD
#define TWEENER_H_HEADER_GUARD

#include <stdint.h>  /* uint8_t */
#include <stdbool.h> /* bool, true, false */
#include <assert.h>  /* assert */

#include "timer.h"
#include "utils/math.h"
#include "utils/common.h"

typedef union {
	float f32;
	Vec2  vec2;
} TweenValue;

typedef union {
	void *value;
	void (*hook)(float);
} TweenTarget;

typedef void (*TweenHandler)(TweenTarget, void*, void*, float);

typedef struct {
	TweenTarget target;
	TweenValue  src, dest;

	Timer        timer;
	Easer        easer;
	TweenHandler handler;
} Tweener;

void Tweener_setupFloat(Tweener *this, float *target, float dest, int time,
                        Easer easer, void (*callback)(void));
void Tweener_setupVec2(Tweener *this, Vec2 *target, Vec2 dest, int time,
                       Easer easer, void (*callback)(void));
void Tweener_setupFloatFn(Tweener *this, void (*hook)(float), float src, float dest, int time,
                          Easer easer, void (*callback)(void));

void Tweener_update  (Tweener *this, double dt);
bool Tweener_isActive(Tweener *this);

#endif
