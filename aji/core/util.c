/**
 * Aji
 *
 * License: MIT
 *   Since: 2016
 */
#include <aji/core/util.h>

void
Aji_FreeArgv(int argc, char *argv[]) {
    if (argv) {
        for (int i = 0; i < argc; ++i) {
            free(argv[i]);
        }
        free(argv);
    }
}

void
Aji_ShowArgv(int argc, char *argv[]) {
    for (int i = 0; i < argc; ++i) {
        printf("argv[%d] = [%s]\n", i, argv[i]);
    }
}

int
Aji_RandRange(int min, int max) {
    return min + (int)(rand() * (max - min + 1.0) / (1.0 + RAND_MAX));
}

int
Aji_SafeSystem(const char *cmdline, int option) {
    if (option & AJI_SAFESYSTEM__UNSAFE) {
        return system(cmdline);
    }

#ifdef AJI__WINDOWS
    int flag = 0;
    if (option & AJI_SAFESYSTEM__EDIT) {
        // option for edit command
        flag = CREATE_NEW_CONSOLE;
    } else {
        flag = 0;
    }

    PROCESS_INFORMATION pi = {0};
    STARTUPINFO si = { sizeof(STARTUPINFO) };

    // Start the child process.
    if (!CreateProcess(NULL, // No module name (use command line)
        (char *) cmdline, // Command line
        NULL, // Process handle not inheritable
        NULL, // Thread handle not inheritable
        FALSE, // Set handle inheritance to FALSE
        flag, // No creation flags
        NULL, // Use parent's environment block
        NULL, // Use parent's starting directory
        &si, // Pointer to STARTUPINFO structure
        &pi) // Pointer to PROCESS_INFORMATION structure
    ) {
        AjiErr_Err("failed to create sub process");
        return 1;
    }

    if (option & AJI_SAFESYSTEM__EDIT) {
        // case of edit command, to not wait exit of child process
        return 0;
    }

    if (option & AJI_SAFESYSTEM__DETACH) {
        // not wait child process
        return 0;
    }

    // success to fork
    HANDLE child_process = pi.hProcess;
    if (!CloseHandle(pi.hThread)) {
        AjiErr_Err("failed to close handle");
        return 1;
    }

    // wait for child process
    DWORD r = WaitForSingleObject(child_process, INFINITE);
    switch(r) {
    case WAIT_FAILED:
        AjiErr_Err("child process was failed");
        return 1;
    case WAIT_ABANDONED:
        AjiErr_Err("child process was abandoned");
        return 1;
    case WAIT_OBJECT_0: // success
        break;
    case WAIT_TIMEOUT:
        AjiErr_Err("child process was timeout");
        return 1;
    default:
        return 1;
    }

    // get exit code of child process
    DWORD exit_code;
    if (!GetExitCodeProcess(child_process, &exit_code)) {
        AjiErr_Err("failed to get exit code of child process");
        return 1;
    }

    return exit_code;

#else
    if (option & AJI_SAFESYSTEM__UNSAFE_UNIX_ONLY) {
        return system(cmdline);
    }

    AjiCL *cl = AjiCL_New();
    if (!AjiCL_ParseStrOpts(cl, cmdline, 0)) {
        AjiErr_Err("failed to parse command line \"%s\"", cmdline);
        AjiCL_Del(cl);
        return -1;
    }

    int argc = AjiCL_Len(cl);
    char **argv = AjiCL_EscDel(cl);
    if (!argv) {
        AjiErr_Err("failed to escape and delete of clk");
        return -1;
    }

    int status = 0;

    switch (fork()) {
    default: // parent
        Aji_FreeArgv(argc, argv);
        wait(&status);
        break;
    case 0: // child
        if (execv(argv[0], argv) == -1) {
            AjiErr_Err("failed to Aji_SafeSystem");
            Aji_FreeArgv(argc, argv);
            _exit(1);
        }
        break;
    case -1: // error
        AjiErr_Err("failed to fork");
        return -1;
        break;
    }

    return status;
#endif
}

