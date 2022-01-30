#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "SDL_pixels.h"
#include "SDL_render.h"
#include "SDL_timer.h"
#include "SDL_video.h"
#include "_application.h"
#include "application.h"

#define FRAMEBUFFER_POS(_x, _y) ((_y) * (app->config->width) + (_x))
#define a_inline inline __attribute__((always_inline))

static int update(struct Application *app);
static int update_events(struct Application *app);
static int update_draw(struct Application *app);
static int update_compute(struct Application *app);
static int update_fps(struct Application *app);
static a_inline void set_update_screen(struct Application *app, bool on);
static a_inline void set_sleep_mode(struct Application *app, bool on);

void Application_free(struct Application *app);

struct Application *Application_init(struct Config *config)
{
	struct Application *app;
	{
		/* Create context. */
		TALLOC_CTX *mem_ctx = talloc_new(NULL);
		if (mem_ctx == NULL) {
			char *reason = strerror(errno);
			log_fatal("Failed to create mem_ctx: %s", reason);
			goto mem_ctx_err;
		}
		/* Allocate application. */
		app = talloc_zero(mem_ctx, struct Application);
		if (app == NULL) {
			char *reason = strerror(errno);
			talloc_free(mem_ctx);
			log_fatal("Failed to allocate application: %s", reason);
			goto application_err;
		};
		app->mem_ctx = mem_ctx;
		app->config = config;
	}

	/* Initialize logging */
	log_set_level(LOG_DEBUG);
	log_debug("Logging initialized");

	/* Initialize SDL. */
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		log_fatal("SDL_Init: %s", SDL_GetError());
		goto generic_err;
	}
	log_debug("Initialized SDL");

	app->window = SDL_CreateWindow(app->config->title,
	                     SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
	                     app->config->width, app->config->height,
	                     0);
	if (app->window == NULL) {
		log_fatal("SDL_CreateWindow: %s", SDL_GetError());
		goto generic_err;
	}

	app->renderer = SDL_CreateRenderer(app->window, -1, 0);
	if (app->renderer == NULL) {
		log_fatal("SDL_CreateRenderer: %s", SDL_GetError());
		goto generic_err;
	}

	/* Setup framebuffer */
	app->texture = SDL_CreateTexture(app->renderer, SDL_PIXELFORMAT_RGB24,
	                                 SDL_TEXTUREACCESS_STREAMING,
	                                 app->config->width, app->config->height);
	if (app->texture == NULL) {
		log_fatal("SDL_CreateTexture: %s", SDL_GetError());
		goto generic_err;
	}
	app->framebuffer = talloc_zero_size(app->mem_ctx, sizeof(struct Pixel) * app->config->width * app->config->height);
	if (app->framebuffer == NULL) {
		log_fatal("Failed to allocate framebuffer: %s", strerror(errno));
		goto generic_err;
	}
	/* Statup */
	set_sleep_mode(app, false);
	set_update_screen(app, true);

	return app;
generic_err:
	Application_free(app);
	app = NULL;
application_err:
mem_ctx_err:
	return NULL;
}

void Application_main(struct Application *app)
{
	int ret = 0;

	while (ret == 0) {
		ret = update(app);
	}

	return;
}

static void set_update_screen(struct Application *app, bool on)
{
	if (!on) {
		app->update_screen = false;
	} else {
		app->update_screen = true;
		app->last_update_time = SDL_GetTicks64();
	}
}

static void set_sleep_mode(struct Application *app, bool on)
{
	if (!on) {
		app->sleep_mode = false;
	} else {
		app->sleep_mode = true;
		app->update_screen = false;
	}
}

/* 0-continue,1-stop */
static int update(struct Application *app)
{
	int ret = 0;

	app->current_update_time = SDL_GetTicks64();

	ret = update_events(app);

	ret += update_fps(app);

	if (!app->sleep_mode)
		ret += update_compute(app);

	if (app->update_screen) {
		ret += update_draw(app);
		app->update_screen = 0;
	}

	return ret;
}

