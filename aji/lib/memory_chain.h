#pragma once

/* 2022/02/20
   Ajiの速度改善のために作成したが、計測するまでもなくめちゃくちゃ遅い。
   既存の実装より速度が悪化したのでお蔵入り。 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void
AjiMemChain_Destroy(void);

void
AjiMemChain_Free(void *ptr);

void *
AjiMemChain_Malloc(size_t size);

void *
AjiMemChain_Calloc(size_t nmemb, size_t size);

void *
AjiMemChain_Realloc(void *ptr, size_t size);
