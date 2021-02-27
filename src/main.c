#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>

#include "application.h"
#include "util/printer.h"

/* Configure printer. */
bool print_pdebug = true;
bool print_pinfo = true;
bool print_pwarn = true;
bool print_perr = true;


int main() {
	struct Config conf = {
		.width =  600,
		.height = 600,
		.title = "123",
		.version = {0, 1, 0}
	};

	struct Application *app = application_init(&conf);

	application_main(app);

	application_free(app);

	return 0;
}
