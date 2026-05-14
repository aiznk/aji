#pragma once

#if defined(_WIN32) || defined(_WIN64)
# define AJI_PIPE__WINDOWS 1
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <aji/lib/windows.h>
#include <aji/lib/pipe.h>
#include <aji/lib/memory.h>

enum {
    AJI_PIPE__READ = 0,
    AJI_PIPE__WRITE = 1,
};

struct AjiPipe;
typedef struct AjiPipe AjiPipe;

/**
 * Destruct AjiObj
 *
 * @param[in] *self 
 */
void 
AjiPipe_Del(AjiPipe *self);

/**
 * Construct AjiObj
 * If failed to construct then show errors and exit from process
 *
 * @param[in] void 
 *
 * @return success to pointer to AjiPipe dynamic allocate memory
 */
AjiPipe * 
AjiPipe_New(void);

/**
 * Close pipe
 *
 * @param[in] *self 
 *
 * @return success to pointer to self
 * @return faield to pointer to NULL
 */
AjiPipe * 
AjiPipe_Close(AjiPipe *self);

/**
 * Open pipe
 *
 * @param[in] *self 
 * @param[in] flags number of open flags
 *
 * @return success to pointer to self
 * @return failed to pointer to NULL
 */
AjiPipe * 
AjiPipe_Open(AjiPipe *self, int flags);
