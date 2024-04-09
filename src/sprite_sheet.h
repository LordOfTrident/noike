#ifndef SPRITE_SHEET_H_HEADER_GUARD
#define SPRITE_SHEET_H_HEADER_GUARD

#include <string.h> /* strlen, strcat */
#include <assert.h> /* assert */

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "utils/common.h"
#include "utils/math.h"

typedef struct {
	int rows, cols;
	const char *path;
} SpriteLoadInfo;

typedef struct {
	Vec2 size;
	SDL_Texture *tex;

	int framesCount, rows, cols;
	SpriteLoadInfo info;
} Sprite;

void Sprite_setup  (Sprite *this, const char *folder, SpriteLoadInfo loadInfo, SDL_Renderer *ren);
void Sprite_cleanup(Sprite *this);

typedef struct {
	SDL_Renderer *ren;
	Sprite       *sprites;
	int           spritesCount;
} SpriteSheet;

void SpriteSheet_setup(SpriteSheet *this, const char *folder,
                       int spritesCount, SpriteLoadInfo *loadInfo, SDL_Renderer *ren);
void SpriteSheet_cleanup(SpriteSheet *this);

void SpriteSheet_render    (SpriteSheet *this, int id, int frame, Rect dest);
void SpriteSheet_renderCrop(SpriteSheet *this, int id, int frame, Rect src, Rect dest);

#endif
