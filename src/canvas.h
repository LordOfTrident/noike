#ifndef WPEC_H_HEADER_GUARD
#define WPEC_H_HEADER_GUARD

#include <stdbool.h> /* bool, true, false */
#include <stdlib.h>  /* size_t */
#include <string.h>  /* memset */
#include <stdint.h>  /* uint32_t */

#include <SDL2/SDL.h>

#include "utils/common.h"
#include "utils/math.h"

#define CANVAS_PIXEL 0xFFFFFF

typedef struct {
	SDL_Texture  *tex;
	SDL_Renderer *ren;

	uint32_t *buf;
	Vec2      size;
} Canvas;

void Canvas_setup  (Canvas *this, Vec2 size, SDL_Renderer *ren);
void Canvas_cleanup(Canvas *this);
void Canvas_render (Canvas *this, Rect rect);

uint32_t *Canvas_getPixel(Canvas *this, Vec2 pos);

void Canvas_flipPixel  (Canvas *this, Vec2 pos);
void Canvas_flipRect   (Canvas *this, Rect rect);
void Canvas_flipOutline(Canvas *this, Rect rect);

#endif
