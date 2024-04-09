#include "scene.h"

Scene Scene_new(SceneState *state, SceneFns fns, size_t minTimersCount) {
	Scene this = {0};
	this.state = state;
	this.fns   = fns;
	this.state->timersCap    = minTimersCount + 8;
	this.state->particlesCap = 64;
	this.state->tweenersCap  = 8;
	this.state->timersPoolSlotsStart = minTimersCount;
	return this;
}

void Scene_setup(Scene *this) {
	assert(this->state != NULL);

	this->state->timers    = new(Timer,    this->state->timersCap);
	this->state->particles = new(Particle, this->state->particlesCap);
	this->state->tweeners  = new(Tweener,  this->state->tweenersCap);

	zeroArray(this->state->timers,    this->state->timersCap);
	zeroArray(this->state->particles, this->state->particlesCap);
	zeroArray(this->state->tweeners,  this->state->tweenersCap);

	this->state->enabled = true;
	this->state->isSetup = true;
	Scene_callFn(this, setup);
}

void Scene_cleanup(Scene *this) {
	assert(this->state != NULL);

	this->state->isSetup = false;
	Scene_callFn(this, cleanup);

	if (this->state->timers    != NULL) xfree(this->state->timers);
	if (this->state->particles != NULL) xfree(this->state->particles);
	if (this->state->tweeners  != NULL) xfree(this->state->tweeners);
}

void Scene_update(Scene *this, double dt) {
	if (!this->state->enabled)
		return;

	for (size_t i = 0; i < this->state->timersCap; ++ i)
		Timer_update(this->state->timers + i, dt);

	for (size_t i = 0; i < this->state->particlesCap; ++ i)
		Particle_update(this->state->particles + i, dt);

	for (size_t i = 0; i < this->state->tweenersCap; ++ i)
		Tweener_update(this->state->tweeners + i, dt);

	/* A timer/tweener callback could have disabled the scene, so we need another check for that */
	if (this->state->enabled && this->fns.update != NULL)
		this->fns.update(dt);
}

void Scene_handleEvents(Scene *this) {
	if (this->state->enabled)
		Scene_callFn(this, handleEvents);
}

void Scene_render(Scene *this) {
	Scene_callFn(this, render);
}
