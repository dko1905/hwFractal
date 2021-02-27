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

static const char *requested_extensions[] = {
	"VK_EXT_debug_utils"
};
static const char *requested_validation_layers[] = {
	"VK_LAYER_KHRONOS_validation"
};

int main() {
	struct Config conf = {
		.width =  600,
		.height = 600,
		.title = "123",
		.version = {0, 1, 0},
		/* Vulkan */
		.requested_extension_count = 1,
		.requested_extensions = requested_extensions,
		.requested_validation_layer_count = 1,
		.requested_validation_layers = requested_validation_layers
	};

	struct Application *app = application_init(&conf);

	application_main(app);

	application_free(app);

	return 0;
}