AjiCStrVec *
Aji_ArgsByOptind(int argc, char *argv[], int optind) {
    AjiCStrVec *args = AjiCStrVec_New();

    // DO NOT DELETE FOR DEBUG.
    //
    // printf("argc[%d] optind[%d]\n", argc, optind);
    // for (int i = 0; i < argc; ++i) {
    // 	printf("%d %s\n", i, argv[i]);
    // }

    AjiCStrVec_Push(args, argv[0]);
    for (int i = optind; i < argc; ++i) {
        AjiCStrVec_Push(args, argv[i]);
    }

    return args;
}

char *
Aji_TrimFirstLine(char *dst, int32_t dstsz, const char *text) {
    if (!dst || !dstsz || !text) {
        return NULL;
    }

    char *dp = dst;
    const char *dend = dst + dstsz - 1; // -1 for final nil

    for (const char *p = text; *p && dp < dend; ++p) {
        if (*p == '\r' && *(p+1) == '\n') {
            break;
        } else if (*p == '\r') {
            break;
        } else if (*p == '\n') {
            break;
        } else {
            *dp++ = *p;
        }
    }

    *dp = '\0';
    return dst;
}

void
Aji_ClearScreen(void) {
#ifdef AJI__WINDOWS
    system("cls");
#else
    system("clear");
#endif
}

static char *
read_path_var_from_resource(const AjiConfig *config, const char *rcpath) {
    char *src = AjiFile_ReadCopyFromPath(rcpath);

    AjiTkr *tkr = AjiTkr_New(AjiTkrOpt_New());
    AjiAST *ast = AjiAST_New(config);
    AjiGC *gc = AjiGC_New();
    AjiOpts *opts = AjiOpts_New();
    AjiLexEnv *lex_env = AjiLexEnv_New(AJI_LEX_ENV_TYPE__ROOT, gc, NULL);

    AjiTkr_Parse(tkr, src);
    free(src);
    src = NULL;
    if (AjiTkr_HasErrStack(tkr)) {
        AjiErr_Err("%s", AjiTkr_GetcFirstErrMsg(tkr));
        return NULL;
    }

    AjiAST_Clear(ast);
    AjiAST_MoveOpts(ast, opts);
    opts = NULL;

    AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
    if (AjiAST_HasErrs(ast)) {
        AjiErr_Err("%s", AjiAST_GetcFirstErrMsg(ast));
        return NULL;
    }

    AjiTrv_Trav(ast, lex_env);
    if (AjiAST_HasErrs(ast)) {
        AjiErr_Err("%s", AjiAST_GetcFirstErrMsg(ast));
        return NULL;
    }

    AjiTkr_Del(tkr);
    AjiAST_Del(ast);

    AjiObjDict *varmap = AjiLexEnv_GetVarmapAtHeadScope(lex_env);
    const AjiObjDictItem *item = AjiObjDict_Getc(varmap, "PATH");
    if (!item) {
        AjiGC_Del(gc);
        return NULL;
    }

    AjiLexEnv_PopNewlineOfStdoutBuf(lex_env);
    printf("%s", AjiLexEnv_GetcStdoutBufSuperGlobal(lex_env));
    fflush(stdout);

    const char *s = AjiUni_GetcMB(&item->value->real_obj.unicode);
    char *path = AjiCStr_Dup(s);
    if (!path) {
        AjiGC_Del(gc);
        return NULL;        
    }

    AjiGC_Del(gc);

    return path;
}

AjiCStrVec *
Aji_SplitToVec(const char *str, int ch) {
    if (!str) {
        return NULL;
    }

    AjiCStrVec *vec = AjiCStrVec_New();
    AjiStr *s = AjiStr_New();

    for (const char *p = str; *p; ++p) {
        if (*p == ch) {
            if (AjiStr_Len(s)) {
                AjiCStrVec_PushBack(vec, AjiStr_Getc(s));
                AjiStr_Clear(s);
            }
        } else {
            AjiStr_PushBack(s, *p);
        }
    }

    if (AjiStr_Len(s)) {
        AjiCStrVec_PushBack(vec, AjiStr_Getc(s));
    }

    AjiStr_Del(s);
    return vec;
}

