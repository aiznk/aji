#pragma once

#include <aji/lib/error.h>
#include <aji/lib/memory.h>
#include <aji/lib/file.h>
#include <aji/lib/path.h>
#include <aji/core/constant.h>

typedef struct AjiConfig {
    char line_encoding[32+1];  // line encoding "cr" | "crlf" | "lf"
    char std_lib_dir_path[AJI_FILE__NPATH];  // standard libraries directory path
} AjiConfig;

/**
 * destruct AjiConfig_t
 * 
 * @param[in] *self 
 */
void
AjiConfig_Del(AjiConfig *self);

/**
 * construct AjiConfig_t
 * 
 * @return pointer to AjiConfig dynamic allocate memory
 */
AjiConfig *
AjiConfig_New(void);

/**
 * initialize AjiConfig
 * 
 * @param[in] *self 
 * 
 * @return pointer to self
 */
AjiConfig *
AjiConfig_Init(AjiConfig *self);
