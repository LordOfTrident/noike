#include "core.h"

Core core;

static void Core_loadSounds(const char **sounds, size_t soundsCount) {
	core.sounds      = new(Mix_Chunk*, soundsCount);
	core.soundsCount = soundsCount;

	for (size_t i = 0; i < soundsCount; ++ i) {
		char *path     = fsJoinPath(fsGetExecFolder(), sounds[i]);
		core.sounds[i] = Mix_LoadWAV(path);
		if (core.sounds[i] == NULL)
			fatal("Failed to load sound: %s", SDL_GetError());

		xfree(path);
	}
}

static void Core_loadMusic(const char **music, size_t musicCount) {
	core.music      = new(Mix_Music*, musicCount);
	core.musicCount = musicCount;

	for (size_t i = 0; i < musicCount; ++ i) {
		char *path    = fsJoinPath(fsGetExecFolder(), music[i]);
		core.music[i] = Mix_LoadMUS(path);
		if (core.music[i] == NULL)
			fatal("Failed to load music: %s", SDL_GetError());

		xfree(path);
	}
}

void Core_setup(int w, int h, const char *title, Scene initialScene, GameResources res) {
	zeroStruct(&core);
	core.winSize = (Vec2){w, h};

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		fatal("Error while initializing SDL2: %s", SDL_GetError());

	if (IMG_Init(IMG_INIT_PNG) < 0)
		fatal("Error while initializing SDL2_image: %s", SDL_GetError());

	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 200) < 0)
		fatal("Error while initializing SDL2_mixer: %s", SDL_GetError());

	core.win = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
	                            core.winSize.x, core.winSize.y, SDL_WINDOW_SHOWN);
	if (core.win == NULL)
		fatal("Error while creating the window: %s", SDL_GetError());

	core.ren = SDL_CreateRenderer(core.win, -1,
	                              SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (core.ren == NULL)
		fatal("Error while creating the renderer: %s", SDL_GetError());

	core.scr = SDL_CreateTexture(core.ren, SDL_PIXELFORMAT_RGBA8888,
	                             SDL_TEXTUREACCESS_TARGET, core.winSize.x, core.winSize.y);
	if (core.scr == NULL)
		fatal("Error while creating the screen texture: %s", SDL_GetError());

	if (SDL_RenderSetLogicalSize(core.ren, core.winSize.x, core.winSize.y) < 0)
		fatal("Error while setting logical render size: %s", SDL_GetError());

	if (SDL_SetRenderDrawBlendMode(core.ren, SDL_BLENDMODE_BLEND) < 0)
		fatal("Error while setting blend mode: %s", SDL_GetError());

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

	/* Setting the window to resizable after creation instead of passing it as a flag to the
	   SDL_CreateWindow function makes i3wm float the window instead of tiling it, a little trick */
	SDL_SetWindowResizable(core.win, true);

	Log_info("Initialized video");

	SDL_RendererInfo renInfo;
    SDL_GetRendererInfo(core.ren, &renInfo);
    Log_info("Using renderer %s", renInfo.name);

   	SpriteSheet_setup(&core.sheet, fsGetExecFolder(),
   	                  res.spritesCount, res.sprites, core.ren);

	Core_loadSounds(res.sounds, res.soundsCount);
	Core_loadMusic(res.music, res.musicCount);

   	Core_changeScene(initialScene);
}

void Core_cleanup(void) {
	SpriteSheet_cleanup(&core.sheet);

	for (size_t i = 0; i < core.soundsCount; ++ i)
		Mix_FreeChunk(core.sounds[i]);

	for (size_t i = 0; i < core.musicCount; ++ i)
		Mix_FreeMusic(core.music[i]);

	xfree(core.sounds);
	xfree(core.music);

	for (int i = 0; i < core.scenesCount; ++ i)
		Scene_callFn(core.scenes + i, cleanup);

	SDL_DestroyTexture(core.scr);
	SDL_DestroyRenderer(core.ren);
	SDL_DestroyWindow(core.win);

	Mix_CloseAudio();
	IMG_Quit();
	SDL_Quit();
}

void Core_render(void) {
	SDL_SetRenderDrawColor(core.ren, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(core.ren);

	/* This screen texture stuff is a workaround to fix a weird bug when using a viewport with
	   logical render size. the bug is that the screen is suddenly not centered when resizing
	   the window. */
	SDL_SetRenderTarget(core.ren, core.scr);
	SDL_RenderSetViewport(core.ren, NULL);
	SDL_RenderClear(core.ren);

	for (int i = 0; i < core.scenesCount; ++ i) {
		Scene_render(core.scenes + i);

		for (size_t j = 0; j < core.scenes[i].state->particlesCap; ++ j)
			Particle_render(core.scenes[i].state->particles + j, core.ren);

		SDL_RenderSetViewport(core.ren, NULL);
	}

	SDL_SetRenderTarget(core.ren, NULL);
	SDL_RenderCopy(core.ren, core.scr, NULL, NULL);
	SDL_RenderPresent(core.ren);
}

static void Core_toggleFullscreen(void) {
	core.fullscreen = !core.fullscreen;
	/* SDL_WINDOW_FULLSCREEN */
	SDL_SetWindowFullscreen(core.win, SDL_WINDOW_FULLSCREEN_DESKTOP * core.fullscreen);
}

void Core_handleEvents(void) {
	while (SDL_PollEvent(&core.evt)) {
		switch (core.evt.type) {
		case SDL_QUIT: core.quit = true; break;
		case SDL_KEYDOWN:
			switch (core.evt.key.keysym.sym) {
			case SDLK_ESCAPE: Core_toggleFullscreen(); break;
			}
			break;
		}

		for (int i = 0; i < core.scenesCount; ++ i)
			Scene_handleEvents(core.scenes + i);
	}
}

void Core_update(double dt) {
	for (int i = 0; i < core.scenesCount; ++ i)
		Scene_update(core.scenes + i, dt);

	for (int i = 0; i < core.scenesToDeleteCount; ++ i)
		Scene_cleanup(core.sceneDeleteQueue + i);

	core.scenesToDeleteCount = 0;
}

static void Core_addScene(Scene scene) {
	core.scenes[core.scenesCount ++] = scene;
	Scene_setup(&scene);
}

static void Core_removeScene(void) {
	core.sceneDeleteQueue[core.scenesToDeleteCount ++] = Core_getScene();
	-- core.scenesCount;
}

void Core_changeScene(Scene scene) {
	if (core.scenesCount > 0)
		Core_removeScene();

	Core_addScene(scene);
}

void Core_pushScene(Scene scene) {
	if (core.scenesCount > 0) {
		Scene scene = Core_getScene();

		scene.state->enabled = false;
		Scene_callFn(&scene, onDisabled);
	}

	Core_addScene(scene);
}

void Core_popScene(void) {
	/* Do not allow to pop off the last scene */
	assert(core.scenesCount > 1);

	Core_removeScene();

	if (core.scenesCount > 0) {
		Scene scene = Core_getScene();

		scene.state->enabled = true;
		Scene_callFn(&scene, onEnabled);
	}
}

Scene Core_getScene(void) {
	return core.scenes[core.scenesCount - 1];
}
