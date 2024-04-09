#ifndef CONFIG_H_HEADER_GUARD
#define CONFIG_H_HEADER_GUARD

#include "sprite_sheet.h"

#define FPS 60

#define COLS         14
#define ROWS         10
#define TILE_SIZE    20
#define CANVAS_SCALE 3
#define MARGIN       10
#define PADDING      10
#define HEADER       88

#define WIN_W (CANVAS_W + MARGIN * 2)
#define WIN_H (CANVAS_H + MARGIN * 3 + HEADER)

#define CANVAS_W (COLS * TILE_SIZE * CANVAS_SCALE)
#define CANVAS_H (ROWS * TILE_SIZE * CANVAS_SCALE)
#define CANVAS_X MARGIN
#define CANVAS_Y (HEADER + MARGIN * 2)

#define MAX_HEARTS  3
#define MAX_RADARS  3
#define COUNTER_LEN 3

#define SNAKE_MOVE_FREQUENCY 2

#define MAX_SNAKE_LEN COLS * ROWS

#define SCREEN_SHAKE_SIZE MARGIN

#define LOG_PATH   "noike.log"
#define SCORE_PATH "best-score.txt"

#define LOG_APPEND false

enum {
	SpriteTimerBar = 0,
	SpriteHealthBar,
	SpriteRadarBar,
	SpritePause,
	SpriteDigits,
	SpriteFrame,
	SpriteTutorial,
	SpriteIntro,
	SpriteLost,
	SpriteScore,
	SpriteBestScore,
	SpriteContinue,
	SpriteVolume,

	SpritesCount,
};

extern SpriteLoadInfo spritesToLoad[SpritesCount];

enum {
	SoundBoom = 0,
	SoundBoom2,
	SoundPop,
	SoundPop2,
	SoundPop3,
	SoundRadar,

	SoundsCount,
};

extern const char *soundsToLoad[SoundsCount];

enum {
	MusicMenu = 0,
	MusicDead,
	MusicTrack1,
	MusicTrack2,
	MusicTrack3,

	MusicCount,
};

extern const char *musicToLoad[MusicCount];

#endif
