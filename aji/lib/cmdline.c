#include <aji/lib/cmdline.h>

/*****************
* AjiCmdlineObj *
*****************/

void
AjiCmdlineObj_Del(AjiCmdlineObj *self) {
    if (!self) {
        return;
    }

    AjiStr_Del(self->command);
    AjiCL_Del(self->cl);
    free(self);
}

AjiCmdlineObj *
AjiCmdlineObj_New(AjiCmdlineObjType type) {
    AjiCmdlineObj *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->type = type;
    self->command = AjiStr_New();
    if (!self->command) {
        AjiCmdlineObj_Del(self);
        return NULL;
    }

    self->cl = AjiCL_New();
    if (!self->cl) {
        AjiCmdlineObj_Del(self);
        return NULL;
    }

    return self;
}

AjiCmdlineObj *
AjiCmdlineObj_Parse(AjiCmdlineObj *self, const char *line) {
    if (!AjiCL_ParseStr(self->cl, line)) {
        return NULL;
    }

    return self;
}

void
AjiCmdlineObj_Show(const AjiCmdlineObj *self, FILE *fout) {
    if (!self || !fout) {
        return;
    }

    switch (self->type) {
    default: break;
    case AJI_CMDLINE_OBJ_TYPE__CMD:
        printf("%s\n", AjiStr_Getc(self->command));
        break;
    }
}

/**********
* cmdline *
**********/

enum {
    CMDLINE_OBJS_SIZE = 4,
};

struct AjiCmdline {
    AjiCmdlineObj **objs;
    int32_t capa;
    int32_t len;
    char what[1024];
};

void
AjiCmdline_Del(AjiCmdline *self) {
    if (!self) {
        return;
    }

    for (int32_t i = 0; i < self->len; ++i) {
        AjiCmdlineObj *obj = self->objs[i];
        AjiCmdlineObj_Del(obj);
    }

    free(self->objs);
    free(self);
}

AjiCmdline *
AjiCmdline_New(void) {
    AjiCmdline *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    int32_t size = sizeof(AjiCmdlineObj *);
    self->capa = CMDLINE_OBJS_SIZE;
    self->objs = AjiMem_Calloc(self->capa + 1, size);
    if (!self->objs) {
        AjiCmdline_Del(self);
        return NULL;
    } 

    return self;
}

AjiCmdline *
AjiCmdline_Resize(AjiCmdline *self, int32_t capa) {
    if (!self || capa <= 0) {
        return NULL;
    }

    if (capa < self->len) {
        int32_t dif = self->len - capa;
        for (int32_t i = dif; i < self->len; ++i) {
            AjiCmdlineObj_Del(self->objs[i]);
            self->objs[i] = NULL;
        }
    }

    int32_t objsize = sizeof(AjiCmdlineObj *);
    int32_t size = objsize * capa + objsize;
    AjiCmdlineObj **tmp = AjiMem_Realloc(self->objs, size);
    if (!tmp) {
        return NULL;
    }
    self->objs = tmp;
    self->capa = capa;

    return self;
}

AjiCmdline *
AjiCmdline_MoveBack(AjiCmdline *self, AjiCmdlineObj *move_obj) {
    if (!self || !move_obj) {
        return NULL;
    }

    if (self->len >= self->capa) {
        if (!AjiCmdline_Resize(self, self->capa*2)) {
            return NULL;
        }
    }

    self->objs[self->len++] = AjiMem_Move(move_obj);

    return self;
}

bool
AjiCmdline_HasErr(const AjiCmdline *self) {
    if (!self) {
        return true;
    }

    return self->what[0] != '\0';
}

int32_t
AjiCmdline_Len(const AjiCmdline *self) {
    if (!self) {
        return -1;
    }

    return self->len;
}

const AjiCmdlineObj *
AjiCmdline_Getc(const AjiCmdline *self, int32_t index) {
    if (!self || index < 0 || index >= self->len) {
        return NULL;
    }

    return self->objs[index];
}

void
AjiCmdline_Clear(AjiCmdline *self) {
    if (!self) {
        return;
    }

    for (int32_t i = 0; i < self->len; ++i) {
        AjiCmdlineObj *obj = self->objs[i];
        AjiCmdlineObj_Del(obj);
        self->objs[i] = NULL;
    }

    self->len = 0;
    self->what[0] = '\0';
}

