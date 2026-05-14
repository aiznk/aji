#pragma once

/* this module not working on find() and sort().
   so you don't use this module. */

#include <stdint.h>
#include <stdbool.h>
#include <aji/lib/memory.h>

struct AjiVoidVec;
typedef struct AjiVoidVec AjiVoidVec;

struct AjiVoidVec {
    void **vec;
    int32_t len;
    int32_t capa;
    void (*deleter)(void *);
    void *(*deep_copy)(const void *);
    void *(*shallow_copy)(const void *);
    int (*sort_compar)(const void *, const void *);
    int (*find_compar)(const void *, const void *);
};

void
AjiVoidVec_Del(AjiVoidVec *self);

void **
AjiVoidVec_EscDel(AjiVoidVec *self);

AjiVoidVec *
AjiVoidVec_New(
    void (*deleter)(void *),
    void *(*deep_copy)(const void *),
    void *(*shallow_copy)(const void *),
    int (*sort_compar)(const void *, const void *),
    int (*find_compar)(const void *, const void *)
);

AjiVoidVec *
AjiVoidVec_DeepCopy(const AjiVoidVec *other);

AjiVoidVec *
AjiVoidVec_ShallowCopy(const AjiVoidVec *other);

AjiVoidVec *
AjiVoidVec_PushBack(AjiVoidVec *self, const void *ptr);

void *
AjiVoidVec_PopMove(AjiVoidVec *self);

AjiVoidVec *
AjiVoidVec_MoveBack(AjiVoidVec *self, void *ptr);

AjiVoidVec *
AjiVoidVec_Sort(AjiVoidVec *self);

const void *
AjiVoidVec_Getc(const AjiVoidVec *self, int idx);

void *
AjiVoidVec_Get(const AjiVoidVec *self, int idx);

int32_t
AjiVoidVec_Len(const AjiVoidVec *self);

const AjiVoidVec *
AjiVoidVec_Show(const AjiVoidVec *self, FILE *fout);

void
AjiVoidVec_Clear(AjiVoidVec *self);

AjiVoidVec * 
AjiVoidVec_Resize(AjiVoidVec *self, int32_t capa);

void *
AjiVoidVec_Find(const AjiVoidVec *self, const void *ptr);

const void *
AjiVoidVec_Findc(const AjiVoidVec *self, const void *ptr);
