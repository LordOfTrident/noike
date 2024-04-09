#include "scene.h"
#include "shared.h"
#include "public.h"
#include "../core.h"

#include <stdint.h> /* uint8_t */

#include "../config.h"

#define timer(NAME) (state.base.timers + Timer##NAME)

enum {
	TimersCount = 0,
};

static struct {
	SceneState base;

	bool  takeInput;
	float darken, scale;
} state;

static void enableInput(void) {
	state.takeInput = true;
}

static void setup(void) {
	state.darken = 127;
	tweenFloat(&state, &state.scale, 1, 320, easeOutExpo, enableInput);
	tweenFloatFn(&state, setMusicVolume, 0, 1, 320, NULL, NULL);

	playMusic(music(Menu));
}

static void render(void) {
	Rect viewport = Rect_new(CANVAS_X, CANVAS_Y, CANVAS_W, CANVAS_H);
	setViewport(viewport);

	SDL_SetRenderDrawColor(core.ren, 0, 0, 0, state.darken);
	SDL_RenderFillRect(core.ren, NULL);

	setViewport(Rect_lerpSize(viewport, state.scale));
	SDL_RenderSetScale(core.ren, state.scale, state.scale);

	float offset = sin((float)SDL_GetTicks() / 160) * 5;

	SpriteSheet_render(&core.sheet, SpriteIntro, 0, (Rect){
		(Vec2){viewport.size.x / 2 - sprite(Intro).size.x / 2, PADDING * 4 + offset},
		sprite(Intro).size,
	});

	SpriteSheet_render(&core.sheet, SpriteTutorial, 0, (Rect){
		(Vec2){
			viewport.size.x / 2 - sprite(Tutorial).size.x / 2,
			viewport.size.y - sprite(Tutorial).size.y - PADDING * 4 + offset,
		},
		sprite(Tutorial).size,
	});
}

static void start(void) {
	Core_popScene();
	GameScene_playMusic();
}

static void handleEvents(void) {
	if (!state.takeInput)
		return;

	switch (core.evt.type) {
	case SDL_KEYDOWN:
		switch (core.evt.key.keysym.sym) {
		case SDLK_SPACE:
			state.takeInput = false;
			tweenFloat(&state, &state.darken, 0, 320, NULL, start);
			tweenFloat(&state, &state.scale,  0, 320, easeInBack, NULL);
			tweenFloatFn(&state, setMusicVolume, 1, 0, 320, NULL, NULL);
			break;
		}
		break;
	}
}

Scene IntroScene_new(void) {
	zeroStruct(&state);
	return Scene_new((SceneState*)&state, (SceneFns){
		.setup        = setup,
		.render       = render,
		.handleEvents = handleEvents,
	}, TimersCount);
}