AjiCmdline *
AjiCmdline_Parse(AjiCmdline *self, const char *line) {
    if (!self || !line) {
        return NULL;
    }

    AjiStr *buf = AjiStr_New();
    int32_t m = 0;
    AjiCmdline_Clear(self);

    for (const char *p = line; *p; ++p) {
        switch (m) {
        case 0: // first
            if (*p == '\\') {
                ++p;
                if (*p != '\0') {
                    AjiStr_PushBack(buf, '\\');
                    AjiStr_PushBack(buf, *p);
                } else {
                    --p;
                }
            } else if (*p == '"') {
                AjiStr_PushBack(buf, *p);
                m = 10;
            } else if (*p == '|') {
                if (!AjiStr_Len(buf)) {
                    snprintf(self->what, sizeof self->what, "invalid command line");
                    AjiStr_Del(buf);
                    return NULL;
                }

                // move back cmd object
                AjiCmdlineObj *obj = AjiCmdlineObj_New(AJI_CMDLINE_OBJ_TYPE__CMD);
                if (!AjiCmdlineObj_Parse(obj, AjiStr_Getc(buf))) {
                    snprintf(self->what, sizeof self->what, "failed to parse mini command line");
                    AjiStr_Del(buf);
                    return NULL;
                }

                AjiStr *copied = AjiStr_Strip(buf, " ");
                AjiStr_Del(buf);
                buf = copied;

                AjiStr_Del(obj->command);
                obj->command = AjiMem_Move(buf);
                buf = AjiStr_New();

                if (!AjiCmdline_MoveBack(self, obj)) {
                    snprintf(self->what, sizeof self->what, "failed to move back command object");
                    AjiStr_Del(buf);
                    return NULL;
                }
                obj = NULL;

                // move back pipe object
                obj = AjiCmdlineObj_New(AJI_CMDLINE_OBJ_TYPE__PIPE);

                if (!AjiCmdline_MoveBack(self, obj)) {
                    snprintf(self->what, sizeof self->what, "failed to move back PIPE object");
                    AjiStr_Del(buf);
                    return NULL;
                }
            } else if (*p == '&' && *(p+1) == '&') {
                ++p;

                if (!AjiStr_Len(buf)) {
                    snprintf(self->what, sizeof self->what, "invalid command line (2)");
                    AjiStr_Del(buf);
                    return NULL;
                }

                // move back cmd object
                AjiCmdlineObj *obj = AjiCmdlineObj_New(AJI_CMDLINE_OBJ_TYPE__CMD);
                if (!AjiCmdlineObj_Parse(obj, AjiStr_Getc(buf))) {
                    snprintf(self->what, sizeof self->what, "failed to parse mini command line");
                    AjiStr_Del(buf);
                    return NULL;
                }

                AjiStr *copied = AjiStr_Strip(buf, " ");
                AjiStr_Del(buf);
                buf = copied;

                AjiStr_Del(obj->command);
                obj->command = buf;
                buf = AjiStr_New();

                if (!AjiCmdline_MoveBack(self, obj)) {
                    snprintf(self->what, sizeof self->what, "failed to move back command object");
                    AjiStr_Del(buf);
                    return NULL;
                }
                obj = NULL;

                // move back and object
                obj = AjiCmdlineObj_New(AJI_CMDLINE_OBJ_TYPE__AND);

                if (!AjiCmdline_MoveBack(self, obj)) {
                    snprintf(self->what, sizeof self->what, "failed to move back AND object");
                    AjiStr_Del(buf);
                    return NULL;
                }
            } else if (*p == '>') {
                if (!AjiStr_Len(buf)) {
                    snprintf(self->what, sizeof self->what, "invalid command line (3)");
                    AjiStr_Del(buf);
                    return NULL;
                }

                // move back cmd object
                AjiCmdlineObj *obj = AjiCmdlineObj_New(AJI_CMDLINE_OBJ_TYPE__CMD);
                if (!AjiCmdlineObj_Parse(obj, AjiStr_Getc(buf))) {
                    snprintf(self->what, sizeof self->what, "failed to parse mini command line");
                    AjiStr_Del(buf);
                    return NULL;
                }

                AjiStr *copied = AjiStr_Strip(buf, " ");
                AjiStr_Del(buf);
                buf = copied;

                AjiStr_Del(obj->command);
                obj->command = buf;
                buf = AjiStr_New();

                if (!AjiCmdline_MoveBack(self, obj)) {
                    snprintf(self->what, sizeof self->what, "failed to move back command object");
                    AjiStr_Del(buf);
                    return NULL;
                }
                obj = NULL;

                // move back and object
                obj = AjiCmdlineObj_New(AJI_CMDLINE_OBJ_TYPE__REDIRECT);

                if (!AjiCmdline_MoveBack(self, obj)) {
                    snprintf(self->what, sizeof self->what, "failed to move back REDIRECT object");
                    AjiStr_Del(buf);
                    return NULL;
                }
            } else {
                AjiStr_PushBack(buf, *p);
            }
            break;
        case 10: // found "
            if (*p == '\\') {
                ++p;
                if (*p != '\0') {
                    AjiStr_PushBack(buf, '\\');
                    AjiStr_PushBack(buf, *p);
                } else {
                    --p;
                }
            } else if (*p == '"') {
                AjiStr_PushBack(buf, *p);
                m = 0;
            } else {
                AjiStr_PushBack(buf, *p);
            }
            break;
        }
    }

    if (AjiStr_Len(buf)) {
        // move back cmd object
        AjiCmdlineObj *obj = AjiCmdlineObj_New(AJI_CMDLINE_OBJ_TYPE__CMD);
        if (!AjiCmdlineObj_Parse(obj, AjiStr_Getc(buf))) {
            snprintf(self->what, sizeof self->what, "failed to parse mini command line (2)");
            AjiCmdlineObj_Del(obj);
            AjiStr_Del(buf);
            return NULL;
        }

        AjiStr *copied = AjiStr_Strip(buf, " ");
        AjiStr_Del(buf);
        buf = copied;
        
        AjiStr_Del(obj->command);
        obj->command = AjiMem_Move(buf);
        buf = NULL;

        if (!AjiCmdline_MoveBack(self, obj)) {
            snprintf(self->what, sizeof self->what, "failed to move back command object (2)");
            AjiCmdlineObj_Del(obj);
            AjiStr_Del(buf);
            return NULL;
        }
    }

    AjiStr_Del(buf);
    return self;
}

void
AjiCmdline_Show(AjiCmdline *self, FILE *fout) {
    for (int32_t i = 0; i < self->len; i += 1) {
        AjiCmdlineObj *o = self->objs[i];
        AjiCmdlineObj_Show(o, fout);
    }
}
