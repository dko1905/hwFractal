#ifndef APP_H
#define APP_H
#include <talloc.h>
#include <SDL2/SDL.h>

#include "SDL_surface.h"
#include "lib/log.h"

#include "config.h"

struct Application {
	TALLOC_CTX *mem_ctx;
	struct Config *config;

	/* SDL2 */
	SDL_Window   *window;
	SDL_Renderer *renderer;
	SDL_Event     event;

	/* Rendering */
	uint32_t prev_time;
	bool update_screen;
	bool sleep_mode;
	uint64_t last_update_time;
	uint64_t current_update_time;

	/* Framebuffer */
	SDL_Texture  *texture;
	struct Pixel *framebuffer;
};

struct Pixel {
	uint8_t r;
	uint8_t b;
	uint8_t g;
};

#endif /* APP */
