#include "snake.h"

void Snake_setup(Snake *this, Vec2 pos, Dir dir, float speed, size_t cap) {
	zeroStruct(this);
	this->cap      = cap;
	this->body     = new(Vec2, cap);
	this->head     = this->body;
	this->len      = 2;
	this->dir      = dir;
	this->nextDir  = dir;
	*this->head    = pos;
	this->progress = 1;
	this->speed    = speed;

	Vec2 offset   = Dir_toVec2(Dir_opposite(dir));
	this->body[1] = Vec2_add(*this->head, offset);
	this->tip     = Vec2_add(*this->head, Vec2_scale(offset, 2));
}

void Snake_cleanup(Snake *this) {
	xfree(this->body);
}

void Snake_move(Snake *this) {
	this->progress += this->speed;
	if (this->progress < 1)
		return;

	this->progress = 0;
	this->dir      = this->nextDir;
	this->tip      = Snake_back(this);

	for (; this->growRequest > 0; -- this->growRequest) {
		assert((size_t)this->len < this->cap);
		this->body[this->len] = Snake_back(this);
		++ this->len;
	}

	for (size_t i = this->len - 1; i > 0; -- i)
		this->body[i] = this->body[i - 1];

	*this->head = Vec2_add(*this->head, Dir_toVec2(this->dir));
}

void Snake_turn(Snake *this, Dir dir) {
	if ((this->dir - dir) % 2)
		this->nextDir = dir;
}

void Snake_grow(Snake *this) {
	++ this->growRequest;
}

Vec2 Snake_back(Snake *this) {
	return this->body[this->len - 1];
}

bool Snake_bodyContains(Snake *this, Vec2 pos) {
	for (int i = 1; i < this->len; ++ i) {
		if (Vec2_equals(this->body[i], pos))
			return true;
	}

	return false;
}
