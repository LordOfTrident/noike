#include "scene.h"
#include "shared.h"
#include "public.h"
#include "../core.h"

#include <stdint.h> /* uint8_t */

#include "../config.h"
#include "../ui.h"

#define timer(NAME) (state.base.timers + Timer##NAME)

enum {
	TimerTear = 0,

	TimersCount,
};

static struct {
	SceneState base;

	bool  takeInput;
	float darken, scale;

	Rect scoreRect, bestScoreRect, continueRect, lostRect;
	int  statsWidth;

	int score, bestScore;
} state;

static void enableInput(void) {
	state.takeInput = true;
}

static void showContinue(void) {
	tweenVec2(&state, &state.continueRect.pos, (Vec2){
		CANVAS_W / 2 - state.continueRect.size.x / 2,
		state.continueRect.pos.y,
	}, 740, easeOutQuart, enableInput);

	tweenVec2(&state, &state.lostRect.pos, (Vec2){
		state.lostRect.pos.x,
		state.continueRect.pos.y - state.lostRect.size.y - PADDING * 3,
	}, 320, easeOutExpo, NULL);
}

static void continueDelay(void) {
	Rect spawnArea = {
		Vec2_add(state.bestScoreRect.pos, (Vec2){state.bestScoreRect.size.x, 1}),
		Vec2_mul(sprite(Digits).size, (Vec2){COUNTER_LEN, 1}),
	};
	playSound(sound(Pop));
	emitParticlesInRect(&state, spawnArea, 50, (Range){5, 16}, (Range){-0.2, 0.2},
	                    (Range){0, 360}, (Range){800, 1600}, 255, 255, 255);
	state.bestScore = GameScene_getBestScore();

	timeoutFn(&state, 480, showContinue);
}

static void showBestScore(void) {
	Rect spawnArea = {
		Vec2_add(state.scoreRect.pos, (Vec2){state.scoreRect.size.x, 1}),
		Vec2_mul(sprite(Digits).size, (Vec2){COUNTER_LEN, 1}),
	};
	playSound(sound(Pop));
	emitParticlesInRect(&state, spawnArea, 50, (Range){5, 16}, (Range){-0.2, 0.2},
	                    (Range){0, 360}, (Range){800, 1600}, 255, 255, 255);
	state.score = GameScene_getScore();

	tweenVec2(&state, &state.bestScoreRect.pos, (Vec2){
		CANVAS_W / 2 - (state.statsWidth + sprite(Digits).size.x * COUNTER_LEN) / 2,
		state.bestScoreRect.pos.y,
	}, 480, easeOutExpo, continueDelay);
}

static void showScore(void) {
	tweenVec2(&state, &state.scoreRect.pos, (Vec2){
		CANVAS_W / 2 - (state.statsWidth + sprite(Digits).size.x * COUNTER_LEN) / 2,
		state.scoreRect.pos.y,
	}, 480, easeOutExpo, showBestScore);
}

static void tear(void) {
	Canvas *canv = GameScene_getCanvas();

	/* Canvas tear effect for the You Lost scene */
	for (int x = 0; x < canv->size.x; ++ x) {
		int iters = 5 * easeInOutBounce(sin((float)SDL_GetTicks() / 320 + x * 2));
		for (int i = 0; i < iters; ++ i) {
			for (int y = canv->size.y; y --> 1;) {
				uint32_t *dest = Canvas_getPixel(canv, (Vec2){x, y});
				uint32_t *src  = Canvas_getPixel(canv, (Vec2){x, y - 1});
				assert(dest != NULL && src != NULL);
				*dest = *src;
			}

			if (rand() % 2 == 0)
				Canvas_flipPixel(canv, (Vec2){x, 0});
		}
	}

	Timer_start(timer(Tear));
}

static void setup(void) {
	state.bestScoreRect = (Rect){
		(Vec2){
			-sprite(BestScore).size.x - sprite(Digits).size.x * COUNTER_LEN,
			CANVAS_H - PADDING * 4 - PADDING * 2 - sprite(Digits).size.y,
		},
		sprite(BestScore).size,
	};

	state.scoreRect = (Rect){
		(Vec2){
			state.bestScoreRect.pos.x,
			state.bestScoreRect.pos.y - PADDING * 2 - sprite(Digits).size.y,
		},
		sprite(Score).size,
	};

	state.continueRect = (Rect){
		(Vec2){
			-sprite(Continue).size.x,
			CANVAS_H / 2 - sprite(Continue).size.y / 2 + PADDING * 3,
		},
		sprite(Continue).size,
	};

	state.lostRect = (Rect){
		Vec2_sub(Vec2_scale((Vec2){CANVAS_W, CANVAS_H}, 0.5),
		         Vec2_scale(sprite(Lost).size, 0.5)),
		sprite(Lost).size,
	};

	state.statsWidth = max(sprite(Score).size.x, sprite(BestScore).size.x);

	Timer_setup(timer(Tear), 16, tear);
	Timer_start(timer(Tear));

	tweenFloat(&state, &state.darken, 127, 800, easeOutExpo, NULL);
	tweenFloat(&state, &state.scale, 1, 800, easeOutExpo, NULL);
	tweenFloatFn(&state, setMusicVolume, 0, 1, 320, NULL, NULL);

	timeoutFn(&state, 1000, showScore);

	playMusic(music(Dead));
}

static void render(void) {
	Rect viewport = Rect_new(CANVAS_X, CANVAS_Y, CANVAS_W, CANVAS_H);
	setViewport(viewport);

	SDL_SetRenderDrawColor(core.ren, 0, 0, 0, state.darken);
	SDL_RenderFillRect(core.ren, NULL);

	setViewport(Rect_lerpSize(viewport, state.scale));
	SDL_RenderSetScale(core.ren, state.scale, state.scale);

	SpriteSheet_render(&core.sheet, SpriteLost,     0, state.lostRect);
	SpriteSheet_render(&core.sheet, SpriteContinue, 0, state.continueRect);

	SpriteSheet_render(&core.sheet, SpriteScore, 0, state.scoreRect);
	uiCounter((Rect){
		Vec2_add(state.scoreRect.pos, (Vec2){state.statsWidth, 0}),
		sprite(Digits).size,
	}, state.score, COUNTER_LEN, &core.sheet, SpriteDigits);

	SpriteSheet_render(&core.sheet, SpriteBestScore, 0, state.bestScoreRect);
	uiCounter((Rect){
		Vec2_add(state.bestScoreRect.pos, (Vec2){state.statsWidth, 0}),
		sprite(Digits).size,
	}, state.bestScore, COUNTER_LEN, &core.sheet, SpriteDigits);
}

static void restart(void) {
	Core_popScene();
	GameScene_restart();
}

static void handleEvents(void) {
	if (!state.takeInput)
		return;

	switch (core.evt.type) {
	case SDL_KEYDOWN:
		switch (core.evt.key.keysym.sym) {
		case SDLK_SPACE:
			state.takeInput = false;
			tweenFloat(&state, &state.scale, 0, 320, easeInBack, restart);
			tweenFloatFn(&state, setMusicVolume, 1, 0, 320, NULL, NULL);
			break;
		}
		break;
	}
}

Scene LostScene_new(void) {
	zeroStruct(&state);
	return Scene_new((SceneState*)&state, (SceneFns){
		.setup        = setup,
		.render       = render,
		.handleEvents = handleEvents,
	}, TimersCount);
}
