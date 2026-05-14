#include <aji/lib/pipe.h>

struct AjiPipe {
#ifdef AJI_PIPE__WINDOWS
    HANDLE handles[2];
#else
    int fds[2];
#endif
};

void
AjiPipe_Del(AjiPipe *self) {
    if (!self) {
        return;
    }

    AjiPipe_Close(self);
    free(self);
}

AjiPipe *
AjiPipe_New(void) {
    AjiPipe *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        perror("AjiMem_Calloc");
        exit(1);
    }

    return self;
}

AjiPipe *
AjiPipe_Close(AjiPipe *self) {
#ifdef AJI_PIPE__WINDOWS
    CloseHandle(self->handles[AJI_PIPE__READ]);
    self->handles[AJI_PIPE__READ] = NULL;
    CloseHandle(self->handles[AJI_PIPE__WRITE]);
    self->handles[AJI_PIPE__WRITE] = NULL;
#else
    close(self->fds[AJI_PIPE__READ]);
    self->fds[AJI_PIPE__READ] = -1;
    close(self->fds[AJI_PIPE__WRITE]);
    self->fds[AJI_PIPE__WRITE] = -1;
#endif
    return self;
}

AjiPipe *
AjiPipe_Open(AjiPipe *self, int flags) {
    if (!AjiPipe_Close(self)) {
        return NULL;
    }

#ifdef AJI_PIPE__WINDOWS
    HANDLE hs[2];
    if (!CreatePipe(&hs[AJI_PIPE__READ], &hs[AJI_PIPE__WRITE], NULL, 0)) {
        return NULL;
    }

    if (!DuplicateHandle(
        GetCurrentProcess(),
        hs[AJI_PIPE__READ],
        GetCurrentProcess(),
        &self->handles[AJI_PIPE__READ],
        0,
        TRUE,
        DUPLICATE_SAME_ACCESS
    )) {
        return NULL;
    }
    CloseHandle(hs[AJI_PIPE__READ]);

    if (!DuplicateHandle(
        GetCurrentProcess(),
        hs[AJI_PIPE__WRITE],
        GetCurrentProcess(),
        &self->handles[AJI_PIPE__WRITE],
        0,
        TRUE,
        DUPLICATE_SAME_ACCESS
    )) {
        CloseHandle(self->handles[AJI_PIPE__READ]);
        return NULL;
    }
    CloseHandle(hs[AJI_PIPE__WRITE]);
#else
    if (pipe(self->fds) == -1) {
        return NULL;
    }
#endif
    return self;
}
