#include "particle.h"

void Particle_setup(Particle *this, Rect rect, float vel,
                    float angle, int time, int r, int g, int b) {
	zeroStruct(this);
	this->x    = rect.pos.x;
	this->y    = rect.pos.y;
	this->size = rect.size;
	this->vel  = vel;
	this->dx   = cos(angle * (M_PI / 180));
	this->dy   = sin(angle * (M_PI / 180));
	this->r    = r;
	this->g    = g;
	this->b    = b;

	Timer_setup(&this->timer, time, NULL);
	Timer_start(&this->timer);
}

void Particle_render(Particle *this, SDL_Renderer *ren) {
	if (!Particle_isActive(this))
		return;

	float a = (1 - Timer_progress(&this->timer)) * 255;
	SDL_SetRenderDrawColor(ren, this->r, this->g, this->b, a);
	SDL_Rect rect = Rect_toSDL((Rect){
		Vec2_sub((Vec2){this->x, this->y}, Vec2_scale(this->size, 0.5)),
		this->size,
	});
	SDL_RenderFillRect(ren, &rect);
}

void Particle_update(Particle *this, double dt) {
	if (!Particle_isActive(this))
		return;

	float vel = this->vel * (1 - Timer_progress(&this->timer));
	this->x += this->dx * vel * dt;
	this->y += this->dy * vel * dt;

	Timer_update(&this->timer, dt);
}

bool Particle_isActive(Particle *this) {
	return Timer_isActive(&this->timer);
}
