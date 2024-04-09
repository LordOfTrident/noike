#ifndef SCENES_SCENE_H_HEADER_GUARD
#define SCENES_SCENE_H_HEADER_GUARD

#include <stdbool.h> /* bool, true, false */
#include <stdlib.h>  /* size_t */

#include "../utils/common.h"
#include "../timer.h"
#include "../particle.h"
#include "../tweener.h"

typedef struct {
	bool enabled, isSetup;

	Timer    *timers;
	size_t    timersCap, timersPoolSlotsStart;
	Particle *particles;
	size_t    particlesCap;
	Tweener  *tweeners;
	size_t    tweenersCap;
} SceneState;

typedef struct {
	void (*setup)       (void);
	void (*cleanup)     (void);
	void (*render)      (void);
	void (*handleEvents)(void);
	void (*update)      (double);
	void (*onEnabled)   (void);
	void (*onDisabled)  (void);
} SceneFns;

typedef struct {
	SceneState *state;
	SceneFns    fns;
} Scene;

#define Scene_callFn(SCENE, FN)      \
	do {                             \
		if ((SCENE)->fns.FN != NULL) \
			(SCENE)->fns.FN();       \
	} while (0)

Scene Scene_new(SceneState *state, SceneFns fns, size_t minTimersCount);

void Scene_setup  (Scene *this);
void Scene_cleanup(Scene *this);

void Scene_update      (Scene *this, double dt);
void Scene_handleEvents(Scene *this);
void Scene_render      (Scene *this);

#endif
