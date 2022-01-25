#ifndef APP_H
#define APP_H
#include <talloc.h>

#include "config.h"

struct Application;
struct Application *Application_init(const struct Config *config);
void Application_main(struct Application *app);
void Application_free(struct Application *app);

#endif /* APP */
