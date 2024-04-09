#ifndef CORE_H_HEADER_GUARD
#define CORE_H_HEADER_GUARD

#include <time.h>   /* time */
#include <assert.h> /* assert */

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include "scenes/scene.h"
#include "utils/common.h"
#include "sprite_sheet.h"

#define MAX_SCENES 8

typedef struct {
	SDL_Window   *win;
	SDL_Renderer *ren;
	SDL_Texture  *scr;
	SDL_Event     evt;
	Vec2 winSize;

	Scene scenes[MAX_SCENES];
	int   scenesCount;
	Scene sceneDeleteQueue[MAX_SCENES];
	int   scenesToDeleteCount;

	SpriteSheet sheet;

	Mix_Chunk **sounds;
	size_t      soundsCount;
	Mix_Music **music;
	size_t      musicCount;

	bool quit, fullscreen;
} Core;

#define sprite(NAME) core.sheet.sprites[Sprite##NAME]
#define sound(NAME)  core.sounds[Sound##NAME]
#define music(NAME)  core.music[Music##NAME]

extern Core core;

typedef struct {
	SpriteLoadInfo *sprites;
	size_t          spritesCount;

	const char **sounds;
	size_t       soundsCount;

	const char **music;
	size_t       musicCount;
} GameResources;

void Core_setup(int w, int h, const char *title, Scene initialScene, GameResources res);
void Core_cleanup(void);

void Core_render(void);
void Core_handleEvents(void);
void Core_update(double dt);

void  Core_changeScene(Scene scene);
void  Core_pushScene(Scene scene);
void  Core_popScene(void);
Scene Core_getScene(void);

#endif
