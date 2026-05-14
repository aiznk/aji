#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    int len;
    int capa;
    char *buf;
} string;

char dummy_buf[1];
#define STR_INIT { .len=0, .capa=4, .buf=dummy_buf }

void
str_init(string *self) {
    if (!self) {
        return;
    }

    self->len = 0;
    self->capa = 4;
    self->buf = AjiMem_Calloc(self->capa + 1, sizeof(char));
    if (!self->buf) {
        return;
    }
}

void
str_destroy(string *self) {
    if (!self) {
        return;
    }

    if (self->buf != dummy_buf) {
        free(self->buf);
    }
}

void
str_del(string *self) {
    if (!self) {
        return;
    }

    free(self->buf);
    free(self);
}

string *
str_new(void) {
    string *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->capa = 4;
    self->buf = AjiMem_Calloc(self->capa + 1, sizeof(char));
    if (!self->buf) {
        return NULL;
    }

    return self;
}

static void
test_static_1(long n) {
    for (long i = 0; i < n; i += 1) {
        string s = STR_INIT;
        str_destroy(&s);
    }
}

static void
test_static_2(long n) {
    for (long i = 0; i < n; i += 1) {
        string s = {0};
        str_init(&s);
        str_destroy(&s);
    }
}

static void
test_dynamic(long n) {
    for (long i = 0; i < n; i += 1) {
        string *s = str_new();
        str_del(s);
    }
}

int
main(void) {
    clock_t start,end;
    long n = 100000000;

    start = clock();
    test_static_1(n);
    end = clock();
    printf("static_1: %.2f seconds\n", (double) (end - start) / CLOCKS_PER_SEC);

    start = clock();
    test_static_2(n);
    end = clock();
    printf("static_2: %.2f seconds\n", (double) (end - start) / CLOCKS_PER_SEC);

    start = clock();
    test_dynamic(n);
    end = clock();
    printf("dynamic: %.2f seconds\n", (double) (end - start) / CLOCKS_PER_SEC);
    
    /*  result

        $ gcc test_static_dynamic.c
        $ ./a.out
        static_1: 1.80 seconds
        static_2: 2.09 seconds
        dynamic: 3.67 seconds
    */

    return 0;
}
