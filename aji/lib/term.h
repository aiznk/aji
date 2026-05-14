#pragma once

#include <aji/lib/windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#if defined(_WIN32) || defined(_WIN64)
# define AJI_TERM__WINDOWS
#endif

enum {
    AJI_TERM__NULL,
    
    // fg, bg
    AJI_TERM__BLACK,
    AJI_TERM__RED,
    AJI_TERM__GREEN,
    AJI_TERM__YELLOW,
    AJI_TERM__BLUE,
    AJI_TERM__MAGENTA,
    AJI_TERM__CYAN,
    AJI_TERM__GRAY,
    AJI_TERM__WHITE,
    AJI_TERM__DEFAULT,

    // opt
    AJI_TERM__UNDER,
    AJI_TERM__BRIGHT,
    AJI_TERM__REVERSE,
};

#define AjiTerm_CPrintf(fg, bg, opt, fmt, ...) \
    AjiTerm_CFPrintf(stdout, fg, bg, opt, fmt, __VA_ARGS__)

/**
 * print format with color at stream
 *
 * @param[in] *fout destination stream
 * @param[in] fg    fore ground color
 * @param[in] bg    back ground color
 * @param[in] opt   optional attributes
 * @param[in] *fmt  format
 * @param[in] ...   arguments
 *
 * @return print length
 */
int 
AjiTerm_CFPrintf(FILE *fout, int fg, int bg, int opt, const char *fmt, ...);

