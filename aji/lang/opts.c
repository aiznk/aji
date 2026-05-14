#include <aji/lang/opts.h>

struct AjiOpts {
    AjiDict *opts;
    AjiCStrVec *args;
};

void
AjiOpts_Del(AjiOpts *self) {
    if (!self) {
        return;
    }
    
    AjiDict_Del(self->opts);
    AjiCStrVec_Del(self->args);
    free(self);
}

AjiOpts *
AjiOpts_New(void) {
    AjiOpts *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->opts = AjiDict_New(100);
    if (!self->opts) {
        AjiOpts_Del(self);
        return NULL;
    }

    self->args = AjiCStrVec_New();
    if (!self->args) {
        AjiOpts_Del(self);
        return NULL;
    }

    return self;
}

AjiOpts *
AjiOpts_DeepCopy(const AjiOpts *other) {
    if (!other) {
        return NULL;
    }

    AjiOpts *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->opts = AjiDict_DeepCopy(other->opts);
    if (!self->opts) {
        AjiOpts_Del(self);
        return NULL;
    }

    self->args = AjiCStrVec_DeepCopy(other->args);
    if (!self->args) {
        AjiOpts_Del(self);
        return NULL;
    }

    return self;
}

AjiOpts *
AjiOpts_ShallowCopy(const AjiOpts *other) {
    if (!other) {
        return NULL;
    }

    AjiOpts *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->opts = AjiDict_ShallowCopy(other->opts);
    if (!self->opts) {
        AjiOpts_Del(self);
        return NULL;
    }

    self->args = AjiCStrVec_ShallowCopy(other->args);
    if (!self->args) {
        AjiOpts_Del(self);
        return NULL;
    }

    return self;    
}

void
AjiOpts_Clear(AjiOpts *self) {
    if (!self) {
        return;
    }

    AjiDict_Clear(self->opts);
    AjiCStrVec_Clear(self->args);
}

AjiOpts *
AjiOpts_Parse(AjiOpts *self, int argc, char *argv[]) {
    if (!self) {
        return NULL;
    }
    if (argc <= 0) {
        return self;
    }

    int m = 0;
    AjiStr *key = AjiStr_New();
    if (!key) {
        return NULL;
    }

    AjiCStrVec_Clear(self->args);
    AjiCStrVec_PushBack(self->args, argv[0]);

    for (int i = 1; i < argc && argv[i]; ++i) {
        const char *arg = argv[i];
        switch (m) {
        case 0:
            if (arg[0] == '-' && arg[1] == '-') {
                AjiStr_Set(key, arg+2);
                m = 10;
            } else if (arg[0] == '-') {
                AjiStr_Set(key, arg+1);
                m = 20;
            } else {
                if (!AjiCStrVec_PushBack(self->args, arg)) {
                    return NULL;
                }
            }
            break;
        case 10: // found long option
            if (arg[0] == '-' && arg[1] == '-') {
                AjiDict_Set(self->opts, AjiStr_Getc(key), "");
                AjiStr_Set(key, arg+2);
                // keep current mode
            } else if (arg[0] == '-') {
                AjiDict_Set(self->opts, AjiStr_Getc(key), "");
                AjiStr_Set(key, arg+1);
                m = 20;
            } else {
                // store option value
                AjiDict_Set(self->opts, AjiStr_Getc(key), arg);
                AjiStr_Clear(key);
                m = 0;
            }
            break;
        case 20: // found short option
            if (arg[0] == '-' && arg[1] == '-') {
                AjiDict_Set(self->opts, AjiStr_Getc(key), "");
                AjiStr_Set(key, arg+2);
                m = 10;
            } else if (arg[0] == '-') {
                AjiDict_Set(self->opts, AjiStr_Getc(key), "");
                AjiStr_Set(key, arg+1);
                // keep current mode
            } else {
                // store option value
                AjiDict_Set(self->opts, AjiStr_Getc(key), arg);
                AjiStr_Clear(key);
                m = 0;
            }
            break;
        }
    }

    if (AjiStr_Len(key)) {
        AjiDict_Set(self->opts, AjiStr_Getc(key), "");
    }

    AjiStr_Del(key);
    return self;
}

const char *
AjiOpts_Getc(const AjiOpts *self, const char *optname) {
    if (!self || !optname) {
        return NULL;
    }

    const AjiDictItem *item = AjiDict_Getc(self->opts, optname);
    if (!item) {
        return NULL;
    }
    return item->value;
}

bool
AjiOpts_Has(const AjiOpts *self, const char *optname) {
    if (!self || !optname) {
        return NULL;
    }

    return AjiDict_HasKey(self->opts, optname);
}

const char *
AjiOpts_GetcArgs(const AjiOpts *self, int32_t idx) {
    if (!self) {
        return NULL;
    }
    if (idx < 0 || idx >= AjiCStrVec_Len(self->args)) {
        return NULL;
    }

    return AjiCStrVec_Getc(self->args, idx);
}

int32_t
AjiOpts_ArgsLen(const AjiOpts *self) {
    if (!self) {
        return -1;
    }
    return AjiCStrVec_Len(self->args);
}

void
AjiOpts_Dump(const AjiOpts *self, FILE *fout) {
    AjiDict_Dump(self->opts, fout);
    AjiCStrVec_Dump(self->args, fout);    
}
