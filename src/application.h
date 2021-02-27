#ifndef APPLICATION_H
#define APPLICAITON_H
#include "config.h"

struct Application;

struct Application *application_init(struct Config *config);
void application_free(struct Application *application);
void application_main(struct Application *application);

#endif
