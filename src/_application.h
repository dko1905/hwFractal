#ifndef APP_H
#define APP_H
#include <talloc.h>
#include <SDL2/SDL.h>

#include "lib/log.h"

#include "config.h"

struct Application {
	TALLOC_CTX *mem_ctx;
	const struct Config *config;

	/* SDL2 */
	SDL_Window   *window;
	SDL_Renderer *renderer;
	SDL_Surface  *surface;
	SDL_Event     event;
};

#endif /* APP */