/* Handle all input events. */
static int update_events(struct Application *app)
{
	int ret = 0;

	/* Poll events. */
	while ((ret = SDL_PollEvent(&app->event)) == 1) {
		switch (app->event.type) {
		case SDL_WINDOWEVENT:
			if (app->event.window.event == SDL_WINDOWEVENT_RESIZED) {
				log_debug("Received resize event, updating screen");
				set_update_screen(app, true);
                        } else {
				log_debug("Received unknown window event, updating screen");
				set_update_screen(app, true);
			}
			break;
		case SDL_QUIT:
			return 1;
		}
	}

	return 0;
}

/* Draw new frame. */
static int update_draw(struct Application *app)
{
	int ret = 0;

	/* Clear screen. */
	ret = SDL_SetRenderDrawColor(app->renderer, 0x00, 0x00, 0x00, 0x00);
	if (ret < 0) goto sdl_err;
	ret = SDL_RenderClear(app->renderer);
	if (ret < 0) goto sdl_err;

	/* Render framebuffer using texture. */
	struct Pixel *pixels; /* w-only access to texture buffer. */
	int pitch = 0; /* NOTUSED */
	ret = SDL_LockTexture(app->texture, NULL, (void**)&pixels, &pitch);
	if (ret < 0) {
		SDL_UnlockTexture(app->texture);
		goto sdl_err;
	}
	memcpy(pixels, app->framebuffer, sizeof(struct Pixel) * app->config->width * app->config->height);
	SDL_UnlockTexture(app->texture);
	ret = SDL_RenderCopy(app->renderer, app->texture, NULL, NULL);
	if (ret < 0) goto sdl_err;

	/* Render hidden framebuffer. */
	SDL_RenderPresent(app->renderer);

	return 0;
sdl_err:
	log_fatal("update_draw: %s", SDL_GetError());
	return 1;
}

static int n = 101;
static int l = 0;
static int update_compute(struct Application *app)
{
	if (n > 100) {
		n = 0;
		for (size_t i = 0; i < app->config->width; i++) {
			for (size_t j = 0; j < app->config->height; j++) {
				struct Pixel off = {255, 0, 0};
				struct Pixel on = {0, 255, 255};
				if ((i + l) % 10 == 0) {
					app->framebuffer[FRAMEBUFFER_POS(i, j)] = on;
				} else {
					app->framebuffer[FRAMEBUFFER_POS(i, j)] = off;
				}
			}
		}
		set_update_screen(app, true);
		l += 1;
		if (l > 500) set_sleep_mode(app, true);
	} else {
		n++;
	}

	return 0;
}

static int update_fps(struct Application *app)
{
	if (app->update_screen) return 0;

	/* Update screen every second. */
	if (app->sleep_mode) {
		if (app->current_update_time - app->last_update_time > app->config->update_timeout) {
			set_update_screen(app, true);
		} else {
			SDL_Delay(app->config->update_timeout);
			set_update_screen(app, true);
		}
	}

	return 0;
}

void Application_free(struct Application *app)
{
	/* Check for invalid state. */
	if (app == NULL) return;
	if (app->mem_ctx == NULL) return;

	/* Free framebuffer. */
	if (app->texture != NULL) SDL_DestroyTexture(app->texture);
	if (app->framebuffer != NULL) talloc_free(app->framebuffer);

	/* Free SDL. */
	if (app->renderer != NULL) SDL_DestroyRenderer(app->renderer);
	if (app->window != NULL) SDL_DestroyWindow(app->window);
	SDL_Quit();

	/* Do memory diagnostics: */
	if (app->config->talloc_report_mem_usage == 1) {
		talloc_report(app->mem_ctx, stderr);
	} else if (app->config->talloc_report_mem_usage == 2) {
		talloc_report_full(app->mem_ctx, stderr);
	}

	/* Free meta. */
	TALLOC_CTX *mem_ctx = app->mem_ctx;
	memset(app, 0, sizeof(*app));
	talloc_free(app);
	talloc_free(mem_ctx);
}

