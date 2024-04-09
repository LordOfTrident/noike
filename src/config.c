#include "config.h"

#define RESOURCES_FOLDER "res"

#define spritePath(FILENAME) (RESOURCES_FOLDER PATH_SEP "sprites" PATH_SEP FILENAME)

SpriteLoadInfo spritesToLoad[SpritesCount] = {
	[SpritePause]     = {.path = spritePath("pause.png")},
	[SpriteTutorial]  = {.path = spritePath("tutorial.png")},
	[SpriteIntro]     = {.path = spritePath("intro.png")},
	[SpriteLost]      = {.path = spritePath("lost.png")},
	[SpriteScore]     = {.path = spritePath("score.png")},
	[SpriteBestScore] = {.path = spritePath("best-score.png")},
	[SpriteContinue]  = {.path = spritePath("continue.png")},
	[SpriteTimerBar]  = {
		.path = spritePath("timer-bar.png"),
		.rows = 2,
	},
	[SpriteHealthBar] = {
		.path = spritePath("health-bar.png"),
		.rows = 2,
	},
	[SpriteRadarBar] = {
		.path = spritePath("radar-bar.png"),
		.rows = 2,
	},
	[SpriteDigits] = {
		.path = spritePath("digits.png"),
		.cols = 10,
	},
	[SpriteFrame] = {
		.path = spritePath("frame.png"),
		.cols = 3,
		.rows = 3,
	},
	[SpriteVolume] = {
		.path = spritePath("volume.png"),
		.rows = 4,
	},
};

#define soundPath(FILENAME) (RESOURCES_FOLDER PATH_SEP "sounds" PATH_SEP FILENAME)

const char *soundsToLoad[SoundsCount] = {
	[SoundBoom]  = soundPath("boom.wav"),
	[SoundBoom2] = soundPath("boom2.wav"),
	[SoundPop]   = soundPath("pop.wav"),
	[SoundPop2]  = soundPath("pop2.wav"),
	[SoundPop3]  = soundPath("pop3.wav"),
	[SoundRadar] = soundPath("radar.wav"),
};

#define musicPath(FILENAME) (RESOURCES_FOLDER PATH_SEP "music" PATH_SEP FILENAME)

const char *musicToLoad[MusicCount] = {
	[MusicMenu]   = musicPath("menu.wav"),
	[MusicDead]   = musicPath("dead.wav"),
	[MusicTrack1] = musicPath("track1.wav"),
	[MusicTrack2] = musicPath("track2.wav"),
	[MusicTrack3] = musicPath("track3.wav"),
};
