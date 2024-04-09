#include "math.h"

float lerp(float a, float b, float t) {
    return a * (1.0 - t) + (b * t);
}

float min(float a, float b) {
	return a > b? b : a;
}

float max(float a, float b) {
	return a > b? a : b;
}

float easeInSine(float t) {
	return sin(1.5707963 * t);
}

float easeOutSine(float t) {
	return 1 + sin(1.5707963 * (t - 1));
}

float easeInOutSine(float t) {
	return 0.5 * (1 + sin(3.1415926 * (t - 0.5)));
}

float easeInQuad(float t) {
	return t * t;
}

float easeOutQuad(float t) {
	return t * (2 - t);
}

float easeInOutQuad(float t) {
	return t < 0.5? 2 * t * t : t * (4 - 2 * t) - 1;
}

float easeInCubic(float t) {
	return t * t * t;
}

float easeOutCubic(float t) {
	-- t;
	return 1 + t * t * t;
}

float easeInOutCubic(float t) {
	return t < 0.5? 4 * t * t * t : 1 + (t - 1) * (2 * (t - 2)) * (2 * (t - 2));
}

float easeInQuart(float t) {
	t *= t;
	return t * t;
}

float easeOutQuart(float t) {
	-- t;
	t = t * t;
	return 1 - t * t;
}

float easeInOutQuart(float t) {
	if (t < 0.5) {
		t *= t;
		return 8 * t * t;
	} else {
		-- t;
		t = t * t;
		return 1 - 8 * t * t;
	}
}

float easeInQuint(float t) {
	float t2 = t * t;
	return t * t2 * t2;
}

float easeOutQuint(float t) {
	--  t;
	float t2 = t * t;
	return 1 + t * t2 * t2;
}

float easeInOutQuint(float t) {
	float t2;
	if (t < 0.5) {
		t2 = t * t;
		return 16 * t * t2 * t2;
	} else {
		-- t;
		t2 = t * t;
		return 1 + 16 * t * t2 * t2;
	}
}

float easeInExpo(float t) {
	return (pow(2, 8 * t) - 1) / 255;
}

float easeOutExpo(float t) {
	return 1 - pow(2, -8 * t);
}

float easeInOutExpo(float t) {
	if (t < 0.5)
		return (pow(2, 16 * t) - 1) / 510;
	else
		return 1 - 0.5 * pow(2, -16 * (t - 0.5));
}

float easeInCirc(float t) {
	return 1 - sqrt(1 - t);
}

float easeOutCirc(float t) {
	return sqrt(t);
}

float easeInOutCirc(float t) {
	if (t < 0.5)
		return (1 - sqrt(1 - 2 * t)) * 0.5;
	else
		return (1 + sqrt(2 * t - 1)) * 0.5;
}

float easeInBack(float t) {
	return t * t * (2.70158 * t - 1.70158);
}

float easeOutBack(float t) {
	-- t;
	return 1 + t * t * (2.70158 * t + 1.70158);
}

float easeInOutBack(float t) {
	if (t < 0.5)
		return t * t * (7 * t - 2.5) * 2;
	else {
		-- t;
		return 1 + t * t * 2 * (7 * t + 2.5);
	}
}

float easeInElastic(float t) {
	float t2 = t * t;
	return t2 * t2 * sin(t * M_PI * 4.5);
}

float easeOutElastic(float t) {
	float t2 = (t - 1) * (t - 1);
	return 1 - t2 * t2 * cos(t * M_PI * 4.5);
}

float easeInOutElastic(float t) {
	float t2;
	if( t < 0.45 ) {
		t2 = t * t;
		return 8 * t2 * t2 * sin(t * M_PI * 9);
	} else if (t < 0.55)
		return 0.5 + 0.75 * sin(t * M_PI * 4);
	else {
		t2 = (t - 1) * (t - 1);
		return 1 - 8 * t2 * t2 * sin(t * M_PI * 9);
	}
}

