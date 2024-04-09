#include "scene.h"
#include "shared.h"
#include "public.h"
#include "../core.h"

#include <stdio.h>   /* FILE, fopen, freopen, fclose */
#include <stdint.h>  /* uint8_t, uintptr_t */
#include <stdlib.h>  /* rand, srand */
#include <stdbool.h> /* bool true, false */
#include <string.h>  /* strerror */
#include <errno.h>   /* errno */

#include "../snake.h"
#include "../canvas.h"
#include "../config.h"
#include "../ui.h"
#include "../particle.h"
#include "../utils/math.h"

#define timer(NAME) (state.base.timers + Timer##NAME)

enum {
	TimerAppleSpawn = 0,
	TimerAppleLifetime,
	TimerShake,
	TimerRadar,
	TimerLost,
	TimerMove,

	TimersCount,
};

static struct {
	SceneState base;

	Snake   snake;
	Canvas  canv;
	Vec2    apple;
	uint8_t hearts, radars;

	Rect uiCounterRect;

	bool musicMuted;
	int  bestScore;
} state;

static void spawnApple(void);
static void outOfTime(void);
static void radarFindApple(void);
static void youLost(void);
static void move(void);

static void setup(void) {
	Canvas_setup(&state.canv, Vec2_scale((Vec2){COLS, ROWS}, TILE_SIZE), core.ren);

	srand((uintptr_t)&state + time(NULL));
	for (int y = 0; y < state.canv.size.y; ++ y) {
		for (int x = 0; x < state.canv.size.x; ++ x) {
			if (rand() % 2)
				Canvas_flipPixel(&state.canv, (Vec2){x, y});
		}
	}

	Timer_setup(timer(AppleSpawn),    1280, spawnApple);
	Timer_setup(timer(AppleLifetime), 5920, outOfTime);
	Timer_setup(timer(Shake),         480,  NULL);
	Timer_setup(timer(Radar),         800,  radarFindApple);
	Timer_setup(timer(Lost),          640,  youLost);
	Timer_setup(timer(Move),          32,   move);

	state.musicMuted = false;
	GameScene_restart();
}

static void cleanup(void) {
	Canvas_cleanup(&state.canv);
}

static void flipTileOutline(Vec2 pos) {
	Canvas_flipOutline(&state.canv, Rect_scale(Rect_new(pos.x, pos.y, 1, 1), TILE_SIZE));
}

static void flipTileFractionOutline(Vec2 pos, Dir dir, float fraction) {
	pos = Vec2_scale(pos, TILE_SIZE);

	int  tileFraction = fraction * TILE_SIZE;
	Vec2 size         = Vec2_int(TILE_SIZE);
	switch (dir) {
	case DirUp:    pos.y += TILE_SIZE - tileFraction; /* Fall through */
	case DirDown:  size.y = tileFraction; break;
	case DirLeft:  pos.x += TILE_SIZE - tileFraction; /* Fall through */
	case DirRight: size.x = tileFraction; break;
	}

	Canvas_flipOutline(&state.canv, (Rect){pos, size});
}

static void flipSnakeOutline(void) {
	Dir dir = Vec2_dirTo(Snake_back(&state.snake), state.snake.tip);

	for (int i = 1; i < state.snake.len; ++ i) {
		if (Vec2_equals(state.snake.body[i], state.snake.body[i - 1]))
			continue;

		flipTileOutline(state.snake.body[i]);
	}

	flipTileFractionOutline(*state.snake.head, state.snake.dir, state.snake.progress);

	if (!Vec2_equals(Snake_back(&state.snake), state.snake.tip))
		flipTileFractionOutline(state.snake.tip, dir, 1 - state.snake.progress);
}

static void flipApple(void) {
	Canvas_flipRect(&state.canv, (Rect){Vec2_scale(state.apple, TILE_SIZE), Vec2_int(TILE_SIZE)});
}

static void move(void) {
	if (Timer_isActive(timer(Lost)))
		return;

	if (!Timer_isActive(timer(Radar)))
		Snake_move(&state.snake);

	flipSnakeOutline();
	Timer_start(timer(Move));
}

static int getScore(void) {
	return state.snake.len - 2;
}

static void spawnApple(void) {
	playSound(sound(Pop2));

	Vec2 availableTiles[COLS * ROWS];
	int  availableTilesCount = 0;

	Vec2 pos = {0, 0};
	for (size_t i = 0; i < arraySize(availableTiles); ++ i) {
		if (!Vec2_equals(*state.snake.head, pos) && !Vec2_equals(state.snake.tip, pos) &&
		    !Snake_bodyContains(&state.snake, pos))
			availableTiles[availableTilesCount ++] = pos;

		if ((i + 1) % COLS == 0) {
			pos.x = 0;
			++ pos.y;
		} else
			++ pos.x;
	}

	if (availableTilesCount == 0)
		unreachable(); /* TODO: You won the game! */

	state.apple = availableTiles[rand() % availableTilesCount];
	Timer_start(timer(AppleLifetime));

	flipApple();
}

