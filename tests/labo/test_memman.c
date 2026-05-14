#include <stdio.h>
#include <stdlib.h>

enum {
    POOL_SIZE = 300,
    BLOCK_SIZE = 100,
};

int main(void) {
    void *p = AjiMem_Malloc(POOL_SIZE);

    for (size_t i = 0; i < POOL_SIZE; i += 1) {
        printf("%p\n", (p + i));
    }

    return 0;
}
