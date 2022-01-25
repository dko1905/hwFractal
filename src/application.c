#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "_application.h"
#include "application.h"

void Application_free(struct Application *app);

struct Application *Application_init(const struct Config *config)
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

	if (SDL_CreateWindowAndRenderer(app->config->width, app->config->height,
	                                SDL_WINDOW_RESIZABLE, &app->window,
                                        &app->renderer) < 0) {
		log_fatal("SDL_CreateWindowAndRenderer: %s", SDL_GetError());
		goto generic_err;
	}

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
	while (1) {
		SDL_PollEvent(&app->event);
		if (app->event.type == SDL_QUIT) break;
		SDL_SetRenderDrawColor(app->renderer, 0x00, 0x00, 0x00, 0x00);
		SDL_RenderClear(app->renderer);
		SDL_RenderPresent(app->renderer);
	}
}

void Application_free(struct Application *app)
{
	/* Check for invalid state. */
	if (app == NULL) return;
	if (app->mem_ctx == NULL) return;

	/* Free SDL. */
	if (app->renderer != NULL) SDL_DestroyRenderer(app->renderer);
	if (app->window != NULL) SDL_DestroyWindow(app->window);
	SDL_Quit();

	/* Do memory diagnostics: */
	if (app->config->reportMemoryUsage == 1) {
		talloc_report(app->mem_ctx, stderr);
	} else if (app->config->reportMemoryUsage == 2) {
		talloc_report_full(app->mem_ctx, stderr);
	}

	/* Free meta. */
	TALLOC_CTX *mem_ctx = app->mem_ctx;
	memset(app, 0, sizeof(*app));
	talloc_free(app);
	talloc_free(mem_ctx);
}

