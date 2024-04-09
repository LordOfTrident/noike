#include "scene.h"
#include "shared.h"
#include "public.h"
#include "../core.h"

#include <stdint.h> /* uint8_t */

#include "../config.h"

#define timer(NAME) (state.base.timers + Timer##NAME)

enum {
	TimerMuteDelay,

	TimersCount = 0,
};

static struct {
	SceneState base;

	bool  takeInput;
	float darken, scale;

	float volumeState;
} state;

static void enableInput(void) {
	state.takeInput = true;
}

static void setup(void) {
	Timer_setup(timer(MuteDelay), 220, NULL);

	tweenFloat(&state, &state.darken, 127, 80, NULL, enableInput);
	tweenFloat(&state, &state.scale,  1, 80, easeOutExpo, NULL);

	if (!GameScene_isMusicMuted())
		tweenFloatFn(&state, setMusicVolume, 1, 0, 80, NULL, Mix_PauseMusic);

	state.volumeState = GameScene_isMusicMuted();
}

static void render(void) {
	Rect viewport = Rect_new(CANVAS_X, CANVAS_Y, CANVAS_W, CANVAS_H);
	setViewport(viewport);

	SDL_SetRenderDrawColor(core.ren, 0, 0, 0, state.darken);
	SDL_RenderFillRect(core.ren, NULL);

	setViewport(Rect_lerpSize(viewport, state.scale));
	SDL_RenderSetScale(core.ren, state.scale, state.scale);

	Rect pauseRect = {
		Vec2_sub(Vec2_scale(viewport.size, 0.5), Vec2_scale(sprite(Pause).size, 0.5)),
		sprite(Pause).size,
	};
	SpriteSheet_render(&core.sheet, SpritePause, 0, pauseRect);

	SpriteSheet_render(&core.sheet, SpriteVolume, state.volumeState * 3, (Rect){
		(Vec2){

			viewport.size.x / 2 - sprite(Volume).size.x / 2,
			viewport.size.y - sprite(Volume).size.y - PADDING * 2,
		},
		sprite(Volume).size,
	});
}

static void handleEvents(void) {
	if (!state.takeInput)
		return;

	switch (core.evt.type) {
	case SDL_KEYDOWN:
		switch (core.evt.key.keysym.sym) {
		case SDLK_SPACE:
			state.takeInput = false;
			tweenFloat(&state, &state.darken, 0, 80, NULL, Core_popScene);
			tweenFloat(&state, &state.scale,  0, 80, easeInExpo, NULL);

			if (!GameScene_isMusicMuted())
				tweenFloatFn(&state, setMusicVolume, 0, 1, 80, NULL, Mix_ResumeMusic);
			break;

		case SDLK_m:
			if (Timer_isActive(timer(MuteDelay)))
				break;

			GameScene_muteMusic(!GameScene_isMusicMuted());

			playSound(GameScene_isMusicMuted()? sound(Pop3) : sound(Pop));

			tweenFloat(&state, &state.volumeState, GameScene_isMusicMuted(), 160, NULL, NULL);
			Timer_start(timer(MuteDelay));
			break;
		}
		break;
	}
}

Scene PausedScene_new(void) {
	zeroStruct(&state);
	return Scene_new((SceneState*)&state, (SceneFns){
		.setup        = setup,
		.render       = render,
		.handleEvents = handleEvents,
	}, TimersCount);
}
