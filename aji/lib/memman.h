#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>

void
AjiMemman_Free(void *ptr);

void *
AjiMemman_Malloc(size_t size);

void *
AjiMemman_Calloc(size_t nmemb, size_t size);

void *
AjiMemman_Realloc(void *ptr, size_t size);

void
AjiMemman_Destroy(void);
