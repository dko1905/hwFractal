#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>

#include "clw/clw.h"
#include "util/printer.h"

/* Configure printer. */
bool print_pdebug = true;
bool print_pinfo = true;
bool print_pwarn = true;
bool print_perr = true;

int main() {
	struct clw_info *info = NULL;
	struct clw_config config;

	srand(time(NULL)); /* Configure random numbers. */
	memset(&config, 0, sizeof(config));
	config.kernel_use_file = true;
	config.kernel_path = "resources/kernel.opencl";
	config.gpu_choice = 0;

	if (clw_init(&info, &config) != 0) {
		perr("clw_init: Fail: %s", strerror(errno));
		return -1;
	}

	clw_test2(info, 10);

	clw_free(&info);

	return 0;
}
