#ifndef SCENES_PUBLIC_H_HEADER_GUARD
#define SCENES_PUBLIC_H_HEADER_GUARD

#include "scene.h"
#include "../canvas.h"

Scene   GameScene_new(void);
int     GameScene_getScore(void);
int     GameScene_getBestScore(void);
Canvas *GameScene_getCanvas(void);
void    GameScene_restart(void);
void    GameScene_playMusic(void);
bool    GameScene_isMusicMuted(void);
void    GameScene_muteMusic(bool mute);

Scene PausedScene_new(void);
Scene IntroScene_new(void);
Scene LostScene_new(void);

#endif
