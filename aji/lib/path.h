/* TODO test */
#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#if defined(_WIN32) || defined(_WIN64)
# define AJI_PATH__WINDOWS 1 /* cap: path.h */
#else
# undef AJI_PATH__WINDOWS
#endif

char *
AjiPath_PopBackOf(char *path, int32_t ch);

char *
AjiPath_PopTailSlash(char *path);
