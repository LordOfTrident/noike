#include "sprite_sheet.h"

void Sprite_setup(Sprite *this, const char *folder, SpriteLoadInfo loadInfo, SDL_Renderer *ren) {
	zeroStruct(this);
	this->cols = loadInfo.cols <= 0? 1 : loadInfo.cols;
	this->rows = loadInfo.rows <= 0? 1 : loadInfo.rows;
	this->framesCount = this->cols * this->rows;

	char        *path = fsJoinPath(folder, loadInfo.path);
	SDL_Surface *surf = IMG_Load(path);
	if (surf == NULL)
		fatal("Failed to load sprite image: %s", SDL_GetError());

	this->tex = SDL_CreateTextureFromSurface(ren, surf);
	if (this->tex == NULL)
		fatal("Failed to create sprite image texture: %s", SDL_GetError());

	Log_info("Loaded sprite from %s", path);

	SDL_FreeSurface(surf);
	xfree(path);

	int w, h;
	SDL_QueryTexture(this->tex, NULL, NULL, &w, &h);
	this->size.x = w / this->cols;
	this->size.y = h / this->rows;
}

void Sprite_cleanup(Sprite *this) {
	SDL_DestroyTexture(this->tex);
}

void SpriteSheet_setup(SpriteSheet *this, const char *folder,
                       int spritesCount, SpriteLoadInfo *loadInfo, SDL_Renderer *ren) {
	zeroStruct(this);
	this->spritesCount = spritesCount;
	this->sprites      = new(Sprite, spritesCount);
	this->ren          = ren;

	for (int i = 0; i < this->spritesCount; ++ i)
		Sprite_setup(this->sprites + i, folder, loadInfo[i], ren);
}

void SpriteSheet_cleanup(SpriteSheet *this) {
	for (int i = 0; i < this->spritesCount; ++ i)
		Sprite_cleanup(this->sprites + i);

	xfree(this->sprites);
}

void SpriteSheet_render(SpriteSheet *this, int id, int frame, Rect dest) {
	SpriteSheet_renderCrop(this, id, frame, (Rect){Vec2_int(0), this->sprites[id].size}, dest);
}

void SpriteSheet_renderCrop(SpriteSheet *this, int id, int frame, Rect src, Rect dest) {
	assert(id < this->spritesCount);
	Sprite *sprite = this->sprites + id;
	assert(frame < sprite->framesCount);

	src.pos.x += frame % sprite->cols * sprite->size.x;
	src.pos.y += frame / sprite->cols * sprite->size.y;
	src.size.x = src.size.x > sprite->size.x? sprite->size.x : src.size.x;
	src.size.y = src.size.y > sprite->size.y? sprite->size.y : src.size.y;

	SDL_Rect srcSDL = Rect_toSDL(src), destSDL = Rect_toSDL(dest);
	SDL_RenderCopy(this->ren, sprite->tex, &srcSDL, &destSDL);
}
