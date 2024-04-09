#ifndef MATH_H_HEADER_GUARD
#define MATH_H_HEADER_GUARD

#include <stdbool.h> /* bool, true, false */
#include <math.h>    /* M_PI */
#include <assert.h>  /* assert */

#include "common.h"

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

float lerp(float a, float b, float t);

float min(float a, float b);
float max(float a, float b);

typedef float (*Easer)(float);

float easeInSine      (float t);
float easeOutSine     (float t);
float easeInOutSine   (float t);
float easeInQuad      (float t);
float easeOutQuad     (float t);
float easeInOutQuad   (float t);
float easeInCubic     (float t);
float easeOutCubic    (float t);
float easeInOutCubic  (float t);
float easeInQuart     (float t);
float easeOutQuart    (float t);
float easeInOutQuart  (float t);
float easeInQuint     (float t);
float easeOutQuint    (float t);
float easeInOutQuint  (float t);
float easeInExpo      (float t);
float easeOutExpo     (float t);
float easeInOutExpo   (float t);
float easeInCirc      (float t);
float easeOutCirc     (float t);
float easeInOutCirc   (float t);
float easeInBack      (float t);
float easeOutBack     (float t);
float easeInOutBack   (float t);
float easeInElastic   (float t);
float easeOutElastic  (float t);
float easeInOutElastic(float t);
float easeInBounce    (float t);
float easeOutBounce   (float t);
float easeInOutBounce (float t);

typedef struct {
	float min, max;
} Range;

float Range_random(Range this, int precision);

typedef struct {
	int x, y;
} Vec2;

Vec2 Vec2_int(int x);

Vec2 Vec2_add   (Vec2 a, Vec2 b);
Vec2 Vec2_sub   (Vec2 a, Vec2 b);
Vec2 Vec2_mul   (Vec2 a, Vec2 b);
Vec2 Vec2_div   (Vec2 a, Vec2 b);
bool Vec2_equals(Vec2 a, Vec2 b);
Vec2 Vec2_scale (Vec2 this, float scale);
int  Vec2_area  (Vec2 this);

typedef struct {
	Vec2 pos, size;
} Rect;

Rect     Rect_new(int x, int y, int w, int h);
int      Rect_area    (Rect this);
Vec2     Rect_end     (Rect this);
SDL_Rect Rect_toSDL   (Rect this);
Rect     Rect_scale   (Rect this, int scale);
Range    Rect_rangeX  (Rect this);
Range    Rect_rangeY  (Rect this);
Rect     Rect_lerpSize(Rect this, float t);

bool Vec2_inRect(Vec2 this, Rect rect);

typedef enum {
	DirUp = 0,
	DirLeft,
	DirDown,
	DirRight,
} Dir;

Dir  Dir_opposite(Dir dir);
Vec2 Dir_toVec2(Dir dir);

Dir Vec2_dirTo(Vec2 this, Vec2 target);

#endif
