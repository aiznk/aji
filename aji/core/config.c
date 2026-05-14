#include <aji/core/config.h>

void
AjiConfig_Del(AjiConfig *self) {
    if (self) {
        free(self);
    }
}

AjiConfig *
AjiConfig_New(void) {
    AjiConfig *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    return self;
}

char *
Aji_PopTailSlash(char *path);

AjiConfig *
AjiConfig_Init(AjiConfig *self) {
    strcpy(self->line_encoding, "lf");

    // standard libraries
    if (!AjiFile_Solve(self->std_lib_dir_path, sizeof self->std_lib_dir_path, "lib")) {
        AjiErr_Err("failed to solve path for standard libraries directory");
        return NULL;
    }

    return self;
}
