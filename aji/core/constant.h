#pragma once

#define AJI__VERSION "0.35.53"

#if defined(_WIN32) || defined(_WIN64)
# define AJI__WINDOWS 1 /* cap: core/constant.h */
#else
# undef AJI__WINDOWS
#endif
