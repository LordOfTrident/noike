#include "canvas.h"

void Canvas_setup(Canvas *this, Vec2 size, SDL_Renderer *ren) {
	zeroStruct(this);
	this->size = size;
	this->ren  = ren;

	size_t cap = Vec2_area(size);
	this->buf  = new(uint32_t, cap);
	memset(this->buf, 0, cap * sizeof(uint32_t));

	this->tex = SDL_CreateTexture(this->ren, SDL_PIXELFORMAT_ABGR8888,
	                              SDL_TEXTUREACCESS_STREAMING, size.x, size.y);
	if (this->tex == NULL)
		fatal("Error while creating canvas texture: %s", SDL_GetError());
}

void Canvas_cleanup(Canvas *this) {
	xfree(this->buf);
	SDL_DestroyTexture(this->tex);
}

void Canvas_render(Canvas *this, Rect rect) {
	SDL_UpdateTexture(this->tex, NULL, this->buf, this->size.x * sizeof(*this->buf));

	SDL_Rect dest = Rect_toSDL(rect);
	SDL_RenderCopy(this->ren, this->tex, NULL, &dest);
}

uint32_t *Canvas_getPixel(Canvas *this, Vec2 pos) {
	if (pos.x < 0 || pos.y < 0 || pos.x >= this->size.x || pos.y >= this->size.y)
		return NULL;

	return this->buf + (pos.y * this->size.x + pos.x);
}

void Canvas_flipPixel(Canvas *this, Vec2 pos) {
	uint32_t *pixel = Canvas_getPixel(this, pos);
	if (pixel == NULL)
		return;

	*pixel = CANVAS_PIXEL - *pixel;
}

void Canvas_flipRect(Canvas *this, Rect rect) {
	for (int y = rect.pos.y; y < Rect_end(rect).y; ++ y) {
		for (int x = rect.pos.x; x < Rect_end(rect).x; ++ x)
			Canvas_flipPixel(this, (Vec2){x, y});
	}
}

void Canvas_flipOutline(Canvas *this, Rect rect) {
	Canvas_flipRect(this, (Rect){rect.pos, (Vec2){rect.size.x, 1}});
	Canvas_flipRect(this, (Rect){rect.pos, (Vec2){1, rect.size.y}});
	Canvas_flipRect(this, (Rect){(Vec2){rect.pos.x, Rect_end(rect).y}, (Vec2){rect.size.x, 1}});
	Canvas_flipRect(this, (Rect){(Vec2){Rect_end(rect).x, rect.pos.y}, (Vec2){1, rect.size.y}});
}
