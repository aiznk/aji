#include <aji/lib/memman.h>

// AjiMem_Reallocateしちゃうとオブジェクトたちが持ってるメモリ領域が変わっちゃうんじゃないの
// アドレスが変わっちゃうでしょ

enum {
    POOL_SIZE = 10000,
    BLOCK_SIZE = 100,
    USED_SIZE = 10000,
};

typedef struct {
    bool is_used;
    size_t beg;
    size_t end;
} Used;

typedef struct {
    void *pool;
    size_t pool_size;
    size_t pool_len;
    size_t index;
    Used *used;  // array
    size_t used_len;
} Memman;

static Memman memman;
static bool is_initialized;

static void
die(const char *fmt, ...) {
    fflush(stderr);
    fflush(stdout);
    fprintf(stderr, "memman: ");
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, "\n");
    fflush(stderr);
    exit(1);
}

static void
init(void) {
    if (is_initialized) {
        return;
    }

    if (memman.pool) {
        free(memman.pool);
    }
    memman = (Memman) {0};

    memman.pool_size = POOL_SIZE;
    memman.pool_len = memman.pool_size / BLOCK_SIZE;
    memman.pool = malloc(memman.pool_size);
    if (!memman.pool) {
        die("failed to allocate memory");
    }
    // printf("init pool_size[%ld] pool_len[%ld]\n", 
    //     memman.pool_size, memman.pool_len);

    memman.used_len = USED_SIZE;
    memman.used = calloc(memman.used_len, sizeof(Used));
    if (!memman.used) {
        die("failed to allocate used array");
    }

    is_initialized = true;
}

void
AjiMemman_Free(void *ptr) {
    init();

    for (size_t i = 0; i < memman.pool_len; i += 1) {
        size_t beg = i * BLOCK_SIZE;
        if ((memman.pool + beg) == ptr) {
            memman.used[i].is_used = false;
            break;
        }
    }
}

void *
AjiMemman_Malloc(size_t total) {
    init();

    size_t amari = total % BLOCK_SIZE;
    size_t n = total / BLOCK_SIZE;
    if (amari) {
        n += 1;
    }

    size_t cnt;
    size_t beg;
    size_t end;
    int m;
    bool found;
    cnt = 0;
    beg = 0;
    end = 0;
    m = 0;
    found = false;

#define final() { \
    found = true; \
    memman.used[beg].is_used = true; \
    memman.used[beg].beg = beg; \
    memman.used[beg].end = end; \
    goto done; \
}

#define resize_used() { \
    size_t old_len = memman.used_len; \
    size_t old_size = sizeof(Used) * old_len; \
    memman.used_len *= 2; \
    size_t used_size = sizeof(Used) * memman.used_len; \
    memman.used = realloc(memman.used, used_size); \
    if (!memman.used) { \
        die("failed to re-allocate used array"); \
    } \
    memset(memman.used + old_len, 0, used_size - old_size); \
}

    for (size_t i = 0; i < memman.pool_len; i += 1) {
        if (i >= memman.used_len) {
            resize_used();
        }

        if (cnt >= n) {
            end = i;
            final();
        }

        switch (m) {
        case 0:
            if (!memman.used[i].is_used) {
                cnt += 1;
                m = 10;
                beg = i;
            }
            break;
        case 10:
            if (!memman.used[i].is_used) {
                cnt += 1;
            } else {
                m = 0;
                end = i;
                if (cnt >= n) {
                    final();
                } else {
                    cnt = 0;
                }
            }
            break;
        }
    }
done:
    if (!found) {
        die("not found");
    }

    // printf("total[%ld] beg[%ld] end[%ld] pool_size[%ld] pool_len[%ld]\n",
    //    total, beg, end, memman.pool_size, memman.pool_len);
    size_t blocks = beg * BLOCK_SIZE;
    return (memman.pool + blocks);
}

void *
AjiMemman_Calloc(size_t nmemb, size_t size) {
    size_t total = nmemb * size;
    void *p = AjiMemman_Malloc(total);
    if (!p) {
        return NULL;
    }
    
    memset(p, 0, total);
    return p;
}

void *
AjiMemman_Realloc(void *ptr, size_t size) {
    init();

    size_t amari = size % BLOCK_SIZE;
    size_t n = size / BLOCK_SIZE;
    if (amari) {
        n += 1;
    }

    Used *used;

    for (size_t i = 0; i < memman.pool_len; i += 1) {
        size_t beg = i * BLOCK_SIZE;
        if ((memman.pool + beg) == ptr) {
            used = &memman.used[i];
            size_t dist = used->end - used->beg;
            if (dist < n) {
                goto AjiMem_Reallocate;
            } else {
                return ptr;
            }
        }
    }

    // not found allocated memory
    return NULL;
AjiMem_Reallocate: {
    size_t dist = used->end - used->beg;
    size_t bytes = dist * BLOCK_SIZE;
    void *p = AjiMemman_Malloc(size);
    used->is_used = false;
    memmove(p, ptr, bytes);
    return p;
}
}

void
AjiMemman_Destroy(void) {
    free(memman.pool);
    free(memman.used);
    memman.pool = NULL;
    is_initialized = false;
}
