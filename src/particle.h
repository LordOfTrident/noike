#ifndef PARTICLE_H_HEADER_GUARD
#define PARTICLE_H_HEADER_GUARD

#include <stdbool.h> /* bool, true, false */
#include <math.h>    /* cos, sin */

#include <SDL2/SDL.h>

#include "utils/common.h"
#include "utils/math.h"
#include "timer.h"

typedef struct {
	Timer timer;
	Vec2  size;
	float x, y;
	float dx, dy, vel;
	int   r, g, b;
} Particle;

void Particle_setup(Particle *this, Rect rect, float vel,
                    float angle, int time, int r, int g, int b);
void Particle_render  (Particle *this, SDL_Renderer *ren);
void Particle_update  (Particle *this, double dt);
bool Particle_isActive(Particle *this);

#endif
