#include "ui.h"

enum {
	Slice9TL = 0,
	Slice9TC,
	Slice9TR,

	Slice9CL,
	Slice9CC,
	Slice9CR,

	Slice9BL,
	Slice9BC,
	Slice9BR,
};

void uiFrame(Rect rect, SpriteSheet *sheet, int id) {
	assert(id < sheet->spritesCount);
	Sprite *sprite = sheet->sprites + id;
	assert(rect.size.x >= sprite->size.x * 2);
	assert(rect.size.y >= sprite->size.y * 2);

	Rect dest;

	dest.pos  = rect.pos;
	dest.size = sprite->size;
	SpriteSheet_render(sheet, id, Slice9TL, dest);

	dest.pos.x += dest.size.x;
	dest.size.x = rect.size.x - sprite->size.x * 2;
	SpriteSheet_render(sheet, id, Slice9TC, dest);

	dest.pos.x += dest.size.x;
	dest.size.x = sprite->size.x;
	SpriteSheet_render(sheet, id, Slice9TR, dest);

	dest.pos  = (Vec2){rect.pos.x,     dest.pos.y + dest.size.y};
	dest.size = (Vec2){sprite->size.x, rect.size.y - sprite->size.y * 2};
	SpriteSheet_render(sheet, id, Slice9CL, dest);

	dest.pos.x += dest.size.x;
	dest.size.x = rect.size.x - sprite->size.x * 2;
	SpriteSheet_render(sheet, id, Slice9CC, dest);

	dest.pos.x += dest.size.x;
	dest.size.x = sprite->size.x;
	SpriteSheet_render(sheet, id, Slice9CR, dest);

	dest.pos  = (Vec2){rect.pos.x, dest.pos.y + dest.size.y};
	dest.size = sprite->size;
	SpriteSheet_render(sheet, id, Slice9BL, dest);

	dest.pos.x += dest.size.x;
	dest.size.x = rect.size.x - sprite->size.x * 2;
	SpriteSheet_render(sheet, id, Slice9BC, dest);

	dest.pos.x += dest.size.x;
	dest.size.x = sprite->size.x;
	SpriteSheet_render(sheet, id, Slice9BR, dest);
}

void uiCounter(Rect rect, int value, int len, SpriteSheet *sheet, int id) {
	for (int i = len; i --> 0;) {
		SpriteSheet_render(sheet, id, value % 10, (Rect){
			(Vec2){rect.pos.x + rect.size.x * i, rect.pos.y},
			rect.size,
		});

		value /= 10;
	}
}

void uiProgressBar(Rect rect, float progress, float *prevProgress, float t,
                   SpriteSheet *sheet, int id, int frameFill, int frameTrack) {
	assert(id < sheet->spritesCount);
	Sprite *sprite = sheet->sprites + id;

	float lerpedProgress = lerp(*prevProgress, progress, t);

	Rect src     = {.size = sprite->size};
	Rect dest    = rect;
	dest.size.x *= lerpedProgress;
	src.size.x  *= lerpedProgress;
	SpriteSheet_renderCrop(sheet, id, frameFill, src, dest);

	src.pos.x  += src.size.x;
	src.size.x  = sprite->size.x - src.size.x;
	dest.pos.x += dest.size.x;
	dest.size.x = rect.size.x - dest.size.x;
	SpriteSheet_renderCrop(sheet, id, frameTrack, src, dest);

	*prevProgress = lerpedProgress;
}
