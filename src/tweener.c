#include "tweener.h"

static void tweenHandlerFloat(TweenTarget target, float *src, float *dest, float t) {
	float *value = (float*)target.value;
	*value = lerp(*src, *dest, t);
}

static void tweenHandlerVec2(TweenTarget target, Vec2 *src, Vec2 *dest, float t) {
	Vec2 *value = (Vec2*)target.value;
	value->x = lerp(src->x, dest->x, t);
	value->y = lerp(src->y, dest->y, t);
}

static void tweenHandlerFloatFn(TweenTarget target, float *src, float *dest, float t) {
	target.hook(lerp(*src, *dest, t));
}

void Tweener_setup(Tweener *this, TweenTarget target, TweenValue src, TweenValue dest,
                   int time, Easer easer, void (*callback)(void), TweenHandler handler) {
	zeroStruct(this);
	this->target  = target;
	this->src     = src;
	this->dest    = dest;
	this->easer   = easer;
	this->handler = handler;
	Timer_setup(&this->timer, time, callback);
	Timer_start(&this->timer);
}

void Tweener_setupFloat(Tweener *this, float *target, float dest, int time,
                        Easer easer, void (*callback)(void)) {
	Tweener_setup(this, (TweenTarget){.value = target}, (TweenValue){.f32 = *target},
	              (TweenValue){.f32 = dest}, time, easer, callback,
	              (TweenHandler)tweenHandlerFloat);
}

void Tweener_setupVec2(Tweener *this, Vec2 *target, Vec2  dest, int time,
                       Easer easer, void (*callback)(void)) {
	Tweener_setup(this, (TweenTarget){.value = target}, (TweenValue){.vec2 = *target},
	              (TweenValue){.vec2 = dest}, time, easer, callback,
	              (TweenHandler)tweenHandlerVec2);
}

void Tweener_setupFloatFn(Tweener *this, void (*hook)(float), float src, float dest, int time,
                          Easer easer, void (*callback)(void)) {
	Tweener_setup(this, (TweenTarget){.hook = hook}, (TweenValue){.f32 = src},
	              (TweenValue){.f32 = dest}, time, easer, callback,
	              (TweenHandler)tweenHandlerFloatFn);
}

void Tweener_update(Tweener *this, double dt) {
	if (!Tweener_isActive(this))
		return;

	Timer_update(&this->timer, dt);

	float t = Timer_progress(&this->timer);
	this->handler(this->target, &this->src, &this->dest, this->easer != NULL? this->easer(t) : t);
}

bool Tweener_isActive(Tweener *this) {
	return Timer_isActive(&this->timer);
}
