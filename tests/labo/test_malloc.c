#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        return 1;
    }

    const char *cmd = argv[1];
    const int n = atoi(argv[2]);
    clock_t start, end;

    if (cmd[0] == '1') {
        start = clock();
        for (int i = 0; i < n; i += 1) {
            int *p = AjiMem_Malloc(sizeof(int));
            p = 0;
            free(p);
        }
        end = clock();
        printf("%.2f\n", (double) (end - start) / CLOCKS_PER_SEC);
    } else if (cmd[0] == '2') {
        start = clock();
        int *p = AjiMem_Malloc(sizeof(int) * n);
        p = 0;
        free(p);
        end = clock();
        printf("%.2f\n", (double) (end - start) / CLOCKS_PER_SEC);
    }
}

