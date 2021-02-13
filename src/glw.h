/* === OpenGL wrapper === */
#ifndef GLW_H
#define GLW_H

#include "runtime_config.h"

/* Forward declare struct to store info about the window/session. */
struct glw_info;

int glw_init(struct glw_info **_info_p, const struct runtime_config *_config);
void glw_free(struct glw_info **_info_p);
void glw_render(const struct glw_info *_info, const struct runtime_config *_config);
int glw_poll(const struct glw_info *_info, const struct runtime_config *_config);

#endif