float easeInBounce(float t) {
	return pow(2, 6 * (t - 1)) * fabs(sin(t * M_PI * 3.5));
}

float easeOutBounce(float t) {
	return 1 - pow(2, -6 * t) * fabs(cos(t * M_PI * 3.5));
}

float easeInOutBounce(float t) {
	if( t < 0.5 ) {
		return 8 * pow(2, 8 * (t - 1)) * fabs(sin(t * M_PI * 7));
	} else {
		return 1 - 8 * pow(2, -8 * t) * fabs(sin(t * M_PI * 7));
	}
}

float Range_random(Range this, int precision) {
	assert(precision >= 0);
	precision = pow(10, precision);

	if (this.max == this.min)
		return floor(this.max * precision) / precision;

	assert(this.max > this.min);
	this.min = this.min * precision;
	this.max = this.max * precision;

	return (this.min + rand() % (int)(this.max - this.min + 1)) / precision;
}

Vec2 Vec2_int(int x) {
	return (Vec2){x, x};
}

#define defineVec2BinOp(NAME, OP)                        \
	Vec2 Vec2_##NAME(Vec2 a, Vec2 b) {                   \
		return (Vec2){.x = a.x OP b.x, .y = a.y OP b.y}; \
	}

defineVec2BinOp(add, +)
defineVec2BinOp(sub, -)
defineVec2BinOp(mul, *)
defineVec2BinOp(div, /)

Vec2 Vec2_scale(Vec2 this, float scale) {
	return (Vec2){
		scale * this.x,
		scale * this.y,
	};
}

int Vec2_area(Vec2 this) {
	return this.x * this.y;
}

bool Vec2_equals(Vec2 a, Vec2 b) {
	return a.x == b.x && a.y == b.y;
}

Rect Rect_new(int x, int y, int w, int h) {
	return (Rect){
		.pos  = (Vec2){x, y},
		.size = (Vec2){w, h},
	};
}

Vec2 Rect_end(Rect this) {
	return Vec2_add(this.pos, this.size);
}

SDL_Rect Rect_toSDL(Rect this) {
	return (SDL_Rect){
		.x = this.pos.x,
		.y = this.pos.y,
		.w = this.size.x,
		.h = this.size.y,
	};
}

Rect Rect_scale(Rect this, int scale) {
	return (Rect){
		Vec2_scale(this.pos,  scale),
		Vec2_scale(this.size, scale),
	};
}

Range Rect_rangeX(Rect this) {
	return (Range){this.pos.x, Rect_end(this).x};
}

Range Rect_rangeY(Rect this) {
	return (Range){this.pos.y, Rect_end(this).y};
}

Rect Rect_lerpSize(Rect this, float t) {
	Vec2 size = Vec2_scale(this.size, t);
	return (Rect) {
		Vec2_add(this.pos, Vec2_sub(Vec2_scale(this.size, 0.5), Vec2_scale(size, 0.5))),
		size,
	};
}

bool Vec2_inRect(Vec2 this, Rect rect) {
	Vec2 end = Rect_end(rect);
	return this.x >= rect.pos.x && this.y >= rect.pos.y && this.x < end.x && this.y < end.y;
}

Dir Dir_opposite(Dir dir) {
	return (dir + 2) % (DirRight + 1);
}

Vec2 Dir_toVec2(Dir dir) {
	switch (dir) {
	case DirUp:    return (Vec2){.x =  0, .y = -1};
	case DirLeft:  return (Vec2){.x = -1, .y =  0};
	case DirDown:  return (Vec2){.x =  0, .y =  1};
	case DirRight: return (Vec2){.x =  1, .y =  0};

	default: unreachable();
	}
}

Dir Vec2_dirTo(Vec2 this, Vec2 target) {
	if (this.x == target.x)
		return this.y < target.y? DirDown : DirUp;
	else if (this.y == target.y)
		return this.x < target.x? DirRight : DirLeft;
	else
		unreachable();
}
