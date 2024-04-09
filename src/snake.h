#ifndef SNAKE_H_HEADER_GUARD
#define SNAKE_H_HEADER_GUARD

#include <stdbool.h> /* bool, true, false */
#include <assert.h>  /* assert */

#include "utils/common.h"
#include "utils/math.h"
#include "config.h"

typedef struct {
	Vec2  *body, *head, tip;
	size_t cap;
	int    len, growRequest;
	Dir    dir, nextDir;
	float  progress, speed;
} Snake;

void Snake_setup       (Snake *this, Vec2 pos, Dir dir, float speed, size_t cap);
void Snake_cleanup     (Snake *this);
void Snake_move        (Snake *this);
void Snake_turn        (Snake *this, Dir dir);
void Snake_grow        (Snake *this);
Vec2 Snake_back        (Snake *this);
bool Snake_bodyContains(Snake *this, Vec2 pos);

#endif