static AjiCStrVec *
split_path_var(const char *path) {
    return Aji_SplitToVec(path, ':');
}

AjiCStrVec *
Aji_PushFrontArgv(int argc, char *argv[], const char *front) {
    AjiCStrVec *vec = AjiCStrVec_New();

    AjiCStrVec_PushBack(vec, front);

    for (int32_t i = 0; i < argc; ++i) {
        AjiCStrVec_PushBack(vec, argv[i]);
    }

    return AjiMem_Move(vec);
}

bool
Aji_IsDotFile(const char *path) {
    return strcmp(path, "..") == 0 || strcmp(path, ".") == 0;
}

char *
Aji_PopTailSlash(char *path) {
    int32_t pathlen = strlen(path);
#ifdef AJI__WINDOWS
    if (pathlen == 3 && path[2] == '\\') {
        return path;
    } else {
        return AjiPath_PopTailSlash(path);
    }
#else
    if (pathlen == 1 && path[0] == '/') {
        return path;
    } else {
        return AjiPath_PopTailSlash(path);
    }
#endif
}

AjiStr *
Aji_Escape(AjiStr *dst, const char **p, const char *ignore) {
    if (!dst || !p) {
        return NULL;
    }

    switch (**p) {
    default:
        AjiStr_PushBack(dst, **p);
        break;
    case '\a':
        if (strchr(ignore, **p)) {
            AjiStr_PushBack(dst, **p);
        } else {
            AjiStr_PushBack(dst, '\\');
            AjiStr_PushBack(dst, 'a');            
        }
        break;
    case '\b':
        if (strchr(ignore, **p)) {
            AjiStr_PushBack(dst, **p);
        } else {
            AjiStr_PushBack(dst, '\\');
            AjiStr_PushBack(dst, 'b');
        }
        break;
    case '\n':
        if (strchr(ignore, **p)) {
            AjiStr_PushBack(dst, **p);
        } else {
            AjiStr_PushBack(dst, '\\');
            AjiStr_PushBack(dst, 'n');
        }
        break;
    case '\r':
        if (strchr(ignore, **p)) {
            AjiStr_PushBack(dst, **p);
        } else {
            AjiStr_PushBack(dst, '\\');
            AjiStr_PushBack(dst, 'r');
        }
        break;
    case '\f':
        if (strchr(ignore, **p)) {
            AjiStr_PushBack(dst, **p);
        } else {
            AjiStr_PushBack(dst, '\\');
            AjiStr_PushBack(dst, 'f');
        }
        break;
    case '\t':
        if (strchr(ignore, **p)) {
            AjiStr_PushBack(dst, **p);
        } else {
            AjiStr_PushBack(dst, '\\');
            AjiStr_PushBack(dst, 't');
        }
        break;
    case '\v':
        if (strchr(ignore, **p)) {
            AjiStr_PushBack(dst, **p);
        } else {
            AjiStr_PushBack(dst, '\\');
            AjiStr_PushBack(dst, 'v');
        }
        break;
    case '\\':
        if (strchr(ignore, **p)) {
            AjiStr_PushBack(dst, **p);
        } else {
            AjiStr_PushBack(dst, '\\');
            AjiStr_PushBack(dst, '\\');
        }
        break;
    case '\?':
        if (strchr(ignore, **p)) {
            AjiStr_PushBack(dst, **p);
        } else {
            AjiStr_PushBack(dst, '\\');
            AjiStr_PushBack(dst, '?');
        }
        break;
    case '\0':
        if (strchr(ignore, **p)) {
            AjiStr_PushBack(dst, **p);
        } else {
            AjiStr_PushBack(dst, '\\');
            AjiStr_PushBack(dst, '0');
        }
        break;    
    case '\'':
        if (strchr(ignore, **p)) {
            AjiStr_PushBack(dst, **p);
        } else {
            AjiStr_PushBack(dst, '\\');
            AjiStr_PushBack(dst, '\'');
        }
        break;
    case '\"':
        if (strchr(ignore, **p)) {
            AjiStr_PushBack(dst, **p);
        } else {
            AjiStr_PushBack(dst, '\\');
            AjiStr_PushBack(dst, '"');
        }
        break;
    }

    return dst;
}