static void foundApple(void) {
	Snake_grow(&state.snake);

	Mix_PlayChannel(-1, sound(Pop), 0);

	Timer_stop(timer(AppleLifetime));
	Timer_start(timer(AppleSpawn));

	Rect spawnArea = {
		state.uiCounterRect.pos,
		(Vec2){state.uiCounterRect.size.x * COUNTER_LEN, state.uiCounterRect.size.y},
	};
	emitParticlesInRect(&state, spawnArea, 50, (Range){3, 14}, (Range){-0.1, 0.1},
	                    (Range){0, 360}, (Range){800, 1600}, 255, 255, 255);
}

static void died(void) {
	tweenFloatFn(&state, setMusicVolume, 1, 0, 320, NULL, NULL);
	Timer_start(timer(Lost));
	Mix_PlayChannel(-1, sound(Boom2), 0);
}

static void outOfTime(void) {
	Timer_start(timer(Shake));

	-- state.hearts;
	if (state.hearts > 0) {
		Timer_start(timer(AppleSpawn));
		Mix_PlayChannel(-1, sound(Boom), 0);
	} else
		died();
}

static void useRadar(void) {
	if (Timer_isActive(timer(Radar)) || state.radars <= 0 || !Timer_isActive(timer(AppleLifetime)))
		return;

	Mix_PlayChannel(-1, sound(Radar), 0);

	Timer_start(timer(Radar));
	Timer_pause(timer(AppleLifetime));
	-- state.radars;
}

static void radarFindApple(void) {
	Timer_resume(timer(AppleLifetime));
	flipApple();
}

static void saveScoreToFile(const char *path) {
	FILE *file = fopen(path, "w");
	if (file == NULL)
		Log_warn("Cannot save score: %s", strerror(errno));

	fprintf(file, "%i", getScore());
	fclose(file);
}

static void youLost(void) {
	char *path = fsJoinPath(fsGetExecFolder(), SCORE_PATH);
	FILE *file = fopen(path, "r");
	if (file == NULL)
		saveScoreToFile(path);
	else {
		int ret = fscanf(file, "%i", &state.bestScore);
		fclose(file);

		if (getScore() > state.bestScore || ret != 1) {
			saveScoreToFile(path);
			state.bestScore = getScore();
		}
	}

	Core_pushScene(LostScene_new());
}

static void renderHeader(void) {
	/* Header frame */
	Rect frameRect = {
		Vec2_int(MARGIN),
		(Vec2){state.canv.size.x * CANVAS_SCALE, HEADER},
	};
	uiFrame(frameRect, &core.sheet, SpriteFrame);

	/* Score counter */
	state.uiCounterRect = (Rect){
		Vec2_add(frameRect.pos, Vec2_int(PADDING)),
		sprite(Digits).size,
	};
	uiCounter(state.uiCounterRect, getScore(), COUNTER_LEN, &core.sheet, SpriteDigits);

	/* Health bar (hearts) */
	Rect healthBarRect = {
		Vec2_add(frameRect.pos, (Vec2){
			frameRect.size.x - PADDING - sprite(HealthBar).size.x,
			PADDING,
		}),
		sprite(HealthBar).size,
	};
	{
		static float prev = 1;
		float t = (float)state.hearts / MAX_HEARTS;

		uiProgressBar(healthBarRect, t, &prev, 0.15, &core.sheet, SpriteHealthBar, 0, 1);
	}

	/* Radar bar */
	{
		static float prev = 1;
		float t = (float)state.radars / MAX_RADARS;

		uiProgressBar((Rect){
			Vec2_sub(healthBarRect.pos, (Vec2){sprite(RadarBar).size.x + PADDING * 3, 0}),
			sprite(RadarBar).size,
		}, t, &prev, 0.15, &core.sheet, SpriteRadarBar, 0, 1);
	}

	/* Timer bar */
	{
		static float prev = 0;
		float t = 0;
		if (Timer_isActive(timer(AppleSpawn)))
			t = Timer_progress(timer(AppleSpawn));
		else if (Timer_isActive(timer(AppleLifetime)))
			t = 1 - Timer_progress(timer(AppleLifetime));

		uiProgressBar((Rect){
			Vec2_add(frameRect.pos, (Vec2){
				PADDING,
				frameRect.size.y - PADDING - sprite(TimerBar).size.y
			}),
			sprite(TimerBar).size,
		}, easeInOutQuad(t), &prev, 0.35, &core.sheet, SpriteTimerBar, 0, 1);
	}
}

static void renderOutline(Rect rect, int w, int r, int g, int b, int a) {
	SDL_SetRenderDrawColor(core.ren, r, g, b, a);
	SDL_Rect dest;

	dest = Rect_toSDL((Rect){rect.pos, (Vec2){rect.size.x - w, w}});
	SDL_RenderFillRect(core.ren, &dest);

	dest = Rect_toSDL((Rect){Vec2_add(rect.pos, (Vec2){0, w}), (Vec2){w, rect.size.y - w * 2}});
	SDL_RenderFillRect(core.ren, &dest);

	dest = Rect_toSDL((Rect){(Vec2){rect.pos.x, Rect_end(rect).y - w}, (Vec2){rect.size.x - w, w}});
	SDL_RenderFillRect(core.ren, &dest);

	dest = Rect_toSDL((Rect){(Vec2){Rect_end(rect).x - w, rect.pos.y}, (Vec2){w, rect.size.y}});
	SDL_RenderFillRect(core.ren, &dest);
}

