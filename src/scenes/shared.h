#ifndef SCENES_SHARED_H_HEADER_GUARD
#define SCENES_SHARED_H_HEADER_GUARD

#include "scene.h"
#include "../core.h"
#include "../utils/math.h"
#include "../utils/common.h"
#include "../timer.h"
#include "../particle.h"
#include "../tweener.h"

inline void mustBeSceneState(SceneState state) {(void)state;}

void emitParticlesInRect_(SceneState *this, Rect rect, size_t amount,
                          Range size, Range vel, Range angle, Range time, int r, int g, int b);
#define emitParticlesInRect(STATE, ...) \
	(mustBeSceneState((STATE)->base),   \
	 emitParticlesInRect_((SceneState*)(STATE), __VA_ARGS__))

void tweenFloat_(SceneState *this, float *target, float dest, int time,
                 Easer easer, void (*callback)(void));
#define tweenFloat(STATE, ...)        \
	(mustBeSceneState((STATE)->base), \
	 tweenFloat_((SceneState*)(STATE), __VA_ARGS__))

void tweenVec2_(SceneState *this, Vec2 *target, Vec2 dest, int time,
                Easer easer, void (*callback)(void));
#define tweenVec2(STATE, ...)         \
	(mustBeSceneState((STATE)->base), \
	 tweenVec2_((SceneState*)(STATE), __VA_ARGS__))

void tweenFloatFn_(SceneState *this, void (*hook)(float), float src, float dest, int time,
                   Easer easer, void (*callback)(void));
#define tweenFloatFn(STATE, ...)      \
	(mustBeSceneState((STATE)->base), \
	 tweenFloatFn_((SceneState*)(STATE), __VA_ARGS__))

void timeoutFn_(SceneState *this, int time, void (*callback)(void));
#define timeoutFn(STATE, ...)         \
	(mustBeSceneState((STATE)->base), \
	 timeoutFn_((SceneState*)(STATE), __VA_ARGS__))

void setViewport(Rect rect);
void resetViewport(void);

void playMusic(Mix_Music *music);
void playSound(Mix_Chunk *sound);
void setMusicVolume(float volume);

#endif
