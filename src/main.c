#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>

#include "application.h"

int main() {
	struct Config conf = {
		.logErrorsToStderr = true,
		.reportMemoryUsage = 1,

		.title = "Hello",
	};

	struct Application *app = Application_init(&conf);

	Application_main(app);

	Application_free(app);

	return 0;
}
