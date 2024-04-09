#include "shared.h"

void emitParticlesInRect_(SceneState *this, Rect rect, size_t amount,
                          Range size, Range vel, Range angle, Range time, int r, int g, int b) {
	for (size_t i = 0; amount > 0; ++ i) {
		if (i >= this->particlesCap) {
			size_t requiredSpace = 1;
			while (amount > requiredSpace)
				requiredSpace *= 2;

			size_t prevCap      = this->particlesCap;
			this->particlesCap += requiredSpace;
			this->particles     = resize(this->particles, this->particlesCap);
			zeroArray(this->particles + prevCap, requiredSpace);
		} else if (Particle_isActive(this->particles + i))
			continue;

		-- amount;

		Particle_setup(this->particles + i, (Rect){
			(Vec2){
				Range_random(Rect_rangeX(rect), 0),
				Range_random(Rect_rangeY(rect), 0),
			},
			Vec2_int(Range_random(size, 0)),
		}, Range_random(vel, 4), Range_random(angle, 4), Range_random(time, 0), r, g, b);
	}
}

static Tweener *getFreeTweenerSlot(SceneState *this) {
	for (size_t i = 0;; ++ i) {
		if (i >= this->tweenersCap) {
			size_t prevCap     = this->tweenersCap;
			this->tweenersCap *= 2;
			this->tweeners     = resize(this->tweeners, this->tweenersCap);
			zeroArray(this->tweeners + prevCap, prevCap);
		} else if (Tweener_isActive(this->tweeners + i))
			continue;

		return this->tweeners + i;
	}
}

void tweenFloat_(SceneState *this, float *target, float dest, int time,
                 Easer easer, void (*callback)(void)) {
	Tweener_setupFloat(getFreeTweenerSlot(this), target, dest, time, easer, callback);
}

void tweenVec2_(SceneState *this, Vec2 *target, Vec2 dest, int time,
                Easer easer, void (*callback)(void)) {
	Tweener_setupVec2(getFreeTweenerSlot(this), target, dest, time, easer, callback);
}

void tweenFloatFn_(SceneState *this, void (*hook)(float), float src, float dest, int time,
                   Easer easer, void (*callback)(void)) {
	Tweener_setupFloatFn(getFreeTweenerSlot(this), hook, src, dest, time, easer, callback);
}

static Timer *getFreeTimerSlot(SceneState *this) {
	for (size_t i = this->timersPoolSlotsStart;; ++ i) {
		if (i >= this->timersCap) {
			size_t prevCap   = this->timersCap;
			this->timersCap *= 2;
			this->timers     = resize(this->timers, this->timersCap);
			zeroArray(this->timers + prevCap, prevCap);
		} else if (Timer_isActive(this->timers + i))
			continue;

		return this->timers + i;
	}
}

void timeoutFn_(SceneState *this, int time, void (*callback)(void)) {
	Timer *timer = getFreeTimerSlot(this);
	Timer_setup(timer, time, callback);
	Timer_start(timer);
}

void setViewport(Rect rect) {
	SDL_Rect viewport = Rect_toSDL(rect);
	SDL_RenderSetViewport(core.ren, &viewport);
}

void resetViewport(void) {
	SDL_RenderSetViewport(core.ren, NULL);
}

void playMusic(Mix_Music *music) {
	if (Mix_PlayMusic(music, -1) < 0)
		fatal("Failed to play music: %s", SDL_GetError());
}

void playSound(Mix_Chunk *sound) {
	if (Mix_PlayChannel(-1, sound, 0) < 0)
		fatal("Failed to play sound: %s", SDL_GetError());
}

void setMusicVolume(float volume) {
	Mix_VolumeMusic(volume * MIX_MAX_VOLUME);
}
