#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>

#include "lib/log.h"

#include "application.h"

int main() {
	struct Config conf = {
		.title = "Hello",
		.width = 600,
		.height = 600,
		.fps = 60,
		.update_timeout = 250, /* 250ms */

		.talloc_log_err = true,
		.talloc_report_mem_usage = 1
	};

	struct Application *app = Application_init(&conf);
	if (app == NULL) return 1;

	Application_main(app);

	Application_free(app);

	return 0;
}
