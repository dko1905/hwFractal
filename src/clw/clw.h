#ifndef CLW_CLW_H
#define CLW_CLW_H
#include <stdio.h>
#include <stdbool.h>

struct clw_info;
struct clw_config {
	/* Use file for source. */
	bool kernel_use_file;
	/* NULL when `kernel_use_file` is 1, kernel source. */
	char *kernel_str;
	/* NULL when `kernel_use_file` is 0, else path to kernel source. */
	char *kernel_path;
	/* Here you can choose what GPU you want to use, -1 is auto/user input .*/
	int gpu_choice;
};

int clw_init(struct clw_info **_info_p, const struct clw_config *_config);
void clw_free(struct clw_info **_info_p);

#endif
