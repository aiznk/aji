#pragma once

#include <aji/lib/string.h>
#include <aji/lib/cstring_vector.h>

typedef struct {
    int argc;
    char **argv;
    int cmd_argc;
    char **cmd_argv;
} AjiDistriArgs;

/**
 * distribute program arguments to application side and command side
 * 
 * @param[in] *dargs pointer to AjiDistriArgs 
 * @param[in] argc   number of arguments
 * @param[in] **argv arguments
 * 
 * @return pointer to dargs
 */
AjiDistriArgs *
AjiDistriArgs_Distribute(AjiDistriArgs *dargs, int argc, char **argv);
