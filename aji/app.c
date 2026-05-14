/**
 * Aji
 *
 * license: MIT
 *  author: noname
 *   since: 2016
 */
#include <aji/app.h>

/**
 * program option
 */
struct AjiAppOpts {
    bool is_help;
    bool is_version;
    bool is_debug;
};

/**
 * application structure
 */
typedef struct {
    int argc;
    char **argv;
    int cmd_argc;
    char **cmd_argv;
    AjiConfig *config;
    struct AjiAppOpts opts;
    AjiErrStack *errstack;
    AjiKit *kit;
} AjiApp;

static int
AjiApp_Run(AjiApp *self, int argc, char *argv[]);

/**
 * parse options
 *
 * @param[in] self
 * @return success to true
 * @return failed to false
 */
static bool
AjiApp_ParseOpts(AjiApp *self) {
    static struct option longopts[] = {
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 'V'},
        {"debug", no_argument, 0, 'd'},
        {0},
    };

    // init status
    self->opts = (struct AjiAppOpts){0};
    optind = 0;
    opterr = 0;

    // parse options
    for (;;) {
        int optsindex;
        int cur = getopt_long(self->argc, self->argv, "hVd", longopts, &optsindex);
        if (cur == -1) {
            break;
        }

        switch (cur) {
        case 'h': self->opts.is_help = true; break;
        case 'V': self->opts.is_version = true; break;
        case 'd': self->opts.is_debug = true; break;
        case '?':
        default:
            Aji_PushErr(AJI_EXC__RUNTIME_ERR, "invalid option");
            return false; break;
        }
    }

    if (self->argc < optind) {
        return false;
    }

    return true;
}

/**
 * destruct module
 *
 * @param[in] self
 */
static void
AjiApp_Del(AjiApp *self) {
    if (self) {
        // Global_Destroy() のあとに Kit_Del() を行う。
        // KitはGCをdeleteする。GlobalはLexEnvのオブジェクトをdeleteする。
        // GCを先にdeleteするとダブルフリーなどが起こる。
        // これはGCが参照カウントを考慮せずにdeleteするためである。
        AjiGlobal_Destroy();
        AjiKit_Del(self->kit);

        AjiConfig_Del(self->config);
        AjiErrStack_Del(self->errstack);
        Aji_FreeArgv(self->argc, self->argv);
        Aji_FreeArgv(self->cmd_argc, self->cmd_argv);
        free(self);
    }
}

/**
 * deploy Aji's environment at user's file system
 *
 * @param[in] self
 * @return success to true
 * @return failed to false
 */
static bool
AjiApp_DeployEnv(const AjiApp *self) {
    char userhome[AJI_FILE__NPATH];
    if (!AjiFile_GetUserHome(userhome, sizeof userhome)) {
        Aji_PushErr(AJI_EXC__RUNTIME_ERR,
            "failed to get user's home directory. what is your file system?");
        return false;
    }

    // make application directory
    char appdir[AJI_FILE__NPATH];
    if (!AjiFile_SolveFmt(appdir, sizeof appdir, "%s/.aji", userhome)) {
        Aji_PushErr(AJI_EXC__RUNTIME_ERR,
            "faield to create application directory path");
        return false;
    }

    if (!AjiFile_IsExists(appdir)) {
        if (AjiFile_MkdirQ(appdir) != 0) {
            Aji_PushErr(AJI_EXC__RUNTIME_ERR,
                "failed to make application directory");
            return false;
        }
    }

    return true;
}

/**
 * construct module
 *
 * @param[in] argc
 * @param[in] argv
 *
 * @return success to pointer to dynamic allocate memory to AjiApp
 * @return failed to NULL
 */
static AjiApp *
AjiApp_New(void) {
    AjiApp *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->errstack = AjiErrStack_New();
    if (!self->errstack) {
        AjiApp_Del(self);
        return NULL;
    }

    self->config = AjiConfig_New();
    if (!self->config) {
        AjiApp_Del(self);
        return NULL;
    }

    return self;
}

/**
 * show usage of module
 *
 * @param[in] app
 */
static void
AjiApp_Usage(AjiApp *app) {
    static const char usage[] =
        "Aji is programming language.\n"
        "\n"
        "Usage:\n"
        "\n"
        "    aji [options]... [file] [arguments]...\n"
        "\n"
        "The options are:\n"
        "\n"
        "    -h, --help       show usage\n"
        "    -V, --version    show version\n"
        "    -d, --debug      debug mode\n"
        "\n"
    ;
    fprintf(stderr,
        "%s"
    , usage);
}

/**
 * show version of module
 *
 * @param[in] self
 */
static void
AjiApp_Version(AjiApp *self) {
    fflush(stdout);
    fflush(stderr);
    printf("%s\n", AJI__VERSION);
    fflush(stdout);
}

static bool
AjiApp_ParseArgs(AjiApp *self, int argc, char *argv[]) {
    AjiDistriArgs dargs = {0};
    AjiDistriArgs_Distribute(&dargs, argc, argv);
    self->argc = dargs.argc;
    self->argv = dargs.argv;
    self->cmd_argc = dargs.cmd_argc;
    self->cmd_argv = dargs.cmd_argv;
    return true;
}