AjiStr *
Aji_EscapeText(AjiStr *dst, const char *s, const char *ignore) {
    if (!dst || !s) {
        return NULL;
    }

    for (const char *p = s; *p; p += 1) {
        Aji_Escape(dst, &p, ignore);
    }

    return dst;
}

void
Aji_Unescape(AjiStr *dst, const char **p, const char *ignore) {
    if (**p != '\\') {
        return;
    }

    *p += 1;

    switch (**p) {
    default:
        AjiStr_PushBack(dst, '\\');
        AjiStr_PushBack(dst, **p);
        break;
    case 'a':
        if (!ignore || !strchr(ignore, 'a')) {
            AjiStr_PushBack(dst, '\a');
        } else {
            AjiStr_PushBack(dst, '\\');
            AjiStr_PushBack(dst, **p);            
        }
        break;
    case 'b':
        if (!ignore || !strchr(ignore, 'b')) {
            AjiStr_PushBack(dst, '\b');
        } else {
            AjiStr_PushBack(dst, '\\');
            AjiStr_PushBack(dst, **p);            
        }
        break;
    case 'n':
        if (!ignore || !strchr(ignore, 'n')) {
            AjiStr_PushBack(dst, '\n');
        } else {
            AjiStr_PushBack(dst, '\\');
            AjiStr_PushBack(dst, **p);            
        }
        break;
    case 'r':
        if (!ignore || !strchr(ignore, 'r')) {
            AjiStr_PushBack(dst, '\r');
        } else {
            AjiStr_PushBack(dst, '\\');
            AjiStr_PushBack(dst, **p);            
        }
        break;
    case 'f':
        if (!ignore || !strchr(ignore, 'f')) {
            AjiStr_PushBack(dst, '\f');
        } else {
            AjiStr_PushBack(dst, '\\');
            AjiStr_PushBack(dst, **p);            
        }
        break;
    case 't':
        if (!ignore || !strchr(ignore, 't')) {
            AjiStr_PushBack(dst, '\t');
        } else {
            AjiStr_PushBack(dst, '\\');
            AjiStr_PushBack(dst, **p);            
        }
        break;
    case 'v':
        if (!ignore || !strchr(ignore, 'v')) {
            AjiStr_PushBack(dst, '\v');
        } else {
            AjiStr_PushBack(dst, '\\');
            AjiStr_PushBack(dst, **p);            
        }
        break;
    case '\\':
        if (!ignore || !strchr(ignore, '\\')) {
            AjiStr_PushBack(dst, '\\');
        } else {
            AjiStr_PushBack(dst, '\\');
            AjiStr_PushBack(dst, **p);            
        }
        break;
    case '?':
        if (!ignore || !strchr(ignore, '?')) {
            AjiStr_PushBack(dst, '\?');
        } else {
            AjiStr_PushBack(dst, '\\');
            AjiStr_PushBack(dst, **p);            
        }
        break;
    case '0':
        if (!ignore || !strchr(ignore, '0')) {
            AjiStr_PushBack(dst, '\0');
        } else {
            AjiStr_PushBack(dst, '\\');
            AjiStr_PushBack(dst, **p);            
        }
        break;
    case '\'':
        if (!ignore || !strchr(ignore, '\'')) {
            AjiStr_PushBack(dst, '\'');
        } else {
            AjiStr_PushBack(dst, '\\');
            AjiStr_PushBack(dst, **p);            
        }
        break;
    case '"':
        if (!ignore || !strchr(ignore, '"')) {
            AjiStr_PushBack(dst, '"');
        } else {
            AjiStr_PushBack(dst, '\\');
            AjiStr_PushBack(dst, **p);            
        }
        break;
    }
}

void
Aji_UnescapeText(AjiStr *dst, const char *s, const char *ignore) {
    if (!dst || !s) {
        return;
    }

    for (const char *p = s; *p; p += 1) {
        if (*p == '\\') {
            Aji_Unescape(dst, &p, ignore);
        } else {
            AjiStr_PushBack(dst, *p);
        }
    }
}