static void renderCanvas(void) {
	Rect viewport = Rect_new(CANVAS_X, CANVAS_Y, CANVAS_W, CANVAS_H);
	setViewport(viewport);

	Vec2 offset = {0, 0};
	if (Timer_isActive(timer(Shake))) {
		offset = Vec2_scale((Vec2){
			MARGIN - rand() % (MARGIN * 2),
			MARGIN - rand() % (MARGIN * 2),
		}, 1 - Timer_progress(timer(Shake)));
	}

	Rect canvRect = {offset, viewport.size};
	Canvas_render(&state.canv, canvRect);

	static bool radar = false;
	if (Timer_isActive(timer(Radar))) {
		static int  tileScrSize = TILE_SIZE * CANVAS_SCALE;
		static Vec2 pos;

		Vec2 snakeHeadScrPos = Vec2_add(Vec2_scale(*state.snake.head, tileScrSize), canvRect.pos);
		Vec2 size = Vec2_int(easeInBack(Timer_progress(timer(Radar))) * CANVAS_W * 1.5);

		if (!radar)
			pos = Vec2_add(snakeHeadScrPos, Vec2_int(tileScrSize / 2));

		radar = true;
		renderOutline((Rect){
			Vec2_sub(pos, Vec2_scale(size, 0.5)),
			size,
		}, 5 + 60 * easeInQuad(Timer_progress(timer(Radar))),
		255, 255, 255, easeOutQuad(1 - Timer_progress(timer(Radar))) * 255);
	} else
		radar = false;

	resetViewport();
}

static void render(void) {
	renderCanvas();
	renderHeader();
}

static void handleEvents(void) {
	switch (core.evt.type) {
	case SDL_KEYDOWN:
		switch (core.evt.key.keysym.sym) {
		case SDLK_SPACE: Core_pushScene(PausedScene_new()); break;

		case SDLK_w: Snake_turn(&state.snake, DirUp);    break;
		case SDLK_a: Snake_turn(&state.snake, DirLeft);  break;
		case SDLK_s: Snake_turn(&state.snake, DirDown);  break;
		case SDLK_d: Snake_turn(&state.snake, DirRight); break;

		case SDLK_r: useRadar(); break;
		}
		break;
	}
}

static void update(double dt) {
	(void)dt;

	/* One of the timer callbacks couldve changed the scene, so we have to check for that again */
	if (Timer_isActive(timer(Lost)))
		return;

	if (Vec2_equals(*state.snake.head, state.apple) && Timer_isActive(timer(AppleLifetime)))
		foundApple();

	if (!Vec2_inRect(*state.snake.head, Rect_new(0, 0, COLS, ROWS)) ||
	    Snake_bodyContains(&state.snake, *state.snake.head)) {
		state.hearts = 0;
		died();

		Timer_start(timer(Shake));
		Timer_stop(timer(AppleLifetime));
		Timer_stop(timer(AppleSpawn));
	}
}

static void onDisabled(void) {
	/* Reset all particles */
	zeroArray(state.base.particles, state.base.particlesCap);
}

Scene GameScene_new(void) {
	zeroStruct(&state);
	return Scene_new((SceneState*)&state, (SceneFns){
		.setup        = setup,
		.cleanup      = cleanup,
		.render       = render,
		.handleEvents = handleEvents,
		.update       = update,
		.onDisabled   = onDisabled,
	}, TimersCount);
}

int GameScene_getScore(void) {
	assert(state.base.isSetup);
	return getScore();
}

int GameScene_getBestScore(void) {
	assert(state.base.isSetup);
	return state.bestScore;
}

Canvas *GameScene_getCanvas(void) {
	assert(state.base.isSetup);
	return &state.canv;
}

void GameScene_restart(void) {
	assert(state.base.isSetup);
	Core_pushScene(IntroScene_new());

	Snake_setup(&state.snake, (Vec2){2, ROWS / 2}, DirRight, 0.15, COLS * ROWS);
	state.hearts = MAX_HEARTS;
	state.radars = MAX_RADARS;

	Timer_start(timer(AppleSpawn));
	Timer_start(timer(Move));
}

void GameScene_playMusic(void) {
	assert(state.base.isSetup);

	if (state.musicMuted)
		setMusicVolume(0);
	else
		tweenFloatFn(&state, setMusicVolume, 0, 1, 320, NULL, NULL);

	playMusic(core.music[MusicTrack1 + rand() % (MusicCount - MusicTrack1)]);
}

bool GameScene_isMusicMuted(void) {
	assert(state.base.isSetup);
	return state.musicMuted;
}

void GameScene_muteMusic(bool mute) {
	assert(state.base.isSetup);
	state.musicMuted = mute;

	if (state.base.enabled)
		setMusicVolume(!mute);
}