static bool
AjiApp_Init(AjiApp *self, int argc, char *argv[]) {
    if (!AjiConfig_Init(self->config)) {
        Aji_PushErr(AJI_EXC__RUNTIME_ERR, "failed to configuration");
        return false;
    }

    if (!AjiApp_ParseArgs(self, argc, argv)) {
        Aji_PushErr(AJI_EXC__RUNTIME_ERR, "failed to parse arguments");
        return false;
    }

    if (!AjiApp_ParseOpts(self)) {
        Aji_PushErr(AJI_EXC__RUNTIME_ERR, "failed to parse options");
        return false;
    }

    if (!AjiApp_DeployEnv(self)) {
        Aji_PushErr(AJI_EXC__RUNTIME_ERR,
            "failed to deploy environment at file system");
        return false;
    }

    if (!AjiGlobal_Init(self->config)) {
        Aji_PushErr(AJI_EXC__RUNTIME_ERR, "failed to init global objects");
        return false;
    }

    return true;
}

extern int aji_exit_code;

typedef struct {
    int exit_code;
    bool is_exit_error;
}  AjiTraceKitResult;

static AjiTraceKitResult
AjiApp_TraceKit(const AjiApp *self, const AjiKit *kit, FILE *fout) {
    AjiTraceKitResult result = {0};

    const AjiErrStack *es = AjiKit_GetcErrStack(kit);
    const AjiErrElem *elem = AjiErrStack_GetcFirst(es);
    if (elem->exc == AJI_EXC__EXIT_ERR) {
        result.exit_code = aji_exit_code;
        result.is_exit_error = true;
        return result;
    }
    
    if (self->opts.is_debug) {
        AjiKit_TraceErrDebug(kit, fout);
    } else {
        AjiKit_TraceErr(kit, fout);
    }

    return result;
}

static int
_AjiApp_Run(AjiApp *self) {
    char *content = AjiFile_ReadCopy(stdin);
    if (!content) {
        Aji_PushErr(AJI_EXC__RUNTIME_ERR, "failed to read from stdin");
        AjiGlobal_Destroy();
        return 1;
    }

    if (self->kit) {
        AjiKit_Del(self->kit);
    }

    self->kit = AjiKit_New(self->config);
    AjiLexEnv *lex_env = AjiKit_GetRexEnv(self->kit);
    AjiLexEnv_SetUseSuperGlobalBuf(lex_env, false);  // no use stdout/stderr buffer

    if (!AjiKit_CompileFromStr(self->kit, content)) {
        AjiTraceKitResult result = AjiApp_TraceKit(self, self->kit, stderr);
        if (result.is_exit_error) {
            return result.exit_code;
        }
        Aji_PushErr(AJI_EXC__RUNTIME_ERR, "failed to compile from stdin");
        return 1;
    }

    fflush(stdout);
    fflush(stderr);

    free(content);
    return 0;
}

static int
AjiApp_RunArgs(AjiApp *self) {
    int argc = self->cmd_argc;
    char **argv = self->cmd_argv;
    if (!argc) {
        Aji_PushErr(AJI_EXC__RUNTIME_ERR, "invalid arguments");
        return 1;
    }

    const char *path = argv[0];
    if (!AjiFile_IsExists(path)) {
        Aji_PushErr(AJI_EXC__RUNTIME_ERR, "not found \"%s\"", path);
        return 1;
    }

    if (self->kit) {
        AjiKit_Del(self->kit);
    }

    self->kit = AjiKit_New(self->config);
    AjiKit_SetUseBuf(self->kit, false);  // no use stdout/stderr buffer
    
    if (!AjiKit_CompileFromPathArgs(self->kit, path, argc, argv)) {
        AjiTraceKitResult result = AjiApp_TraceKit(self, self->kit, stderr);
        if (result.is_exit_error) {
            return result.exit_code;
        }
        Aji_PushErr(AJI_EXC__RUNTIME_ERR, "failed to compile \"%s\"", path);
        return 1;
    }

    fflush(stdout);
    fflush(stderr);

    return 0;
}

/**
 * run module
 *
 * @param[in] self
 *
 * @return success to 0
 * @return failed to not 0
 */
static int
AjiApp_Run(AjiApp *self, int argc, char *argv[]) {
    if (!AjiApp_Init(self, argc, argv)) {
        return 1;
    }

    if (self->opts.is_help) {
        AjiApp_Usage(self);
        return 0;
    }

    if (self->opts.is_version) {
        AjiApp_Version(self);
        return 0;
    }

    if (self->cmd_argc >= 1) {
        return AjiApp_RunArgs(self);
    }

    return _AjiApp_Run(self);
}

/**
 * stack trace
 * 
 * @param[in] *self
 */
static void
AjiApp_Trace(const AjiApp *self) {
    if (!AjiErrStack_Len(self->errstack)) {
        return;
    }

    fflush(stdout);
    AjiErrStack_TraceSimple(self->errstack, stderr);
    fflush(stderr);        
}

/**
 * main routine
 *
 * @param[in] argc
 * @param[in] argv
 *
 * @return success to 0
 * @return failed to not 0
 */
int
main(int argc, char *argv[]) {
    // set locale for unicode object (char32_t, char16_t)
    setlocale(LC_CTYPE, "");

    AjiApp *app = AjiApp_New();
    if (!app) {
        AjiErr_Die("failed to start application");
    }

    int result = AjiApp_Run(app, argc, argv);
    if (result != 0) {
        AjiApp_Trace(app);
    }

    AjiApp_Del(app);

    fflush(stdout);
    return result;
}
