#include <string.h> /* strerror */
#include <errno.h>  /* errno */
#include <stdint.h> /* uint64_t */

#include "core.h"
#include "utils/common.h"
#include "scenes/public.h"
#include "config.h"

int    argc;
char **argv;

int main(int argc_, char **argv_) {
	argc = argc_;
	argv = argv_;

	if (Log_setup(fsJoinPath(fsGetExecFolder(), LOG_PATH), LOG_APPEND) != 0)
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING,
		                         "Failed to create logs", strerror(errno), NULL);

	Core_setup(WIN_W, WIN_H, "Noike", GameScene_new(), (GameResources){
		.sprites      = spritesToLoad,
		.spritesCount = SpritesCount,

		.sounds      = soundsToLoad,
		.soundsCount = SoundsCount,

		.music      = musicToLoad,
		.musicCount = MusicCount,
	});

	uint64_t last = 0, now = SDL_GetPerformanceCounter();

	while (!core.quit) {
		last = now;
		now  = SDL_GetPerformanceCounter();
		double dt = (double)(now - last) * 1000 / (double)SDL_GetPerformanceFrequency();

		Core_render();
		Core_handleEvents();
		Core_update(dt);
	}
	Core_cleanup();

	Log_cleanup();
	return 0;
}
