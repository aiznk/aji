/**
 * Aji
 *
 * License: MIT
 *   Since: 2016
 */
#include <tests/tests.h>

/*********
* macros *
*********/

#define showbuf() printf("stdout[%s]\n", AjiLexEnv_GetcStdoutBufSuperGlobal(lex_env))
#define showerr() printf("stderr[%s]\n", AjiLexEnv_GetcStderrBufSuperGlobal(lex_env))
#define showdetail() printf("detail[%s]\n", AjiAST_GetcFirstErrMsg(ast))
#define trace() AjiErrStack_TraceDebug(ast->error_stack, stderr)
#define ERR AjiErrStack_Trace(ast->error_stack, stderr)
#define eq(s) assert(!strcmp(AjiLexEnv_GetcStdoutBufSuperGlobal(lex_env), s))
#define ast_debug(stmt) { \
    AjiAST_SetDebug(ast, true); \
    stmt; \
    AjiAST_SetDebug(ast, false); \
}
#define trv_ready \
    AjiConfig *config = AjiConfig_New(); \
    AjiGlobal_Init(config); \
    AjiTkrOpt *opt = AjiTkrOpt_New(); \
    AjiTkr *tkr = AjiTkr_New(AjiMem_Move(opt)); \
    AjiAST *ast = AjiAST_New(config); \
    AjiGC *gc = AjiGC_New(); \
    AjiLexEnv *lex_env = AjiLexEnv_New(AJI_LEX_ENV_TYPE__ROOT, gc, NULL); \

#define trv_cleanup \
    AjiAST_Del(ast); \
    AjiTkr_Del(tkr); \
    AjiGlobal_Destroy(); \
    AjiGC_Del(gc); \
    AjiConfig_Del(config); \

#define check_ok(code, hope) \
    { \
        trv_ready; \
        AjiTkr_Parse(tkr, code); \
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr)); \
        AjiTrv_Trav(ast, lex_env); \
        if (AjiAST_HasErrs(ast)) {\
            printf("%s\n", AjiAST_GetcLastErrMsg(ast));\
        }\
        assert(!AjiAST_HasErrs(ast)); \
        assert(!strcmp(AjiLexEnv_GetcStdoutBufSuperGlobal(lex_env), hope)); \
        trv_cleanup; \
    }
#define check_stderr(code, hope) \
    { \
        trv_ready; \
        AjiTkr_Parse(tkr, code); \
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr)); \
        AjiTrv_Trav(ast, lex_env); \
        assert(!AjiAST_HasErrs(ast)); \
        assert(!strcmp(AjiLexEnv_GetcStderrBufSuperGlobal(lex_env), hope)); \
        trv_cleanup; \
    }
#define check_ok_debug_compile(code, hope) \
    { \
        trv_ready; \
        AjiTkr_Parse(tkr, code); \
        ast_debug(AjiCC_Compile(ast, AjiTkr_GetToks(tkr))); \
        AjiTrv_Trav(ast, lex_env); \
        assert(!AjiAST_HasErrs(ast)); \
        assert(!strcmp(AjiLexEnv_GetcStdoutBufSuperGlobal(lex_env), hope)); \
        trv_cleanup; \
    }
#define check_ok_debug_traverse(code, hope) \
    { \
        trv_ready; \
        AjiTkr_Parse(tkr, code); \
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr)); \
        ast_debug(AjiTrv_Trav(ast, lex_env)); \
        assert(!AjiAST_HasErrs(ast)); \
        assert(!strcmp(AjiLexEnv_GetcStdoutBufSuperGlobal(lex_env), hope)); \
        trv_cleanup; \
    }
#define check_ok_trace(code, hope) \
    { \
        trv_ready; \
        AjiTkr_Parse(tkr, code); \
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr)); \
        AjiTrv_Trav(ast, lex_env); \
        trace(); \
        assert(!AjiAST_HasErrs(ast)); \
        assert(!strcmp(AjiLexEnv_GetcStdoutBufSuperGlobal(lex_env), hope)); \
        trv_cleanup; \
    }
#define check_ok_showbuf(code, hope) \
    { \
        trv_ready; \
        AjiTkr_Parse(tkr, code); \
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr)); \
        AjiTrv_Trav(ast, lex_env); \
        assert(!AjiAST_HasErrs(ast)); \
        showbuf(); \
        assert(!strcmp(AjiLexEnv_GetcStdoutBufSuperGlobal(lex_env), hope)); \
        trv_cleanup; \
    }
#define check_fail(code, hope) \
    { \
        trv_ready; \
        AjiTkr_Parse(tkr, code); \
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr)); \
        AjiTrv_Trav(ast, lex_env); \
        assert(AjiAST_HasErrs(ast)); \
        assert(!strcmp(AjiAST_GetcFirstErrMsg(ast), hope)); \
        trv_cleanup; \
    }
#define check_fail_showbuf(code, hope) \
    { \
        trv_ready; \
        AjiTkr_Parse(tkr, code); \
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr)); \
        AjiTrv_Trav(ast, lex_env); \
        const char *msg = AjiAST_GetcFirstErrMsg(ast); \
        if (msg) printf("%s\n", msg); \
        else printf("%p\n", msg); \
        assert(AjiAST_HasErrs(ast)); \
        assert(!strcmp(AjiAST_GetcFirstErrMsg(ast), hope)); \
        trv_cleanup; \
    }
#define check_fail_trace(code, hope) \
    { \
        trv_ready; \
        AjiTkr_Parse(tkr, code); \
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr)); \
        AjiTrv_Trav(ast, lex_env); \
        trace(); \
        assert(AjiAST_HasErrs(ast)); \
        assert(!strcmp(AjiAST_GetcFirstErrMsg(ast), hope)); \
        trv_cleanup; \
    }
#define check_fail_debug_compile(code, hope) \
    { \
        trv_ready; \
        AjiTkr_Parse(tkr, code); \
        ast_debug(AjiCC_Compile(ast, AjiTkr_GetToks(tkr))); \
        AjiTrv_Trav(ast, lex_env); \
        assert(AjiAST_HasErrs(ast)); \
        assert(!strcmp(AjiAST_GetcFirstErrMsg(ast), hope)); \
        trv_cleanup; \
    }
#define check_fail_debug_traverse(code, hope) \
    { \
        trv_ready; \
        AjiTkr_Parse(tkr, code); \
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr)); \
        ast_debug(AjiTrv_Trav(ast, lex_env)); \
        assert(AjiAST_HasErrs(ast)); \
        assert(!strcmp(AjiAST_GetcFirstErrMsg(ast), hope)); \
        trv_cleanup; \
    }

/********
* utils *
********/

/**
 * Show error message and exit from process.
 *
 * @param string fmt message format.
 * @param ...    ... format arguments.
 */
static void
die(const char *fmt, ...) {
    if (!fmt) {
        return;
    }

    size_t fmtlen = strlen(fmt);
    va_list args;
    va_start(args, fmt);

    fflush(stdout);
    fprintf(stderr, "die: ");

    if (isalpha(fmt[0])) {
        fprintf(stderr, "%c", toupper(fmt[0]));
        vfprintf(stderr, fmt+1, args);
    } else {
        vfprintf(stderr, fmt, args);
    }

    if (fmtlen && fmt[fmtlen-1] != '.') {
        fprintf(stderr, ". ");
    }

    if (errno != 0) {
        fprintf(stderr, "%s.", strerror(errno));
    }

    fprintf(stderr, "\n");

    va_end(args);
    fflush(stderr);
    exit(EXIT_FAILURE);
}

/**
 * Show error message.
 *
 * @param string fmt message format.
 * @param ...    ... format arguments.
 */
static void
warn(const char *fmt, ...) {
    if (!fmt) {
        return;
    }

    size_t fmtlen = strlen(fmt);
    va_list args;
    va_start(args, fmt);

    fflush(stdout);
    fprintf(stderr, "die: ");

    if (isalpha(fmt[0])) {
        fprintf(stderr, "%c", toupper(fmt[0]));
        vfprintf(stderr, fmt+1, args);
    } else {
        vfprintf(stderr, fmt, args);
    }

    if (fmtlen && fmt[fmtlen-1] != '.') {
        fprintf(stderr, ". ");
    }

    if (errno != 0) {
        fprintf(stderr, "%s.", strerror(errno));
    }

    fprintf(stderr, "\n");

    va_end(args);
    fflush(stderr);
}

/**
 * solve path
 * fix for valgrind issue
 *
 * @param[in] *dst
 * @param[in] dstsz
 * @param[in] *path
 *
 * @return
 */
static char *
solve_path(char *dst, int32_t dstsz, const char *path) {
    char tmp[AJI_FILE__NPATH] = {0};
    assert(AjiFile_Solve(tmp, sizeof tmp, path));
    snprintf(dst, dstsz, "%s", tmp);
    return dst;
}

/********
* tests *
********/

struct testcase {
    const char *name;
    void (*test)(void);
};

struct testmodule {
    const char *name;
    const struct testcase *tests;
};

/********
* vec *
********/

void
_freeescvec(char **vec) {
    for (char **p = vec; *p; ++p) {
        free(*p);
    }
    free(vec);
}

int
_countescvec(char **vec) {
    int i = 0;
    for (char **p = vec; *p; ++p) {
        ++i;
    }
    return i;
}

void
test_AjiCStrVec_New(void) {
    AjiCStrVec *vec = AjiCStrVec_New();
    assert(vec != NULL);
    AjiCStrVec_Del(vec);
}

void
test_AjiCStrVec_EscDel(void) {
    // test
    AjiCStrVec *vec = AjiCStrVec_New();
    assert(vec != NULL);

    assert(AjiCStrVec_EscDel(NULL) == NULL);

    char **escvec = AjiCStrVec_EscDel(vec);
    assert(escvec != NULL);

    int i;
    for (i = 0; escvec[i]; ++i) {
    }
    assert(i == 0);
    _freeescvec(escvec);

    // test
    vec = AjiCStrVec_New();
    assert(vec != NULL);

    assert(AjiCStrVec_Push(vec, "0") != NULL);
    assert(AjiCStrVec_Push(vec, "1") != NULL);
    assert(AjiCStrVec_Push(vec, "2") != NULL);

    escvec = AjiCStrVec_EscDel(vec);
    assert(_countescvec(escvec) == 3);
    assert(strcmp(escvec[0], "0") == 0);
    assert(strcmp(escvec[1], "1") == 0);
    assert(strcmp(escvec[2], "2") == 0);
    _freeescvec(escvec);
}

void
test_AjiCStrVec_Push(void) {
    AjiCStrVec *vec = AjiCStrVec_New();
    assert(vec != NULL);

    assert(AjiCStrVec_Push(NULL, "1") == NULL);
    assert(AjiCStrVec_Push(vec, NULL) == NULL);
    assert(AjiCStrVec_Push(vec, "") != NULL);
    assert(AjiCStrVec_Push(vec, "1") != NULL);

    assert(AjiCStrVec_Len(vec) == 2);

    AjiCStrVec_Del(vec);
}

void
test_AjiCStrVec_PushBack(void) {
    AjiCStrVec *vec = AjiCStrVec_New();
    assert(vec != NULL);

    assert(AjiCStrVec_PushBack(NULL, "1") == NULL);
    assert(AjiCStrVec_PushBack(vec, NULL) == NULL);
    assert(AjiCStrVec_PushBack(vec, "") != NULL);
    assert(AjiCStrVec_PushBack(vec, "1") != NULL);

    assert(AjiCStrVec_Len(vec) == 2);

    AjiCStrVec_Del(vec);
}

void
test_AjiCStrVec_ExtendBackOther(void) {
    AjiCStrVec *vec1 = AjiCStrVec_New();
    assert(vec1 != NULL);
    AjiCStrVec *vec2 = AjiCStrVec_New();
    assert(vec2 != NULL);

    AjiCStrVec_PushBack(vec1, "ccc");
    AjiCStrVec_PushBack(vec1, "ddd");

    AjiCStrVec_PushBack(vec2, "aaa");
    AjiCStrVec_PushBack(vec2, "bbb");

    AjiCStrVec_ExtendBackOther(vec2, vec1);

    assert(!strcmp(AjiCStrVec_Getc(vec2, 0), "aaa"));
    assert(!strcmp(AjiCStrVec_Getc(vec2, 1), "bbb"));
    assert(!strcmp(AjiCStrVec_Getc(vec2, 2), "ccc"));
    assert(!strcmp(AjiCStrVec_Getc(vec2, 3), "ddd"));

    assert(AjiCStrVec_Len(vec1) == 2);

    AjiCStrVec_Del(vec1);
    AjiCStrVec_Del(vec2);
}

void
test_AjiCStrVec_PopMove(void) {
    AjiCStrVec *vec = AjiCStrVec_New();
    assert(vec);

    assert(AjiCStrVec_PushBack(vec, "1"));
    assert(AjiCStrVec_PushBack(vec, "2"));
    char *p = AjiCStrVec_PopMove(vec);
    assert(p);
    assert(!strcmp(p, "2"));
    free(p);

    p = AjiCStrVec_PopMove(vec);
    assert(p);
    assert(!strcmp(p, "1"));
    free(p);

    p = AjiCStrVec_PopMove(vec);
    assert(!p);

    AjiCStrVec_Del(vec);
}

void
test_AjiCStrVec_Move(void) {
    AjiCStrVec *vec = AjiCStrVec_New();
    assert(vec != NULL);

    assert(AjiCStrVec_Move(vec, NULL) != NULL);
    assert(AjiCStrVec_Getc(vec, 0) == NULL);

    char *ptr = AjiCStr_EDup("string");
    assert(ptr != NULL);

    assert(AjiCStrVec_Move(vec, ptr) != NULL);
    assert(strcmp(AjiCStrVec_Getc(vec, 1), "string") == 0);

    AjiCStrVec_Del(vec);
}

void
test_AjiCStrVec_Sort(void) {
    AjiCStrVec *vec = AjiCStrVec_New();
    assert(vec != NULL);

    assert(AjiCStrVec_Sort(NULL) == NULL);

    assert(AjiCStrVec_Push(vec, "1") != NULL);
    assert(AjiCStrVec_Push(vec, "2") != NULL);
    assert(AjiCStrVec_Push(vec, "0") != NULL);

    assert(AjiCStrVec_Sort(vec) != NULL);
    assert(strcmp(AjiCStrVec_Getc(vec, 0), "0") == 0);
    assert(strcmp(AjiCStrVec_Getc(vec, 1), "1") == 0);
    assert(strcmp(AjiCStrVec_Getc(vec, 2), "2") == 0);

    AjiCStrVec_Del(vec);
}

void
test_AjiCStrVec_Getc(void) {
    AjiCStrVec *vec = AjiCStrVec_New();
    assert(vec != NULL);

    assert(AjiCStrVec_Getc(NULL, 0) == NULL);
    assert(AjiCStrVec_Getc(vec, 0) == NULL);
    assert(AjiCStrVec_Getc(vec, -1) == NULL);

    assert(AjiCStrVec_Push(vec, "0") != NULL);
    assert(AjiCStrVec_Push(vec, "1") != NULL);
    assert(AjiCStrVec_Push(vec, "2") != NULL);

    assert(strcmp(AjiCStrVec_Getc(vec, 0), "0") == 0);
    assert(strcmp(AjiCStrVec_Getc(vec, 1), "1") == 0);
    assert(strcmp(AjiCStrVec_Getc(vec, 2), "2") == 0);
    assert(AjiCStrVec_Getc(vec, 3) == NULL);

    AjiCStrVec_Del(vec);
}

void
test_AjiCStrVec_Len(void) {
    AjiCStrVec *vec = AjiCStrVec_New();
    assert(vec != NULL);

    assert(AjiCStrVec_Len(NULL) == 0);
    assert(AjiCStrVec_Len(vec) == 0);

    assert(AjiCStrVec_Push(vec, "0") != NULL);
    assert(AjiCStrVec_Push(vec, "1") != NULL);
    assert(AjiCStrVec_Push(vec, "2") != NULL);
    assert(AjiCStrVec_Len(vec) == 3);

    AjiCStrVec_Del(vec);
}

void
test_AjiCStrVec_Show(void) {
    AjiCStrVec *vec = AjiCStrVec_New();
    assert(vec != NULL);

    assert(AjiCStrVec_Show(NULL, stdout) == NULL);
    assert(AjiCStrVec_Show(vec, NULL) == NULL);
    assert(AjiCStrVec_Show(vec, stdout) != NULL);

    AjiCStrVec_Del(vec);
}

void
test_AjiCStrVec_Clear(void) {
    AjiCStrVec *vec = AjiCStrVec_New();
    assert(vec);

    assert(AjiCStrVec_PushBack(vec, "1"));
    assert(AjiCStrVec_PushBack(vec, "2"));
    assert(AjiCStrVec_Len(vec) == 2);
    AjiCStrVec_Clear(vec);
    assert(AjiCStrVec_Len(vec) == 0);

    AjiCStrVec_Del(vec);
}

void
test_AjiCStrVec_Resize(void) {
    AjiCStrVec *vec = AjiCStrVec_New();
    assert(AjiCStrVec_Resize(vec, 32));
    assert(AjiCStrVec_Resize(vec, 8));
    assert(AjiCStrVec_Resize(vec, 16));
    AjiCStrVec_Del(vec);
}

static const struct testcase
cstrvec_tests[] = {
    {"AjiCStrVec_New", test_AjiCStrVec_New},
    {"AjiCStrVec_EscDel", test_AjiCStrVec_EscDel},
    {"AjiCStrVec_Push", test_AjiCStrVec_Push},
    {"AjiCStrVec_PushBack", test_AjiCStrVec_PushBack},
    {"AjiCStrVec_ExtendBackOther", test_AjiCStrVec_ExtendBackOther},
    {"AjiCStrVec_PopMove", test_AjiCStrVec_PopMove},
    {"AjiCStrVec_Move", test_AjiCStrVec_Move},
    {"AjiCStrVec_Sort", test_AjiCStrVec_Sort},
    {"AjiCStrVec_Getc", test_AjiCStrVec_Getc},
    {"AjiCStrVec_Len", test_AjiCStrVec_Len},
    {"AjiCStrVec_Show", test_AjiCStrVec_Show},
    {"AjiCStrVec_Clear", test_AjiCStrVec_Clear},
    {"AjiCStrVec_Resize", test_AjiCStrVec_Resize},
    {0},
};

/**********
* cmdline *
**********/

void
test_AjiCmdline_New(void) {
    AjiCmdline *cmdline = AjiCmdline_New();
    assert(cmdline);
    AjiCmdline_Del(cmdline);
}

void
test_AjiCmdline_Del(void) {
    AjiCmdline *cmdline = AjiCmdline_New();
    assert(cmdline);
    AjiCmdline_Del(cmdline);
}

void
test_AjiCmdline_Parse_0(void) {
    AjiCmdline *cmdline = AjiCmdline_New();
    assert(cmdline);

    assert(AjiCmdline_Parse(cmdline, "abc"));

    AjiCmdline_Del(cmdline);
}

void
test_AjiCmdline_Parse_1(void) {
    AjiCmdline *cmdline = AjiCmdline_New();
    assert(cmdline);

    assert(AjiCmdline_Parse(cmdline, "abc && def"));

    AjiCmdline_Del(cmdline);
}

void
test_AjiCmdline_Parse_2(void) {
    AjiCmdline *cmdline = AjiCmdline_New();
    assert(cmdline);

    assert(AjiCmdline_Parse(cmdline, "abc | def"));

    AjiCmdline_Del(cmdline);
}

void
test_AjiCmdline_Parse_3(void) {
    AjiCmdline *cmdline = AjiCmdline_New();
    assert(cmdline);

    assert(AjiCmdline_Parse(cmdline, "abc > def"));

    AjiCmdline_Del(cmdline);
}

void
test_AjiCmdline_Parse(void) {
    AjiCmdline *cmdline = AjiCmdline_New();
    assert(cmdline);

    const AjiCmdlineObj *obj = NULL;

    assert(AjiCmdline_Parse(cmdline, "abc && def | ghi > jkl"));
    assert(AjiCmdline_Len(cmdline) == 7);
    obj = AjiCmdline_Getc(cmdline, 0);
    assert(obj);
    assert(obj->type == AJI_CMDLINE_OBJ_TYPE__CMD);
    assert(!strcmp(AjiStr_Getc(obj->command), "abc"));
    assert(AjiCL_Len(obj->cl) == 1);
    obj = AjiCmdline_Getc(cmdline, 1);
    assert(obj);
    assert(obj->type == AJI_CMDLINE_OBJ_TYPE__AND);
    obj = AjiCmdline_Getc(cmdline, 2);
    assert(obj);
    assert(obj->type == AJI_CMDLINE_OBJ_TYPE__CMD);
    assert(!strcmp(AjiStr_Getc(obj->command), "def"));
    assert(AjiCL_Len(obj->cl) == 1);
    obj = AjiCmdline_Getc(cmdline, 3);
    assert(obj);
    assert(obj->type == AJI_CMDLINE_OBJ_TYPE__PIPE);
    obj = AjiCmdline_Getc(cmdline, 4);
    assert(obj);
    assert(obj->type == AJI_CMDLINE_OBJ_TYPE__CMD);
    assert(!strcmp(AjiStr_Getc(obj->command), "ghi"));
    assert(AjiCL_Len(obj->cl) == 1);
    obj = AjiCmdline_Getc(cmdline, 5);
    assert(obj);
    assert(obj->type == AJI_CMDLINE_OBJ_TYPE__REDIRECT);
    obj = AjiCmdline_Getc(cmdline, 6);
    assert(obj);
    assert(obj->type == AJI_CMDLINE_OBJ_TYPE__CMD);
    assert(!strcmp(AjiStr_Getc(obj->command), "jkl"));
    assert(AjiCL_Len(obj->cl) == 1);

    AjiCmdline_Del(cmdline);
}

void
test_AjiCmdline_Parse_pipe(void) {
    AjiCmdline *cmdline = AjiCmdline_New();
    assert(cmdline);

    const AjiCmdlineObj *obj = NULL;

    assert(AjiCmdline_Parse(cmdline, "abc"));
    assert(AjiCmdline_Len(cmdline) == 1);
    obj = AjiCmdline_Getc(cmdline, 0);
    assert(obj);
    assert(obj->type == AJI_CMDLINE_OBJ_TYPE__CMD);
    assert(!strcmp(AjiStr_Getc(obj->command), "abc"));
    assert(AjiCL_Len(obj->cl) == 1);

    assert(AjiCmdline_Parse(cmdline, "abc | def"));
    assert(AjiCmdline_Len(cmdline) == 3);
    obj = AjiCmdline_Getc(cmdline, 0);
    assert(obj);
    assert(obj->type == AJI_CMDLINE_OBJ_TYPE__CMD);
    assert(!strcmp(AjiStr_Getc(obj->command), "abc"));
    assert(AjiCL_Len(obj->cl) == 1);
    obj = AjiCmdline_Getc(cmdline, 1);
    assert(obj);
    assert(obj->type == AJI_CMDLINE_OBJ_TYPE__PIPE);
    obj = AjiCmdline_Getc(cmdline, 2);
    assert(obj);
    assert(obj->type == AJI_CMDLINE_OBJ_TYPE__CMD);
    assert(!strcmp(AjiStr_Getc(obj->command), "def"));
    assert(AjiCL_Len(obj->cl) == 1);

    assert(AjiCmdline_Parse(cmdline, "abc -d efg | hij -d \"klm\""));
    assert(AjiCmdline_Len(cmdline) == 3);
    obj = AjiCmdline_Getc(cmdline, 0);
    assert(obj);
    assert(obj->type == AJI_CMDLINE_OBJ_TYPE__CMD);
    assert(!strcmp(AjiStr_Getc(obj->command), "abc -d efg"));
    assert(AjiCL_Len(obj->cl) == 3);
    obj = AjiCmdline_Getc(cmdline, 1);
    assert(obj);
    assert(obj->type == AJI_CMDLINE_OBJ_TYPE__PIPE);
    obj = AjiCmdline_Getc(cmdline, 2);
    assert(obj);
    assert(obj->type == AJI_CMDLINE_OBJ_TYPE__CMD);
    assert(!strcmp(AjiStr_Getc(obj->command), "hij -d \"klm\""));
    assert(AjiCL_Len(obj->cl) == 3);

    assert(AjiCmdline_Parse(cmdline, "a | b | c | d | e"));
    assert(AjiCmdline_Len(cmdline) == 9);

    AjiCmdline_Del(cmdline);
}

void
test_AjiCmdline_Parse_and(void) {
    AjiCmdline *cmdline = AjiCmdline_New();
    assert(cmdline);

    const AjiCmdlineObj *obj = NULL;

    assert(AjiCmdline_Parse(cmdline, "abc && def"));
    assert(AjiCmdline_Len(cmdline) == 3);
    obj = AjiCmdline_Getc(cmdline, 0);
    assert(obj);
    assert(obj->type == AJI_CMDLINE_OBJ_TYPE__CMD);
    assert(!strcmp(AjiStr_Getc(obj->command), "abc"));
    assert(AjiCL_Len(obj->cl) == 1);
    obj = AjiCmdline_Getc(cmdline, 1);
    assert(obj);
    assert(obj->type == AJI_CMDLINE_OBJ_TYPE__AND);
    obj = AjiCmdline_Getc(cmdline, 2);
    assert(obj);
    assert(obj->type == AJI_CMDLINE_OBJ_TYPE__CMD);
    assert(!strcmp(AjiStr_Getc(obj->command), "def"));
    assert(AjiCL_Len(obj->cl) == 1);

    assert(AjiCmdline_Parse(cmdline, "abc -d efg && hij -d \"klm\""));
    assert(AjiCmdline_Len(cmdline) == 3);
    obj = AjiCmdline_Getc(cmdline, 0);
    assert(obj);
    assert(obj->type == AJI_CMDLINE_OBJ_TYPE__CMD);
    assert(!strcmp(AjiStr_Getc(obj->command), "abc -d efg"));
    assert(AjiCL_Len(obj->cl) == 3);
    obj = AjiCmdline_Getc(cmdline, 1);
    assert(obj);
    assert(obj->type == AJI_CMDLINE_OBJ_TYPE__AND);
    obj = AjiCmdline_Getc(cmdline, 2);
    assert(obj);
    assert(obj->type == AJI_CMDLINE_OBJ_TYPE__CMD);
    assert(!strcmp(AjiStr_Getc(obj->command), "hij -d \"klm\""));
    assert(AjiCL_Len(obj->cl) == 3);

    assert(AjiCmdline_Parse(cmdline, "a && b && c && d && e"));
    assert(AjiCmdline_Len(cmdline) == 9);

    AjiCmdline_Del(cmdline);
}

void
test_AjiCmdline_Parse_redirect(void) {
    AjiCmdline *cmdline = AjiCmdline_New();
    assert(cmdline);

    const AjiCmdlineObj *obj = NULL;

    assert(AjiCmdline_Parse(cmdline, "abc > def"));
    assert(AjiCmdline_Len(cmdline) == 3);
    obj = AjiCmdline_Getc(cmdline, 0);
    assert(obj);
    assert(obj->type == AJI_CMDLINE_OBJ_TYPE__CMD);
    assert(!strcmp(AjiStr_Getc(obj->command), "abc"));
    assert(AjiCL_Len(obj->cl) == 1);
    obj = AjiCmdline_Getc(cmdline, 1);
    assert(obj);
    assert(obj->type == AJI_CMDLINE_OBJ_TYPE__REDIRECT);
    obj = AjiCmdline_Getc(cmdline, 2);
    assert(obj);
    assert(obj->type == AJI_CMDLINE_OBJ_TYPE__CMD);
    assert(!strcmp(AjiStr_Getc(obj->command), "def"));
    assert(AjiCL_Len(obj->cl) == 1);

    assert(AjiCmdline_Parse(cmdline, "abc -d efg > hij -d \"klm\""));
    assert(AjiCmdline_Len(cmdline) == 3);
    obj = AjiCmdline_Getc(cmdline, 0);
    assert(obj);
    assert(obj->type == AJI_CMDLINE_OBJ_TYPE__CMD);
    assert(!strcmp(AjiStr_Getc(obj->command), "abc -d efg"));
    assert(AjiCL_Len(obj->cl) == 3);
    obj = AjiCmdline_Getc(cmdline, 1);
    assert(obj);
    assert(obj->type == AJI_CMDLINE_OBJ_TYPE__REDIRECT);
    obj = AjiCmdline_Getc(cmdline, 2);
    assert(obj);
    assert(obj->type == AJI_CMDLINE_OBJ_TYPE__CMD);
    assert(!strcmp(AjiStr_Getc(obj->command), "hij -d \"klm\""));
    assert(AjiCL_Len(obj->cl) == 3);

    assert(AjiCmdline_Parse(cmdline, "a > b > c > d > e"));
    assert(AjiCmdline_Len(cmdline) == 9);

    AjiCmdline_Del(cmdline);
}

void
test_AjiCmdline_Resize(void) {
    AjiCmdline *cmdline = AjiCmdline_New();
    assert(cmdline);

    assert(AjiCmdline_Resize(NULL, 0) == NULL);
    assert(AjiCmdline_Resize(cmdline, 0) == NULL);

    assert(AjiCmdline_Resize(cmdline, 32));
    assert(AjiCmdline_Resize(cmdline, 8));
    assert(AjiCmdline_Resize(cmdline, 16));

    AjiCmdline_Del(cmdline);
}

void
test_AjiCmdline_MoveBack(void) {
    AjiCmdline *cmdline = AjiCmdline_New();
    assert(cmdline);

    assert(AjiCmdline_MoveBack(NULL, NULL) == NULL);
    assert(AjiCmdline_MoveBack(cmdline, NULL) == NULL);

    AjiCmdlineObj *obj = AjiCmdlineObj_New(AJI_CMDLINE_OBJ_TYPE__CMD);

    assert(AjiCmdline_MoveBack(cmdline, AjiMem_Move(obj)));

    AjiCmdline_Del(cmdline);
}

void
test_AjiCmdline_Len(void) {
    AjiCmdline *cmdline = AjiCmdline_New();
    assert(cmdline);

    assert(AjiCmdline_MoveBack(NULL, NULL) == NULL);
    assert(AjiCmdline_MoveBack(cmdline, NULL) == NULL);
    assert(AjiCmdline_Len(NULL) == -1);

    AjiCmdlineObj *obj1 = AjiCmdlineObj_New(AJI_CMDLINE_OBJ_TYPE__CMD);
    AjiCmdlineObj *obj2 = AjiCmdlineObj_New(AJI_CMDLINE_OBJ_TYPE__CMD);

    assert(AjiCmdline_MoveBack(cmdline, AjiMem_Move(obj1)));
    assert(AjiCmdline_Len(cmdline) == 1);

    assert(AjiCmdline_MoveBack(cmdline, AjiMem_Move(obj2)));
    assert(AjiCmdline_Len(cmdline) == 2);

    AjiCmdline_Del(cmdline);
}

void
test_AjiCmdline_Clear(void) {
    AjiCmdline *cmdline = AjiCmdline_New();
    assert(cmdline);

    AjiCmdline_Clear(NULL);

    AjiCmdlineObj *obj = AjiCmdlineObj_New(AJI_CMDLINE_OBJ_TYPE__CMD);

    assert(AjiCmdline_MoveBack(cmdline, AjiMem_Move(obj)));
    assert(AjiCmdline_Len(cmdline) == 1);

    AjiCmdline_Clear(cmdline);
    assert(AjiCmdline_Len(cmdline) == 0);

    AjiCmdline_Del(cmdline);
}

void
test_AjiCmdline_Getc(void) {
    AjiCmdline *cmdline = AjiCmdline_New();
    assert(cmdline);

    assert(AjiCmdline_Getc(NULL, -1) == NULL);
    assert(AjiCmdline_Getc(cmdline, -1) == NULL);
    assert(AjiCmdline_Getc(cmdline, 0) == NULL);

    AjiCmdlineObj *obj = AjiCmdlineObj_New(AJI_CMDLINE_OBJ_TYPE__CMD);

    assert(AjiCmdline_MoveBack(cmdline, AjiMem_Move(obj)));
    assert(AjiCmdline_Len(cmdline) == 1);

    assert(AjiCmdline_Getc(cmdline, 0));
    assert(AjiCmdline_Getc(cmdline, 1) == NULL);

    AjiCmdline_Del(cmdline);
}

void
test_AjiCmdline_HasErr(void) {
    AjiCmdline *cmdline = AjiCmdline_New();
    assert(cmdline);

    assert(AjiCmdline_Parse(cmdline, "||||") == NULL);
    assert(AjiCmdline_HasErr(cmdline));

    AjiCmdline_Del(cmdline);
}

static const struct testcase
cmdline_tests[] = {
    {"AjiCmdline_New", test_AjiCmdline_New},
    {"AjiCmdline_Del", test_AjiCmdline_Del},
    {"AjiCmdline_Parse", test_AjiCmdline_Parse},
    {"AjiCmdline_Parse_0", test_AjiCmdline_Parse_0},
    {"AjiCmdline_Parse_1", test_AjiCmdline_Parse_1},
    {"AjiCmdline_Parse_2", test_AjiCmdline_Parse_2},
    {"AjiCmdline_Parse_3", test_AjiCmdline_Parse_3},
    {"AjiCmdline_Parse_pipe", test_AjiCmdline_Parse_pipe},
    {"AjiCmdline_Parse_and", test_AjiCmdline_Parse_and},
    {"AjiCmdline_Parse_redirect", test_AjiCmdline_Parse_redirect},
    {"AjiCmdline_Resize", test_AjiCmdline_Resize},
    {"AjiCmdline_MoveBack", test_AjiCmdline_MoveBack},
    {"AjiCmdline_Clear", test_AjiCmdline_Clear},
    {"AjiCmdline_Getc", test_AjiCmdline_Getc},
    {"AjiCmdline_HasErr", test_AjiCmdline_HasErr},
    {0},
};

/**********
* cstring *
**********/

static void
test_cstring_AjiCStr_Copy(void) {
    const char *s = "test";
    char dst[5];

    assert(AjiCStr_Copy(NULL, 0, NULL) == NULL);
    assert(AjiCStr_Copy(dst, 0, NULL) == NULL);

    assert(AjiCStr_Copy(dst, 0, s));
    assert(!strcmp(dst, ""));

    assert(AjiCStr_Copy(dst, sizeof dst, s));
    assert(!strcmp(dst, "test"));
}

static void
test_cstring_AjiCStr_PopNewline(void) {
    char a[] = "test\n";

    assert(AjiCStr_PopNewline(NULL) == NULL);

    assert(AjiCStr_PopNewline(a));
    assert(!strcmp(a, "test"));

    char b[] = "b\r\n";
    assert(AjiCStr_PopNewline(b));
    assert(!strcmp(b, "b"));

    char c[] = "c\r\n\n";
    assert(AjiCStr_PopNewline(c));
    assert(!strcmp(c, "c"));
}

static void
test_cstring_AjiCStr_PopLastNewline(void) {
    char a[] = "test\n";

    assert(AjiCStr_PopLastNewline(NULL) == NULL);

    assert(AjiCStr_PopLastNewline(a));
    assert(!strcmp(a, "test"));

    char b[] = "b\r\n";
    assert(AjiCStr_PopLastNewline(b));
    assert(!strcmp(b, "b"));

    char c[] = "c\r\n\n";
    assert(AjiCStr_PopLastNewline(c));
    assert(!strcmp(c, "c\r\n"));

    char d[] = "d\r";
    assert(AjiCStr_PopLastNewline(d));
    assert(!strcmp(d, "d"));
}

static void
test_cstring_AjiCStr_CopyWithout(void) {
    char dst[100];

    assert(AjiCStr_CopyWithout(NULL, 0, NULL, NULL) == NULL);
    assert(AjiCStr_CopyWithout(dst, 0, NULL, NULL) == NULL);
    assert(AjiCStr_CopyWithout(dst, sizeof dst, NULL, NULL) == NULL);
    assert(AjiCStr_CopyWithout(dst, sizeof dst, "abcd", NULL) == NULL);

    assert(AjiCStr_CopyWithout(dst, sizeof dst, "abcd", "bc"));
    assert(!strcmp(dst, "ad"));

    assert(AjiCStr_CopyWithout(dst, sizeof dst, "abcd", "cd"));
    assert(!strcmp(dst, "ab"));

    assert(AjiCStr_CopyWithout(dst, sizeof dst, "abcd", "bcd"));
    assert(!strcmp(dst, "a"));

    assert(AjiCStr_CopyWithout(dst, sizeof dst, "abcd", "abcd"));
    assert(!strcmp(dst, ""));

    assert(AjiCStr_CopyWithout(dst, sizeof dst, "abcd", "a"));
    assert(!strcmp(dst, "bcd"));

    assert(AjiCStr_CopyWithout(dst, sizeof dst, "abcd", "ab"));
    assert(!strcmp(dst, "cd"));

    assert(AjiCStr_CopyWithout(dst, sizeof dst, "abcd", "abc"));
    assert(!strcmp(dst, "d"));

    assert(AjiCStr_CopyWithout(dst, sizeof dst, "abcd", "ad"));
    assert(!strcmp(dst, "bc"));

    assert(AjiCStr_CopyWithout(dst, sizeof dst, "abcd", "xyz"));
    assert(!strcmp(dst, "abcd"));

    assert(AjiCStr_CopyWithout(dst, sizeof dst, "abcd", "axyz"));
    assert(!strcmp(dst, "bcd"));

    assert(AjiCStr_CopyWithout(dst, sizeof dst, "abc123def456", "") != NULL);
    assert(strcmp(dst, "abc123def456") == 0);
    assert(AjiCStr_CopyWithout(dst, sizeof dst, "abc123def456", "123456") != NULL);
    assert(strcmp(dst, "abcdef") == 0);
    assert(AjiCStr_CopyWithout(dst, sizeof dst, "abc123def456", "abcdef") != NULL);
    assert(strcmp(dst, "123456") == 0);
}

static void
test_cstring_AjiCStr_App(void) {
    char dst[100] = {0};

    assert(AjiCStr_App(dst, sizeof dst, NULL) == NULL);
    assert(AjiCStr_App(NULL, sizeof dst, "source") == NULL);
    assert(AjiCStr_App(dst, 0, "source") == NULL);

    assert(AjiCStr_App(dst, 3, "source") != NULL);
    assert(strcmp(dst, "so") == 0);

    *dst = '\0';
    assert(AjiCStr_App(dst, sizeof dst, "source") != NULL);
    assert(strcmp(dst, "source") == 0);
    assert(AjiCStr_App(dst, sizeof dst, " is available.") != NULL);
    assert(strcmp(dst, "source is available.") == 0);
    assert(AjiCStr_App(dst, sizeof dst, "") != NULL);
    assert(strcmp(dst, "source is available.") == 0);
}

static void
test_cstring_AjiCStr_AppFmt(void) {
    char dst[100] = {0};

    assert(AjiCStr_AppFmt(dst, sizeof dst, NULL) == NULL);
    assert(AjiCStr_AppFmt(NULL, sizeof dst, "source") == NULL);
    assert(AjiCStr_AppFmt(dst, 0, "source") == NULL);

    assert(AjiCStr_AppFmt(dst, 3, "source") != NULL);
    assert(strcmp(dst, "so") == 0);

    *dst = '\0';
    assert(AjiCStr_AppFmt(dst, sizeof dst, "source") != NULL);
    assert(strcmp(dst, "source") == 0);
    assert(AjiCStr_AppFmt(dst, sizeof dst, " is available.") != NULL);
    assert(strcmp(dst, "source is available.") == 0);
    assert(AjiCStr_AppFmt(dst, sizeof dst, "") != NULL);
    assert(strcmp(dst, "source is available.") == 0);

    *dst = '\0';
    assert(AjiCStr_AppFmt(dst, sizeof dst, "n %d is %c", 10, 'i') != NULL);
    assert(strcmp(dst, "n 10 is i") == 0);
}

static void
test_cstring_AjiCStr_EDup(void) {
    char *p = AjiCStr_EDup("string");
    assert(strcmp(p, "string") == 0);
    free(p);
}

static void
test_cstring_AjiCStr_Split(void) {
    assert(AjiCStr_Split(NULL, '\0') == NULL);

    char **vec = AjiCStr_Split("abc\ndef", '\n');

    assert(!strcmp(vec[0], "abc"));
    assert(!strcmp(vec[1], "def"));
    assert(vec[2] == NULL);

    free(vec[0]);
    free(vec[1]);
    free(vec);

    vec = AjiCStr_Split("abc\ndef\n", '\n');

    assert(!strcmp(vec[0], "abc"));
    assert(!strcmp(vec[1], "def"));
    assert(!strcmp(vec[2], ""));
    assert(vec[3] == NULL);

    free(vec[0]);
    free(vec[1]);
    free(vec[2]);
    free(vec);

    vec = AjiCStr_SplitIgnoreEmpty("abc\ndef\n", '\n');

    assert(!strcmp(vec[0], "abc"));
    assert(!strcmp(vec[1], "def"));
    assert(vec[2] == NULL);

    free(vec[0]);
    free(vec[1]);
    free(vec);
}

static void
test_cstring_AjiCStr_Eq(void) {
    // assert(!AjiCStr_Eq(NULL, NULL));
    // assert(!AjiCStr_Eq("abc", NULL));

    assert(AjiCStr_Eq("abc", "abc"));
    assert(!AjiCStr_Eq("abc", "def"));
}

static void
test_cstring_AjiCStr_IsDigit(void) {
    assert(!AjiCStr_IsDigit(NULL));

    assert(AjiCStr_IsDigit("123"));
    assert(!AjiCStr_IsDigit("abc"));
    assert(!AjiCStr_IsDigit("12ab"));
}

static void
test_cstring_AjiCStr_EndsWith(void) {
    assert(!AjiCStr_EndsWith(NULL, NULL));
    assert(!AjiCStr_EndsWith(NULL, ""));
    assert(!AjiCStr_EndsWith("", NULL));

    assert(AjiCStr_EndsWith("this.is.a.pen", ".pen"));
    assert(!AjiCStr_EndsWith("this.is.a.pen", ".dog"));
}

static void
test_cstring_AjiCStr_StartsWith(void) {
    assert(!AjiCStr_StartsWith(NULL, NULL));
    assert(!AjiCStr_StartsWith(NULL, ""));
    assert(!AjiCStr_StartsWith("", NULL));
    
    assert(AjiCStr_StartsWith("this.is.a.pen", "this."));
    assert(!AjiCStr_StartsWith("this.is.a.pen", ".that"));
}

static const struct testcase
cstring_tests[] = {
    {"AjiCStr_Copy", test_cstring_AjiCStr_Copy},
    {"AjiCStr_PopNewline", test_cstring_AjiCStr_PopNewline},
    {"AjiCStr_PopLastNewline", test_cstring_AjiCStr_PopLastNewline},
    {"AjiCStr_CopyWithout", test_cstring_AjiCStr_CopyWithout},
    {"AjiCStr_App", test_cstring_AjiCStr_App},
    {"AjiCStr_AppFmt", test_cstring_AjiCStr_AppFmt},
    {"AjiCStr_EDup", test_cstring_AjiCStr_EDup},
    {"AjiCStr_Split", test_cstring_AjiCStr_Split},
    {"AjiCStr_Eq", test_cstring_AjiCStr_Eq},
    {"AjiCStr_IsDigit", test_cstring_AjiCStr_IsDigit},
    {"AjiCStr_EndsWith", test_cstring_AjiCStr_EndsWith},
    {"AjiCStr_StartsWith", test_cstring_AjiCStr_StartsWith},
    {0},
};

/*********
* string *
*********/

static void
test_AjiStr_Destroy(void) {
    AjiStr s = AJI_STR__INIT;
    AjiStr_Destroy(&s);
}

static void
test_AjiStr_EscDestroy(void) {
    AjiStr str = AJI_STR__INIT;
    char *s = AjiStr_EscDestroy(&str);
    assert(s && strlen(s) == 0);
    free(s);

    str = AJI_STR__INIT;
    AjiStr_App(&str, "ababa");
    s = AjiStr_EscDestroy(&str);
    assert(s && !strcmp(s, "ababa"));
    free(s);
}

static void
test_AjiStr_Init(void) {
    AjiStr s;
    AjiStr_Init(&s);
    AjiStr_App(&s, "ababa");
    assert(!strcmp(AjiStr_Getc(&s), "ababa"));
    AjiStr_Destroy(&s);
}

static void
test_AjiStr_InitCStr(void) {
    AjiStr s;
    AjiStr_InitCStr(&s, "ababa");
    assert(!strcmp(AjiStr_Getc(&s), "ababa"));
    AjiStr_Destroy(&s);
}

static void
test_AjiStr_Del(void) {
    AjiStr *s = AjiStr_New();
    assert(s != NULL);
    AjiStr_Del(NULL);
    AjiStr_Del(s);
}

static void
test_AjiStr_EscDel(void) {
    AjiStr *s = AjiStr_New();
    assert(s != NULL);
    assert(AjiStr_EscDel(NULL) == NULL);
    char *ptr = AjiStr_EscDel(s);
    assert(ptr != NULL);
    free(ptr);
}

static void
test_AjiStr_New(void) {
    AjiStr *s = AjiStr_New();
    assert(s != NULL);
    AjiStr_Del(s);
}

static void
test_AjiStr_NewCStr(void) {
    assert(AjiStr_NewCStr(NULL) == NULL);
    
    AjiStr *s = AjiStr_NewCStr("abc");
    assert(s);
    assert(!strcmp(AjiStr_Getc(s), "abc"));
    AjiStr_Del(s);
}

static void
test_AjiStr_DeepCopy(void) {
    AjiStr *s = AjiStr_New();
    assert(s != NULL);
    assert(AjiStr_Set(s, "1234") != NULL);
    assert(AjiStr_DeepCopy(NULL) == NULL);
    AjiStr *o = AjiStr_DeepCopy(s);
    assert(o != NULL);
    assert(strcmp(AjiStr_Getc(o), "1234") == 0);
    AjiStr_Del(o);
    AjiStr_Del(s);
}

static void
test_AjiStr_Len(void) {
    AjiStr *s = AjiStr_New();
    assert(s != NULL);
    assert(AjiStr_Len(NULL) == -1);
    assert(AjiStr_Len(s) == 0);
    assert(AjiStr_App(s, "abc") != NULL);
    assert(AjiStr_Len(s) == 3);
    AjiStr_Del(s);
}

static void
test_AjiStr_Capa(void) {
    AjiStr *s = AjiStr_New();
    assert(s != NULL);
    assert(AjiStr_Capa(NULL) == -1);
    assert(AjiStr_Capa(s) == 0);
    assert(AjiStr_App(s, "1234") != NULL);
    assert(AjiStr_Capa(s) == 8);
    AjiStr_Del(s);
}

static void
test_AjiStr_Getc(void) {
    AjiStr *s = AjiStr_New();
    assert(s != NULL);
    assert(AjiStr_Getc(NULL) == NULL);
    assert(strcmp(AjiStr_Getc(s), "") == 0);
    assert(AjiStr_App(s, "1234") != NULL);
    assert(strcmp(AjiStr_Getc(s), "1234") == 0);
    AjiStr_Del(s);
}

static void
test_AjiStr_Empty(void) {
    AjiStr *s = AjiStr_New();
    assert(s != NULL);
    assert(AjiStr_Empty(NULL) == 0);
    assert(AjiStr_Empty(s) == 1);
    assert(AjiStr_App(s, "1234") != NULL);
    assert(AjiStr_Empty(s) == 0);
    AjiStr_Del(s);
}

static void
test_AjiStr_Clear(void) {
    AjiStr *s = AjiStr_New();
    assert(s != NULL);
    assert(AjiStr_App(NULL, "1234") == NULL);
    assert(AjiStr_App(s, NULL) == NULL);
    assert(AjiStr_App(s, "1234") != NULL);
    assert(AjiStr_Len(s) == 4);
    AjiStr_Clear(s);
    assert(AjiStr_Len(s) == 0);
    AjiStr_Del(s);
}

static void
test_AjiStr_Set(void) {
    AjiStr *s = AjiStr_New();
    assert(s != NULL);
    assert(AjiStr_Set(NULL, "1234") == NULL);
    assert(AjiStr_Set(s, NULL) == NULL);
    assert(AjiStr_Set(s, "1234") != NULL);
    assert(strcmp(AjiStr_Getc(s), "1234") == 0);
    assert(AjiStr_Set(s, "12") != NULL);
    assert(strcmp(AjiStr_Getc(s), "12") == 0);
    AjiStr_Del(s);
}

static void
test_AjiStr_Resize(void) {
    AjiStr *s = AjiStr_New();
    assert(s != NULL);
    assert(AjiStr_Capa(NULL) == -1);
    assert(AjiStr_Capa(s) == 0);
    assert(AjiStr_Resize(s, 4*2) != NULL);
    assert(AjiStr_Capa(s) == 8);
    AjiStr_Del(s);
}

static void
test_AjiStr_PushBack(void) {
    AjiStr *s = AjiStr_New();
    assert(s != NULL);
    assert(AjiStr_PushBack(NULL, '1') == NULL);
    assert(AjiStr_PushBack(s, 0) == NULL);
    assert(AjiStr_PushBack(s, '\0') == NULL);
    assert(AjiStr_PushBack(s, '1') != NULL);
    assert(AjiStr_PushBack(s, '2') != NULL);
    assert(strcmp(AjiStr_Getc(s), "12") == 0);
    AjiStr_Del(s);
}

static void
test_AjiStr_PopBack(void) {
    AjiStr *s = AjiStr_New();
    assert(s != NULL);
    assert(AjiStr_PopBack(NULL) == '\0');
    assert(AjiStr_Set(s, "1234") != NULL);
    assert(strcmp(AjiStr_Getc(s), "1234") == 0);
    assert(AjiStr_PopBack(s) == '4');
    assert(AjiStr_PopBack(s) == '3');
    assert(strcmp(AjiStr_Getc(s), "12") == 0);
    AjiStr_Del(s);
}

static void
test_AjiStr_PushFront(void) {
    AjiStr *s = AjiStr_New();
    assert(s != NULL);
    assert(AjiStr_PushFront(NULL, '1') == NULL);
    assert(AjiStr_PushFront(s, 0) == NULL);
    assert(AjiStr_PushFront(s, '\0') == NULL);
    assert(AjiStr_PushFront(s, '1') != NULL);
    assert(AjiStr_PushFront(s, '2') != NULL);
    assert(strcmp(AjiStr_Getc(s), "21") == 0);
    AjiStr_Del(s);
}

static void
test_AjiStr_PopFront(void) {
    AjiStr *s = AjiStr_New();
    assert(s != NULL);
    assert(AjiStr_PopFront(NULL) == '\0');
    assert(AjiStr_Set(s, "1234") != NULL);
    assert(AjiStr_PopFront(s) == '1');
    assert(AjiStr_PopFront(s) == '2');
    assert(strcmp(AjiStr_Getc(s), "34") == 0);
    AjiStr_Del(s);
}

static void
test_AjiStr_App(void) {
    AjiStr *s = AjiStr_New();
    assert(s != NULL);
    assert(AjiStr_App(NULL, "1234") == NULL);
    assert(AjiStr_App(s, NULL) == NULL);
    assert(AjiStr_App(s, "1234") != NULL);
    assert(strcmp(AjiStr_Getc(s), "1234") == 0);
    AjiStr_Del(s);
}

static void
test_AjiStr_AppStream(void) {
    AjiStr *s = AjiStr_New();
    assert(s != NULL);

    char curdir[1024];
    char path[1024];
    assert(AjiFile_RealPath(curdir, sizeof curdir, ".") != NULL);
    assert(AjiFile_SolveFmt(path, sizeof path, "%s/tests/tests.c", curdir) != NULL);

    printf("path[%s]\n", path);
    FILE *fin = fopen(path, "r");
    assert(fin != NULL);
    assert(AjiStr_AppStream(NULL, fin) == NULL);
    assert(AjiStr_AppStream(s, NULL) == NULL);
    assert(AjiStr_AppStream(s, fin) != NULL);
    assert(fclose(fin) == 0);

    AjiStr_Del(s);
}

static void
test_AjiStr_AppOther(void) {
    AjiStr *s = AjiStr_New();
    assert(s != NULL);
    assert(AjiStr_Set(s, "1234") != NULL);
    AjiStr *o = AjiStr_New();
    assert(o != NULL);
    assert(AjiStr_Set(o, "1234") != NULL);
    assert(AjiStr_AppOther(NULL, o) == NULL);
    assert(AjiStr_AppOther(s, NULL) == NULL);
    assert(AjiStr_AppOther(s, o) != NULL);
    assert(strcmp(AjiStr_Getc(s), "12341234") == 0);
    AjiStr_Del(o);
    AjiStr_Del(s);

    s = AjiStr_New();
    assert(AjiStr_Set(s, "1234") != NULL);
    assert(AjiStr_AppOther(s, s) != NULL);
    assert(strcmp(AjiStr_Getc(s), "12341234") == 0);
    AjiStr_Del(s);
}

static void
test_AjiStr_AppFmt(void) {
    AjiStr *s = AjiStr_New();
    assert(s != NULL);
    char buf[1024];
    assert(AjiStr_AppFmt(NULL, buf, sizeof buf, "%s", "test") == NULL);
    assert(AjiStr_AppFmt(s, NULL, sizeof buf, "%s", "test") == NULL);
    assert(AjiStr_AppFmt(s, buf, 0, "%s", "test") == NULL);
    assert(AjiStr_AppFmt(s, buf, sizeof buf, NULL, "test") == NULL);
    assert(AjiStr_AppFmt(s, buf, sizeof buf, "%s %d %c", "1234", 1, '2') != NULL);
    assert(strcmp(AjiStr_Getc(s), "1234 1 2") == 0);
    AjiStr_Del(s);
}

static void
test_AjiStr_RStrip(void) {
    AjiStr *s = AjiStr_New();
    assert(s != NULL);
    assert(AjiStr_Set(s, "1234") != NULL);
    assert(AjiStr_RStrip(NULL, "34") == NULL);
    assert(AjiStr_RStrip(s, NULL) == NULL);

    AjiStr *o = AjiStr_RStrip(s, "34");
    assert(o);
    assert(strcmp(AjiStr_Getc(o), "12") == 0);

    AjiStr_Del(o);
    AjiStr_Del(s);
}

static void
test_AjiStr_LStrip(void) {
    AjiStr *s = AjiStr_New();
    assert(s != NULL);
    assert(AjiStr_Set(s, "1234") != NULL);
    assert(AjiStr_LStrip(NULL, "12") == NULL);
    assert(AjiStr_LStrip(s, NULL) == NULL);

    AjiStr *o = AjiStr_LStrip(s, "12");
    assert(o);
    assert(strcmp(AjiStr_Getc(o), "34") == 0);

    AjiStr_Del(o);
    AjiStr_Del(s);
}

static void
test_AjiStr_Strip(void) {
    AjiStr *s = AjiStr_New();
    assert(s != NULL);
    assert(AjiStr_Set(s, "--1234--") != NULL);
    assert(AjiStr_Strip(NULL, "-") == NULL);
    assert(AjiStr_Strip(s, NULL) == NULL);

    AjiStr *o = AjiStr_Strip(s, "-");
    assert(o);
    assert(strcmp(AjiStr_Getc(o), "1234") == 0);

    AjiStr_Del(o);
    AjiStr_Del(s);
}

static void
test_AjiStr_Findc(void) {
    AjiStr *s = AjiStr_New();
    assert(s != NULL);
    assert(AjiStr_Set(s, "1234") != NULL);
    assert(AjiStr_Findc(NULL, "") == NULL);
    assert(AjiStr_Findc(s, NULL) == NULL);
    const char *fnd = AjiStr_Findc(s, "23");
    assert(fnd != NULL);
    assert(strcmp(fnd, "234") == 0);
    AjiStr_Del(s);
}

static void
test_AjiStr_Lower(void) {
    assert(AjiStr_Lower(NULL) == NULL);
    AjiStr *s = AjiStr_New();
    assert(s != NULL);
    assert(AjiStr_Set(s, "ABC") != NULL);
    AjiStr *cp = AjiStr_Lower(s);
    assert(cp);
    assert(!strcmp(AjiStr_Getc(cp), "abc"));
    AjiStr_Del(cp);
    AjiStr_Del(s);
}

static void
test_AjiStr_Upper(void) {
    assert(AjiStr_Upper(NULL) == NULL);
    AjiStr *s = AjiStr_New();
    assert(s != NULL);
    assert(AjiStr_Set(s, "abc") != NULL);
    AjiStr *cp = AjiStr_Upper(s);
    assert(cp);
    assert(!strcmp(AjiStr_Getc(cp), "ABC"));
    AjiStr_Del(cp);
    AjiStr_Del(s);
}

static void
test_AjiStr_Capi(void) {
    assert(AjiStr_Capi(NULL) == NULL);
    AjiStr *s = AjiStr_New();
    assert(s != NULL);
    assert(AjiStr_Set(s, "abc") != NULL);
    AjiStr *cp = AjiStr_Capi(s);
    assert(cp);
    assert(!strcmp(AjiStr_Getc(cp), "Abc"));
    AjiStr_Del(cp);
    AjiStr_Del(s);
}

static void
test_AjiStr_Snake(void) {
    assert(AjiStr_Snake(NULL) == NULL);
    AjiStr *s = AjiStr_New();
    assert(s != NULL);

    assert(AjiStr_Set(s, "abc") != NULL);
    AjiStr *cp = AjiStr_Snake(s);
    assert(cp);
    assert(!strcmp(AjiStr_Getc(cp), "abc"));
    AjiStr_Del(cp);

    assert(AjiStr_Set(s, "abcDefGhi") != NULL);
    cp = AjiStr_Snake(s);
    assert(cp);
    assert(!strcmp(AjiStr_Getc(cp), "abc_def_ghi"));
    AjiStr_Del(cp);

    assert(AjiStr_Set(s, "AbcDefGhi") != NULL);
    cp = AjiStr_Snake(s);
    assert(cp);
    assert(!strcmp(AjiStr_Getc(cp), "abc_def_ghi"));
    AjiStr_Del(cp);

    assert(AjiStr_Set(s, "abc-def-ghi") != NULL);
    cp = AjiStr_Snake(s);
    assert(cp);
    assert(!strcmp(AjiStr_Getc(cp), "abc_def_ghi"));
    AjiStr_Del(cp);

    assert(AjiStr_Set(s, "_abcDefGhi") != NULL);
    cp = AjiStr_Snake(s);
    assert(cp);
    assert(!strcmp(AjiStr_Getc(cp), "abc_def_ghi"));
    AjiStr_Del(cp);

    assert(AjiStr_Set(s, "-abcDefGhi") != NULL);
    cp = AjiStr_Snake(s);
    assert(cp);
    assert(!strcmp(AjiStr_Getc(cp), "abc_def_ghi"));
    AjiStr_Del(cp);

    assert(AjiStr_Set(s, "_-abcDefGhi") != NULL);
    cp = AjiStr_Snake(s);
    assert(cp);
    assert(!strcmp(AjiStr_Getc(cp), "abc_def_ghi"));
    AjiStr_Del(cp);

    assert(AjiStr_Set(s, "abcDefGhi_abc-DefGhi") != NULL);
    cp = AjiStr_Snake(s);
    assert(cp);
    assert(!strcmp(AjiStr_Getc(cp), "abc_def_ghi_abc_def_ghi"));
    AjiStr_Del(cp);

    assert(AjiStr_Set(s, "abcDefGhi__abc--DefGhi") != NULL);
    cp = AjiStr_Snake(s);
    assert(cp);
    assert(!strcmp(AjiStr_Getc(cp), "abc_def_ghi_abc_def_ghi"));
    AjiStr_Del(cp);

    AjiStr_Del(s);
}

static void
test_AjiStr_Camel(void) {
#undef showcp
#define showcp() printf("cp[%s]\n", AjiStr_Getc(cp))

    assert(AjiStr_Camel(NULL) == NULL);
    AjiStr *s = AjiStr_New();
    assert(s != NULL);

    assert(AjiStr_Set(s, "abc") != NULL);
    AjiStr *cp = AjiStr_Camel(s);
    assert(cp);
    assert(!strcmp(AjiStr_Getc(cp), "abc"));
    AjiStr_Del(cp);

    assert(AjiStr_Set(s, "ABC") != NULL);
    cp = AjiStr_Camel(s);
    assert(cp);
    assert(!strcmp(AjiStr_Getc(cp), "aBC"));
    AjiStr_Del(cp);

    assert(AjiStr_Set(s, "AFormatB") != NULL);
    cp = AjiStr_Camel(s);
    assert(cp);
    assert(!strcmp(AjiStr_Getc(cp), "aFormatB"));
    AjiStr_Del(cp);

    assert(AjiStr_Set(s, "ABFormat") != NULL);
    cp = AjiStr_Camel(s);
    assert(cp);
    assert(!strcmp(AjiStr_Getc(cp), "aBFormat"));
    AjiStr_Del(cp);

    assert(AjiStr_Set(s, "abcDefGhi") != NULL);
    cp = AjiStr_Camel(s);
    assert(cp);
    assert(!strcmp(AjiStr_Getc(cp), "abcDefGhi"));
    AjiStr_Del(cp);

    assert(AjiStr_Set(s, "AbcDefGhi") != NULL);
    cp = AjiStr_Camel(s);
    assert(cp);
    assert(!strcmp(AjiStr_Getc(cp), "abcDefGhi"));
    AjiStr_Del(cp);

    assert(AjiStr_Set(s, "abc-def-ghi") != NULL);
    cp = AjiStr_Camel(s);
    assert(cp);
    assert(!strcmp(AjiStr_Getc(cp), "abcDefGhi"));
    AjiStr_Del(cp);

    assert(AjiStr_Set(s, "_abcDefGhi") != NULL);
    cp = AjiStr_Camel(s);
    assert(cp);
    assert(!strcmp(AjiStr_Getc(cp), "abcDefGhi"));
    AjiStr_Del(cp);

    assert(AjiStr_Set(s, "-abcDefGhi") != NULL);
    cp = AjiStr_Camel(s);
    assert(cp);
    assert(!strcmp(AjiStr_Getc(cp), "abcDefGhi"));
    AjiStr_Del(cp);

    assert(AjiStr_Set(s, "_-abcDefGhi") != NULL);
    cp = AjiStr_Camel(s);
    assert(cp);
    assert(!strcmp(AjiStr_Getc(cp), "abcDefGhi"));
    AjiStr_Del(cp);

    assert(AjiStr_Set(s, "abcDefGhi_abc-DefGhi") != NULL);
    cp = AjiStr_Camel(s);
    assert(cp);
    assert(!strcmp(AjiStr_Getc(cp), "abcDefGhiAbcDefGhi"));
    AjiStr_Del(cp);

    assert(AjiStr_Set(s, "abcDefGhi__abc--DefGhi") != NULL);
    cp = AjiStr_Camel(s);
    assert(cp);
    assert(!strcmp(AjiStr_Getc(cp), "abcDefGhiAbcDefGhi"));
    AjiStr_Del(cp);

    AjiStr_Del(s);
}

static void
test_AjiStr_Hacker(void) {
#undef showcp
#define showcp() printf("cp[%s]\n", AjiStr_Getc(cp))

    assert(AjiStr_Hacker(NULL) == NULL);
    AjiStr *s = AjiStr_New();
    assert(s != NULL);

    assert(AjiStr_Set(s, "abc") != NULL);
    AjiStr *cp = AjiStr_Hacker(s);
    assert(cp);
    assert(!strcmp(AjiStr_Getc(cp), "abc"));
    AjiStr_Del(cp);

    assert(AjiStr_Set(s, "ABC") != NULL);
    cp = AjiStr_Hacker(s);
    assert(cp);
    assert(!strcmp(AjiStr_Getc(cp), "abc"));
    AjiStr_Del(cp);

    assert(AjiStr_Set(s, "AFormatB") != NULL);
    cp = AjiStr_Hacker(s);
    assert(cp);
    assert(!strcmp(AjiStr_Getc(cp), "aformatb"));
    AjiStr_Del(cp);

    assert(AjiStr_Set(s, "ABFormat") != NULL);
    cp = AjiStr_Hacker(s);
    assert(cp);
    assert(!strcmp(AjiStr_Getc(cp), "abformat"));
    AjiStr_Del(cp);

    assert(AjiStr_Set(s, "abcDefGhi") != NULL);
    cp = AjiStr_Hacker(s);
    assert(cp);
    assert(!strcmp(AjiStr_Getc(cp), "abcdefghi"));
    AjiStr_Del(cp);

    assert(AjiStr_Set(s, "AbcDefGhi") != NULL);
    cp = AjiStr_Hacker(s);
    assert(cp);
    assert(!strcmp(AjiStr_Getc(cp), "abcdefghi"));
    AjiStr_Del(cp);

    assert(AjiStr_Set(s, "abc-def-ghi") != NULL);
    cp = AjiStr_Hacker(s);
    assert(cp);
    assert(!strcmp(AjiStr_Getc(cp), "abcdefghi"));
    AjiStr_Del(cp);

    assert(AjiStr_Set(s, "_abcDefGhi") != NULL);
    cp = AjiStr_Hacker(s);
    assert(cp);
    assert(!strcmp(AjiStr_Getc(cp), "abcdefghi"));
    AjiStr_Del(cp);

    assert(AjiStr_Set(s, "-abcDefGhi") != NULL);
    cp = AjiStr_Hacker(s);
    assert(cp);
    assert(!strcmp(AjiStr_Getc(cp), "abcdefghi"));
    AjiStr_Del(cp);

    assert(AjiStr_Set(s, "_-abcDefGhi") != NULL);
    cp = AjiStr_Hacker(s);
    assert(cp);
    assert(!strcmp(AjiStr_Getc(cp), "abcdefghi"));
    AjiStr_Del(cp);

    assert(AjiStr_Set(s, "abcDefGhi_abc-DefGhi") != NULL);
    cp = AjiStr_Hacker(s);
    assert(cp);
    assert(!strcmp(AjiStr_Getc(cp), "abcdefghiabcdefghi"));
    AjiStr_Del(cp);

    assert(AjiStr_Set(s, "abcDefGhi__abc--DefGhi") != NULL);
    cp = AjiStr_Hacker(s);
    assert(cp);
    assert(!strcmp(AjiStr_Getc(cp), "abcdefghiabcdefghi"));
    AjiStr_Del(cp);

    assert(AjiStr_Set(s, "abcDefGhi__abc--DefGhi") != NULL);
    cp = AjiStr_Hacker(s);
    assert(cp);
    assert(!strcmp(AjiStr_Getc(cp), "abcdefghiabcdefghi"));
    AjiStr_Del(cp);

    assert(AjiStr_Set(s, "abc0_12def_gh34i") != NULL);
    cp = AjiStr_Hacker(s);
    assert(cp);
    assert(!strcmp(AjiStr_Getc(cp), "abc012defgh34i"));
    AjiStr_Del(cp);

    AjiStr_Del(s);
}

static void
test_AjiStr_Mul(void) {
    AjiStr *s = AjiStr_New();
    AjiStr_Set(s, "abc");

    assert(AjiStr_Mul(NULL, 0) == NULL);

    AjiStr *m = AjiStr_Mul(s, 2);
    assert(!strcmp(AjiStr_Getc(m), "abcabc"));

    AjiStr_Del(m);
    AjiStr_Del(s);
}

static void
test_AjiStr_Indent(void) {
    AjiStr *s = AjiStr_New();
    AjiStr_Set(s, "abc");

    AjiStr *ss = AjiStr_Indent(s, ' ', 2, 4);
    assert(ss);
    assert(!strcmp(AjiStr_Getc(ss), "        abc"));
    AjiStr_Del(ss);

    ss = AjiStr_Indent(s, '\t', 2, 0);
    assert(ss);
    assert(!strcmp(AjiStr_Getc(ss), "\t\tabc"));
    AjiStr_Del(ss);

    AjiStr_Set(s, "abc\ndef\n");

    ss = AjiStr_Indent(s, ' ', 1, 4);
    assert(ss);
    assert(!strcmp(AjiStr_Getc(ss), "    abc\n    def\n"));
    AjiStr_Del(ss);

    AjiStr_Del(s);
}

static void
test_AjiStr_Replace(void) {
    AjiStr *s = AjiStr_New();
    AjiStr *rep = NULL;

    assert(!AjiStr_Replace(NULL, NULL, NULL));
    assert(!AjiStr_Replace(s, NULL, NULL));
    assert(!AjiStr_Replace(s, "a", NULL));

    AjiStr_Set(s, "abcdef");
    rep = AjiStr_Replace(s, "abc", "ABC");
    assert(!strcmp(AjiStr_Getc(rep), "ABCdef"));
    AjiStr_Del(rep);

    AjiStr_Set(s, "abcdef");
    rep = AjiStr_Replace(s, "cde", "CDE");
    assert(!strcmp(AjiStr_Getc(rep), "abCDEf"));
    AjiStr_Del(rep);

    AjiStr_Set(s, "abcdef");
    rep = AjiStr_Replace(s, "def", "DEF");
    assert(!strcmp(AjiStr_Getc(rep), "abcDEF"));
    AjiStr_Del(rep);

    AjiStr_Set(s, "abcdef");
    rep = AjiStr_Replace(s, "a", "AAA");
    assert(!strcmp(AjiStr_Getc(rep), "AAAbcdef"));
    AjiStr_Del(rep);

    AjiStr_Set(s, "abcdef");
    rep = AjiStr_Replace(s, "c", "CCC");
    assert(!strcmp(AjiStr_Getc(rep), "abCCCdef"));
    AjiStr_Del(rep);

    AjiStr_Set(s, "abcdef");
    rep = AjiStr_Replace(s, "f", "FFF");
    assert(!strcmp(AjiStr_Getc(rep), "abcdeFFF"));
    AjiStr_Del(rep);

    AjiStr_Set(s, "abcdef");
    rep = AjiStr_Replace(s, "x", "XXX");
    assert(!strcmp(AjiStr_Getc(rep), "abcdef"));
    AjiStr_Del(rep);

    AjiStr_Set(s, "abcabc");
    rep = AjiStr_Replace(s, "abc", "ABC");
    assert(!strcmp(AjiStr_Getc(rep), "ABCABC"));
    AjiStr_Del(rep);

    AjiStr_Set(s, "abcabcabc");
    rep = AjiStr_Replace(s, "ab", "AB");
    assert(!strcmp(AjiStr_Getc(rep), "ABcABcABc"));
    AjiStr_Del(rep);

    AjiStr_Del(s);
}

static const struct testcase
string_tests[] = {
    {"AjiStr_Destroy", test_AjiStr_Destroy},
    {"AjiStr_EscDestroy", test_AjiStr_EscDestroy},
    {"AjiStr_Init", test_AjiStr_Init},
    {"AjiStr_InitCStr", test_AjiStr_InitCStr},
    {"AjiStr_Del", test_AjiStr_Del},
    {"AjiStr_EscDel", test_AjiStr_EscDel},
    {"AjiStr_New", test_AjiStr_New},
    {"AjiStr_NewCStr", test_AjiStr_NewCStr},
    {"AjiStr_DeepCopy", test_AjiStr_DeepCopy},
    {"AjiStr_DeepCopy", test_AjiStr_DeepCopy},
    {"AjiStr_Len", test_AjiStr_Len},
    {"AjiStr_Capa", test_AjiStr_Capa},
    {"AjiStr_Getc", test_AjiStr_Getc},
    {"AjiStr_Empty", test_AjiStr_Empty},
    {"AjiStr_Clear", test_AjiStr_Clear},
    {"AjiStr_Set", test_AjiStr_Set},
    {"AjiStr_Resize", test_AjiStr_Resize},
    {"AjiStr_PushBack", test_AjiStr_PushBack},
    {"AjiStr_PopBack", test_AjiStr_PopBack},
    {"AjiStr_PushFront", test_AjiStr_PushFront},
    {"AjiStr_PopFront", test_AjiStr_PopFront},
    {"AjiStr_App", test_AjiStr_App},
    {"AjiStr_AppStream", test_AjiStr_AppStream},
    {"AjiStr_AppOther", test_AjiStr_AppOther},
    {"AjiStr_AppFmt", test_AjiStr_AppFmt},
    {"AjiStr_RStrip", test_AjiStr_RStrip},
    {"AjiStr_LStrip", test_AjiStr_LStrip},
    {"AjiStr_Strip", test_AjiStr_Strip},
    {"AjiStr_Findc", test_AjiStr_Findc},
    {"AjiStr_Lower", test_AjiStr_Lower},
    {"AjiStr_Upper", test_AjiStr_Upper},
    {"AjiStr_Capi", test_AjiStr_Capi},
    {"AjiStr_Snake", test_AjiStr_Snake},
    {"AjiStr_Camel", test_AjiStr_Camel},
    {"AjiStr_Hacker", test_AjiStr_Hacker},
    {"AjiStr_Mul", test_AjiStr_Mul},
    {"AjiStr_Indent", test_AjiStr_Indent},
    {"AjiStr_Replace", test_AjiStr_Replace},
    {0},
};

/**********
* unicode *
**********/

static void
test_AjiUni_Destroy(void) {
    AjiUni u = AJI_UNI__INIT;
    AjiUni_Destroy(&u);
}

static void
test_AjiUni_EscDestroy(void) {
    AjiUni u;
    AjiUniType *s;

    u = AJI_UNI__INIT;
    s = AjiUni_EscDestroy(&u);
    assert(s && AjiU_Len(s) == 0);
    free(s);

    assert(AjiUni_InitCStr(&u, "ababa"));
    s = AjiUni_EscDestroy(&u);
    assert(s && !AjiU_StrCmp(s, AJI_UNI__STR("ababa")));
    free(s);
}

static void
test_AjiUni_Init(void) {
    AjiUni u;
    assert(AjiUni_Init(&u));
    assert(AjiUni_Len(&u) == 0);
    AjiUni_Destroy(&u);
}

static void
test_AjiUni_InitCStr(void) {
    AjiUni u;
    assert(AjiUni_InitCStr(&u, "ababa"));
    assert(!strcmp(AjiUni_GetcMB(&u), "ababa"));
    AjiUni_Destroy(&u);
}

static void
test_AjiUni_Del(void) {
    AjiUni *u = AjiUni_New();
    assert(u != NULL);
    AjiUni_Del(NULL);
    AjiUni_Del(u);
}

static void
test_AjiUni_EscDel(void) {
    AjiUni *u = AjiUni_New();
    assert(u != NULL);
    assert(AjiUni_EscDel(NULL) == NULL);
    AjiUniType *ptr = AjiUni_EscDel(u);
    assert(ptr != NULL);
    free(ptr);
}

static void
test_AjiUni_New(void) {
    AjiUni *u = AjiUni_New();
    assert(u != NULL);
    AjiUni_Del(u);
}

static void
test_AjiUni_StaticDeepCopy(void) {
    AjiUni u = AJI_UNI__INIT;
    AjiUni o = AJI_UNI__INIT;
    assert(AjiUni_SetMB(&o, "1234") != NULL);
    assert(AjiUni_StaticDeepCopy(NULL, NULL) == NULL);
    assert(AjiUni_StaticDeepCopy(&u, &o));
    assert(AjiU_StrCmp(AjiUni_Getc(&u), AJI_UNI__STR("1234")) == 0);
    AjiUni_Destroy(&o);
    AjiUni_Destroy(&u);
}

static void
test_AjiUni_StaticShallowCopy(void) {
    AjiUni u = AJI_UNI__INIT;
    AjiUni o = AJI_UNI__INIT;
    assert(AjiUni_SetMB(&o, "1234") != NULL);
    assert(AjiUni_StaticShallowCopy(NULL, NULL) == NULL);
    assert(AjiUni_StaticShallowCopy(&u, &o));
    assert(AjiU_StrCmp(AjiUni_Getc(&u), AJI_UNI__STR("1234")) == 0);
    AjiUni_Destroy(&o);
    AjiUni_Destroy(&u);
}

static void
test_AjiUni_DeepCopy(void) {
    AjiUni *u = AjiUni_New();
    assert(u != NULL);
    assert(AjiUni_SetMB(u, "1234") != NULL);
    assert(AjiUni_DeepCopy(NULL) == NULL);
    AjiUni *o = AjiUni_DeepCopy(u);
    assert(o != NULL);
    assert(AjiU_StrCmp(AjiUni_Getc(o), AJI_UNI__STR("1234")) == 0);
    AjiUni_Del(o);
    AjiUni_Del(u);
}

static void
test_AjiUni_ShallowCopy(void) {
    AjiUni *u = AjiUni_New();
    assert(u != NULL);
    assert(AjiUni_SetMB(u, "1234") != NULL);
    assert(AjiUni_ShallowCopy(NULL) == NULL);
    AjiUni *o = AjiUni_ShallowCopy(u);
    assert(o != NULL);
    assert(AjiU_StrCmp(AjiUni_Getc(o), AJI_UNI__STR("1234")) == 0);
    AjiUni_Del(o);
    AjiUni_Del(u);
}

static void
test_AjiUni_Swap(void) {
    AjiUni u, o;
    AjiUni_InitCStr(&u, "aaa");
    AjiUni_InitCStr(&o, "bbb");
    assert(!strcmp(AjiUni_GetcMB(&u), "aaa"));
    assert(!strcmp(AjiUni_GetcMB(&o), "bbb"));
    AjiUni_Swap(&u, &o);
    assert(!strcmp(AjiUni_GetcMB(&o), "aaa"));
    assert(!strcmp(AjiUni_GetcMB(&u), "bbb"));
    AjiUni_Destroy(&u);
    AjiUni_Destroy(&o);
}

static void
test_AjiUni_Len(void) {
    AjiUni *u = AjiUni_New();
    assert(u != NULL);
    assert(AjiUni_Len(NULL) == -1);
    assert(AjiUni_Len(u) == 0);
    assert(AjiUni_App(u, AJI_UNI__STR("abc")) != NULL);
    assert(AjiUni_Len(u) == 3);
    AjiUni_Del(u);
}

static void
test_AjiUni_Capa(void) {
    AjiUni *u = AjiUni_New();
    assert(u != NULL);
    assert(AjiUni_Capa(NULL) == -1);
    assert(AjiUni_Capa(u) == 0);
    assert(AjiUni_App(u, AJI_UNI__STR("1234")) != NULL);
    assert(AjiUni_Capa(u) == 8);
    AjiUni_Del(u);
}

static void
test_AjiUni_Getc(void) {
    AjiUni *u = AjiUni_New();
    assert(u != NULL);
    assert(AjiUni_Getc(NULL) == NULL);
    assert(AjiU_StrCmp(AjiUni_Getc(u), AJI_UNI__STR("")) == 0);
    assert(AjiUni_App(u, AJI_UNI__STR("1234")) != NULL);
    assert(AjiU_StrCmp(AjiUni_Getc(u), AJI_UNI__STR("1234")) == 0);
    AjiUni_Del(u);
}

static void
test_AjiUni_Empty(void) {
    AjiUni *s = AjiUni_New();
    assert(s != NULL);
    assert(AjiUni_Empty(NULL) == 0);
    assert(AjiUni_Empty(s) == 1);
    assert(AjiUni_App(s, AJI_UNI__STR("1234")) != NULL);
    assert(AjiUni_Empty(s) == 0);
    AjiUni_Del(s);
}

static void
test_AjiUni_Clear(void) {
    AjiUni *u = AjiUni_New();
    assert(u != NULL);
    assert(AjiUni_App(NULL, AJI_UNI__STR("1234")) == NULL);
    assert(AjiUni_App(u, NULL) == NULL);
    assert(AjiUni_App(u, AJI_UNI__STR("1234")) != NULL);
    assert(AjiUni_Len(u) == 4);
    AjiUni_Clear(u);
    assert(AjiUni_Len(u) == 0);
    AjiUni_Del(u);
}

static void
test_AjiUni_Set(void) {
    AjiUni *u = AjiUni_New();
    assert(u != NULL);
    assert(AjiUni_Set(NULL, AJI_UNI__STR("1234")) == NULL);
    assert(AjiUni_Set(u, NULL) == NULL);
    assert(AjiUni_Set(u, AJI_UNI__STR("1234")) != NULL);
    assert(AjiU_StrCmp(AjiUni_Getc(u), AJI_UNI__STR("1234")) == 0);
    assert(AjiUni_Set(u, AJI_UNI__STR("12")) != NULL);
    assert(AjiU_StrCmp(AjiUni_Getc(u), AJI_UNI__STR("12")) == 0);
    AjiUni_Del(u);
}

static void
test_AjiUni_Resize(void) {
    AjiUni *u = AjiUni_New();
    assert(u != NULL);
    assert(AjiUni_Capa(NULL) == -1);
    assert(AjiUni_Capa(u) == 0);
    assert(AjiUni_Resize(u, 4 * 2) != NULL);
    assert(AjiUni_Capa(u) == 8);
    AjiUni_Del(u);
}

static void
test_AjiUni_PushBack(void) {
    AjiUni *u = AjiUni_New();
    assert(u != NULL);

    assert(AjiUni_PushBack(NULL, AJI_UNI__CH('1')) == NULL);
    assert(AjiUni_PushBack(u, 0) == NULL);
    assert(AjiUni_PushBack(u, AJI_UNI__CH('\0')) == NULL);
    assert(AjiUni_PushBack(u, AJI_UNI__CH('1')) != NULL);
    assert(AjiUni_PushBack(u, AJI_UNI__CH('2')) != NULL);
    assert(AjiU_StrCmp(AjiUni_Getc(u), AJI_UNI__STR("12")) == 0);
    
    AjiUni_Clear(u);
    assert(AjiUni_PushBack(u, AJI_UNI__CH('あ')) != NULL);
    assert(AjiUni_PushBack(u, AJI_UNI__CH('い')) != NULL);
    assert(AjiU_StrCmp(AjiUni_Getc(u), AJI_UNI__STR("あい")) == 0);

    AjiUni_Del(u);
}

static void
test_AjiUni_PopBack(void) {
    AjiUni *u = AjiUni_New();
    assert(u != NULL);
    assert(AjiUni_PopBack(NULL) == AJI_UNI__CH('\0'));
    assert(AjiUni_Set(u, AJI_UNI__STR("1234")) != NULL);
    assert(AjiU_StrCmp(AjiUni_Getc(u), AJI_UNI__STR("1234")) == 0);
    assert(AjiUni_PopBack(u) == AJI_UNI__CH('4'));
    assert(AjiUni_PopBack(u) == AJI_UNI__CH('3'));
    assert(AjiU_StrCmp(AjiUni_Getc(u), AJI_UNI__STR("12")) == 0);
    AjiUni_Del(u);
}

static void
test_AjiUni_PushFront(void) {
    AjiUni *u = AjiUni_New();
    assert(u != NULL);
    assert(AjiUni_PushFront(NULL, AJI_UNI__CH('1')) == NULL);
    assert(AjiUni_PushFront(u, 0) == NULL);
    assert(AjiUni_PushFront(u, AJI_UNI__CH('\0')) == NULL);
    assert(AjiUni_PushFront(u, AJI_UNI__CH('1')) != NULL);
    assert(AjiUni_PushFront(u, AJI_UNI__CH('2')) != NULL);
    assert(AjiU_StrCmp(AjiUni_Getc(u), AJI_UNI__STR("21")) == 0);
    AjiUni_Del(u);
}

static void
test_AjiUni_PopFront(void) {
    AjiUni *u = AjiUni_New();
    assert(u != NULL);
    assert(AjiUni_PopFront(NULL) == '\0');
    assert(AjiUni_Set(u, AJI_UNI__STR("1234")) != NULL);
    assert(AjiUni_PopFront(u) == AJI_UNI__CH('1'));
    assert(AjiUni_PopFront(u) == AJI_UNI__CH('2'));
    assert(AjiU_StrCmp(AjiUni_Getc(u), AJI_UNI__STR("34")) == 0);
    AjiUni_Del(u);
}

static void
test_AjiUni_App(void) {
    AjiUni *u = AjiUni_New();
    assert(u != NULL);
    assert(AjiUni_App(NULL, AJI_UNI__STR("1234")) == NULL);
    assert(AjiUni_App(u, NULL) == NULL);
    assert(AjiUni_App(u, AJI_UNI__STR("1234")) != NULL);
    assert(AjiU_StrCmp(AjiUni_Getc(u), AJI_UNI__STR("1234")) == 0);
    AjiUni_Del(u);
}

static void
test_AjiUni_AppStream(void) {
    AjiUni *s = AjiUni_New();
    assert(s != NULL);

    char curdir[1024];
    char path[1024];
    assert(AjiFile_RealPath(curdir, sizeof curdir, ".") != NULL);
    assert(AjiFile_SolveFmt(path, sizeof path, "%s/tests/tests.c", curdir) != NULL);

    FILE *fin = fopen(path, "r");
    assert(fin != NULL);
    assert(AjiUni_AppStream(NULL, fin) == NULL);
    assert(AjiUni_AppStream(s, NULL) == NULL);
    assert(AjiUni_AppStream(s, fin) != NULL);
    assert(fclose(fin) == 0);

    AjiUni_Del(s);
}

static void
test_AjiUni_AppOther(void) {
    AjiUni *u = AjiUni_New();
    assert(u != NULL);
    assert(AjiUni_Set(u, AJI_UNI__STR("1234")) != NULL);
    AjiUni *o = AjiUni_New();
    assert(o != NULL);
    assert(AjiUni_Set(o, AJI_UNI__STR("1234")) != NULL);
    assert(AjiUni_AppOther(NULL, o) == NULL);
    assert(AjiUni_AppOther(u, NULL) == NULL);
    assert(AjiUni_AppOther(u, o) != NULL);
    assert(AjiU_StrCmp(AjiUni_Getc(u), AJI_UNI__STR("12341234")) == 0);
    AjiUni_Del(o);
    AjiUni_Del(u);

    u = AjiUni_New();
    assert(AjiUni_Set(u, AJI_UNI__STR("1234")) != NULL);
    assert(AjiUni_AppOther(u, u) != NULL);
    assert(AjiU_StrCmp(AjiUni_Getc(u), AJI_UNI__STR("12341234")) == 0);
    AjiUni_Del(u);
}

static void
test_AjiUni_AppFmt(void) {
    AjiUni *u = AjiUni_New();
    assert(u != NULL);
    char buf[1024];
    assert(AjiUni_AppFmt(NULL, buf, sizeof buf, "%s", "test") == NULL);
    assert(AjiUni_AppFmt(u, NULL, sizeof buf, "%s", "test") == NULL);
    assert(AjiUni_AppFmt(u, buf, 0, "%s", "test") == NULL);
    assert(AjiUni_AppFmt(u, buf, sizeof buf, NULL, "test") == NULL);
    assert(AjiUni_AppFmt(u, buf, sizeof buf, "%s %d %c", "1234", 1, '2') != NULL);
    assert(AjiU_StrCmp(AjiUni_Getc(u), AJI_UNI__STR("1234 1 2")) == 0);
    AjiUni_Del(u);
}

static void
test_AjiUni_RStrip(void) {
    AjiUni *u = AjiUni_New();
    assert(u != NULL);
    assert(AjiUni_Set(u, AJI_UNI__STR("1234")) != NULL);
    assert(AjiUni_RStrip(NULL, AJI_UNI__STR("34")) == NULL);
    assert(AjiUni_RStrip(u, NULL) == NULL);

    AjiUni *o = AjiUni_RStrip(u, AJI_UNI__STR("34"));
    assert(o);
    assert(AjiU_StrCmp(AjiUni_Getc(o), AJI_UNI__STR("12")) == 0);

    AjiUni_Del(o);
    AjiUni_Del(u);
}

static void
test_AjiUni_LStrip(void) {
    AjiUni *u = AjiUni_New();
    assert(u != NULL);
    assert(AjiUni_Set(u, AJI_UNI__STR("1234")) != NULL);
    assert(AjiUni_LStrip(NULL, AJI_UNI__STR("12")) == NULL);
    assert(AjiUni_LStrip(u, NULL) == NULL);

    AjiUni *o = AjiUni_LStrip(u, AJI_UNI__STR("12"));
    assert(o);
    assert(AjiU_StrCmp(AjiUni_Getc(o), AJI_UNI__STR("34")) == 0);

    AjiUni_Del(o);
    AjiUni_Del(u);
}

static void
test_AjiUni_Strip(void) {
    AjiUni *u = AjiUni_New();
    assert(u != NULL);
    assert(AjiUni_Set(u, AJI_UNI__STR("--1234--")) != NULL);
    assert(AjiUni_Strip(NULL, AJI_UNI__STR("-")) == NULL);
    assert(AjiUni_Strip(u, NULL) == NULL);

    AjiUni *o = AjiUni_Strip(u, AJI_UNI__STR("-"));
    assert(o);
    assert(AjiU_StrCmp(AjiUni_Getc(o), AJI_UNI__STR("1234")) == 0);

    AjiUni_Del(o);
    AjiUni_Del(u);
}

static void
test_AjiUni_Lower(void) {
    assert(AjiUni_Lower(NULL) == NULL);
    AjiUni *u = AjiUni_New();
    assert(u != NULL);
    assert(AjiUni_Set(u, AJI_UNI__STR("ABC")) != NULL);
    AjiUni *cp = AjiUni_Lower(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("abc")));
    AjiUni_Del(cp);
    AjiUni_Del(u);
}

static void
test_AjiUni_Upper(void) {
    assert(AjiUni_Upper(NULL) == NULL);
    AjiUni *u = AjiUni_New();
    assert(u != NULL);
    assert(AjiUni_Set(u, AJI_UNI__STR("abc")) != NULL);
    AjiUni *cp = AjiUni_Upper(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("ABC")));
    AjiUni_Del(cp);
    AjiUni_Del(u);
}

static void
test_AjiUni_Capi(void) {
    assert(AjiUni_Capi(NULL) == NULL);
    AjiUni *u = AjiUni_New();
    assert(u != NULL);
    assert(AjiUni_Set(u, AJI_UNI__STR("abc")) != NULL);
    AjiUni *cp = AjiUni_Capi(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("Abc")));
    AjiUni_Del(cp);
    AjiUni_Del(u);
}

static void
test_AjiUni_Snake(void) {
    assert(AjiUni_Snake(NULL) == NULL);
    AjiUni *u = AjiUni_New();
    assert(u != NULL);

    assert(AjiUni_Set(u, AJI_UNI__STR("abc")) != NULL);
    AjiUni *cp = AjiUni_Snake(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("abc")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("abcDefGhi")) != NULL);
    cp = AjiUni_Snake(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("abc_def_ghi")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("AbcDefGhi")) != NULL);
    cp = AjiUni_Snake(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("abc_def_ghi")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("abc-def-ghi")) != NULL);
    cp = AjiUni_Snake(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("abc_def_ghi")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("_abcDefGhi")) != NULL);
    cp = AjiUni_Snake(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("abc_def_ghi")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("-abcDefGhi")) != NULL);
    cp = AjiUni_Snake(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("abc_def_ghi")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("_-abcDefGhi")) != NULL);
    cp = AjiUni_Snake(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("abc_def_ghi")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("abcDefGhi_abc-DefGhi")) != NULL);
    cp = AjiUni_Snake(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("abc_def_ghi_abc_def_ghi")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("abcDefGhi__abc--DefGhi")) != NULL);
    cp = AjiUni_Snake(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("abc_def_ghi_abc_def_ghi")));
    AjiUni_Del(cp);

    AjiUni_Del(u);
}

static void
test_AjiUni_Camel(void) {
    assert(AjiUni_Camel(NULL) == NULL);
    AjiUni *u = AjiUni_New();
    assert(u != NULL);

    assert(AjiUni_Set(u, AJI_UNI__STR("abc")) != NULL);
    AjiUni *cp = AjiUni_Camel(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("abc")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("ABC")) != NULL);
    cp = AjiUni_Camel(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("aBC")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("AFormatB")) != NULL);
    cp = AjiUni_Camel(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("aFormatB")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("ABFormat")) != NULL);
    cp = AjiUni_Camel(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("aBFormat")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("abcDefGhi")) != NULL);
    cp = AjiUni_Camel(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("abcDefGhi")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("AbcDefGhi")) != NULL);
    cp = AjiUni_Camel(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("abcDefGhi")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("abc-def-ghi")) != NULL);
    cp = AjiUni_Camel(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("abcDefGhi")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("_abcDefGhi")) != NULL);
    cp = AjiUni_Camel(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("abcDefGhi")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("-abcDefGhi")) != NULL);
    cp = AjiUni_Camel(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("abcDefGhi")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("_-abcDefGhi")) != NULL);
    cp = AjiUni_Camel(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("abcDefGhi")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("abcDefGhi_abc-DefGhi")) != NULL);
    cp = AjiUni_Camel(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("abcDefGhiAbcDefGhi")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("abcDefGhi__abc--DefGhi")) != NULL);
    cp = AjiUni_Camel(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("abcDefGhiAbcDefGhi")));
    AjiUni_Del(cp);

    AjiUni_Del(u);
}

static void
test_AjiUni_Hacker(void) {
    assert(AjiUni_Hacker(NULL) == NULL);
    AjiUni *u = AjiUni_New();
    assert(u != NULL);

    assert(AjiUni_Set(u, AJI_UNI__STR("abc")) != NULL);
    AjiUni *cp = AjiUni_Hacker(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("abc")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("ABC")) != NULL);
    cp = AjiUni_Hacker(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("abc")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("AFormatB")) != NULL);
    cp = AjiUni_Hacker(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("aformatb")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("ABFormat")) != NULL);
    cp = AjiUni_Hacker(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("abformat")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("abcDefGhi")) != NULL);
    cp = AjiUni_Hacker(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("abcdefghi")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("AbcDefGhi")) != NULL);
    cp = AjiUni_Hacker(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("abcdefghi")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("abc-def-ghi")) != NULL);
    cp = AjiUni_Hacker(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("abcdefghi")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("_abcDefGhi")) != NULL);
    cp = AjiUni_Hacker(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("abcdefghi")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("-abcDefGhi")) != NULL);
    cp = AjiUni_Hacker(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("abcdefghi")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("_-abcDefGhi")) != NULL);
    cp = AjiUni_Hacker(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("abcdefghi")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("abcDefGhi_abc-DefGhi")) != NULL);
    cp = AjiUni_Hacker(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("abcdefghiabcdefghi")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("abcDefGhi__abc--DefGhi")) != NULL);
    cp = AjiUni_Hacker(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("abcdefghiabcdefghi")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("abcDefGhi__abc--DefGhi")) != NULL);
    cp = AjiUni_Hacker(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("abcdefghiabcdefghi")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("abc0_12def_gh34i")) != NULL);
    cp = AjiUni_Hacker(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("abc012defgh34i")));
    AjiUni_Del(cp);

    AjiUni_Del(u);
}

static void
test_AjiUni_Pascal(void) {
    assert(AjiUni_Hacker(NULL) == NULL);
    AjiUni *u = AjiUni_New();
    assert(u != NULL);

    assert(AjiUni_Set(u, AJI_UNI__STR("abc")) != NULL);
    AjiUni *cp = AjiUni_Pascal(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("Abc")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("ABC")) != NULL);
    cp = AjiUni_Pascal(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("ABC")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("AFormatB")) != NULL);
    cp = AjiUni_Pascal(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("AFormatB")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("ABFormat")) != NULL);
    cp = AjiUni_Pascal(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("ABFormat")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("abcDefGhi")) != NULL);
    cp = AjiUni_Pascal(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("AbcDefGhi")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("AbcDefGhi")) != NULL);
    cp = AjiUni_Pascal(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("AbcDefGhi")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("abc-def-ghi")) != NULL);
    cp = AjiUni_Pascal(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("AbcDefGhi")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("_abcDefGhi")) != NULL);
    cp = AjiUni_Pascal(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("_AbcDefGhi")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("-abcDefGhi")) != NULL);
    cp = AjiUni_Pascal(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("-AbcDefGhi")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("_-abcDefGhi")) != NULL);
    cp = AjiUni_Pascal(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("_-AbcDefGhi")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("abcDefGhi_abc-DefGhi")) != NULL);
    cp = AjiUni_Pascal(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("AbcDefGhiAbcDefGhi")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("abcDefGhi__abc--DefGhi")) != NULL);
    cp = AjiUni_Pascal(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("AbcDefGhi_Abc-DefGhi")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("abcDefGhi__abc--DefGhi")) != NULL);
    cp = AjiUni_Pascal(u);
    assert(cp);
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("AbcDefGhi_Abc-DefGhi")));
    AjiUni_Del(cp);

    assert(AjiUni_Set(u, AJI_UNI__STR("abc0_12def_gh34i")) != NULL);
    cp = AjiUni_Pascal(u);
    assert(cp);
    puts(AjiUni_GetcMB(cp));
    assert(!AjiU_StrCmp(AjiUni_Getc(cp), AJI_UNI__STR("Abc0_12DefGh34I")));
    AjiUni_Del(cp);

    AjiUni_Del(u);
}

static void
test_AjiUni_Get(void) {
    AjiUni *u = AjiUni_New();
    assert(u);

    assert(AjiUni_Get(NULL) == NULL);

    AjiUni_SetMB(u, "abc");

    AjiUniType *s = AjiUni_Get(u);
    assert(AjiU_StrCmp(s, U"abc") == 0);

    AjiUni_Del(u);
}

static void
test_AjiUni_ToMB(void) {
    AjiUni *u = AjiUni_New();
    assert(u);

    assert(AjiUni_ToMB(NULL) == NULL);

    AjiUni_SetMB(u, "abc");

    char *s = AjiUni_ToMB(u);
    assert(strcmp(s, "abc") == 0);

    free(s);
    AjiUni_Del(u);
}

static void
test_AjiUni_SetMB(void) {
    AjiUni *u = AjiUni_New();
    assert(u);

    assert(AjiUni_SetMB(NULL, NULL) == NULL);
    assert(AjiUni_SetMB(u, NULL) == NULL);

    AjiUni_SetMB(u, "abc");

    AjiUniType *s = AjiUni_Get(u);
    assert(AjiU_StrCmp(s, U"abc") == 0);

    AjiUni_Del(u);
}

static void
test_AjiUni_GetcMB(void) {
    AjiUni *u = AjiUni_New();
    assert(u);

    assert(AjiUni_GetcMB(NULL) == NULL);

    AjiUni_SetMB(u, "abc");

    const char *s = AjiUni_GetcMB(u);
    assert(strcmp(s, "abc") == 0);

    AjiUni_Del(u);
}

static void
test_AjiUni_Mul(void) {
    AjiUni *u = AjiUni_New();
    assert(u);

    assert(AjiUni_Mul(NULL, 0) == NULL);

    AjiUni_SetMB(u, "abc");

    AjiUni *o = AjiUni_Mul(u, 3);
    const char *s = AjiUni_GetcMB(o);
    assert(strcmp(s, "abcabcabc") == 0);

    AjiUni_Del(o);
    AjiUni_Del(u);
}

static void
test_AjiUni_Split(void) {
    AjiUni *u = AjiUni_New();
    assert(u);

    AjiUni_SetMB(u, "あいう\nかきく\nさしす");
    AjiUni **vec = AjiUni_Split(u, AJI_UNI__STR("\n"));
    printf("vec[0] = [%s]\n", AjiUni_GetcMB(vec[0]));
    assert(!AjiU_StrCmp(AjiUni_Getc(vec[0]), AJI_UNI__STR("あいう")));
    assert(!AjiU_StrCmp(AjiUni_Getc(vec[1]), AJI_UNI__STR("かきく")));
    assert(!AjiU_StrCmp(AjiUni_Getc(vec[2]), AJI_UNI__STR("さしす")));
    assert(vec[3] == NULL);

    for (AjiUni **p = vec; *p; ++p) {
        AjiUni_Del(*p);
    }
    free(vec);

    AjiUni_SetMB(u, "あいう\nかきく\n");
    vec = AjiUni_Split(u, AJI_UNI__STR("\n"));
    assert(!AjiU_StrCmp(AjiUni_Getc(vec[0]), AJI_UNI__STR("あいう")));
    assert(!AjiU_StrCmp(AjiUni_Getc(vec[1]), AJI_UNI__STR("かきく")));
    assert(vec[3] == NULL);

    for (AjiUni **p = vec; *p; ++p) {
        AjiUni_Del(*p);
    }
    free(vec);

    AjiUni_SetMB(u, "あいうアイウかきくアイウ");
    vec = AjiUni_Split(u, AJI_UNI__STR("アイウ"));
    assert(!AjiU_StrCmp(AjiUni_Getc(vec[0]), AJI_UNI__STR("あいう")));
    assert(!AjiU_StrCmp(AjiUni_Getc(vec[1]), AJI_UNI__STR("かきく")));
    assert(vec[3] == NULL);

    for (AjiUni **p = vec; *p; ++p) {
        AjiUni_Del(*p);
    }
    free(vec);

    AjiUni_Del(u);
}

static void
test_AjiUni_IsDigit(void) {
    AjiUni *u = AjiUni_New();
    AjiUni_SetMB(u, "123");
    assert(AjiUni_IsDigit(u));
    AjiUni_SetMB(u, "abc");
    assert(!AjiUni_IsDigit(u));
    AjiUni_SetMB(u, "12ab");
    assert(!AjiUni_IsDigit(u));
    AjiUni_Del(u);
}

static void
test_AjiUni_IsAlpha(void) {
    AjiUni *u = AjiUni_New();
    AjiUni_SetMB(u, "123");
    assert(!AjiUni_IsAlpha(u));
    AjiUni_SetMB(u, "abc");
    assert(AjiUni_IsAlpha(u));
    AjiUni_SetMB(u, "12ab");
    assert(!AjiUni_IsAlpha(u));
    AjiUni_Del(u);
}

static void
test_AjiChar32_Len(void) {
    const char32_t *s = U"abc";
    assert(AjiChar32_Len(s) == 3);
}

static void
test_AjiChar16_Len(void) {
    const char16_t *s = u"abc";
    assert(AjiChar16_Len(s) == 3);
}

static void
test_AjiChar32_Dup(void) {
    const char32_t *s = U"abc";
    char32_t *o = AjiChar32_Dup(s);
    assert(AjiChar32_StrCmp(s, o) == 0);
    free(o);
}

static void
test_AjiChar16_Dup(void) {
    const char16_t *s = u"abc";
    char16_t *o = AjiChar16_Dup(s);
    assert(AjiChar16_StrCmp(s, o) == 0);
    free(o);
}

static void
test_AjiChar32_IsAlpha(void) {
    assert(AjiChar32_IsAlpha(U'a'));
}

static void
test_AjiChar16_IsAlpha(void) {
    assert(AjiChar16_IsAlpha(u'a'));
}

static void
test_AjiChar32_IsLower(void) {
    assert(AjiChar32_IsLower(U'a'));
    assert(!AjiChar32_IsLower(U'A'));
}

static void
test_AjiChar16_IsLower(void) {
    assert(AjiChar16_IsLower(u'a'));
    assert(!AjiChar16_IsLower(u'A'));
}

static void
test_AjiChar32_IsUpper(void) {
    assert(AjiChar32_IsUpper(U'A'));
    assert(!AjiChar32_IsUpper(U'a'));
}

static void
test_AjiChar16_IsUpper(void) {
    assert(AjiChar16_IsUpper(u'A'));
    assert(!AjiChar16_IsUpper(u'a'));
}

static void
test_AjiChar32_ToLower(void) {
    assert(AjiChar32_ToLower(U'A') == U'a');
    assert(AjiChar32_ToLower(U'a') == U'a');
}

static void
test_AjiChar16_ToLower(void) {
    assert(AjiChar16_ToLower(u'A') == u'a');
    assert(AjiChar16_ToLower(u'a') == u'a');
}

static void
test_AjiChar32_ToUpper(void) {
    assert(AjiChar32_ToUpper(U'A') == U'A');
    assert(AjiChar32_ToUpper(U'a') == U'A');
}

static void
test_AjiChar16_ToUpper(void) {
    assert(AjiChar16_ToUpper(u'A') == u'A');
    assert(AjiChar16_ToUpper(u'a') == u'A');
}

static void
test_AjiChar32_IsDigit(void) {
    assert(!AjiChar32_IsDigit(U'A'));
    assert(AjiChar32_IsDigit(U'1'));
}

static void
test_AjiChar16_IsDigit(void) {
    assert(!AjiChar16_IsDigit(u'A'));
    assert(AjiChar16_IsDigit(u'1'));
}

static void
test_AjiChar32_StrCmp(void) {
    assert(AjiChar32_StrCmp(U"abc", U"abc") == 0);
    assert(AjiChar32_StrCmp(U"abc", U"def") != 0);
}

static void
test_AjiChar16_StrCmp(void) {
    assert(AjiChar16_StrCmp(u"abc", u"abc") == 0);
    assert(AjiChar16_StrCmp(u"abc", u"def") != 0);
}

static const struct testcase
unicode_tests[] = {
    {"AjiUni_Destroy", test_AjiUni_Destroy},
    {"AjiUni_EscDestroy", test_AjiUni_EscDestroy},
    {"AjiUni_Init", test_AjiUni_Init},
    {"AjiUni_InitCStr", test_AjiUni_InitCStr},
    {"AjiUni_Del", test_AjiUni_Del},
    {"AjiUni_EscDel", test_AjiUni_EscDel},
    {"AjiUni_New", test_AjiUni_New},
    {"AjiUni_StaticDeepCopy", test_AjiUni_StaticDeepCopy},
    {"AjiUni_StaticShallowCopy", test_AjiUni_StaticShallowCopy},
    {"AjiUni_DeepCopy", test_AjiUni_DeepCopy},
    {"AjiUni_ShallowCopy", test_AjiUni_ShallowCopy},
    {"AjiUni_Swap", test_AjiUni_Swap},
    {"AjiUni_Len", test_AjiUni_Len},
    {"AjiUni_Capa", test_AjiUni_Capa},
    {"AjiUni_Getc", test_AjiUni_Getc},
    {"AjiUni_Empty", test_AjiUni_Empty},
    {"AjiUni_Clear", test_AjiUni_Clear},
    {"AjiUni_Set", test_AjiUni_Set},
    {"AjiUni_Resize", test_AjiUni_Resize},
    {"AjiUni_PushBack", test_AjiUni_PushBack},
    {"AjiUni_PopBack", test_AjiUni_PopBack},
    {"AjiUni_PushFront", test_AjiUni_PushFront},
    {"AjiUni_PopFront", test_AjiUni_PopFront},
    {"AjiUni_App", test_AjiUni_App},
    {"AjiUni_AppStream", test_AjiUni_AppStream},
    {"AjiUni_AppOther", test_AjiUni_AppOther},
    {"AjiUni_AppFmt", test_AjiUni_AppFmt},
    {"AjiUni_RStrip", test_AjiUni_RStrip},
    {"AjiUni_LStrip", test_AjiUni_LStrip},
    {"AjiUni_Strip", test_AjiUni_Strip},
    // {"uni_findc", test_uni_findc},
    {"AjiUni_Lower", test_AjiUni_Lower},
    {"AjiUni_Upper", test_AjiUni_Upper},
    {"AjiUni_Capi", test_AjiUni_Capi},
    {"AjiUni_Snake", test_AjiUni_Snake},
    {"AjiUni_Camel", test_AjiUni_Camel},
    {"AjiUni_Hacker", test_AjiUni_Hacker},
    {"AjiUni_Pascal", test_AjiUni_Pascal},
    {"AjiUni_Get", test_AjiUni_Get},
    {"AjiUni_ToMB", test_AjiUni_ToMB},
    {"AjiUni_SetMB", test_AjiUni_SetMB},
    {"AjiUni_GetcMB", test_AjiUni_GetcMB},
    {"AjiUni_Mul", test_AjiUni_Mul},
    {"AjiUni_Split", test_AjiUni_Split},
    {"AjiUni_IsDigit", test_AjiUni_IsDigit},
    {"AjiUni_IsAlpha", test_AjiUni_IsAlpha},
    {"AjiChar32_Len", test_AjiChar32_Len},
    {"AjiChar16_Len", test_AjiChar16_Len},
    {"AjiChar32_Dup", test_AjiChar32_Dup},
    {"AjiChar16_Dup", test_AjiChar16_Dup},
    {"AjiChar32_IsAlpha", test_AjiChar32_IsAlpha},
    {"AjiChar16_IsAlpha", test_AjiChar16_IsAlpha},
    {"AjiChar32_IsLower", test_AjiChar32_IsLower},
    {"AjiChar16_IsLower", test_AjiChar16_IsLower},
    {"AjiChar32_IsUpper", test_AjiChar32_IsUpper},
    {"AjiChar16_IsUpper", test_AjiChar16_IsUpper},
    {"AjiChar32_ToUpper", test_AjiChar32_ToUpper},
    {"AjiChar16_ToUpper", test_AjiChar16_ToUpper},
    {"AjiChar32_ToLower", test_AjiChar32_ToLower},
    {"AjiChar16_ToLower", test_AjiChar16_ToLower},
    {"AjiChar32_IsDigit", test_AjiChar32_IsDigit},
    {"AjiChar16_IsDigit", test_AjiChar16_IsDigit},
    {"AjiChar32_StrCmp", test_AjiChar32_StrCmp},
    {"AjiChar16_StrCmp", test_AjiChar16_StrCmp},
    {0},
};

/********
* bytes *
********/

static void
test_AJI_BYTES__INIT(void) {
    AjiBytes bytes = AJI_BYTES__INIT;
    assert(bytes.nmemb == 0);
    assert(bytes.capa == 0);
    AjiBytes_Destroy(&bytes);
}

static void
test_AjiBytes_New(void) {
    AjiBytes *bytes = AjiBytes_New();
    assert(bytes);
    assert(bytes->nmemb == 0);
    assert(bytes->capa == 0);
    AjiBytes_Del(bytes);
}

static void
test_AjiBytes_Set(void) {
    AjiBytesType buf1[4] = {1, 2, 3, 4};
    AjiBytesType buf2[4] = {5, 6, 7, 8};

    AjiBytes *bytes = AjiBytes_New();
    assert(AjiBytes_Set(bytes, buf1, 4));
    assert(bytes->buf[0] == 1);
    assert(bytes->buf[1] == 2);
    assert(bytes->buf[2] == 3);
    assert(bytes->buf[3] == 4);
    assert(AjiBytes_Set(bytes, buf2, 4));
    assert(bytes->buf[0] == 5);
    assert(bytes->buf[1] == 6);
    assert(bytes->buf[2] == 7);
    assert(bytes->buf[3] == 8);
    AjiBytes_Del(bytes);
}

static void
test_AjiBytes_App(void) {
    AjiBytesType buf1[4] = {1, 2, 3, 4};
    AjiBytesType buf2[4] = {5, 6, 7, 8};

    AjiBytes *bytes = AjiBytes_New();
    assert(AjiBytes_App(bytes, buf1, 4));
    assert(bytes->buf[0] == 1);
    assert(bytes->buf[1] == 2);
    assert(bytes->buf[2] == 3);
    assert(bytes->buf[3] == 4);
    assert(bytes->nmemb == 4);
    assert(bytes->capa == 4);
    assert(AjiBytes_App(bytes, buf2, 4));
    assert(bytes->buf[0] == 1);
    assert(bytes->buf[1] == 2);
    assert(bytes->buf[2] == 3);
    assert(bytes->buf[3] == 4);
    assert(bytes->buf[4] == 5);
    assert(bytes->buf[5] == 6);
    assert(bytes->buf[6] == 7);
    assert(bytes->buf[7] == 8);
    assert(bytes->nmemb == 8);
    assert(bytes->capa == 8);
    AjiBytes_Del(bytes);
}

static void
test_AjiBytes_PushBack(void) {
    AjiBytes *bytes = AjiBytes_New();
    assert(AjiBytes_PushBack(bytes, 1));
    assert(AjiBytes_PushBack(bytes, 2));
    assert(AjiBytes_PushBack(bytes, 3));
    assert(AjiBytes_PushBack(bytes, 4));
    assert(bytes->buf[0] == 1);
    assert(bytes->buf[1] == 2);
    assert(bytes->buf[2] == 3);
    assert(bytes->buf[3] == 4);
    assert(bytes->nmemb == 4);
    assert(bytes->capa == 4);
    AjiBytes_Del(bytes);
}

static void
test_AjiBytes_PopBack(void) {
    AjiBytesType buf1[4] = {1, 2, 3, 4};

    AjiBytes *bytes = AjiBytes_New();
    assert(AjiBytes_Set(bytes, buf1, 4));
    assert(bytes->buf[0] == 1);
    assert(bytes->buf[1] == 2);
    assert(bytes->buf[2] == 3);
    assert(bytes->buf[3] == 4);
    assert(AjiBytes_PopBack(bytes) == 4);
    assert(AjiBytes_PopBack(bytes) == 3);
    assert(AjiBytes_PopBack(bytes) == 2);
    assert(AjiBytes_PopBack(bytes) == 1);
    assert(AjiBytes_PopBack(bytes) == 0);
    AjiBytes_Del(bytes);
}

static void
test_AjiBytes_Swap(void) {
    AjiBytesType buf1[4] = {1, 2, 3, 4};

    AjiBytes b1 = AJI_BYTES__INIT;
    AjiBytes_Set(&b1, buf1, 4);
    assert(b1.buf[0] == 1);
    assert(b1.buf[1] == 2);
    assert(b1.buf[2] == 3);
    assert(b1.buf[3] == 4);    
    assert(b1.nmemb == 4);    
    assert(b1.capa == 4);    

    AjiBytes b2 = AJI_BYTES__INIT;
    assert(b2.nmemb == 0);
    assert(b2.capa == 0);

    AjiBytes_Swap(&b1, &b2);
    assert(b2.buf[0] == 1);
    assert(b2.buf[1] == 2);
    assert(b2.buf[2] == 3);
    assert(b2.buf[3] == 4);    
    assert(b2.nmemb == 4);    
    assert(b2.capa == 4);    
    assert(b1.nmemb == 0);
    assert(b1.capa == 0);

    AjiBytes_Destroy(&b1);
    AjiBytes_Destroy(&b2);
}

static void
test_AjiBytes_DeepCopy(void) {
    AjiBytesType buf1[4] = {1, 2, 3, 4};

    AjiBytes b1 = AJI_BYTES__INIT;
    AjiBytes_Set(&b1, buf1, 4);
    assert(b1.buf[0] == 1);
    assert(b1.buf[1] == 2);
    assert(b1.buf[2] == 3);
    assert(b1.buf[3] == 4);    
    assert(b1.nmemb == 4);    
    assert(b1.capa == 4);    

    AjiBytes *b2 = AjiBytes_DeepCopy(&b1);
    assert(b2->buf[0] == 1);
    assert(b2->buf[1] == 2);
    assert(b2->buf[2] == 3);
    assert(b2->buf[3] == 4);    
    assert(b2->nmemb == 4);
    assert(b2->capa == 4);

    AjiBytes_Destroy(&b1);
    AjiBytes_Del(b2);
}

static void
test_AjiBytes_StaticDeepCopy(void) {
    AjiBytesType buf1[4] = {1, 2, 3, 4};

    AjiBytes b1 = AJI_BYTES__INIT;
    AjiBytes_Set(&b1, buf1, 4);
    assert(b1.buf[0] == 1);
    assert(b1.buf[1] == 2);
    assert(b1.buf[2] == 3);
    assert(b1.buf[3] == 4);    
    assert(b1.nmemb == 4);    
    assert(b1.capa == 4);    

    AjiBytes b2 = AJI_BYTES__INIT;
    assert(AjiBytes_StaticDeepCopy(&b2, &b1));
    assert(b2.buf[0] == 1);
    assert(b2.buf[1] == 2);
    assert(b2.buf[2] == 3);
    assert(b2.buf[3] == 4);    
    assert(b2.nmemb == 4);
    assert(b2.capa == 4);

    AjiBytes_Destroy(&b1);
    AjiBytes_Destroy(&b2);
}

static void
test_AjiBytes_Eq(void) {
    AjiBytesType buf1[4] = {1, 2, 3, 4};

    AjiBytes b1 = AJI_BYTES__INIT;
    AjiBytes_Set(&b1, buf1, 4);
    assert(b1.buf[0] == 1);
    assert(b1.buf[1] == 2);
    assert(b1.buf[2] == 3);
    assert(b1.buf[3] == 4);    
    assert(b1.nmemb == 4);    
    assert(b1.capa == 4);    

    AjiBytes b2 = AJI_BYTES__INIT;
    
    assert(!AjiBytes_Eq(&b2, &b1));

    assert(AjiBytes_StaticDeepCopy(&b2, &b1));
    assert(b2.buf[0] == 1);
    assert(b2.buf[1] == 2);
    assert(b2.buf[2] == 3);
    assert(b2.buf[3] == 4);    
    assert(b2.nmemb == 4);
    assert(b2.capa == 4);

    assert(AjiBytes_Eq(&b2, &b1));

    AjiBytes_Destroy(&b1);
    AjiBytes_Destroy(&b2);
}

static const struct testcase
bytes_tests[] = {
    {"AJI_BYTES__INIT", test_AJI_BYTES__INIT},
    {"AjiBytes_New", test_AjiBytes_New},
    {"AjiBytes_Set", test_AjiBytes_Set},
    {"AjiBytes_App", test_AjiBytes_App},
    {"AjiBytes_PushBack", test_AjiBytes_PushBack},
    {"AjiBytes_PopBack", test_AjiBytes_PopBack},
    {"AjiBytes_Swap", test_AjiBytes_Swap},
    {"AjiBytes_DeepCopy", test_AjiBytes_DeepCopy},
    {"AjiBytes_StaticDeepCopy", test_AjiBytes_StaticDeepCopy},
    {"AjiBytes_Eq", test_AjiBytes_Eq},
    {0},
};

/*******
* file *
*******/

static const char *
get_test_fcontent(void) {
    return "1234567\n";
}

static const char *
get_test_fcontent_nonewline(void) {
    return "1234567";
}

static const char *
get_test_finpath(void) {
    static char path[AJI_FILE__NPATH];

#ifdef AJI_TESTS__WINDOWS
    char tmp[AJI_FILE__NPATH];
    assert(AjiFile_GetUserHome(tmp, sizeof tmp) != NULL);
    assert(AjiFile_SolveFmt(path, sizeof path, "%s/cap.test.file", tmp) != NULL);
#else
    assert(AjiFile_Solve(path, sizeof path, "/tmp/cap.test.file") != NULL);
#endif

    if (!AjiFile_IsExists(path)) {
        FILE *f = AjiFile_Open(path, "wb");
        assert(f != NULL);
        fprintf(f, "%s", get_test_fcontent());
        assert(AjiFile_Close(f) == 0);
    }
    return path;
}

static void
remove_test_file(void) {
    const char *path = get_test_finpath();
    if (AjiFile_IsExists(path)) {
        assert(AjiFile_Remove(path) == 0);
    }
}

static FILE *
get_test_fin(void) {
    FILE *fin = AjiFile_Open(get_test_finpath(), "rb");
    assert(fin != NULL);
    return fin;
}

static int
get_test_finsize(void) {
    return strlen(get_test_fcontent());
}

static const char *
get_test_dirpath(void) {
    static char path[AJI_FILE__NPATH];
#ifdef AJI_TESTS__WINDOWS
    assert(AjiFile_GetUserHome(path, sizeof path) != NULL);
#else
    assert(AjiFile_Solve(path, sizeof path, "/tmp") != NULL);
#endif
    return path;
}

static void
test_AjiFile_Close(void) {
    FILE* f = AjiFile_Open(get_test_finpath(), "rb");
    assert(f != NULL);
    assert(AjiFile_Close(NULL) != 0);
    assert(AjiFile_Close(f) == 0);
}

static void
test_AjiFile_Open(void) {
    test_AjiFile_Close();
}

static void
test_AjiFile_Copy(void) {
    FILE *f = AjiFile_Open(get_test_finpath(), "rb");
    assert(f != NULL);
    // TODO
    assert(AjiFile_Close(f) == 0);
}

static void
test_AjiFile_CloseDir(void) {
    DIR *f = AjiFile_OpenDir(get_test_dirpath());
    assert(f != NULL);
    assert(AjiFile_CloseDir(NULL) == -1);
    assert(AjiFile_CloseDir(f) == 0);
}

static void
test_AjiFile_OpenDir(void) {
    test_AjiFile_CloseDir();
}

static void
test_AjiFile_RealPath(void) {
    char path[AJI_FILE__NPATH];

    assert(AjiFile_RealPath(NULL, sizeof path, "/tmp/../tmp") == NULL);
    assert(AjiFile_RealPath(path, 0, "/tmp/../tmp") == NULL);
    assert(AjiFile_RealPath(path, sizeof path, NULL) == NULL);

    char userhome[AJI_FILE__NPATH];
    assert(AjiFile_GetUserHome(userhome, sizeof userhome));

    char src[AJI_FILE__NPATH + 5] = {0};
    snprintf(src, sizeof src, "%s%c..", userhome, AJI_FILE__SEP);
    assert(AjiFile_RealPath(path, sizeof path, src) != NULL);
}

static void
test_AjiFile_IsExists(void) {
    assert(AjiFile_IsExists(NULL) == false);
    assert(AjiFile_IsExists(get_test_dirpath()));
    assert(!AjiFile_IsExists("/nothing/directory"));
}

static void
test_AjiFile_MkdirMode(void) {
    // TODO
}

static void
test_AjiFile_MkdirQ(void) {
    assert(AjiFile_MkdirQ(NULL) != 0);
}

static void
test_AjiFile_Trunc(void) {
    char path[AJI_FILE__NPATH];
    char userhome[AJI_FILE__NPATH];
    assert(AjiFile_GetUserHome(userhome, sizeof userhome) != NULL);
    assert(AjiFile_SolveFmt(path, sizeof path, "%s/cap.ftrunc", userhome) != NULL);

    assert(!AjiFile_IsExists(path));
    assert(!AjiFile_Trunc(NULL));
    assert(AjiFile_Trunc(path));
    assert(AjiFile_IsExists(path));
    assert(AjiFile_Remove(path) == 0);
}

static void
test_AjiFile_Solve(void) {
    char path[AJI_FILE__NPATH];
    assert(AjiFile_Solve(NULL, sizeof path, "/tmp/../tmp") == NULL);
    assert(AjiFile_Solve(path, 0, "/tmp/../tmp") == NULL);
    assert(AjiFile_Solve(path, sizeof path, NULL) == NULL);
    assert(AjiFile_Solve(path, sizeof path, get_test_dirpath()) != NULL);
}

static void
test_AjiFile_SolveCopy(void) {
    assert(!AjiFile_SolveCopy(NULL));
    char *path = AjiFile_SolveCopy(get_test_dirpath());
    assert(path != NULL);
    assert(strcmp(path, get_test_dirpath()) == 0);
    free(path);
}

static void
test_AjiFile_SolveFmt(void) {
    char path[1024];
    assert(AjiFile_SolveFmt(NULL, sizeof path, "/%s/../%s", "tmp", "tmp") == NULL);
    assert(AjiFile_SolveFmt(path, 0, "/%s/../%s", "tmp", "tmp") == NULL);
    assert(AjiFile_SolveFmt(path, sizeof path, NULL, "tmp", "tmp") == NULL);
    assert(AjiFile_SolveFmt(path, sizeof path, "%s", get_test_dirpath()) != NULL);
}

static void
test_AjiFile_IsDir(void) {
    assert(!AjiFile_IsDir(NULL));
    assert(AjiFile_IsDir(get_test_dirpath()));
    assert(!AjiFile_IsDir("/not/found/directory"));
}

static void
test_AjiFile_ReadCopy(void) {
    FILE *fin = AjiFile_Open(get_test_finpath(), "rb");
    assert(fin != NULL);
    assert(!AjiFile_ReadCopy(NULL));
    char *p = AjiFile_ReadCopy(fin);
    AjiFile_Close(fin);
    assert(p != NULL);
    free(p);
}

static void
test_AjiFile_Size(void) {
    FILE *fin = AjiFile_Open(get_test_finpath(), "rb");
    assert(fin != NULL);
    assert(AjiFile_Size(NULL) == -1);
    assert(AjiFile_Size(fin) == get_test_finsize());
    assert(AjiFile_Close(fin) == 0);
}

static void
test_AjiFile_Suffix(void) {
    assert(AjiFile_Suffix(NULL) == NULL);
    const char *suf = AjiFile_Suffix("/this/is/text/file.txt");
    assert(suf != NULL);
    assert(strcmp(suf, "txt") == 0);
}

static void
test_AjiFile_DirName(void) {
    char name[AJI_FILE__NPATH];
    char userhome[AJI_FILE__NPATH];
    char path[AJI_FILE__NPATH];
    assert(AjiFile_GetUserHome(userhome, sizeof userhome));
    assert(AjiFile_SolveFmt(path, sizeof path, "%s/file", userhome));

    assert(AjiFile_DirName(NULL, sizeof name, path) == NULL);
    assert(AjiFile_DirName(name, 0, path) == NULL);
    assert(AjiFile_DirName(name, sizeof name, NULL) == NULL);
    assert(AjiFile_DirName(name, sizeof name, path) != NULL);
    assert(strcmp(name, userhome) == 0);
}

static void
test_AjiFile_BaseName(void) {
    char name[AJI_FILE__NPATH];
    char userhome[AJI_FILE__NPATH];
    char path[AJI_FILE__NPATH];
    assert(AjiFile_GetUserHome(userhome, sizeof userhome));
    assert(AjiFile_SolveFmt(path, sizeof path, "%s/file.txt", userhome));

    assert(AjiFile_BaseName(NULL, sizeof name, path) == NULL);
    assert(AjiFile_BaseName(name, 0, path) == NULL);
    assert(AjiFile_BaseName(name, sizeof name, NULL) == NULL);
    assert(AjiFile_BaseName(name, sizeof name, path) != NULL);
    assert(strcmp(name, "file.txt") == 0);
}

static void
test_AjiFile_GetLine(void) {
    FILE *fin = get_test_fin();
    assert(fin != NULL);
    char line[1024];
    assert(AjiFile_GetLine(NULL, sizeof line, fin) == EOF);
    assert(AjiFile_GetLine(line, 0, fin) == EOF);
    assert(AjiFile_GetLine(line, sizeof line, NULL) == EOF);
    assert(AjiFile_GetLine(line, sizeof line, fin) != EOF);
    assert(strcmp(get_test_fcontent_nonewline(), line) == 0);
    assert(AjiFile_Close(fin) == 0);
}

static void
test_AjiFile_ReadLine(void) {
    char line[1024];
    assert(AjiFile_ReadLine(NULL, sizeof line, get_test_finpath()) == NULL);
    assert(AjiFile_ReadLine(line, 0, get_test_finpath()) == NULL);
    assert(AjiFile_ReadLine(line, sizeof line, NULL) == NULL);
    assert(AjiFile_ReadLine(line, sizeof line, get_test_finpath()) != NULL);
    assert(strcmp(line, get_test_fcontent_nonewline()) == 0);
}

static void
test_AjiFile_WriteLine(void) {
    assert(AjiFile_WriteLine(NULL, get_test_finpath()) == NULL);
    assert(AjiFile_WriteLine(get_test_fcontent_nonewline(), NULL) == NULL);
    assert(AjiFile_WriteLine(get_test_fcontent_nonewline(), get_test_finpath()));
    test_AjiFile_ReadLine();
}

static void
test_AjiDirNode_Del(void) {
    AjiDir_Close(NULL);
    assert(AjiDir_Open(NULL) == NULL);
    assert(AjiDir_Read(NULL) == NULL);
    AjiDirNode_Del(NULL);

    struct AjiDir *dir = AjiDir_Open(get_test_dirpath());
    assert(dir != NULL);

    for (struct AjiDirNode *node; (node = AjiDir_Read(dir)); ) {
        const char *dname = AjiDirNode_Name(node);
        assert(dname != NULL);
        AjiDirNode_Del(node);
    }

    assert(AjiDir_Close(dir) == 0);
}

static void
test_AjiDirNode_Name(void) {
    // test_AjiDir_Close
}

static void
test_AjiDirNode_Type(void) {
    // test_AjiDirNode_Type
}

static void
test_AjiDir_Close(void) {
    // test_AjiDir_Close
}

static void
test_AjiDir_Open(void) {
    // test_AjiDir_Open
}

static void
test_AjiDir_Read(void) {
    // test_AjiDir_Read
}

static void
test_AjiFile_ConvLineEnc(void) {
    char *encoded;

    encoded = AjiFile_ConvLineEnc(NULL, "abc");
    assert(!encoded);

    encoded = AjiFile_ConvLineEnc("nothing", "abc");
    assert(!encoded);

    encoded = AjiFile_ConvLineEnc("crlf", NULL);
    assert(!encoded);

    encoded = AjiFile_ConvLineEnc("crlf", "abc");
    assert(encoded);
    assert(!strcmp(encoded, "abc"));
    free(encoded);

    // to crlf
    encoded = AjiFile_ConvLineEnc("crlf", "abc\r\ndef\r\n");
    assert(encoded);
    assert(!strcmp(encoded, "abc\r\ndef\r\n"));
    free(encoded);

    encoded = AjiFile_ConvLineEnc("crlf", "abc\rdef\r");
    assert(encoded);
    assert(!strcmp(encoded, "abc\r\ndef\r\n"));
    free(encoded);

    encoded = AjiFile_ConvLineEnc("crlf", "abc\ndef\n");
    assert(encoded);
    assert(!strcmp(encoded, "abc\r\ndef\r\n"));
    free(encoded);

    // to cr
    encoded = AjiFile_ConvLineEnc("cr", "abc\r\ndef\r\n");
    assert(encoded);
    assert(!strcmp(encoded, "abc\rdef\r"));
    free(encoded);

    encoded = AjiFile_ConvLineEnc("cr", "abc\rdef\r");
    assert(encoded);
    assert(!strcmp(encoded, "abc\rdef\r"));
    free(encoded);

    encoded = AjiFile_ConvLineEnc("cr", "abc\ndef\n");
    assert(encoded);
    assert(!strcmp(encoded, "abc\rdef\r"));
    free(encoded);

    // to lf
    encoded = AjiFile_ConvLineEnc("lf", "abc\r\ndef\r\n");
    assert(encoded);
    assert(!strcmp(encoded, "abc\ndef\n"));
    free(encoded);

    encoded = AjiFile_ConvLineEnc("lf", "abc\rdef\r");
    assert(encoded);
    assert(!strcmp(encoded, "abc\ndef\n"));
    free(encoded);

    encoded = AjiFile_ConvLineEnc("lf", "abc\ndef\n");
    assert(encoded);
    assert(!strcmp(encoded, "abc\ndef\n"));
    free(encoded);
}

static void
test_AjiFile_GetUserHome(void) {
    // can't test    
}

static void
test_AjiFile_Remove(void) {
    if (!AjiFile_IsExists("tests/file/")) {
        AjiFile_MkdirQ("tests/file/");
    }
    AjiFile_Trunc("tests/file/remove.txt");
    assert(AjiFile_IsExists("tests/file/remove.txt"));
    AjiFile_Remove("tests/file/remove.txt");
    assert(!AjiFile_IsExists("tests/file/remove.txt"));
}

static void
test_AjiFile_Rename(void) {
    if (!AjiFile_IsExists("tests/file/")) {
        AjiFile_MkdirQ("tests/file/");
    }
    AjiFile_Trunc("tests/file/rename.txt");
    assert(AjiFile_IsExists("tests/file/rename.txt"));
    AjiFile_Rename("tests/file/rename.txt", "tests/file/renamed.txt");
    assert(AjiFile_IsExists("tests/file/renamed.txt"));
    AjiFile_Remove("tests/file/renamed.txt");
}

static void
test_AjiFile_ReadLines(void) {
    if (!AjiFile_IsExists("tests/file/")) {
        AjiFile_MkdirQ("tests/file/");
    }
    FILE *fout = fopen("tests/file/lines.txt", "wt");
    assert(fout);
    fputs("123\n", fout);
    fputs("223\n", fout);
    fputs("323\n", fout);
    fclose(fout);

    char **lines = AjiFile_ReadLines("tests/file/lines.txt");
    assert(lines);
    assert(!strcmp(lines[0], "123"));
    assert(!strcmp(lines[1], "223"));
    assert(!strcmp(lines[2], "323"));
    assert(lines[3] == NULL);

    AjiFile_Remove("tests/file/lines.txt");

    for (char **p = lines; *p; p += 1) {
        free(*p);
    }
    free(lines);
}

/**
 * 0 memory leaks
 * 2020/02/25
 */
static const struct testcase
file_tests[] = {
    {"AjiFile_Close", test_AjiFile_Close},
    {"AjiFile_Open", test_AjiFile_Open},
    {"AjiFile_Copy", test_AjiFile_Copy},
    {"AjiFile_CloseDir", test_AjiFile_CloseDir},
    {"AjiFile_OpenDir", test_AjiFile_OpenDir},
    {"AjiFile_RealPath", test_AjiFile_RealPath},
    {"AjiFile_IsExists", test_AjiFile_IsExists},
    {"AjiFile_MkdirMode", test_AjiFile_MkdirMode},
    {"AjiFile_MkdirQ", test_AjiFile_MkdirQ},
    {"AjiFile_Trunc", test_AjiFile_Trunc},
    {"AjiFile_Solve", test_AjiFile_Solve},
    {"AjiFile_SolveCopy", test_AjiFile_SolveCopy},
    {"AjiFile_SolveFmt", test_AjiFile_SolveFmt},
    {"AjiFile_IsDir", test_AjiFile_IsDir},
    {"AjiFile_ReadCopy", test_AjiFile_ReadCopy},
    {"AjiFile_Size", test_AjiFile_Size},
    {"AjiFile_Suffix", test_AjiFile_Suffix},
    {"AjiFile_DirName", test_AjiFile_DirName},
    {"AjiFile_BaseName", test_AjiFile_BaseName},
    {"AjiFile_GetLine", test_AjiFile_GetLine},
    {"AjiFile_ReadLine", test_AjiFile_ReadLine},
    {"AjiFile_WriteLine", test_AjiFile_WriteLine},
    {"AjiDirNode_Del", test_AjiDirNode_Del},
    {"AjiDirNode_Name", test_AjiDirNode_Name},
    {"AjiDirNode_Type", test_AjiDirNode_Type},
    {"AjiDir_Close", test_AjiDir_Close},
    {"AjiDir_Open", test_AjiDir_Open},
    {"AjiDir_Read", test_AjiDir_Read},
    {"AjiFile_ConvLineEnc", test_AjiFile_ConvLineEnc},
    {"AjiFile_GetUserHome", test_AjiFile_GetUserHome},
    {"AjiFile_Remove", test_AjiFile_Remove},
    {"AjiFile_Rename", test_AjiFile_Rename},
    {"AjiFile_ReadLines", test_AjiFile_ReadLines},
    {0},
};

/*****
* cl *
*****/

static void
test_AjiCL_Del(void) {
    AjiCL *cl = AjiCL_New();
    assert(cl != NULL);
    AjiCL_Del(cl);
}

static void
test_AjiCL_EscDel(void) {
    AjiCL *cl = AjiCL_New();
    assert(cl != NULL);
    size_t pveclen = AjiCL_Len(cl);
    char **pvec = AjiCL_EscDel(cl);
    assert(pvec != NULL);
    Aji_FreeArgv(pveclen, pvec);
}

static void
test_AjiCL_New(void) {
    // test_AjiCL_Del
}

static void
test_AjiCL_Resize(void) {
    AjiCL *cl = AjiCL_New();
    assert(cl != NULL);
    assert(AjiCL_Capa(cl) == 4);
    assert(AjiCL_Resize(cl, 8));
    assert(AjiCL_Capa(cl) == 8);
    AjiCL_Del(cl);
}

static void
test_AjiCL_PushBack(void) {
    AjiCL *cl = AjiCL_New();
    assert(cl != NULL);
    assert(AjiCL_Len(cl) == 0);
    assert(AjiCL_PushBack(cl, "123"));
    assert(AjiCL_PushBack(cl, "223"));
    assert(AjiCL_PushBack(cl, "323"));
    assert(strcmp(AjiCL_Getc(cl, 1), "223") == 0);
    assert(AjiCL_Len(cl) == 3);
    AjiCL_Del(cl);
}

static void
test_AjiCL_Getc(void) {
    AjiCL *cl = AjiCL_New();
    assert(cl != NULL);
    assert(AjiCL_PushBack(cl, "123"));
    assert(strcmp(AjiCL_Getc(cl, 0), "123") == 0);
    AjiCL_Del(cl);
}

static void
test_AjiCL_Clear(void) {
    AjiCL *cl = AjiCL_New();
    assert(cl != NULL);
    assert(AjiCL_PushBack(cl, "123"));
    assert(AjiCL_PushBack(cl, "223"));
    assert(AjiCL_Len(cl) == 2);
    AjiCL_Clear(cl);
    assert(AjiCL_Len(cl) == 0);
    AjiCL_Del(cl);
}

static void
test_AjiCL_ParseStrOpts(void) {
    AjiCL *cl = AjiCL_New();
    assert(cl != NULL);

    assert(AjiCL_ParseStrOpts(cl, "cmd -h -ab 123 --help 223", AJI_CL__WRAP));
    assert(strcmp(AjiCL_Getc(cl, 0), "'cmd'") == 0);
    assert(strcmp(AjiCL_Getc(cl, 1), "'-h'") == 0);
    assert(strcmp(AjiCL_Getc(cl, 2), "'-ab'") == 0);
    assert(strcmp(AjiCL_Getc(cl, 3), "'123'") == 0);
    assert(strcmp(AjiCL_Getc(cl, 4), "'--help'") == 0);
    assert(strcmp(AjiCL_Getc(cl, 5), "'223'") == 0);

    assert(AjiCL_ParseStrOpts(cl, "cmd -a 123", AJI_CL__WRAP));
    assert(strcmp(AjiCL_Getc(cl, 0), "'cmd'") == 0);
    assert(strcmp(AjiCL_Getc(cl, 1), "'-a'") == 0);
    assert(strcmp(AjiCL_Getc(cl, 2), "'123'") == 0);

    assert(AjiCL_ParseStrOpts(cl, "\"cmd\" \"-a\" \"123\"", AJI_CL__WRAP));
    assert(strcmp(AjiCL_Getc(cl, 0), "'cmd'") == 0);
    assert(strcmp(AjiCL_Getc(cl, 1), "'-a'") == 0);
    assert(strcmp(AjiCL_Getc(cl, 2), "'123'") == 0);

    assert(AjiCL_ParseStrOpts(cl, "\"cmd\" \"-a\" \"123\"", AJI_CL__WRAP));
    assert(strcmp(AjiCL_Getc(cl, 0), "'cmd'") == 0);
    assert(strcmp(AjiCL_Getc(cl, 1), "'-a'") == 0);
    assert(strcmp(AjiCL_Getc(cl, 2), "'123'") == 0);

    assert(AjiCL_ParseStrOpts(cl, "cmd -a 123", AJI_CL__ESCAPE));
    assert(strcmp(AjiCL_Getc(cl, 0), "cmd") == 0);
    assert(strcmp(AjiCL_Getc(cl, 1), "-a") == 0);
    assert(strcmp(AjiCL_Getc(cl, 2), "123") == 0);

    assert(AjiCL_ParseStrOpts(cl, "cmd -a \"1'23\"", AJI_CL__ESCAPE));
    assert(strcmp(AjiCL_Getc(cl, 0), "cmd") == 0);
    assert(strcmp(AjiCL_Getc(cl, 1), "-a") == 0);
    assert(strcmp(AjiCL_Getc(cl, 2), "1\\'23") == 0);

    AjiCL_Del(cl);
}

static void
test_AjiCL_ParseStr(void) {
    AjiCL *cl = AjiCL_New();
    assert(cl != NULL);

    assert(AjiCL_ParseStr(cl, "cmd -h -ab 123 --help 223"));
    assert(strcmp(AjiCL_Getc(cl, 0), "cmd") == 0);
    assert(strcmp(AjiCL_Getc(cl, 1), "-h") == 0);
    assert(strcmp(AjiCL_Getc(cl, 2), "-ab") == 0);
    assert(strcmp(AjiCL_Getc(cl, 3), "123") == 0);
    assert(strcmp(AjiCL_Getc(cl, 4), "--help") == 0);
    assert(strcmp(AjiCL_Getc(cl, 5), "223") == 0);

    assert(AjiCL_ParseStr(cl, "cmd -a \"abc\""));
    assert(strcmp(AjiCL_Getc(cl, 0), "cmd") == 0);
    assert(strcmp(AjiCL_Getc(cl, 1), "-a") == 0);
    assert(strcmp(AjiCL_Getc(cl, 2), "abc") == 0);

    assert(AjiCL_ParseStr(cl, "cmd -a 'a\"bc'"));
    assert(strcmp(AjiCL_Getc(cl, 0), "cmd") == 0);
    assert(strcmp(AjiCL_Getc(cl, 1), "-a") == 0);
    assert(strcmp(AjiCL_Getc(cl, 2), "a\"bc") == 0);

    assert(AjiCL_ParseStr(cl, "cmd -a=abc"));
    assert(strcmp(AjiCL_Getc(cl, 0), "cmd") == 0);
    assert(strcmp(AjiCL_Getc(cl, 1), "-a") == 0);
    assert(strcmp(AjiCL_Getc(cl, 2), "abc") == 0);

    assert(AjiCL_ParseStr(cl, "cmd -a=\"abc\""));
    assert(strcmp(AjiCL_Getc(cl, 0), "cmd") == 0);
    assert(strcmp(AjiCL_Getc(cl, 1), "-a") == 0);
    assert(strcmp(AjiCL_Getc(cl, 2), "abc") == 0);

    assert(AjiCL_ParseStr(cl, "cmd -a='abc'"));
    assert(strcmp(AjiCL_Getc(cl, 0), "cmd") == 0);
    assert(strcmp(AjiCL_Getc(cl, 1), "-a") == 0);
    assert(strcmp(AjiCL_Getc(cl, 2), "abc") == 0);

    assert(AjiCL_ParseStr(cl, "cmd \"-a\"=\"abc\""));
    assert(strcmp(AjiCL_Getc(cl, 0), "cmd") == 0);
    assert(strcmp(AjiCL_Getc(cl, 1), "-a") == 0);
    assert(strcmp(AjiCL_Getc(cl, 2), "abc") == 0);

    assert(AjiCL_ParseStr(cl, "cmd '-a'='abc'"));
    assert(strcmp(AjiCL_Getc(cl, 0), "cmd") == 0);
    assert(strcmp(AjiCL_Getc(cl, 1), "-a") == 0);
    assert(strcmp(AjiCL_Getc(cl, 2), "abc") == 0);

    assert(AjiCL_ParseStr(cl, "cmd --abc=abc"));
    assert(strcmp(AjiCL_Getc(cl, 0), "cmd") == 0);
    assert(strcmp(AjiCL_Getc(cl, 1), "--abc") == 0);
    assert(strcmp(AjiCL_Getc(cl, 2), "abc") == 0);

    assert(AjiCL_ParseStr(cl, "cmd --abc=\"abc\""));
    assert(strcmp(AjiCL_Getc(cl, 0), "cmd") == 0);
    assert(strcmp(AjiCL_Getc(cl, 1), "--abc") == 0);
    assert(strcmp(AjiCL_Getc(cl, 2), "abc") == 0);

    assert(AjiCL_ParseStr(cl, "cmd --abc='abc'"));
    assert(strcmp(AjiCL_Getc(cl, 0), "cmd") == 0);
    assert(strcmp(AjiCL_Getc(cl, 1), "--abc") == 0);
    assert(strcmp(AjiCL_Getc(cl, 2), "abc") == 0);

    assert(AjiCL_ParseStr(cl, "cmd \"--abc\"=\"abc\""));
    assert(strcmp(AjiCL_Getc(cl, 0), "cmd") == 0);
    assert(strcmp(AjiCL_Getc(cl, 1), "--abc") == 0);
    assert(strcmp(AjiCL_Getc(cl, 2), "abc") == 0);

    assert(AjiCL_ParseStr(cl, "cmd '--abc'='abc'"));
    assert(strcmp(AjiCL_Getc(cl, 0), "cmd") == 0);
    assert(strcmp(AjiCL_Getc(cl, 1), "--abc") == 0);
    assert(strcmp(AjiCL_Getc(cl, 2), "abc") == 0);

    assert(AjiCL_ParseStr(cl, "cat \\blog\\file.txt --tabspaces 4"));
    assert(strcmp(AjiCL_Getc(cl, 0), "cat") == 0);
    assert(strcmp(AjiCL_Getc(cl, 1), "\\blog\\file.txt") == 0);
    assert(strcmp(AjiCL_Getc(cl, 2), "--tabspaces") == 0);
    assert(strcmp(AjiCL_Getc(cl, 3), "4") == 0);

    AjiCL_Del(cl);
}

static void
test_AjiCL_ParseArgvOpts(void) {
    AjiCL *cl = AjiCL_New();
    assert(cl != NULL);

    AjiCL_Del(cl);
}

static void
test_AjiCL_ParseArgv(void) {
    AjiCL *cl = AjiCL_New();
    assert(cl != NULL);

    AjiCL_Del(cl);
}

static void
test_AjiCL_Show(void) {
    AjiCL *cl = AjiCL_New();
    assert(cl != NULL);

    AjiCL_Del(cl);
}

static void
test_AjiCL_Len(void) {
    AjiCL *cl = AjiCL_New();
    assert(cl != NULL);

    AjiCL_Del(cl);
}

static void
test_AjiCL_Capa(void) {
    AjiCL *cl = AjiCL_New();
    assert(cl);

    assert(AjiCL_Capa(cl) == 4);

    AjiCL_Del(cl);
}

static void
test_AjiCL_GetArgv(void) {
    AjiCL *cl = AjiCL_New();
    assert(cl);

    AjiCL_PushBack(cl, "abc");
    AjiCL_PushBack(cl, "def");

    char **argv = AjiCL_GetArgv(cl);
    assert(!strcmp(argv[0], "abc"));
    assert(!strcmp(argv[1], "def"));
    assert(argv[2] == NULL);

    AjiCL_Del(cl);
}

static void
test_AjiCL_GenStr(void) {
    AjiCL *cl = AjiCL_New();
    assert(cl);

    AjiCL_PushBack(cl, "abc");
    AjiCL_PushBack(cl, "def");
    AjiCL_PushBack(cl, "123");

    char *s = AjiCL_GenStr(cl);
    assert(!strcmp(s, "\"abc\" \"def\" \"123\""));

    free(s);
    AjiCL_Del(cl);
}

static const struct testcase
cl_tests[] = {
    {"AjiCL_Del", test_AjiCL_Del},
    {"AjiCL_EscDel", test_AjiCL_EscDel},
    {"AjiCL_New", test_AjiCL_New},
    {"AjiCL_Resize", test_AjiCL_Resize},
    {"AjiCL_Getc", test_AjiCL_Getc},
    {"AjiCL_PushBack", test_AjiCL_PushBack},
    {"AjiCL_Clear", test_AjiCL_Clear},
    {"AjiCL_ParseStrOpts", test_AjiCL_ParseStrOpts},
    {"AjiCL_ParseStr", test_AjiCL_ParseStr},
    {"cl_parseargvopts", test_AjiCL_ParseArgvOpts},
    {"cl_parseargv", test_AjiCL_ParseArgv},
    {"AjiCL_Show", test_AjiCL_Show},
    {"AjiCL_Len", test_AjiCL_Len},
    {"AjiCL_Capa", test_AjiCL_Capa},
    {"AjiCL_GetArgv", test_AjiCL_GetArgv},
    {"AjiCL_GenStr", test_AjiCL_GenStr},
    {0},
};

/********
* error *
********/

static void
test_error_fix_text_1(void) {
    char buf[BUFSIZ] = {0};

    AjiErr_FixTxt(buf, sizeof buf, "text");
    assert(!strcmp(buf, "Text."));
    buf[0] = '\0';

    AjiErr_FixTxt(buf, sizeof buf, "file.text");
    assert(!strcmp(buf, "file.text."));
    buf[0] = '\0';

    AjiErr_FixTxt(buf, sizeof buf, "file...");
    assert(!strcmp(buf, "File..."));
    buf[0] = '\0';

    AjiErr_FixTxt(buf, sizeof buf, "the file...");
    assert(!strcmp(buf, "The file..."));
    buf[0] = '\0';

    AjiErr_FixTxt(buf, sizeof buf, "the file...test");
    assert(!strcmp(buf, "The file...test."));
    buf[0] = '\0';

    AjiErr_FixTxt(buf, sizeof buf, "the file... test string");
    assert(!strcmp(buf, "The file... Test string."));
    buf[0] = '\0';

    AjiErr_FixTxt(buf, sizeof buf, "text. text");
    assert(!strcmp(buf, "Text. Text."));
    buf[0] = '\0';

    AjiErr_FixTxt(buf, sizeof buf, "text.     text");
    assert(!strcmp(buf, "Text. Text."));
    buf[0] = '\0';

    AjiErr_FixTxt(buf, sizeof buf, "Failed to open directory \"/path/to/dir\". failed to remove recursive.");
    assert(!strcmp(buf, "Failed to open directory \"/path/to/dir\". Failed to remove recursive."));
    buf[0] = '\0';

    AjiErr_FixTxt(buf, sizeof buf, "src/core/error_stack.c");
    assert(!strcmp(buf, "src/core/error_stack.c."));
    buf[0] = '\0';

    AjiErr_FixTxt(buf, sizeof buf, "newline\n");
    puts(buf);
    assert(!strcmp(buf, "Newline\n."));
    buf[0] = '\0';
}

static void
test_error__log(void) {
    char buf[BUFSIZ] = {0};
    setbuf(stderr, buf);

    AjiErr_LogUnsafe("file", 100, "func", "warn", "msg");
    // assert(strcmp(buf, "")); // TODO

    setbuf(stderr, NULL);
}

static void
test_error_die(void) {
    // nothing todo
}

static void
test_error_error_1(void) {
    char buf[BUFSIZ] = {0};
    setbuf(stderr, buf);

    AjiErr_Err("this is error");
    // assert(strcmp(buf, "Error: This is error. No such file or directory.\n") == 0);

    setbuf(stderr, NULL);
}

static void
test_error_error_2(void) {
    AjiErr_Err("test1");
    AjiErr_Err("test2");
    AjiErr_Err("test3");
}

/**
 * 0 memory leaks
 * 2020/02/25
 */
static const struct testcase
error_tests[] = {
    {"fix_text_1", test_error_fix_text_1},
    {"_log", test_error__log},
    {"die", test_error_die},
    {"error_1", test_error_error_1},
    {"error_2", test_error_error_2},
    {0},
};

/*******
* util *
*******/

static char **
__create_testargv(int argc) {
    char **argv = AjiMem_Calloc(argc+1, sizeof(char*));
    assert(argv != NULL);

    for (int i = 0; i < argc; ++i) {
        argv[i] = AjiCStr_EDup("abc");
    }

    return argv;
}

static void
test_util_Aji_FreeArgv(void) {
    int argc = 2;
    char **argv = __create_testargv(argc);
    assert(argv != NULL);
    Aji_FreeArgv(argc, argv);
}

static void
test_util_Aji_ShowArgv(void) {
    // TODO
    // this test was failed
    return;

    char buf[1024] = {0};

    int argc = 2;
    char **argv = __create_testargv(argc);
    assert(argv != NULL);

    setbuf(stdout, buf);
    Aji_ShowArgv(argc, argv);
    setbuf(stdout, NULL);

    assert(!strcmp(buf, "abc\nabc\n"));

    Aji_FreeArgv(argc, argv);
}

static void
test_util_Aji_RandRange(void) {
    int min = 0;
    int max = 10;
    int n = Aji_RandRange(min, max);
    for (int i = min; i < max; ++i) {
        if (n == i) {
            return;
        }
    }

    assert(0 && "invalid value range");
}

static void
test_util_Aji_SafeSystem(void) {
    char cmd[1024];
#ifdef AJI_TESTS__WINDOWS
    assert(AjiFile_SolveFmt(cmd, sizeof cmd, "dir") != NULL);
#else
    const char *path = "/tmp/f";
    if (AjiFile_IsExists(path)) {
        assert(remove(path) == 0);
    }
    printf("path[%s]\n", path);
    snprintf(cmd, sizeof cmd, "/bin/sh -c \"touch %s\"", path);
    printf("cmd[%s]\n", cmd);
    assert(Aji_SafeSystem(cmd, AJI_SAFESYSTEM__DEFAULT) == 0);
    assert(AjiFile_IsExists(path));
#endif
}

static void
test_util_Aji_ArgsByOptind(void) {
    char *argv[] = {
        "program",
        "arg1",
        "-a",
        "arg2",
        "-b",
        "barg",
        NULL,
    };
    int argc = 0;
    for (; argv[argc]; ++argc) {
    }

    struct option longopts[] = {
        {"opt1", no_argument, 0, 'a'},
        {"opt2", required_argument, 0, 'b'},
        {0},
    };
    const char *shortopts = "ab:";
    opterr = 0;
    optind = 0;

    for (;;) {
        int optsindex;
        int cur = getopt_long(argc, argv, shortopts, longopts, &optsindex);
        if (cur == -1) {
            break;
        }
    }

    AjiCStrVec *args = Aji_ArgsByOptind(argc, argv, optind);
    // AjiCStrVec_Show(args, stdout);
    assert(strcmp(AjiCStrVec_Getc(args, 0), "program") == 0);
    assert(strcmp(AjiCStrVec_Getc(args, 1), "arg1") == 0);
    assert(strcmp(AjiCStrVec_Getc(args, 2), "arg2") == 0);
    AjiCStrVec_Del(args);
}

static void
test_util_Aji_EscapeText(void) {
    AjiStr *s = AjiStr_New();

    assert(Aji_EscapeText(s, "\n\t\r", "\r"));
    assert(!strcmp(AjiStr_Getc(s), "\\n\\t\r"));

    AjiStr_Del(s);
}

static void
test_util_Aji_PopTailSlash(void) {
    char s[100];
#ifdef AJI_TESTS__WINDOWS
    strcpy(s, "C:\\path\\to\\dir\\");
    assert(Aji_PopTailSlash(s));
    assert(!strcmp(s, "C:\\path\\to\\dir"));

    strcpy(s, "C:\\path\\to\\dir");
    assert(Aji_PopTailSlash(s));
    assert(!strcmp(s, "C:\\path\\to\\dir"));

    strcpy(s, "C:\\");
    assert(Aji_PopTailSlash(s));
    assert(!strcmp(s, "C:\\"));
#else
    strcpy(s, "/path/to/dir/");
    assert(Aji_PopTailSlash(s));
    assert(!strcmp(s, "/path/to/dir"));

    strcpy(s, "/path/to/dir");
    assert(Aji_PopTailSlash(s));
    assert(!strcmp(s, "/path/to/dir"));

    strcpy(s, "/");
    assert(Aji_PopTailSlash(s));
    assert(!strcmp(s, "/"));
#endif
}

static void
test_util_Aji_TrimFirstLine(void) {
    char dst[100];
    const char *lines = "aaa\nbbb\nccc\n";

    Aji_TrimFirstLine(dst, sizeof dst, lines);
    assert(!strcmp(dst, "aaa"));
}

static void
test_util_Aji_ClearScreen(void) {
    // nothing todo
}

static void
test_util_Aji_SplitToVec(void) {
    assert(Aji_SplitToVec(NULL, 0) == NULL);

    AjiCStrVec *vec = Aji_SplitToVec("abc:def:ghi", ':');
    assert(vec);
    assert(AjiCStrVec_Len(vec) == 3);
    assert(!strcmp(AjiCStrVec_Getc(vec, 0), "abc"));
    assert(!strcmp(AjiCStrVec_Getc(vec, 1), "def"));
    assert(!strcmp(AjiCStrVec_Getc(vec, 2), "ghi"));
    AjiCStrVec_Del(vec);

    vec = Aji_SplitToVec("abc:def:ghi:", ':');
    assert(vec);
    assert(AjiCStrVec_Len(vec) == 3);
    assert(!strcmp(AjiCStrVec_Getc(vec, 0), "abc"));
    assert(!strcmp(AjiCStrVec_Getc(vec, 1), "def"));
    assert(!strcmp(AjiCStrVec_Getc(vec, 2), "ghi"));
    AjiCStrVec_Del(vec);
}

static void
test_util_Aji_PushFrontArgv(void) {
    int argc = 2;
    char *argv[] = {"aaa", "bbb", NULL};
    AjiCStrVec *vec = Aji_PushFrontArgv(argc, argv, "ccc");
    assert(AjiCStrVec_Len(vec) == 3);
    assert(!strcmp(AjiCStrVec_Getc(vec, 0), "ccc"));
    assert(!strcmp(AjiCStrVec_Getc(vec, 1), "aaa"));
    assert(!strcmp(AjiCStrVec_Getc(vec, 2), "bbb"));
    AjiCStrVec_Del(vec);
}

static void
test_util_Aji_IsDotFile(void) {
    assert(Aji_IsDotFile("."));
    assert(Aji_IsDotFile(".."));
}

static const struct testcase
util_tests[] = {
    {"Aji_FreeArgv", test_util_Aji_FreeArgv},
    {"Aji_ShowArgv", test_util_Aji_ShowArgv},
    {"Aji_RandRange", test_util_Aji_RandRange},
    {"Aji_SafeSystem", test_util_Aji_SafeSystem},
    {"Aji_ArgsByOptind", test_util_Aji_ArgsByOptind},
    {"Aji_EscapeText", test_util_Aji_EscapeText},
    {"Aji_PopTailSlash", test_util_Aji_PopTailSlash},
    {"Aji_TrimFirstLine", test_util_Aji_TrimFirstLine},
    {"Aji_ClearScreen", test_util_Aji_ClearScreen},
    {"Aji_SplitToVec", test_util_Aji_SplitToVec},
    {"Aji_PushFrontArgv", test_util_Aji_PushFrontArgv},
    {"Aji_IsDotFile", test_util_Aji_IsDotFile},
    {0},
};

/*******
* path *
*******/

static void
test_AjiPath_PopBackOf(void) {
    char s[100];

    assert(AjiPath_PopBackOf(NULL, '?') == NULL);

    strcpy(s, "abc");
    assert(AjiPath_PopBackOf(s, 'c'));
    assert(!strcmp(s, "ab"));

    assert(AjiPath_PopBackOf(s, '?'));
    assert(!strcmp(s, "ab"));
}

static void
test_AjiPath_PopTailSlash(void) {
    char s[100];

    assert(AjiPath_PopTailSlash(NULL) == NULL);

#ifdef AJI_TESTS__WINDOWS
    strcpy(s, "C:\\path\\to\\dir\\");
    assert(AjiPath_PopTailSlash(s));
    assert(!strcmp(s, "C:\\path\\to\\dir"));

    strcpy(s, "C:\\path\\to\\dir");
    assert(AjiPath_PopTailSlash(s));
    assert(!strcmp(s, "C:\\path\\to\\dir"));
#else
    strcpy(s, "/path/to/dir/");
    assert(AjiPath_PopTailSlash(s));
    assert(!strcmp(s, "/path/to/dir"));

    strcpy(s, "/path/to/dir");
    assert(AjiPath_PopTailSlash(s));
    assert(!strcmp(s, "/path/to/dir"));
#endif
}

static const struct testcase
path_tests[] = {
    {"AjiPath_PopBackOf", test_AjiPath_PopBackOf},
    {"AjiPath_PopTailSlash", test_AjiPath_PopTailSlash},
    {0},
};

/*******************
* lib/unicode_path *
*******************/

static void 
test_AjiUniPath_default(void) {
    AjiUniPath *p = AjiUniPath_New();

#ifdef AJI_TESTS__WINDOWS
    assert(AjiUniPath_SetMB(p, "path\\to\\file"));
    assert(!strcmp(AjiUniPath_GetcMB(p), "path\\to\\file"));

    assert(AjiUniPath_Len(p));
    AjiUniPath_Clear(p);
    assert(AjiUniPath_Len(p) == 0);

    AjiUniPath_SetMB(p, "path");
    AjiCStrVec *vec = AjiCStrVec_New();
    AjiCStrVec_PushBack(vec, "to");
    AjiCStrVec_PushBack(vec, "the");
    AjiCStrVec_PushBack(vec, "file");
    AjiUniPath_JoinCStrVec(p, vec);
    assert(!strcmp(AjiUniPath_GetcMB(p), "path\\to\\the\\file"));

    AjiUniPath_Clear(p);
    AjiCStrVec_Clear(vec);
    AjiCStrVec_PushBack(vec, "path");
    AjiCStrVec_PushBack(vec, "to");
    AjiCStrVec_PushBack(vec, "file");
    AjiUniPath_JoinCStrVec(p, vec);
    assert(!strcmp(AjiUniPath_GetcMB(p), "path\\to\\file"));

    AjiUniPath_Clear(p);
    AjiCStrVec_Clear(vec);
    AjiUniPath_JoinCStrVec(p, vec);
    assert(!strcmp(AjiUniPath_GetcMB(p), ""));
#else
    assert(AjiUniPath_SetMB(p, "path\\to\\file"));
    assert(!strcmp(AjiUniPath_GetcMB(p), "path\\to\\file"));

    assert(AjiUniPath_Len(p));
    AjiUniPath_Clear(p);
    assert(AjiUniPath_Len(p) == 0);

    AjiUniPath_SetMB(p, "path");
    AjiCStrVec *vec = AjiCStrVec_New();
    AjiCStrVec_PushBack(vec, "to");
    AjiCStrVec_PushBack(vec, "the");
    AjiCStrVec_PushBack(vec, "file");
    AjiUniPath_JoinCStrVec(p, vec);
    assert(!strcmp(AjiUniPath_GetcMB(p), "path/to/the/file"));

    AjiUniPath_Clear(p);
    AjiCStrVec_Clear(vec);
    AjiCStrVec_PushBack(vec, "path");
    AjiCStrVec_PushBack(vec, "to");
    AjiCStrVec_PushBack(vec, "file");
    AjiUniPath_JoinCStrVec(p, vec);
    assert(!strcmp(AjiUniPath_GetcMB(p), "path/to/file"));

    AjiUniPath_Clear(p);
    AjiCStrVec_Clear(vec);
    AjiUniPath_JoinCStrVec(p, vec);
    assert(!strcmp(AjiUniPath_GetcMB(p), ""));
#endif

    AjiCStrVec_Del(vec);
    AjiUniPath_Del(p);
}

static const struct testcase
unicode_path_tests[] = {
    {"AjiUniPath_default", test_AjiUniPath_default},
    {0},
};


/*****************
* lang/tokenizer *
*****************/

static void
test_AjiTkr_New(void) {
    AjiTkrOpt *opt = AjiTkrOpt_New();
    AjiTkr *tkr = AjiTkr_New(opt);
    AjiTkr_Del(tkr);
}

static void
test_AjiTkr_Parse(void) {
    AjiTkrOpt *opt = AjiTkrOpt_New();
    AjiTkr *tkr = AjiTkr_New(opt);
    const AjiTok *token;

    AjiTkr_Parse(tkr, "abc");
    {
        assert(AjiTkr_ToksLen(tkr) == 1);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__TEXT_BLOCK);
        assert(strcmp(token->text, "abc") == 0);
    }

    AjiTkr_Parse(tkr, "abc{@@}bbc");
    {
        assert(AjiTkr_ToksLen(tkr) == 4);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__TEXT_BLOCK);
        assert(strcmp(token->text, "abc") == 0);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 3);
        assert(token->type == AJI_TOK_TYPE__TEXT_BLOCK);
        assert(strcmp(token->text, "bbc") == 0);
    }

    // test of AjiMemman_Realloc of tokens
    AjiTkr_Parse(tkr, "{@......@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 8);
    }

    AjiTkr_Parse(tkr, "");
    {
        assert(AjiTkr_HasErrStack(tkr) == false);
        assert(AjiTkr_ToksLen(tkr) == 0);
    }

    AjiTkr_Parse(tkr, "{@");
    {
        assert(AjiTkr_ToksLen(tkr) == 1);
        assert(AjiTkr_HasErrStack(tkr) == true);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@@");
    {
        assert(AjiTkr_HasErrStack(tkr) == true);
        assert(strcmp(AjiTkr_GetcFirstErrMsg(tkr), "invalid syntax. single '@' is not supported") == 0);
    }

    AjiTkr_Parse(tkr, "{@@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 2);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@.@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__DOT_OPE);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@..@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 4);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__DOT_OPE);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__DOT_OPE);
        token = AjiTkr_ToksGetc(tkr, 3);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@,@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__COMMA);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@,,@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 4);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__COMMA);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__COMMA);
        token = AjiTkr_ToksGetc(tkr, 3);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@:@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__COLON);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@;@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__SEMICOLON);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@(@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__LPAREN);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@)@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__RPAREN);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@[@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__LBRACKET);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@]@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__RBRACKET);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@{@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__LBRACE);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@}@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__RBRACE);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@()@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 4);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__LPAREN);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RPAREN);
        token = AjiTkr_ToksGetc(tkr, 3);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@a@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__IDENTIFIER);
        assert(strcmp(token->text, "a") == 0);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@abc@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__IDENTIFIER);
        assert(strcmp(token->text, "abc") == 0);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@abc123@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__IDENTIFIER);
        assert(strcmp(token->text, "abc123") == 0);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@abc_123@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__IDENTIFIER);
        assert(strcmp(token->text, "abc_123") == 0);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@123@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__INTEGER);
        assert(token->lvalue == 123);
        assert(strcmp(token->text, "123") == 0);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@-123@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 4);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__OP_SUB);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__INTEGER);
        assert(token->lvalue == 123);
        token = AjiTkr_ToksGetc(tkr, 3);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    /**********
    * as from *
    **********/

    AjiTkr_Parse(tkr, "{@as@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__STMT_AS);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@from@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__FROM);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    /*************
    * statements *
    *************/

    AjiTkr_Parse(tkr, "{@ end @}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__STMT_END);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@ if @}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__STMT_IF);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@ elif @}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__STMT_ELIF);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@ else @}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__STMT_ELSE);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@ for @}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__STMT_FOR);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    /************
    * operators *
    ************/

    AjiTkr_Parse(tkr, "{@ + @}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__OP_ADD);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@ - @}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__OP_SUB);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@ * @}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__OP_MUL);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@ / @}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__OP_DIV);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@ % @}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__OP_MOD);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@ = @}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__OP_ASS);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@ += @}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__OP_ADD_ASS);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@ -= @}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__OP_SUB_ASS);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@ *= @}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__OP_MUL_ASS);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@ /= @}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__OP_DIV_ASS);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@ %= @}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__OP_MOD_ASS);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    /***********************
    * comparison operators *
    ***********************/

    AjiTkr_Parse(tkr, "{@ == @}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__OP_EQ);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@ != @}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__OP_NOT_EQ);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@ <= @}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__OP_LTE);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@ >= @}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__OP_GTE);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@ < @}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__OP_LT);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@ > @}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__OP_GT);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@ or @}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__OP_OR);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@ and @}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__OP_AND);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@ not @}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__OP_NOT);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    /*******
    * expr *
    *******/

    AjiTkr_Parse(tkr, "{@ 1 * 2 @}");
    {
        assert(AjiTkr_ToksLen(tkr) == 5);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__INTEGER);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__OP_MUL);
        token = AjiTkr_ToksGetc(tkr, 3);
        assert(token->type == AJI_TOK_TYPE__INTEGER);
        token = AjiTkr_ToksGetc(tkr, 4);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    /*********
    * others *
    *********/

    AjiTkr_Parse(tkr, "{@\"\"@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__DQ_STRING);
        assert(strcmp(token->text, "") == 0);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@\"abc\"@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__DQ_STRING);
        assert(strcmp(token->text, "abc") == 0);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@\"abc\"\"bbc\"@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 4);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__DQ_STRING);
        assert(strcmp(token->text, "abc") == 0);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__DQ_STRING);
        assert(strcmp(token->text, "bbc") == 0);
        token = AjiTkr_ToksGetc(tkr, 3);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }
/*
    AjiTkr_Parse(tkr,
        "{@ import alias\n"
        "alias.set(\"dtl\", \"run bin/date-line\") @}");
    {
        assert(AjiTkr_ToksLen(tkr) == 13);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__STMT_IMPORT);
        assert(strcmp(token->text, "import") == 0);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__IDENTIFIER);
        assert(strcmp(token->text, "alias") == 0);
        token = AjiTkr_ToksGetc(tkr, 3);
        assert(token->type == AJI_TOK_TYPE__NEWLINE);
        token = AjiTkr_ToksGetc(tkr, 4);
        assert(token->type == AJI_TOK_TYPE__IDENTIFIER);
        assert(strcmp(token->text, "alias") == 0);
        token = AjiTkr_ToksGetc(tkr, 5);
        assert(token->type == AJI_TOK_TYPE__DOT_OPE);
        token = AjiTkr_ToksGetc(tkr, 6);
        assert(token->type == AJI_TOK_TYPE__IDENTIFIER);
        assert(strcmp(token->text, "set") == 0);
        token = AjiTkr_ToksGetc(tkr, 7);
        assert(token->type == AJI_TOK_TYPE__LPAREN);
        token = AjiTkr_ToksGetc(tkr, 8);
        assert(token->type == AJI_TOK_TYPE__DQ_STRING);
        assert(strcmp(token->text, "dtl") == 0);
        token = AjiTkr_ToksGetc(tkr, 9);
        assert(token->type == AJI_TOK_TYPE__COMMA);
        token = AjiTkr_ToksGetc(tkr, 10);
        assert(token->type == AJI_TOK_TYPE__DQ_STRING);
        assert(strcmp(token->text, "run bin/date-line") == 0);
        token = AjiTkr_ToksGetc(tkr, 11);
        assert(token->type == AJI_TOK_TYPE__RPAREN);
        token = AjiTkr_ToksGetc(tkr, 12);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }
*/
    /******************
    * reference block *
    ******************/

    AjiTkr_Parse(tkr, "{:");
    {
        assert(AjiTkr_ToksLen(tkr) == 1);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LDOUBLE_BRACE);
        assert(AjiTkr_HasErrStack(tkr) == true);
        assert(strcmp(AjiTkr_GetcFirstErrMsg(tkr), "not closed by block") == 0);
    }

    AjiTkr_Parse(tkr, "{::}");
    {
        assert(AjiTkr_ToksLen(tkr) == 2);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LDOUBLE_BRACE);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__RDOUBLE_BRACE);
    }

    AjiTkr_Parse(tkr, "{:\n:}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LDOUBLE_BRACE);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__NEWLINE);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RDOUBLE_BRACE);
    }

    AjiTkr_Parse(tkr, "{:abc:}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LDOUBLE_BRACE);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__IDENTIFIER);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RDOUBLE_BRACE);
    }

    AjiTkr_Parse(tkr, "{:abc123:}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LDOUBLE_BRACE);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__IDENTIFIER);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RDOUBLE_BRACE);
    }

    AjiTkr_Parse(tkr, "{:abc_123:}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LDOUBLE_BRACE);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__IDENTIFIER);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RDOUBLE_BRACE);
    }

    AjiTkr_Parse(tkr, "{: 123 :}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LDOUBLE_BRACE);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__INTEGER);
        assert(token->lvalue == 123);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RDOUBLE_BRACE);
    }
/*
    AjiTkr_Parse(tkr, "{: alias.run(\"dtl\") :}");
    {
        assert(AjiTkr_ToksLen(tkr) == 8);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LDOUBLE_BRACE);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__IDENTIFIER);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__DOT_OPE);
        token = AjiTkr_ToksGetc(tkr, 3);
        assert(token->type == AJI_TOK_TYPE__IDENTIFIER);
        token = AjiTkr_ToksGetc(tkr, 4);
        assert(token->type == AJI_TOK_TYPE__LPAREN);
        token = AjiTkr_ToksGetc(tkr, 5);
        assert(token->type == AJI_TOK_TYPE__DQ_STRING);
        token = AjiTkr_ToksGetc(tkr, 6);
        assert(token->type == AJI_TOK_TYPE__RPAREN);
        token = AjiTkr_ToksGetc(tkr, 7);
        assert(token->type == AJI_TOK_TYPE__RDOUBLE_BRACE);
    }
*/
    /*****************************
    * reference block: operators *
    *****************************/

    AjiTkr_Parse(tkr, "{: + :}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LDOUBLE_BRACE);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__OP_ADD);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RDOUBLE_BRACE);
    }

    AjiTkr_Parse(tkr, "{: - :}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LDOUBLE_BRACE);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__OP_SUB);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RDOUBLE_BRACE);
    }

    AjiTkr_Parse(tkr, "{: * :}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LDOUBLE_BRACE);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__OP_MUL);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RDOUBLE_BRACE);
    }

    AjiTkr_Parse(tkr, "{: / :}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LDOUBLE_BRACE);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__OP_DIV);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RDOUBLE_BRACE);
    }

    AjiTkr_Parse(tkr, "{: = :}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LDOUBLE_BRACE);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__OP_ASS);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RDOUBLE_BRACE);
    }

    AjiTkr_Parse(tkr, "{: += :}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LDOUBLE_BRACE);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__OP_ADD_ASS);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RDOUBLE_BRACE);
    }

    AjiTkr_Parse(tkr, "{: -= :}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LDOUBLE_BRACE);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__OP_SUB_ASS);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RDOUBLE_BRACE);
    }

    AjiTkr_Parse(tkr, "{: *= :}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LDOUBLE_BRACE);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__OP_MUL_ASS);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RDOUBLE_BRACE);
    }

    AjiTkr_Parse(tkr, "{: /= :}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LDOUBLE_BRACE);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__OP_DIV_ASS);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RDOUBLE_BRACE);
    }

    /****************************************
    * reference block: comparison operators *
    ****************************************/

    AjiTkr_Parse(tkr, "{: == :}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LDOUBLE_BRACE);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__OP_EQ);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RDOUBLE_BRACE);
    }

    AjiTkr_Parse(tkr, "{: != :}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LDOUBLE_BRACE);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__OP_NOT_EQ);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RDOUBLE_BRACE);
    }

    /**********
    * comment *
    **********/

    AjiTkr_Parse(tkr, "{@\n"
    "// comment\n"
    "@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__NEWLINE);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@\n"
    "// comment\n"
    "// comment\n"
    "@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__NEWLINE);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@\n"
    "// comment\n");
    {
        assert(AjiTkr_HasErrStack(tkr));
        assert(!strcmp(AjiTkr_GetcFirstErrMsg(tkr), "not closed by block"));
    }

    AjiTkr_Parse(tkr, "{@ // comment");
    {
        assert(AjiTkr_HasErrStack(tkr));
        assert(!strcmp(AjiTkr_GetcFirstErrMsg(tkr), "not closed by block"));
    }

    AjiTkr_Parse(tkr, "{@\n"
    "/* comment */"
    "@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__NEWLINE);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@\n"
    "/* comment \n"
    "   comment \n"
    "   comment */"
    "@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__NEWLINE);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    /***********
    * newlines *
    ***********/

    AjiTkr_Parse(tkr, "{@\n@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__NEWLINE);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@\n\n@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 4);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__NEWLINE);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__NEWLINE);
        token = AjiTkr_ToksGetc(tkr, 3);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@\r@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__NEWLINE);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@\r\r@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 4);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__NEWLINE);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__NEWLINE);
        token = AjiTkr_ToksGetc(tkr, 3);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@\r\n@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__NEWLINE);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@\r\n\r\n@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 4);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__NEWLINE);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__NEWLINE);
        token = AjiTkr_ToksGetc(tkr, 3);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@@}\n");
    {
        assert(AjiTkr_ToksLen(tkr) == 2);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@@}\n\n");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__TEXT_BLOCK);
    }

    AjiTkr_Del(tkr);
}

static void
test_tkr_long_code(void) {
    AjiTkrOpt *opt = AjiTkrOpt_New();
    AjiTkr *tkr = AjiTkr_New(opt);
    assert(tkr);

    const char *src = "{@\n"
"    puts(1)\n"
"    puts(2)\n"
"    puts(3)\n"
"    puts(4)\n"
"    puts(5)\n"
"    puts(6)\n"
"    puts(7)\n"
"    puts(8)\n"
"    puts(9)\n"
"    puts(10)\n"
"    puts(11)\n"
"    puts(12)\n"
"    puts(13)\n"
"    puts(14)\n"
"    puts(15)\n"
"    puts(16)\n"
"    puts(17)\n"
"    puts(18)\n"
"    puts(19)\n"
"    puts(20)\n"
"    puts(21)\n"
"    puts(22)\n"
"    puts(23)\n"
"    puts(24)\n"
"@}\n";
    assert(AjiTkr_Parse(tkr, src));
    assert(AjiTkr_ToksLen(tkr) == 123);

    AjiTkr_Del(tkr);
}

static void
test_AjiTkr_DeepCopy(void) {
    AjiTkrOpt *opt = AjiTkrOpt_New();
    AjiTkr *tkr = AjiTkr_New(opt);
    assert(tkr);

    assert(AjiTkr_Parse(tkr, "{@ i = 0 @}"));
    assert(AjiTkr_ToksLen(tkr) == 5);

    AjiTkr *other = AjiTkr_DeepCopy(tkr);
    assert(AjiTkr_ToksLen(other) == 5);

    AjiTkr_Del(other);
    AjiTkr_Del(tkr);
}

static void
test_AjiTkr_Parse_int(void) {
    AjiTkrOpt *opt = AjiTkrOpt_New();
    AjiTkr *tkr = AjiTkr_New(opt);
    const AjiTok *token;

    AjiTkr_Parse(tkr, "{@123@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__INTEGER);
        assert(token->lvalue == 123);
        assert(strcmp(token->text, "123") == 0);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Del(tkr);
}

static void
test_AjiTkr_Parse_int_plus(void) {
    AjiTkrOpt *opt = AjiTkrOpt_New();
    AjiTkr *tkr = AjiTkr_New(opt);
    const AjiTok *token;

    AjiTkr_Parse(tkr, "{@+123@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 4);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__OP_ADD);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__INTEGER);
        assert(token->lvalue == 123);
        assert(strcmp(token->text, "123") == 0);
        token = AjiTkr_ToksGetc(tkr, 3);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Del(tkr);
}

static void
test_AjiTkr_Parse_int_minus(void) {
    AjiTkrOpt *opt = AjiTkrOpt_New();
    AjiTkr *tkr = AjiTkr_New(opt);
    const AjiTok *token;

    AjiTkr_Parse(tkr, "{@-123@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 4);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__OP_SUB);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__INTEGER);
        assert(token->lvalue == 123);
        assert(strcmp(token->text, "123") == 0);
        token = AjiTkr_ToksGetc(tkr, 3);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Del(tkr);
}

static void
test_AjiTkr_Parse_float(void) {
    AjiTkrOpt *opt = AjiTkrOpt_New();
    AjiTkr *tkr = AjiTkr_New(opt);
    const AjiTok *token;

    AjiTkr_Parse(tkr, "{@123.456@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 3);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__FLOAT);
        assert(token->float_value == 123.456);
        assert(strcmp(token->text, "123.456") == 0);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Del(tkr);
}

static void
test_AjiTkr_Parse_float_plus(void) {
    AjiTkrOpt *opt = AjiTkrOpt_New();
    AjiTkr *tkr = AjiTkr_New(opt);
    const AjiTok *token;

    AjiTkr_Parse(tkr, "{@+123.456@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 4);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__OP_ADD);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__FLOAT);
        assert(token->float_value == 123.456);
        assert(strcmp(token->text, "123.456") == 0);
        token = AjiTkr_ToksGetc(tkr, 3);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Del(tkr);
}

static void
test_AjiTkr_Parse_float_minus(void) {
    AjiTkrOpt *opt = AjiTkrOpt_New();
    AjiTkr *tkr = AjiTkr_New(opt);
    const AjiTok *token;

    AjiTkr_Parse(tkr, "{@-123.456@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 4);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__OP_SUB);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__FLOAT);
        assert(token->float_value == 123.456);
        assert(strcmp(token->text, "123.456") == 0);
        token = AjiTkr_ToksGetc(tkr, 3);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Del(tkr);
}

static void
test_AjiTkr_Parse_float_errors(void) {
    AjiTkrOpt *opt = AjiTkrOpt_New();
    AjiTkr *tkr = AjiTkr_New(opt);
    const AjiTok *token;

    assert(AjiTkr_Parse(tkr, "{@123.@}") == NULL);
    assert(strcmp(AjiTkr_GetcFirstErrMsg(tkr), "invalid float") == 0);

    AjiTkr_Parse(tkr, "{@.456@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 4);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__DOT_OPE);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__INTEGER);
        assert(strcmp(token->text, "456") == 0);
        token = AjiTkr_ToksGetc(tkr, 3);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Parse(tkr, "{@123.456.789@}");
    {
        assert(AjiTkr_ToksLen(tkr) == 5);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__FLOAT);
        assert(strcmp(token->text, "123.456") == 0);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__DOT_OPE);
        token = AjiTkr_ToksGetc(tkr, 3);
        assert(token->type == AJI_TOK_TYPE__INTEGER);
        assert(strcmp(token->text, "789") == 0);
        token = AjiTkr_ToksGetc(tkr, 4);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Del(tkr);
}

static void
test_AjiTkr_ExtendFrontOther(void) {
    AjiTkr *a = AjiTkr_New(AjiTkrOpt_New());
    AjiTkr *b = AjiTkr_New(AjiTkrOpt_New());
    const AjiTok *token;

    AjiTkr_Parse(a, "{@ 1 @}");
    AjiTkr_Parse(b, "{@ 2 @}");

    AjiTkr_ExtendFrontOther(a, b);
    {
        assert(AjiTkr_ToksLen(a) == 6);
        token = AjiTkr_ToksGetc(a, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(a, 1);
        assert(token->type == AJI_TOK_TYPE__INTEGER);
        assert(strcmp(token->text, "2") == 0);
        token = AjiTkr_ToksGetc(a, 2);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
        token = AjiTkr_ToksGetc(a, 3);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(a, 4);
        assert(token->type == AJI_TOK_TYPE__INTEGER);
        assert(strcmp(token->text, "1") == 0);
        token = AjiTkr_ToksGetc(a, 5);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Del(a);
    AjiTkr_Del(b);
}

static void
test_AjiTkr_Parse_struct_0(void) {
    AjiTkrOpt *opt = AjiTkrOpt_New();
    AjiTkr *tkr = AjiTkr_New(opt);
    const AjiTok *token;

    assert(AjiTkr_Parse(tkr, "{@123.@}") == NULL);
    assert(strcmp(AjiTkr_GetcFirstErrMsg(tkr), "invalid float") == 0);

    AjiTkr_Parse(tkr, "{@ def func(): struct S: end end @}");
    {
        assert(AjiTkr_ToksLen(tkr) == 12);
        token = AjiTkr_ToksGetc(tkr, 0);
        assert(token->type == AJI_TOK_TYPE__LBRACEAT);
        token = AjiTkr_ToksGetc(tkr, 1);
        assert(token->type == AJI_TOK_TYPE__DEF);
        token = AjiTkr_ToksGetc(tkr, 2);
        assert(token->type == AJI_TOK_TYPE__IDENTIFIER);
        assert(strcmp(token->text, "func") == 0);
        token = AjiTkr_ToksGetc(tkr, 3);
        assert(token->type == AJI_TOK_TYPE__LPAREN);
        token = AjiTkr_ToksGetc(tkr, 4);
        assert(token->type == AJI_TOK_TYPE__RPAREN);
        token = AjiTkr_ToksGetc(tkr, 5);
        assert(token->type == AJI_TOK_TYPE__COLON);
        token = AjiTkr_ToksGetc(tkr, 6);
        assert(token->type == AJI_TOK_TYPE__STRUCT);
        token = AjiTkr_ToksGetc(tkr, 7);
        assert(token->type == AJI_TOK_TYPE__IDENTIFIER);
        assert(strcmp(token->text, "S") == 0);
        token = AjiTkr_ToksGetc(tkr, 8);
        assert(token->type == AJI_TOK_TYPE__COLON);
        token = AjiTkr_ToksGetc(tkr, 9);
        assert(token->type == AJI_TOK_TYPE__STMT_END);
        token = AjiTkr_ToksGetc(tkr, 10);
        assert(token->type == AJI_TOK_TYPE__STMT_END);
        token = AjiTkr_ToksGetc(tkr, 11);
        assert(token->type == AJI_TOK_TYPE__RBRACEAT);
    }

    AjiTkr_Del(tkr);
}

static const struct testcase
tokenizer_tests[] = {
    {"AjiTkr_New", test_AjiTkr_New},
    {"AjiTkr_Parse", test_AjiTkr_Parse},
    {"AjiTkr_Parse_int", test_AjiTkr_Parse_int},
    {"AjiTkr_Parse_int_plus", test_AjiTkr_Parse_int_plus},
    {"AjiTkr_Parse_int_minus", test_AjiTkr_Parse_int_minus},
    {"AjiTkr_Parse_float", test_AjiTkr_Parse_float},
    {"AjiTkr_Parse_float_plus", test_AjiTkr_Parse_float_plus},
    {"AjiTkr_Parse_float_minus", test_AjiTkr_Parse_float_minus},
    {"AjiTkr_Parse_float_errors", test_AjiTkr_Parse_float_errors},
    {"AjiTkr_Parse_struct_0", test_AjiTkr_Parse_struct_0},
    {"AjiTkr_DeepCopy", test_AjiTkr_DeepCopy},
    {"tkr_long_code", test_tkr_long_code},
    {"AjiTkr_ExtendFrontOther", test_AjiTkr_ExtendFrontOther},
    {0},
};

/***********
* compiler *
***********/

static void
test_ast_show_error(const AjiAST *ast) {
    if (AjiAST_HasErrs(ast)) {
        printf("error detail[%s]\n", AjiAST_GetcFirstErrMsg(ast));
    }
}

static void
test_cc_long_code(void) {
    const char *src = "{@\n"
"    puts(1)\n"
"    puts(2)\n"
"    puts(3)\n"
"    puts(4)\n"
"    puts(5)\n"
"    puts(6)\n"
"    puts(7)\n"
"    puts(8)\n"
"    puts(9)\n"
"    puts(10)\n"
"    puts(11)\n"
"    puts(12)\n"
"    puts(13)\n"
"    puts(14)\n"
"    puts(15)\n"
"    puts(16)\n"
"    puts(17)\n"
"    puts(18)\n"
"    puts(19)\n"
"    puts(20)\n"
"    puts(21)\n"
"    puts(22)\n"
"    puts(23)\n"
"    puts(24)\n"
"@}\n";
    AjiConfig *config = AjiConfig_New();
    AjiTkrOpt *opt = AjiTkrOpt_New();
    AjiTkr *tkr = AjiTkr_New(AjiMem_Move(opt));
    AjiAST *ast = AjiAST_New(config);
    const AjiNode *root;

    AjiTkr_Parse(tkr, src);
    AjiAST_Clear(ast);
    AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
    root = AjiAST_GetcRoot(ast);
    assert(root);

    AjiTkr_Del(tkr);
    AjiAST_Del(ast);
    AjiConfig_Del(config);
}

static void
test_cc_basic_0(void) {
    AjiConfig *config = AjiConfig_New();
    AjiTkrOpt *opt = AjiTkrOpt_New();
    AjiTkr *tkr = AjiTkr_New(AjiMem_Move(opt));
    AjiAST *ast = AjiAST_New(config);
    const AjiNode *root;

    AjiTkr_Parse(tkr, "");
    AjiAST_Clear(ast);
    AjiAST_Clear(ast);
    AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
    root = AjiAST_GetcRoot(ast);
    assert(root == NULL);

    AjiTkr_Del(tkr);
    AjiAST_Del(ast);
    AjiConfig_Del(config);
}

static void
test_cc_basic_1(void) {
    AjiConfig *config = AjiConfig_New();
    AjiTkrOpt *opt = AjiTkrOpt_New();
    AjiTkr *tkr = AjiTkr_New(AjiMem_Move(opt));
    AjiAST *ast = AjiAST_New(config);

    AjiTkr_Parse(tkr, "{@ i = 0 @}"); {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Del(tkr);
    AjiAST_Del(ast);
    AjiConfig_Del(config);
}

static void
test_cc_code_block(void) {
    AjiConfig *config = AjiConfig_New();
    AjiTkrOpt *opt = AjiTkrOpt_New();
    AjiTkr *tkr = AjiTkr_New(AjiMem_Move(opt));
    AjiAST *ast = AjiAST_New(config);
    const AjiNode *root;
    AjiProgramNode *program;
    AjiBlocksNode *blocks;
    AjiCodeBlockNode *code_block;

    AjiTkr_Parse(tkr, "{@@}");
    AjiAST_Clear(ast);
    AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
    root = AjiAST_GetcRoot(ast);
    assert(root);
    program = root->real;
    blocks = program->blocks->real;
    code_block = blocks->code_block->real;
    assert(code_block);

    AjiTkr_Del(tkr);
    AjiAST_Del(ast);
    AjiConfig_Del(config);
}

static void
test_cc_code_block_0(void) {
    AjiConfig *config = AjiConfig_New();
    AjiTkrOpt *opt = AjiTkrOpt_New();
    AjiTkr *tkr = AjiTkr_New(AjiMem_Move(opt));
    AjiAST *ast = AjiAST_New(config);

    AjiTkr_Parse(tkr, "{@@}");
    AjiAST_Clear(ast);
    AjiCC_Compile(ast, AjiTkr_GetToks(tkr));

    AjiTkr_Del(tkr);
    AjiAST_Del(ast);
    AjiConfig_Del(config);
}

static void
test_cc_ref_block(void) {
    AjiConfig *config = AjiConfig_New();
    AjiTkrOpt *opt = AjiTkrOpt_New();
    AjiTkr *tkr = AjiTkr_New(AjiMem_Move(opt));
    AjiAST *ast = AjiAST_New(config);

    AjiTkr_Parse(tkr, "{: nil :}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{: 1 :}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{: var :}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{: [1, 2] :}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Del(tkr);
    AjiAST_Del(ast);
    AjiConfig_Del(config);
}

static void
test_cc_ref_block_0(void) {
    AjiConfig *config = AjiConfig_New();
    AjiTkrOpt *opt = AjiTkrOpt_New();
    AjiTkr *tkr = AjiTkr_New(AjiMem_Move(opt));
    AjiAST *ast = AjiAST_New(config);

    AjiTkr_Parse(tkr, "{: nil :}");
    AjiAST_Clear(ast);
    (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));

    AjiTkr_Del(tkr);
    AjiAST_Del(ast);
    AjiConfig_Del(config);
}

static void
test_cc_ref_block_1(void) {
    AjiConfig *config = AjiConfig_New();
    AjiTkrOpt *opt = AjiTkrOpt_New();
    AjiTkr *tkr = AjiTkr_New(AjiMem_Move(opt));
    AjiAST *ast = AjiAST_New(config);

    AjiTkr_Parse(tkr, "{: 1 :}");
    AjiAST_Clear(ast);
    (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));

    AjiTkr_Del(tkr);
    AjiAST_Del(ast);
    AjiConfig_Del(config);
}

static void
test_cc_ref_block_2(void) {
    AjiConfig *config = AjiConfig_New();
    AjiTkrOpt *opt = AjiTkrOpt_New();
    AjiTkr *tkr = AjiTkr_New(AjiMem_Move(opt));
    AjiAST *ast = AjiAST_New(config);

    AjiTkr_Parse(tkr, "{: var :}");
    AjiAST_Clear(ast);
    (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));

    AjiTkr_Del(tkr);
    AjiAST_Del(ast);
    AjiConfig_Del(config);
}

static void
test_cc_ref_block_3(void) {
    AjiConfig *config = AjiConfig_New();
    AjiTkrOpt *opt = AjiTkrOpt_New();
    AjiTkr *tkr = AjiTkr_New(AjiMem_Move(opt));
    AjiAST *ast = AjiAST_New(config);

    AjiTkr_Parse(tkr, "{: [1, 2] :}");
    AjiAST_Clear(ast);
    (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));

    AjiTkr_Del(tkr);
    AjiAST_Del(ast);
    AjiConfig_Del(config);
}

static void
test_cc_formula(void) {
    AjiConfig *config = AjiConfig_New();
    AjiTkrOpt *opt = AjiTkrOpt_New();
    AjiTkr *tkr = AjiTkr_New(AjiMem_Move(opt));
    AjiAST *ast = AjiAST_New(config);

    AjiTkr_Parse(tkr, "{@ a = 1 @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ a = \"abc\" @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ a = b = 1 @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ a = 1, b = 2 @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Del(tkr);
    AjiAST_Del(ast);
    AjiConfig_Del(config);
}

static void
test_cc_dict(void) {
    AjiConfig *config = AjiConfig_New();
    AjiTkrOpt *opt = AjiTkrOpt_New();
    AjiTkr *tkr = AjiTkr_New(AjiMem_Move(opt));
    AjiAST *ast = AjiAST_New(config);

    AjiTkr_Parse(tkr, "{@ { \"key\" : \"value\" } @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ { \"key\" : \"value\", } @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ { \"key1\" : \"value1\", \"key2\" : \"value2\" } @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Del(tkr);
    AjiAST_Del(ast);
    AjiConfig_Del(config);
}

static void
test_cc_dict_0(void) {
    AjiConfig *config = AjiConfig_New();
    AjiTkrOpt *opt = AjiTkrOpt_New();
    AjiTkr *tkr = AjiTkr_New(AjiMem_Move(opt));
    AjiAST *ast = AjiAST_New(config);

    AjiTkr_Parse(tkr, "{@ {} @}");
    AjiAST_Clear(ast);
    (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));

    AjiTkr_Del(tkr);
    AjiAST_Del(ast);
    AjiConfig_Del(config);
}

static void
test_cc_dict_1(void) {
    AjiConfig *config = AjiConfig_New();
    AjiTkrOpt *opt = AjiTkrOpt_New();
    AjiTkr *tkr = AjiTkr_New(AjiMem_Move(opt));
    AjiAST *ast = AjiAST_New(config);

    AjiTkr_Parse(tkr, "{@ { \"key\" : \"value\", } @}");
    AjiAST_Clear(ast);
    AjiAST_Clear(ast);
    (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));

    AjiTkr_Del(tkr);
    AjiAST_Del(ast);
    AjiConfig_Del(config);
}

static void
test_cc_dict_2(void) {
    AjiConfig *config = AjiConfig_New();
    AjiTkrOpt *opt = AjiTkrOpt_New();
    AjiTkr *tkr = AjiTkr_New(AjiMem_Move(opt));
    AjiAST *ast = AjiAST_New(config);

    AjiTkr_Parse(tkr, "{@ { \"key1\" : \"value1\", \"key2\" : \"value2\" } @}");
    AjiAST_Clear(ast);
    (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));

    AjiTkr_Del(tkr);
    AjiAST_Del(ast);
    AjiConfig_Del(config);
}

static void
test_cc_expr(void) {
    AjiConfig *config = AjiConfig_New();
    AjiTkrOpt *opt = AjiTkrOpt_New();
    AjiTkr *tkr = AjiTkr_New(AjiMem_Move(opt));
    AjiAST *ast = AjiAST_New(config);

    AjiTkr_Parse(tkr, "{@ 1 @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ 1 == 2 @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ 1 == 2 == 3 @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ 1 != 2 @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ 1 != 2 != 3 @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ 1 + 2 @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ 1 + 2 + 3 @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ 1 - 2 @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ 1 - 2 - 3 @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ 1 * 2 @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ 1 * 2 * 3 @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ 1 / 2 @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ 1 / 2 / 3 @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Del(tkr);
    AjiAST_Del(ast);
    AjiConfig_Del(config);
}

static void
test_cc_index(void) {
    AjiConfig *config = AjiConfig_New();
    AjiTkrOpt *opt = AjiTkrOpt_New();
    AjiTkr *tkr = AjiTkr_New(AjiMem_Move(opt));
    AjiAST *ast = AjiAST_New(config);

    AjiTkr_Parse(tkr, "{@ a[0] @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ a[0][0] @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Del(tkr);
    AjiAST_Del(ast);
    AjiConfig_Del(config);
}

static void
test_cc_dot(void) {
    AjiConfig *config = AjiConfig_New();
    AjiTkrOpt *opt = AjiTkrOpt_New();
    AjiTkr *tkr = AjiTkr_New(AjiMem_Move(opt));
    AjiAST *ast = AjiAST_New(config);

    AjiTkr_Parse(tkr, "{@ a.b @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ a.b() @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ a.b[0] @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Del(tkr);
    AjiAST_Del(ast);
    AjiConfig_Del(config);
}

static void
test_cc_call(void) {
    AjiConfig *config = AjiConfig_New();
    AjiTkrOpt *opt = AjiTkrOpt_New();
    AjiTkr *tkr = AjiTkr_New(AjiMem_Move(opt));
    AjiAST *ast = AjiAST_New(config);

    AjiTkr_Parse(tkr, "{@ f() @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ f(1) @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ f(1, \"abc\") @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ a.b() @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ f()() @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
   }

    AjiTkr_Parse(tkr, "{@ a[0]() @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Del(tkr);
    AjiAST_Del(ast);
    AjiConfig_Del(config);
}

static void
test_cc_vec(void) {
    AjiConfig *config = AjiConfig_New();
    AjiTkrOpt *opt = AjiTkrOpt_New();
    AjiTkr *tkr = AjiTkr_New(AjiMem_Move(opt));
    AjiAST *ast = AjiAST_New(config);

    AjiTkr_Parse(tkr, "{@ [] @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ [1, 2] @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ [1] @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ [a = 1] @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ [a = 1, b = 2] @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ [1, a = 2] @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Del(tkr);
    AjiAST_Del(ast);
    AjiConfig_Del(config);
}

static void
test_cc_asscalc(void) {
    AjiConfig *config = AjiConfig_New();
    AjiTkrOpt *opt = AjiTkrOpt_New();
    AjiTkr *tkr = AjiTkr_New(AjiMem_Move(opt));
    AjiAST *ast = AjiAST_New(config);

    AjiTkr_Parse(tkr, "{@ a += 1 @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ a += \"b\" @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ a -= 1 @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ a *= 1 @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    /* AjiTkr_Parse(tkr, "{@ a /= 1 @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    } */
/*
    AjiTkr_Parse(tkr, "{@ func() @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }
*/
    AjiTkr_Del(tkr);
    AjiAST_Del(ast);
    AjiConfig_Del(config);
}

static void
test_cc_atom(void) {
    AjiConfig *config = AjiConfig_New();
    AjiTkrOpt *opt = AjiTkrOpt_New();
    AjiTkr *tkr = AjiTkr_New(AjiMem_Move(opt));
    AjiAST *ast = AjiAST_New(config);

    AjiTkr_Parse(tkr, "{@ nil @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ false @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ true @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ 1 @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    /* AjiTkr_Parse(tkr, "{@ -1 @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    } */

    AjiTkr_Parse(tkr, "{@ \"abc\" @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ var @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }
/*
    AjiTkr_Parse(tkr, "{@ f() @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }
*/

    AjiTkr_Del(tkr);
    AjiAST_Del(ast);
    AjiConfig_Del(config);
}

static void
test_AjiCC_Compile(void) {
    // head
    AjiConfig *config = AjiConfig_New();
    AjiTkrOpt *opt = AjiTkrOpt_New();
    AjiTkr *tkr = AjiTkr_New(AjiMem_Move(opt));
    AjiAST *ast = AjiAST_New(config);

    /***********
    * func_def *
    ***********/

    AjiTkr_Parse(tkr, "{@ def func(a, b): end @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ def func(): end @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ def func(): a = 1 end @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@\n"
        "def func():\n"
        "   a = 1\n"
        "end\n"
        "@}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    /*******
    * call *
    *******/
/*
    AjiTkr_Parse(tkr, "{@ func() + 1 @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }
*/
/*
    AjiTkr_Parse(tkr, "{@ my.func() @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ my.func(1) @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ my.func(1, 2) @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ my.func(\"abc\") @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ my.func(\"abc\", \"def\") @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ my.func(\"abc\", \"def\", \"ghi\") @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ my.func(\"\", \"\") @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }
*/
    /************
    * test_list *
    ************/

    AjiTkr_Parse(tkr, "{@ 1, 2 @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ 1, 2, 3 @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ \"abc\", \"def\" @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ \"abc\", \"def\", \"ghi\" @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ 1, \"def\" @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ 1, var @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ 1, var, \"abc\" @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ 1, var, \"abc\", func() @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    /*******
    * test *
    *******/

    AjiTkr_Parse(tkr, "{@ 1 @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ 1 or 1 @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ 1 or 1 or 1 @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ 1 and 1 @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ 1 and 1 and 1 @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ not 1 @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ not not 1 @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ 1 or 1 and 1 @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ 1 and 1 or 1 @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ not 1 or 1 @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ not 1 and 1 @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    /*********
    * blocks *
    *********/

    AjiTkr_Parse(tkr, "{@@}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "abc{@@}def");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@@}{@@}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@@}abc{@@}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@\n@}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@\n\n@}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    /***************
    * if statement *
    ***************/

    AjiTkr_Parse(tkr, "{@ if 1: end @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ if 1 + 2: end @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "abc{@ if 1: end @}def");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@\n\nif 1: end\n\n@}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@\n\nif 1:\n\nend\n\n@}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ if 1: else: end @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ if 1:\n\nelse:\n\nend @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ if 1: elif 2: end @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ if 1:\n\nelif 2:\n\nend @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ if 1: elif 2: else: end @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ if 1:\n\nelif 2:\n\nelse:\n\nend @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ if 0: elif 0: else: a = 1 end @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }
    AjiTkr_Parse(tkr, "{@ if 1: if 2: end end @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ if 1:\n\nif 2:\n\nend\n\nend @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ if 1: if 2: end if 3: end end @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ if 1: @}{@ end @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ if 1: @}abc{@ end @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ if 1: @}{@ else: @}{@ end @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ if 0: @}abc{@ else: @}def{@ end @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ if 1: @}{@ if 2: end @}{@ end @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ if 1: @}abc{@ if 2: end @}def{@ end @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    /****************
    * for statement *
    ****************/

    AjiTkr_Parse(tkr, "{@ for 1; 1; 1: end @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ for 1; 1; 1: if 1: end end @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ for 1; 1; 1: for 1; 1; 1: end end @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ for 1: end @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ for 1: if 1: end end @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ for 1: @}{@ if 1: end @}{@ end @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ for 1: @}{@ end @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ for: end @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ for: @}{@ end @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ for: @}abc{@ end @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ for: @}{@ if 1: end @}{@ end @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ if 1: for 1; 1; 1: end end @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ for 1; 1; 1: @}{@ end @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ for 1; 1; 1: @}abc{@ end @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ for 1; 1; 1: @}{@ if 1: end @}{@ end @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ for 1; 1; 1: @}abc{@ if 1: end @}def{@ end @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    /*******
    * jump *
    *******/

    AjiTkr_Parse(tkr, "{@ for 1; 1; 1: break end @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ for 1; 1; 1: continue end @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Del(tkr);
    AjiAST_Del(ast);
    AjiConfig_Del(config);
    // tail
}

static void
test_cc_import_stmt(void) {
    AjiConfig *config = AjiConfig_New();
    AjiTkrOpt *opt = AjiTkrOpt_New();
    AjiTkr *tkr = AjiTkr_New(AjiMem_Move(opt));
    AjiAST *ast = AjiAST_New(config);

    /**********************
    * import as statement *
    **********************/

    AjiTkr_Parse(tkr, "{@ import \"path/to/module\" as mod \n @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ import @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(AjiAST_HasErrs(ast));
        assert(!strcmp(AjiAST_GetcFirstErrMsg(ast), "not found path in compile import as statement"));
    }

    AjiTkr_Parse(tkr, "{@ import \"path/to/module\" @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ import \"path/to/module\" as @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(AjiAST_HasErrs(ast));
        assert(!strcmp(AjiAST_GetcFirstErrMsg(ast), "not found alias in compile import as statement"));
    }

    AjiTkr_Parse(tkr, "{@ import \n\"path/to/module\" as mod @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(AjiAST_HasErrs(ast));
        assert(!strcmp(AjiAST_GetcFirstErrMsg(ast), "not found path in compile import as statement"));
    }

    AjiTkr_Parse(tkr, "{@ import \"path/to/module\" \n as mod @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(AjiAST_HasErrs(ast));
        assert(!strcmp(AjiAST_GetcFirstErrMsg(ast), "not found blocks"));
    }

    AjiTkr_Parse(tkr, "{@ import \"path/to/module\" as \n mod @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(AjiAST_HasErrs(ast));
        assert(!strcmp(AjiAST_GetcFirstErrMsg(ast), "not found alias in compile import as statement"));
    }

    /************************
    * from import statement *
    ************************/

    AjiTkr_Parse(tkr, "{@ from \"path/to/module\" import func @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ from \"path/to/module\" import func as f @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ from \"path/to/module\" import ( aaa ) @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ from \"path/to/module\" import ( aaa, bbb ) @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ from \"path/to/module\" import ( aaa, bbb, ) @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ from \"path/to/module\" import (\naaa,\nbbb,\n) @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ from \"path/to/module\" import ( aaa as a, bbb ) @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ from \"path/to/module\" import (\n aaa as a, bbb ) @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ from \"path/to/module\" import (\n aaa as a, \nbbb ) @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ from \"path/to/module\" import (\n aaa as a,\n bbb \n) @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ from @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(AjiAST_HasErrs(ast));
        assert(!strcmp(AjiAST_GetcFirstErrMsg(ast), "not found path in compile from import statement"));
    }

    AjiTkr_Parse(tkr, "{@ from \"path/to/module\" @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(AjiAST_HasErrs(ast));
        assert(!strcmp(AjiAST_GetcFirstErrMsg(ast), "not found import in compile from import statement"));
    }

    AjiTkr_Parse(tkr, "{@ from \"path/to/module\" import @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(AjiAST_HasErrs(ast));
        assert(!strcmp(AjiAST_GetcFirstErrMsg(ast), "not found import variables in compile from import statement"));
    }

    AjiTkr_Parse(tkr, "{@ from \"path/to/module\" import \naaa @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(AjiAST_HasErrs(ast));
        assert(!strcmp(AjiAST_GetcFirstErrMsg(ast), "not found import variables in compile from import statement"));
    }

    AjiTkr_Parse(tkr, "{@ from \"path/to/module\" import aaa as @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(AjiAST_HasErrs(ast));
        assert(!strcmp(AjiAST_GetcFirstErrMsg(ast), "not found second identifier in compile import variable"));
    }

    AjiTkr_Parse(tkr, "{@ from \"path/to/module\" import ( aaa @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(AjiAST_HasErrs(ast));
        assert(!strcmp(AjiAST_GetcFirstErrMsg(ast), "invalid token 5 in compile import variables"));
    }

    AjiTkr_Parse(tkr, "{@ from \"path/to/module\" import ( aaa as a @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(AjiAST_HasErrs(ast));
        assert(!strcmp(AjiAST_GetcFirstErrMsg(ast), "invalid token 5 in compile import variables"));
    }

    AjiTkr_Parse(tkr, "{@ from \"path/to/module\" import ( aaa as a, @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(AjiAST_HasErrs(ast));
        assert(!strcmp(AjiAST_GetcFirstErrMsg(ast), "not found import variable in compile import variables"));
    }

    AjiTkr_Parse(tkr, "{@ from \"path/to/module\" import ( aaa as a, bbb @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(AjiAST_HasErrs(ast));
        assert(!strcmp(AjiAST_GetcFirstErrMsg(ast), "invalid token 5 in compile import variables"));
    }

    AjiTkr_Parse(tkr, "{@ from \"path/to/module\" import ( aaa \n as a ) @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(AjiAST_HasErrs(ast));
        assert(!strcmp(AjiAST_GetcFirstErrMsg(ast), "invalid token 46 in compile import variables"));
    }

    AjiTkr_Parse(tkr, "{@ from \"path/to/module\" import ( aaa as \n a ) @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        assert(AjiAST_HasErrs(ast));
        assert(!strcmp(AjiAST_GetcFirstErrMsg(ast), "not found second identifier in compile import variable"));
    }

    AjiTkr_Del(tkr);
    AjiAST_Del(ast);
    AjiConfig_Del(config);
}

static void
test_cc_func_def(void) {
    AjiConfig *config = AjiConfig_New();
    AjiTkrOpt *opt = AjiTkrOpt_New();
    AjiTkr *tkr = AjiTkr_New(AjiMem_Move(opt));
    AjiAST *ast = AjiAST_New(config);

    AjiTkr_Parse(tkr, "{@ def func():\n"
    "end @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ def func():\n"
    "   i = 0\n"
    "end @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ def func():\n"
    "@}123{@\n"
    "end @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ def func():\n"
    "@}123{@\n"
    "@}223{@\n"
    "end @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ def func():\n"
    "   i = 0\n"
    "@}123{@\n"
    "end @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Parse(tkr, "{@ def func():\n"
    "   i = 0\n"
    "@}123{@\n"
    "   j = 1\n"
    "end @}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        assert(!AjiAST_HasErrs(ast));
    }

    AjiTkr_Del(tkr);
    AjiAST_Del(ast);
    AjiConfig_Del(config);
}

/**
 * 0 memory leaks
 * 2020/02/27
 */
static const struct testcase
compiler_tests[] = {
    {"cc_func_def", test_cc_func_def},
    {"AjiCC_Compile", test_AjiCC_Compile},
    {"cc_long_code", test_cc_long_code},
    {"cc_basic_0", test_cc_basic_0},
    {"cc_basic_1", test_cc_basic_1},
    {"cc_code_block", test_cc_code_block},
    {"cc_code_block_0", test_cc_code_block_0},
    {"cc_ref_block", test_cc_ref_block},
    {"cc_ref_block_0", test_cc_ref_block_0},
    {"cc_ref_block_1", test_cc_ref_block_1},
    {"cc_ref_block_2", test_cc_ref_block_2},
    {"cc_ref_block_3", test_cc_ref_block_3},
    {"cc_formula", test_cc_formula},
    {"cc_dict", test_cc_dict},
    {"cc_dict_0", test_cc_dict_0},
    {"cc_dict_1", test_cc_dict_1},
    {"cc_dict_2", test_cc_dict_2},
    {"cc_expr", test_cc_expr},
    {"cc_index", test_cc_index},
    {"cc_dot", test_cc_dot},
    {"cc_call", test_cc_call},
    {"cc_vec", test_cc_vec},
    {"cc_asscalc", test_cc_asscalc},
    {"cc_import_stmt", test_cc_import_stmt},
    {0},
};

/************
* traverser *
************/

static void
test_trv_long_code(void) {
     const char *src = "{@\n"
"    vec = []\n"
"    vec.push(1)\n"
"    vec.push(1)\n"
"    vec.push(1)\n"
"    vec.push(1)\n"
"    vec.push(1)\n"
"    vec.push(1)\n"
"    vec.push(1)\n"
"    vec.push(1)\n"
"    vec.push(1)\n"
"    vec.push(1)\n"
"    vec.push(1)\n"
"    vec.push(1)\n"
"    vec.push(1)\n"
"    vec.push(1)\n"
"    vec.push(1)\n"
"    vec.push(1)\n"
"    vec.push(1)\n"
"    vec.push(1)\n"
"    vec.push(1)\n"
"    vec.push(1)\n"
"    vec.push(1)\n"
"    vec.push(1)\n"
"    vec.push(1)\n"
"    vec.push(1)\n"
"    vec.push(1)\n"
"    vec.push(1)\n"
"    vec.push(1)\n"
"    vec.push(1)\n"
"    vec.push(1)\n"
"    vec.push(1)\n"
"    vec.push(1)\n"
"    vec.push(1)\n"
"    vec.push(1)\n"
"    vec.push(1)\n"
"    vec.push(1)\n"
"    vec.push(1)\n"
"    vec.push(1)\n"
"    puts(\"done\")\n"
"@}\n";

    check_ok(src, "done\n");
}

static void
test_trv_comparison(void) {
    /******
    * int *
    ******/

    check_ok("{@ a = 0 != 1 @}{: a :}", "true");
    check_ok("{@ a = 1 != 1 != 1 @}{: a :}", "true");
    check_ok("{@ a = 1 != 1 != 0 @}{: a :}", "false");
    check_ok("{@ a = 1 == 1 @}{: a :}", "true");
    check_ok("{@ a = 1 == 0 @}{: a :}", "false");
    check_ok("{@ a = 1 == 1 == 1 @}{: a :}", "true");
    check_ok("{@ a = 1 == 1 == 0 @}{: a :}", "false");
    check_ok("{@ a = 1 != 1 @}{: a :}", "false");

    check_ok("{: 1 == 1.0 :}", "true");
    check_ok("{: 1.0 == 1 :}", "true");
    check_ok("{: 1 != 1.0 :}", "false");
    check_ok("{: 1.0 != 1 :}", "false");

    check_ok("{: 1 == 1 :}", "true");
    check_ok("{: 1 != 1 :}", "false");

    check_ok("{: 1 == true :}", "true");
    check_ok("{: 1 != true :}", "false");
    check_ok("{: 1 == false :}", "false");
    check_ok("{: 1 != false :}", "true");
    check_ok("{: true == 1 :}", "true");
    check_ok("{: true != 1 :}", "false");
    check_ok("{: false == 1 :}", "false");
    check_ok("{: false != 1 :}", "true");

    check_ok("{: [] == 1 :}", "false");
    check_ok("{: [] != 1 :}", "true");
    check_ok("{: 1 == [] :}", "false");
    check_ok("{: 1 != [] :}", "true");

    check_ok("{: {} == 1 :}", "false");
    check_ok("{: {} != 1 :}", "true");
    check_ok("{: 1 == {} :}", "false");
    check_ok("{: 1 != {} :}", "true");

    check_ok("{@ def f(): end @}{: f == 1 :}", "false");
    check_ok("{@ def f(): end @}{: f != 1 :}", "true");
    check_ok("{@ def f(): end @}{: 1 == f :}", "false");
    check_ok("{@ def f(): end @}{: 1 != f :}", "true");

    check_ok("{@ struct A: end @}{: A == 1 :}", "false");
    check_ok("{@ struct A: end @}{: A != 1 :}", "true");
    check_ok("{@ struct A: end @}{: 1 == A :}", "false");
    check_ok("{@ struct A: end @}{: 1 != A :}", "true");

    check_ok("{@ struct A: end @}{: A() == 1 :}", "false");
    check_ok("{@ struct A: end @}{: A() != 1 :}", "true");
    check_ok("{@ struct A: end @}{: 1 == A() :}", "false");
    check_ok("{@ struct A: end @}{: 1 != A() :}", "true");

    /********
    * float *
    ********/

    check_ok("{@ a = 0 != 1.0 @}{: a :}", "true");
    check_ok("{@ a = 1.0 != 1.0 != 1.0 @}{: a :}", "true");
    check_ok("{@ a = 1.0 != 1.0 != 0 @}{: a :}", "false");
    check_ok("{@ a = 1.0 == 1.0 @}{: a :}", "true");
    check_ok("{@ a = 1.0 == 0 @}{: a :}", "false");
    check_ok("{@ a = 1.0 == 1.0 == 1.0 @}{: a :}", "true");
    check_ok("{@ a = 1.0 == 1.0 == 0 @}{: a :}", "false");
    check_ok("{@ a = 1.0 != 1.0 @}{: a :}", "false");

    check_ok("{: 1.0 == 1.0 :}", "true");
    check_ok("{: 1.0 != 1.0 :}", "false");

    check_ok("{: 1 == 1.0 :}", "true");
    check_ok("{: 1.0 == 1 :}", "true");
    check_ok("{: 1 != 1.0 :}", "false");
    check_ok("{: 1.0 != 1 :}", "false");

    check_ok("{: 1.0 == true :}", "true");
    check_ok("{: 1.0 != true :}", "false");
    check_ok("{: 1.0 == false :}", "false");
    check_ok("{: 1.0 != false :}", "true");
    check_ok("{: true == 1.0 :}", "true");
    check_ok("{: true != 1.0 :}", "false");
    check_ok("{: false == 1.0 :}", "false");
    check_ok("{: false != 1.0 :}", "true");

    check_ok("{: [] == 1.0 :}", "false");
    check_ok("{: [] != 1.0 :}", "true");
    check_ok("{: 1.0 == [] :}", "false");
    check_ok("{: 1.0 != [] :}", "true");

    check_ok("{: {} == 1.0 :}", "false");
    check_ok("{: {} != 1.0 :}", "true");
    check_ok("{: 1.0 == {} :}", "false");
    check_ok("{: 1.0 != {} :}", "true");

    check_ok("{@ def f(): end @}{: f == 1.0 :}", "false");
    check_ok("{@ def f(): end @}{: f != 1.0 :}", "true");
    check_ok("{@ def f(): end @}{: 1.0 == f :}", "false");
    check_ok("{@ def f(): end @}{: 1.0 != f :}", "true");

    check_ok("{@ struct A: end @}{: A == 1.0 :}", "false");
    check_ok("{@ struct A: end @}{: A != 1.0 :}", "true");
    check_ok("{@ struct A: end @}{: 1.0 == A :}", "false");
    check_ok("{@ struct A: end @}{: 1.0 != A :}", "true");

    check_ok("{@ struct A: end @}{: A() == 1.0 :}", "false");
    check_ok("{@ struct A: end @}{: A() != 1.0 :}", "true");
    check_ok("{@ struct A: end @}{: 1.0 == A() :}", "false");
    check_ok("{@ struct A: end @}{: 1.0 != A() :}", "true");

    /*******
    * bool *
    *******/

    check_ok("{: true == true :}", "true");
    check_ok("{: false == false :}", "true");
    check_ok("{: true != true :}", "false");
    check_ok("{: false != false :}", "false");
    check_ok("{: false == true :}", "false");
    check_ok("{: true == false :}", "false");
    check_ok("{: false != true :}", "true");
    check_ok("{: false != false :}", "false");

    check_ok("{: true == \"a\" :}", "false");
    check_ok("{: false == \"a\" :}", "false");
    check_ok("{: true != \"a\" :}", "true");
    check_ok("{: false != \"a\" :}", "true");
    check_ok("{: \"a\" == true :}", "false");
    check_ok("{: \"a\" == false :}", "false");
    check_ok("{: \"a\" != true :}", "true");
    check_ok("{: \"a\" != false :}", "true");

    check_ok("{: true == [] :}", "false");
    check_ok("{: false == [] :}", "false");
    check_ok("{: true != [] :}", "true");
    check_ok("{: false != [] :}", "true");
    check_ok("{: [] == true :}", "false");
    check_ok("{: [] == false :}", "false");
    check_ok("{: [] != true :}", "true");
    check_ok("{: [] != false :}", "true");

    check_ok("{: true == {} :}", "false");
    check_ok("{: false == {} :}", "false");
    check_ok("{: true != {} :}", "true");
    check_ok("{: false != {} :}", "true");
    check_ok("{: {} == true :}", "false");
    check_ok("{: {} == false :}", "false");
    check_ok("{: {} != true :}", "true");
    check_ok("{: {} != false :}", "true");

    check_ok("{@ def f(): end @}{: true == f :}", "false");
    check_ok("{@ def f(): end @}{: false == f :}", "false");
    check_ok("{@ def f(): end @}{: true != f :}", "true");
    check_ok("{@ def f(): end @}{: false != f :}", "true");
    check_ok("{@ def f(): end @}{: f == true :}", "false");
    check_ok("{@ def f(): end @}{: f == false :}", "false");
    check_ok("{@ def f(): end @}{: f != true :}", "true");
    check_ok("{@ def f(): end @}{: f != false :}", "true");

    check_ok("{@ struct A: end @}{: true == A :}", "false");
    check_ok("{@ struct A: end @}{: false == A :}", "false");
    check_ok("{@ struct A: end @}{: true != A :}", "true");
    check_ok("{@ struct A: end @}{: false != A :}", "true");
    check_ok("{@ struct A: end @}{: A == true :}", "false");
    check_ok("{@ struct A: end @}{: A == false :}", "false");
    check_ok("{@ struct A: end @}{: A != true :}", "true");
    check_ok("{@ struct A: end @}{: A != false :}", "true");

    check_ok("{@ struct A: end @}{: true == A() :}", "false");
    check_ok("{@ struct A: end @}{: false == A() :}", "false");
    check_ok("{@ struct A: end @}{: true != A() :}", "true");
    check_ok("{@ struct A: end @}{: false != A() :}", "true");
    check_ok("{@ struct A: end @}{: A() == true :}", "false");
    check_ok("{@ struct A: end @}{: A() == false :}", "false");
    check_ok("{@ struct A: end @}{: A() != true :}", "true");
    check_ok("{@ struct A: end @}{: A() != false :}", "true");

    /**********
    * unicode *
    **********/

    check_ok("{@ a = 1 == \"abc\" @}{: a :}", "false");
    check_ok("{@ a = \"abc\" == 1 @}{: a :}", "false");
    check_ok("{@ a = \"abc\" == \"abc\" @}{: a :}", "true");
    check_ok("{@ a = 1 != \"abc\" @}{: a :}", "true");
    check_ok("{@ a = \"abc\" != 1 @}{: a :}", "true");
    check_ok("{@ a = \"abc\" != \"def\" @}{: a :}", "true");
    check_ok("{@ a = \"abc\" == \"abc\" @}{: a :}", "true");

    /**
     * well-formed on Python
     * ill-formed on Ruby
     */
    check_ok("{@ a = \"abc\" == \"abc\" == \"def\" @}{: a :}", "false");
    check_ok("{@ a = \"abc\" == \"abc\" @}{: a :}", "true");

    check_ok("{: \"a\" == [] :}", "false");
    check_ok("{: \"a\" != [] :}", "true");
    check_ok("{: [] == \"a\" :}", "false");
    check_ok("{: [] != \"a\" :}", "true");

    check_ok("{: \"a\" == {} :}", "false");
    check_ok("{: \"a\" != {} :}", "true");
    check_ok("{: {} == \"a\" :}", "false");
    check_ok("{: {} != \"a\" :}", "true");

    check_ok("{@ def f(): end @}{: \"a\" == f :}", "false");
    check_ok("{@ def f(): end @}{: \"a\" != f :}", "true");
    check_ok("{@ def f(): end @}{: f == \"a\" :}", "false");
    check_ok("{@ def f(): end @}{: f != \"a\" :}", "true");

    check_ok("{@ struct A: end @}{: \"a\" == A :}", "false");
    check_ok("{@ struct A: end @}{: \"a\" != A :}", "true");
    check_ok("{@ struct A: end @}{: A == \"a\" :}", "false");
    check_ok("{@ struct A: end @}{: A != \"a\" :}", "true");

    check_ok("{@ struct A: end @}{: \"a\" == A() :}", "false");
    check_ok("{@ struct A: end @}{: \"a\" != A() :}", "true");
    check_ok("{@ struct A: end @}{: A() == \"a\" :}", "false");
    check_ok("{@ struct A: end @}{: A() != \"a\" :}", "true");

    /********
    * vec *
    ********/

    check_ok("{: [] == [] :}", "false");
    check_ok("{: [] != [] :}", "true");

    check_ok("{: [] == {} :}", "false");
    check_ok("{: [] != {} :}", "true");
    check_ok("{: {} == [] :}", "false");
    check_ok("{: {} != [] :}", "true");

    check_ok("{@ def f(): end @}{: [] == f :}", "false");
    check_ok("{@ def f(): end @}{: [] != f :}", "true");
    check_ok("{@ def f(): end @}{: f == [] :}", "false");
    check_ok("{@ def f(): end @}{: f != [] :}", "true");

    check_ok("{@ struct A: end @}{: A == [] :}", "false");
    check_ok("{@ struct A: end @}{: A != [] :}", "true");
    check_ok("{@ struct A: end @}{: [] == A :}", "false");
    check_ok("{@ struct A: end @}{: [] != A :}", "true");

    check_ok("{@ struct A: end @}{: A() == [] :}", "false");
    check_ok("{@ struct A: end @}{: A() != [] :}", "true");
    check_ok("{@ struct A: end @}{: [] == A() :}", "false");
    check_ok("{@ struct A: end @}{: [] != A() :}", "true");

    /*******
    * dict *
    *******/

    check_ok("{: {} == {} :}", "false");
    check_ok("{: {} != {} :}", "true");

    check_ok("{@ def f(): end @}{: {} == f :}", "false");
    check_ok("{@ def f(): end @}{: {} != f :}", "true");
    check_ok("{@ def f(): end @}{: f == {} :}", "false");
    check_ok("{@ def f(): end @}{: f != {} :}", "true");

    check_ok("{@ struct A: end @}{: A == {} :}", "false");
    check_ok("{@ struct A: end @}{: A != {} :}", "true");
    check_ok("{@ struct A: end @}{: {} == A :}", "false");
    check_ok("{@ struct A: end @}{: {} != A :}", "true");

    check_ok("{@ struct A: end @}{: A() == {} :}", "false");
    check_ok("{@ struct A: end @}{: A() != {} :}", "true");
    check_ok("{@ struct A: end @}{: {} == A() :}", "false");
    check_ok("{@ struct A: end @}{: {} != A() :}", "true");

    /***********
    * function *
    ***********/

    check_ok("{@ def f(): end \n a = 1 == f @}{: a :}", "false");
    check_ok("{@ def f(): end \n a = \"abc\" == f @}{: a :}", "false");
    check_ok("{@ def f(): end \n a = f != 1 @}{: a :}", "true");
    check_ok("{@ def f(): end \n a = 1 != f @}{: a :}", "true");

    check_ok("{@ struct A: end \n def f(): end @}{: A == f :}", "false")
    check_ok("{@ struct A: end \n def f(): end @}{: A != f :}", "true")
    check_ok("{@ struct A: end \n def f(): end @}{: f == A:}", "false")
    check_ok("{@ struct A: end \n def f(): end @}{: f != A :}", "true")

    /*********
    * object *
    *********/

    check_ok("{@ struct A: end @}{: A() != nil :}", "true");
    check_ok("{@ struct A: end @}{: nil != A() :}", "true");
    check_ok("{@ struct A: end @}{: A() == nil :}", "false");
    check_ok("{@ struct A: end @}{: nil == A() :}", "false");

    check_ok("{@ struct A: end \n def f(): end @}{: A() != f :}", "true");
    check_ok("{@ struct A: end \n def f(): end @}{: f != A() :}", "true");
    check_ok("{@ struct A: end \n def f(): end @}{: A() == f :}", "false");
    check_ok("{@ struct A: end \n def f(): end @}{: f == A() :}", "false");

    check_ok("{@ struct A: end \n vec = [] @}{: A() != vec :}", "true");
    check_ok("{@ struct A: end \n vec = [] @}{: vec != A() :}", "true");
    check_ok("{@ struct A: end \n vec = [] @}{: A() == vec :}", "false");
    check_ok("{@ struct A: end \n vec = [] @}{: vec == A() :}", "false");

    check_ok("{@ struct A: end \n d = {} @}{: A() != d :}", "true");
    check_ok("{@ struct A: end \n d = {} @}{: d != A() :}", "true");
    check_ok("{@ struct A: end \n d = {} @}{: A() == d :}", "false");
    check_ok("{@ struct A: end \n d = {} @}{: d == A() :}", "false");

    check_ok("{@ struct A: end @}{: A() != 1 :}", "true");
    check_ok("{@ struct A: end @}{: 1 != A() :}", "true");
    check_ok("{@ struct A: end @}{: A() == 1 :}", "false");
    check_ok("{@ struct A: end @}{: 1 == A() :}", "false");

    check_ok("{@ struct A: end @}{: A() != \"a\" :}", "true");
    check_ok("{@ struct A: end @}{: \"a\" != A() :}", "true");
    check_ok("{@ struct A: end @}{: A() == \"a\" :}", "false");
    check_ok("{@ struct A: end @}{: \"a\" == A() :}", "false");

    check_ok("{@ struct A: end @}{: A != nil :}", "true");
    check_ok("{@ struct A: end @}{: nil != A :}", "true");
    check_ok("{@ struct A: end @}{: A == nil :}", "false");
    check_ok("{@ struct A: end @}{: nil == A :}", "false");

    check_ok("{@ struct A: end \n def f(): end @}{: A != f :}", "true");
    check_ok("{@ struct A: end \n def f(): end @}{: f != A :}", "true");
    check_ok("{@ struct A: end \n def f(): end @}{: A == f :}", "false");
    check_ok("{@ struct A: end \n def f(): end @}{: f == A :}", "false");

    check_ok("{@ struct A: end \n vec = [] @}{: A != vec :}", "true");
    check_ok("{@ struct A: end \n vec = [] @}{: vec != A :}", "true");
    check_ok("{@ struct A: end \n vec = [] @}{: A == vec :}", "false");
    check_ok("{@ struct A: end \n vec = [] @}{: vec == A :}", "false");

    check_ok("{@ struct A: end \n d = {} @}{: A != d :}", "true");
    check_ok("{@ struct A: end \n d = {} @}{: d != A :}", "true");
    check_ok("{@ struct A: end \n d = {} @}{: A == d :}", "false");
    check_ok("{@ struct A: end \n d = {} @}{: d == A :}", "false");

    check_ok("{@ struct A: end @}{: A != 1 :}", "true");
    check_ok("{@ struct A: end @}{: 1 != A :}", "true");
    check_ok("{@ struct A: end @}{: A == 1 :}", "false");
    check_ok("{@ struct A: end @}{: 1 == A :}", "false");

    check_ok("{@ struct A: end @}{: A != \"a\" :}", "true");
    check_ok("{@ struct A: end @}{: \"a\" != A :}", "true");
    check_ok("{@ struct A: end @}{: A == \"a\" :}", "false");
    check_ok("{@ struct A: end @}{: \"a\" == A :}", "false");

    /*********************************
    * boolean can convert to integer *
    *********************************/

    check_ok("{@ a = true == 1 @}{: a :}", "true");
    check_ok("{@ a = false == 0 @}{: a :}", "true");

    /******
    * lte *
    ******/

    // int
    check_ok("{: 1 <= 2 :}", "true");
    check_ok("{: 2 <= 1 :}", "false");

    check_ok("{: 1.0 <= 2 :}", "true");
    check_ok("{: 1.0 <= 0 :}", "false");
    check_ok("{: 0 <= 1.0 :}", "true");
    check_ok("{: 2 <= 1.0 :}", "false");

    check_ok("{: true <= 2 :}", "true");
    check_ok("{: true <= 0 :}", "false");
    check_ok("{: 0 <= true :}", "true");
    check_ok("{: 2 <= true :}", "false");

    check_ok("{@ def f(n): return n end @}{: 1 <= f(2) :}", "true")

    check_ok("{@ def f(n): return n end @}{: 1 <= ord(\"a\") :}", "true")

    // check_fail("", ""); TODO [] {} def etc

    /******
    * gte *
    ******/

    check_ok("{@ a = 1 >= 2 @}{: a :}", "false");
    check_ok("{@ a = 2 >= 1 @}{: a :}", "true");
    check_ok("{@ a = true >= 2 @}{: a :}", "false");
    check_ok("{@ a = true >= 0 @}{: a :}", "true");
    check_ok("{@ a = 0 >= true @}{: a :}", "false");
    check_ok("{@ a = 2 >= true @}{: a :}", "true");

    /*****
    * lt *
    *****/

    check_ok("{@ a = 1 < 2 @}{: a :}", "true");
    check_ok("{@ a = 2 < 1 @}{: a :}", "false");
    check_ok("{@ a = true < 2 @}{: a :}", "true");
    check_ok("{@ a = true < 1 @}{: a :}", "false");
    check_ok("{@ a = 0 < true @}{: a :}", "true");
    check_ok("{@ a = 1 < true @}{: a :}", "false");

    /*****
    * gt *
    *****/

    check_ok("{@ a = 1 > 1 @}{: a :}", "false");
    check_ok("{@ a = 2 > 1 @}{: a :}", "true");
    check_ok("{@ a = true > 1 @}{: a :}", "false");
    check_ok("{@ a = true > 0 @}{: a :}", "true");
    check_ok("{@ a = 0 > true @}{: a :}", "false");
    check_ok("{@ a = 2 > true @}{: a :}", "true");
}

static void
test_trv_vec_index(void) {
    // check_fail("{@ a[0] @}", "can't index access. \"a\" is not defined");

    check_ok("{@ a = [1, 2] \n @}{: a[0] :}", "1");
    check_ok("{@ a = [1, 2] \n @}{: a[1] :}", "2");
    check_ok("{@ a = [1, 2] \n @}{: a[0] :},{: a[1] :}", "1,2");

    check_fail("{@ a = [1, 2] \n @}{: a[2] :}", "index out of range");

    // check_fail("{@ a = [1, 2] \n @}{: a[-1] :}", "index out of range of vec");

    // check_ok("{@ a = (b, c = 1, 2)[0] \n @}{: a :}", "1");
    // check_ok("{@ a = (b, c = 1, 2)[1] \n @}{: a :}", "2");

    check_ok("{@ a = [[1, 2]] \n @}{: a[0] :}", "[1, 2]");
    check_ok("{@ a = [[1, 2]] \n @}{: a[0][0] :}", "1");
}

static void
test_trv_text_block_old(void) {
    check_ok("abc", "abc");
}

static void
test_trv_ref_block_old(void) {
    check_ok("{: nil :}", "nil");
    check_ok("{: false :}", "false");
    check_ok("{: true :}", "true");
    check_ok("{: 1 :}", "1");
    check_ok("{: 123 :}", "123");
    check_ok("{: \"abc\" :}", "abc");

    check_fail("{: a :}", "\"a\" is not defined in ref block");

    // check_fail("{: alias(\"dtl\", \"run bin/date-line.py\") :}", "nil");

    check_ok("{: 1 + 1 :}", "2");
    check_ok("{: 1 + 1 + 1 :}", "3");
    check_ok("{: [1, 2] :}", "[1, 2]");
}

static void
test_trv_assign_0(void) {
    check_ok("{@ a = 1 @}{: a :}", "1");
    check_ok("{@ a = 0\n"
        "for i=0; i!=4; i+=1:\n"
        "   if i == 2:\n"
        "   end\n"
        "   a += 1\n"
        "end @}{: a :}", "4");
}

static void
test_trv_assign_1(void) {
    check_ok("{@\n"
"   import \"tests/lang/modules/string.aji\" as string\n"
"@}{: string :}", "(module)");
    check_ok("{@\n"
"   import \"tests/lang/modules/string.aji\" as string\n"
"   string.a = 1\n"
"@}", "");
    check_ok("{@\n"
"   import \"tests/lang/modules/string.aji\" as string\n"
"   string.a = 1\n"
"@}{: string :}", "(module)");
    check_ok("{@\n"
"   import \"tests/lang/modules/string.aji\" as string\n"
"   string.a = 1\n"
"@}{: string.a :}", "1");
    check_ok("{@\n"
"   import \"tests/lang/modules/string.aji\" as string\n"
"   string.a = 1\n"
"   string.b = string.a\n"
"@}{: string.b :}", "1");
}

static void
test_trv_assign_2(void) {
    check_ok("{@\n"
"   import \"tests/lang/modules/string.aji\" as string\n"
"   string.a = 1\n"
"@}{: string.a :}", "1");
}

static void
test_trv_assign_3(void) {
    check_ok("{@\n"
"   a = 1\n"
"@}{: a :}", "1");
}

static void
test_trv_assign_4(void) {
    check_ok("{@\n"
    "   a, b = 1, 2\n"
    "@}{: a :},{: b :}", "1,2");
}

static void
test_trv_assign_5(void) {
    check_fail("{@ a @}{: a :}", "\"a\" is not defined in ref block");
}

static void
test_trv_atom_0(void) {
    check_ok("{@ nil @}", "");
    check_ok("{@ false @}", "");
    check_ok("{@ true @}", "");
    check_ok("{@ 1 @}", "");
    check_ok("{@ \"abc\" @}", "");
    check_ok("{@ var @}", "");
}

static void
test_trv_index(void) {
    check_ok("{@ a = \"abc\" @}{: a :}", "abc");
    check_ok("{@ a = \"abc\" @}{: a[0] :}", "a");
    check_ok("{@ a = \"abc\" @}{: a[0] :},{: a[1] :},{: a[2] :}", "a,b,c");
    check_ok("{@ a = [1, 2] @}{: a[0] :}", "1");
    check_ok("{@ a = [1, 2] @}{: a[0] :},{: a[1] :}", "1,2");
    check_ok("{@ a = {\"a\": 1, \"b\": 2} @}{: a[\"a\"] :}", "1");
    check_ok("{@ a = {\"a\": 1, \"b\": 2} @}{: a[\"a\"] :},{: a[\"b\"] :}", "1,2");
    check_ok("{@ a = \"abc\" \n b = a[0] or a[1] @}{: b :}", "a");
    check_ok("{@ a = \"abc\" \n b = a[0] and a[1] @}{: b :}", "b");
    check_ok("{@ a = \"abc\" \n b = not a[0] @}{: b :}", "false");
    check_ok("{@ a = [1,2] \n b = a[0] or a[1] @}{: b :}", "1");
    check_ok("{@ a = [1,2] \n b = a[0] and a[1] @}{: b :}", "2");
    check_ok("{@ a = [1,2] \n b = not a[0] @}{: b :}", "false");
    check_ok("{@ a = {\"a\":1, \"b\":2} \n b = a[\"a\"] or a[\"b\"] @}{: b :}", "1");
    check_ok("{@ a = {\"a\":1, \"b\":2} \n b = a[\"a\"] and a[\"b\"] @}{: b :}", "2");
    check_ok("{@ a = {\"a\":1, \"b\":2} \n b = not a[\"a\"] @}{: b :}", "false");
    check_ok("{@ a = \"abc\" \n b = a[0] == \"a\" @}{: b :}", "true");
    check_ok("{@ a = \"abc\" \n b = \"a\" == a[0] @}{: b :}", "true");
    check_ok("{@ a = \"abc\" \n b = a[0] != \"a\" @}{: b :}", "false");
    check_ok("{@ a = \"abc\" \n b = \"a\" != a[0] @}{: b :}", "false");
    check_ok("{@ a = [1,2] \n b = a[0] == 1 @}{: b :}", "true");
    check_ok("{@ a = [1,2] \n b = 1 == a[0] @}{: b :}", "true");
    check_ok("{@ a = [1,2] \n b = a[0] != 1 @}{: b :}", "false");
    check_ok("{@ a = [1,2] \n b = 1 != a[0] @}{: b :}", "false");
    check_ok("{@ a = {\"a\":1, \"b\":2} \n b = a[\"a\"] == 1 @}{: b :}", "true");
    check_ok("{@ a = {\"a\":1, \"b\":2} \n b = 1 == a[\"a\"] @}{: b :}", "true");
    check_ok("{@ a = {\"a\":1, \"b\":2} \n b = a[\"a\"] != 1 @}{: b :}", "false");
    check_ok("{@ a = {\"a\":1, \"b\":2} \n b = 1 != a[\"a\"] @}{: b :}", "false");
    check_ok("{@ a = \"abc\" \n if a[0] == \"a\": puts(1) end @}", "1\n");
    check_ok("{@ a = [1,2] \n a[0] = 3 @}{: a[0] :},{: a[1] :}", "3,2");
    check_ok("{@ a = [1,2] \n a[0] = 3 \n a[1] = 4 @}{: a[0] :},{: a[1] :}", "3,4");
    check_ok("{@ a = [\"a\",\"b\"] \n a[0] = \"c\" @}{: a[0] :},{: a[1] :}", "c,b");
    check_ok("{@ a = [\"a\",\"b\"] \n a[0] = \"c\" \n a[1] = \"d\" @}{: a[0] :},{: a[1] :}", "c,d");
    check_ok("{@ a = {\"a\":1, \"b\":2 } \n a[\"a\"] = 3 @}{: a[\"a\"] :},{: a[\"b\"] :}", "3,2");
    check_ok("{@ a = {\"a\":1, \"b\":2 } \n a[\"a\"] = 3 \n a[\"b\"] = 4 @}{: a[\"a\"] :},{: a[\"b\"] :}", "3,4");
    check_ok("{@ a = [] a.push(1) @}{: a[0] :}", "1");
    check_ok("{@ a = [\"abc_def\"] @}{: a[0].camel() :}", "abcDef");
}

static void
test_trv_string_index(void) {
    check_ok("{@ a = \"ab\" \n @}{: a[0] :}", "a");
    check_ok("{@ a = \"ab\" \n @}{: a[1] :}", "b");
    check_fail("{@ a = \"ab\" \n @}{: a[2] :}", "index out of range");
    check_ok("{@ a = (\"a\" + \"b\")[0] \n @}{: a :}", "a");
    check_ok("{@ a = (\"a\" + \"b\")[1] \n @}{: a :}", "b");
    check_ok("{@ a = \"ab\"[0][0] @}{: a :}", "a");
}

static void
test_trv_multi_assign(void) {
    // error

    check_fail("{@ a, b = 1, 2, 3 @}{: a :} {: b :}", "can't assign vec to vec. not same length");
    check_fail("{@ a, b = 2 @}", "invalid right operand (1)");

    // success

    check_ok("{@ a, b = 1, 2 @}{: a :} {: b :}", "1 2");
    check_ok("{@ a = 1, 2 @}{: a :}", "[1, 2]");
}

static void
test_trv_multi_assign_2(void) {
    check_ok("{@ a = [1, 2] \n b, c = a @}{: b :},{: c :}", "1,2");
}

static void
test_trv_and_test(void) {
    // nil and objects

    check_ok("{@ a = nil and nil @}{: a :}", "nil");
    check_ok("{@ a = nil and 1 @}{: a :}", "nil");
    check_ok("{@ a = nil and 0 @}{: a :}", "nil");
    check_ok("{@ a = nil and true @}{: a :}", "nil");
    check_ok("{@ a = nil and false @}{: a :}", "nil");
    check_ok("{@ a = nil and \"\" @}{: a :}", "nil");
    check_ok("{@ a = nil and \"abc\" @}{: a :}", "nil");
    check_ok("{@ def f(): end \n a = nil and f @}{: a :}", "nil");
    check_ok("{@ a = nil and [1, 2] @}{: a :}", "nil");
    check_ok("{@ a = nil and [] @}{: a :}", "nil");

    // digit and objects

    check_ok("{@ a = 1 and 1 @}{: a :}", "1");
    check_ok("{@ a = 1 and 2 @}{: a :}", "2");
    check_ok("{@ a = 1 and 2 and 3 @}{: a :}", "3");
    check_ok("{@ a = 1 and nil @}{: a :}", "nil");
    check_ok("{@ a = 0 and nil @}{: a :}", "0");
    check_ok("{@ a = 0 and true @}{: a :}", "0");
    check_ok("{@ a = 1 and true @}{: a :}", "true");
    check_ok("{@ a = 0 and false @}{: a :}", "0");
    check_ok("{@ a = 1 and false @}{: a :}", "false");
    check_ok("{@ a = 1 and \"\" @}{: a :}", "");
    check_ok("{@ a = 0 and \"\" @}{: a :}", "0");

    check_ok("{@ a = 0 and \"abc\" @}{: a :}", "0");
    check_ok("{@ a = 1 and \"abc\" @}{: a :}", "abc");
    check_ok("{@ a = 1 and [] @}{: a :}", "[]");
    check_ok("{@ a = 0 and [] @}{: a :}", "0");
    check_ok("{@ a = 0 and [1, 2] @}{: a :}", "0");
    check_ok("{@ a = 1 and [1, 2] @}{: a :}", "[1, 2]");
    check_ok("{@ def f(): end \n a = 1 and f @}{: a :}", "(function)");
    check_ok("{@ def f(): end \n a = 0 and f @}{: a :}", "0");

    // bool and objects

    check_ok("{@ a = true and nil @}{: a :}", "nil");
    check_ok("{@ a = false and nil @}{: a :}", "false");
    check_ok("{@ a = true and 1 @}{: a :}", "1");
    check_ok("{@ a = true and 0 @}{: a :}", "0");
    check_ok("{@ a = false and 0 @}{: a :}", "false");
    check_ok("{@ a = false and \"\" @}{: a :}", "false");
    check_ok("{@ a = false and \"abc\" @}{: a :}", "false");
    check_ok("{@ a = true and \"abc\" @}{: a :}", "abc");
    check_ok("{@ a = true and [] @}{: a :}", "[]");
    check_ok("{@ a = false and [] @}{: a :}", "false");
    check_ok("{@ a = false and [1, 2] @}{: a :}", "false");
    check_ok("{@ a = true and [1, 2] @}{: a :}", "[1, 2]");
    check_ok("{@ a = true and [] @}{: a :}", "[]");

    // string and other

    check_ok("{@ a = \"abc\" and nil @}{: a :}", "nil");
    check_ok("{@ a = \"abc\" and false @}{: a :}", "false");
    check_ok("{@ a = \"abc\" and true @}{: a :}", "true");
    check_ok("{@ a = \"abc\" and 0 @}{: a :}", "0");
    check_ok("{@ a = \"abc\" and 1 @}{: a :}", "1");
    check_ok("{@ a = \"abc\" and \"\" @}{: a :}", "");
    check_ok("{@ a = \"abc\" and \"def\" @}{: a :}", "def");
    check_ok("{@ a = \"abc\" and [] @}{: a :}", "[]");
    check_ok("{@ a = \"abc\" and [1, 2] @}{: a :}", "[1, 2]");
    check_ok("{@ a = \"abc\" and {} @}{: a :}", "{}");
    check_ok("{@ a = \"abc\" and {\"k\":1} @}{: a :}", "{\"k\": 1}");
    check_ok("{@ def f(): end \n a = \"abc\" and f @}{: a :}", "(function)");
    check_ok("{@ b = 1 \n a = \"abc\" and b @}{: a :}", "1");
    check_ok("{@ b = 0 \n a = \"abc\" and b @}{: a :}", "0");
    check_ok("{@ a = \"\" and nil @}{: a :}", "");
    check_ok("{@ a = \"\" and false @}{: a :}", "");
    check_ok("{@ a = \"\" and true @}{: a :}", "");
    check_ok("{@ a = \"\" and 0 @}{: a :}", "");
    check_ok("{@ a = \"\" and 1 @}{: a :}", "");
    check_ok("{@ a = \"\" and \"\" @}{: a :}", "");
    check_ok("{@ a = \"\" and \"def\" @}{: a :}", "");
    check_ok("{@ a = \"\" and [] @}{: a :}", "");
    check_ok("{@ a = \"\" and [1, 2] @}{: a :}", "");
    check_ok("{@ a = \"\" and {} @}{: a :}", "");
    check_ok("{@ a = \"\" and {\"k\":1} @}{: a :}", "");
    check_ok("{@ def f(): end \n a = \"\" and f @}{: a :}", "");
    check_ok("{@ b = 1 \n a = \"\" and b @}{: a :}", "");
    check_ok("{@ b = 0 \n a = \"\" and b @}{: a :}", "");

    // vec and other

    check_ok("{@ a = [1, 2] and nil @}{: a :}", "nil");
    check_ok("{@ a = [1, 2] and false @}{: a :}", "false");
    check_ok("{@ a = [1, 2] and true @}{: a :}", "true");
    check_ok("{@ a = [1, 2] and 0 @}{: a :}", "0");
    check_ok("{@ a = [1, 2] and 1 @}{: a :}", "1");
    check_ok("{@ a = [1, 2] and \"\" @}{: a :}", "");
    check_ok("{@ a = [1, 2] and \"def\" @}{: a :}", "def");
    check_ok("{@ a = [1, 2] and [] @}{: a :}", "[]");
    check_ok("{@ a = [1, 2] and [1, 2] @}{: a :}", "[1, 2]");
    check_ok("{@ a = [1, 2] and {} @}{: a :}", "{}");
    check_ok("{@ a = [1, 2] and {\"k\":1} @}{: a :}", "{\"k\": 1}");
    check_ok("{@ def f(): end \n a = [1, 2] and f @}{: a :}", "(function)");
    check_ok("{@ b = 1 \n a = [1, 2] and b @}{: a :}", "1");
    check_ok("{@ b = 0 \n a = [1, 2] and b @}{: a :}", "0");
    check_ok("{@ a = [] and nil @}{: a :}", "[]");
    check_ok("{@ a = [] and false @}{: a :}", "[]");
    check_ok("{@ a = [] and true @}{: a :}", "[]");
    check_ok("{@ a = [] and 0 @}{: a :}", "[]");
    check_ok("{@ a = [] and 1 @}{: a :}", "[]");
    check_ok("{@ a = [] and [] @}{: a :}", "[]");
    check_ok("{@ a = [] and \"def\" @}{: a :}", "[]");
    check_ok("{@ a = [] and [] @}{: a :}", "[]");
    check_ok("{@ a = [] and [1, 2] @}{: a :}", "[]");
    check_ok("{@ a = [] and {} @}{: a :}", "[]");
    check_ok("{@ a = [] and {\"k\":1} @}{: a :}", "[]");
    check_ok("{@ def f(): end \n a = [] and f @}{: a :}", "[]");
    check_ok("{@ b = 1 \n a = [] and b @}{: a :}", "[]");
    check_ok("{@ b = 0 \n a = [] and b @}{: a :}", "[]");

    // dict and other

    check_ok("{@ a = {\"k\": 1} and nil @}{: a :}", "nil");
    check_ok("{@ a = {\"k\": 1} and false @}{: a :}", "false");
    check_ok("{@ a = {\"k\": 1} and true @}{: a :}", "true");
    check_ok("{@ a = {\"k\": 1} and 0 @}{: a :}", "0");
    check_ok("{@ a = {\"k\": 1} and 1 @}{: a :}", "1");
    check_ok("{@ a = {\"k\": 1} and \"\" @}{: a :}", "");
    check_ok("{@ a = {\"k\": 1} and \"def\" @}{: a :}", "def");
    check_ok("{@ a = {\"k\": 1} and [] @}{: a :}", "[]");
    check_ok("{@ a = {\"k\": 1} and [1, 2] @}{: a :}", "[1, 2]");
    check_ok("{@ a = {\"k\": 1} and {} @}{: a :}", "{}");
    check_ok("{@ a = {\"k\": 1} and {\"k\": 1} @}{: a :}", "{\"k\": 1}");
    check_ok("{@ def f(): end \n a = {\"k\": 1} and f @}{: a :}", "(function)");
    check_ok("{@ b = 1 \n a = {\"k\": 1} and b @}{: a :}", "1");
    check_ok("{@ b = 0 \n a = {\"k\": 1} and b @}{: a :}", "0");
    check_ok("{@ a = {} and nil @}{: a :}", "{}");
    check_ok("{@ a = {} and false @}{: a :}", "{}");
    check_ok("{@ a = {} and true @}{: a :}", "{}");
    check_ok("{@ a = {} and 0 @}{: a :}", "{}");
    check_ok("{@ a = {} and 1 @}{: a :}", "{}");
    check_ok("{@ a = {} and [] @}{: a :}", "{}");
    check_ok("{@ a = {} and \"def\" @}{: a :}", "{}");
    check_ok("{@ a = {} and [] @}{: a :}", "{}");
    check_ok("{@ a = {} and [1, 2] @}{: a :}", "{}");
    check_ok("{@ a = {} and {} @}{: a :}", "{}");
    check_ok("{@ a = {} and {\"k\": 1} @}{: a :}", "{}");
    check_ok("{@ def f(): end \n a = {} and f @}{: a :}", "{}");
    check_ok("{@ b = 1 \n a = {} and b @}{: a :}", "{}");
    check_ok("{@ b = 0 \n a = {} and b @}{: a :}", "{}");
    check_ok("{@ a = \"abc\" and 1 @}{: a :}", "1");
    check_ok("{@ a = 1 and \"abc\" @}{: a :}", "abc");
    check_ok("{@ def f(): end \n a = 1 and f @}{: a :}", "(function)");
}

static void
test_trv_assign_list(void) {
    // success

    check_ok("{@ a = nil @}{: a :}", "nil");
    check_ok("{@ a = false @}{: a :}", "false");
    check_ok("{@ a = true @}{: a :}", "true");
    check_ok("{@ a = \"abc\" @}{: a :}", "abc");
    check_ok("{@ a = \"abc\"\n b = a @}{: b :}", "abc");
    check_ok("{@ a = 1 @}{: a :}", "1");
    check_ok("{@ a = 1, b = 2 @}{: a :},{: b :}", "1,2");
    check_ok("{@ a = 1 + 2, b = 3 * 4 @}{: a :},{: b :}", "3,12");
    check_ok("{@ a = 1, b = 2, c = 3 @}{: a :},{: b :},{: c :}", "1,2,3");
    check_ok("{@ a = 0 \n b = a = 1 @}{: a :},{: b :}", "1,1");
    check_ok("{@ a = 0 \n b = a = 1, c = b = 1 @}{: a :},{: b :},{: c :}", "1,1,1");
    check_ok("{@ a = \"abc\" @}{: a :}", "abc");
    check_ok("{@ a, b = 1, 2 @}{: a :},{: b :}", "1,2");
}

static void
test_trv_test_list(void) {
    check_ok("{@ 1, 2 @}", "");
    // check_ok("{@ 1, \"abc\", var, alias.set(\"\", \"\") @}", "");
    check_ok("{@ a = 0 \n b = 0 \n a += 1, b += 2 @}{: a :} {: b :}", "1 2");
}

static void
test_trv_negative_0(void) {
    check_ok("{: 1 :}", "1");
    check_ok("{: -1 :}", "-1");
    check_ok("{: 1 + -1 :}", "0");
    check_ok("{: -1 + -1 :}", "-2");
    check_ok("{: 1 - -1 :}", "2");
    check_ok("{: -1 - -1 :}", "0");
    check_ok("{: 1-1 :}", "0");
    check_ok("{: -true :}", "-1");
    check_ok("{: -1.0 :}", "-1.0");
    check_ok("{@ a = 1 @}{: -a :}", "-1");
}

static void
test_trv_dot_0(void) {
    check_ok("{: \"ABC\".lower() :}", "abc");
    check_ok("{: \"abc\".upper() :}", "ABC");
    check_ok("{: \"ABC\".lower().upper() :}", "ABC");
    // check_ok("{: alias.set(\"a\", \"b\") :}", "nil");
}

static void
test_trv_dot_1(void) {
    check_ok("{@\n"
    "   import \"tests/lang/modules/string.aji\" as string\n"
    "@}{: string.variable.upper() :}", "STRING");
}

static void
test_trv_dot_2(void) {
    check_ok("{@\n"
    "    vec = [1, 2]\n"
    "    dst = []\n"
    "    dst.push(vec[1])\n"
    "@}{: dst[0] :}"
    , "2");
}

static void
test_trv_dot_3(void) {
    check_ok("{@\n"
    "    vec = [1, 2]\n"
    "    dst = []\n"
    "    dst.push(vec.pop())\n"
    "@}{: dst[0] :}"
    , "2");
}

static void
test_trv_dot_4(void) {
    check_ok("{@\n"
    "    vec = [[1, 2], [3, 4]]\n"
    "    dst = []\n"
    "    n = dst.push(vec.pop().pop()).pop()\n"
    "@}{: n :}", "4");
}

static void
test_trv_dot_5(void) {
    check_ok("{@\n"
    "    vec = [[[[[[[[1, 2]]]]]]]]\n"
    "    dst = []\n"
    "    n = dst.push(vec.pop().pop().pop().pop().pop().pop().pop().pop()).pop()\n"
    "@}{: n :}", "2");
}

static void
test_trv_dot_6(void) {
    check_ok("{: \"abc\".capitalize() :}"
    , "Abc");
}

static void
test_trv_call(void) {
    check_ok("{@ def f(): return 1 end @}{: f() :}", "1");
    check_ok("{@ puts(1) @}", "1\n");
    check_ok("{@ def f(): return 1 end \n funcs = { \"a\": f } @}{: funcs[\"a\"]() :}", "1");
    check_ok("{@ def a(n): return n*2 end \n def b(): return a end @}{: b()(2) :}", "4");
}

static void
test_trv_builtin_string(void) {
    /********
    * upper *
    ********/

    check_ok("{: \"abc\".upper() :}", "ABC");
    check_ok("{@ a = \"abc\" \n @}{: a.upper() :}", "ABC");
    check_fail("{: nil.upper() :}", "\"upper\" is not defined");

    /********
    * lower *
    ********/

    check_ok("{: \"ABC\".lower() :}", "abc");
    check_ok("{@ a = \"ABC\" \n @}{: a.lower() :}", "abc");
    check_fail("{: nil.lower() :}", "\"lower\" is not defined");

    /*************
    * capitalize *
    *************/

    check_ok("{: \"abc\".capitalize() :}", "Abc");
    check_ok("{@ a = \"abc\" \n @}{: a.capitalize() :}", "Abc");
    check_fail("{: nil.capitalize() :}", "\"capitalize\" is not defined");

    /********
    * snake *
    ********/

    check_ok("{: \"abcDef\".snake() :}", "abc_def");
    check_ok("{@ a = \"abcDef\" \n @}{: a.snake() :}", "abc_def");
    check_fail("{: nil.snake() :}", "\"snake\" is not defined");

    /********
    * camel *
    ********/

    check_ok("{: \"camel_case\".camel() :}", "camelCase");
    check_ok("{@ a = \"camel_case\" \n @}{: a.camel() :}", "camelCase");
    check_fail("{: nil.camel() :}", "\"camel\" is not defined");
}

static void
test_trv_builtin_unicode_split(void) {
    check_ok("{@ toks = \"abc\ndef\nghi\".split(\"\n\") @}"
        "{: len(toks) :}", "3");
    check_ok("{@ toks = \"abc\ndef\nghi\n\".split(\"\n\") @}"
        "{: len(toks) :}", "4");
    check_ok("{@ toks = \"\".split(\"\n\") @}"
        "{: len(toks) :}", "1");
}

static void
test_trv_builtin_unicode_rstrip(void) {
    check_ok("{@ s = \"abc \r\n\".rstrip() @}"
        "{: s :}", "abc");
    check_ok("{@ s = \"abcdef\".rstrip(\"def\") @}"
        "{: s :}", "abc");
}

static void
test_trv_builtin_unicode_lstrip(void) {
    check_ok("{@ s = \"\r\n abc\".lstrip() @}"
        "{: s :}", "abc");
    check_ok("{@ s = \"defabc\".lstrip(\"def\") @}"
        "{: s :}", "abc");
}

static void
test_trv_builtin_unicode_strip(void) {
    check_ok("{@ s = \"\r\n abc\r\n \".strip() @}"
        "{: s :}", "abc");
    check_ok("{@ s = \"defabcdef\".strip(\"def\") @}"
        "{: s :}", "abc");
}

static void
test_trv_builtin_unicode_isdigit(void) {
    check_ok("{: \"123\".isdigit() :}", "true");
    check_ok("{: \"abc\".isdigit() :}", "false");
    check_ok("{: \"123abc\".isdigit() :}", "false");
}

static void
test_trv_builtin_unicode_isalpha(void) {
    check_ok("{: \"123\".isalpha() :}", "false");
    check_ok("{: \"abc\".isalpha() :}", "true");
    check_ok("{: \"123abc\".isalpha() :}", "false");
}

static void
test_trv_builtin_unicode_isspace(void) {
    check_ok("{: \"123\".isspace() :}", "false");
    check_ok("{: \" \".isspace() :}", "true");
    check_ok("{: \"\n\".isspace() :}", "true");
    check_ok("{: \"\t\".isspace() :}", "true");
    check_ok("{: \" \n\t\".isspace() :}", "true");
}

static void
test_trv_builtin_functions(void) {
    /*******
    * puts *
    *******/

    check_ok("{@ puts() @}", "\n");
    check_ok("{@ puts(1) @}", "1\n");
    check_ok("{@ puts(1, 2) @}", "1 2\n");
    check_ok("{@ puts(1, \"abc\") @}", "1 abc\n");
    check_ok("{@ puts(\"abc\") @}", "abc\n");

    /********
    * eputs *
    ********/

    check_stderr("{@ eputs(1) @}", "1\n");
    check_stderr("{@ eputs() @}", "\n");
    check_stderr("{@ eputs(1, 2) @}", "1 2\n");
    check_stderr("{@ eputs(1, \"abc\") @}", "1 abc\n");
    check_stderr("{@ eputs(\"abc\") @}", "abc\n");

    /*****
    * id *
    *****/

    // check_ok("{: id(1) :}", "???");
}

static void
test_trv_builtin_modules_opts_0(void) {
    /*****
    * ok *
    *****/
    trv_ready;

    AjiTkr_Parse(tkr, "{: opts.args(0) :},{: opts.args(1) :}");
    {
        int argc = 2;
        char *argv[] = {
            "cmd",
            "日本語",
            NULL
        };
        AjiOpts *opts = AjiOpts_New();
        AjiOpts_Parse(opts, argc, argv);
        AjiAST_Clear(ast);
        AjiAST_MoveOpts(ast, AjiMem_Move(opts));
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        AjiLexEnv_Clear(lex_env);
        AjiTrv_Trav(ast, lex_env);
        assert(!AjiAST_HasErrs(ast));
        assert(!strcmp(AjiLexEnv_GetcStdoutBufSuperGlobal(lex_env), "cmd,日本語"));
    }

    trv_cleanup;
}

static void
test_trv_builtin_modules_alias_0(void) {
    /*
    AjiTkr_Parse(tkr, "{@ alias.set(1, 2, 3) @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        AjiLexEnv_Clear(lex_env);
        (AjiTrv_Trav(ast, lex_env));
        assert(AjiAST_HasErrs(ast));
        assert(!strcmp(AjiAST_GetcFirstErrMsg(ast), "can't invoke alias.set. key is not string"));
    }
*/}

static void
test_trv_builtin_modules_alias_1(void) {
    /*
    AjiTkr_Parse(tkr, "{@ alias.set() @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        AjiLexEnv_Clear(lex_env);
        (AjiTrv_Trav(ast, lex_env));
        assert(AjiAST_HasErrs(ast));
        assert(!strcmp(AjiAST_GetcFirstErrMsg(ast), "can't invoke alias.set. too few arguments"));
    }

    AjiTkr_Parse(tkr, "{@ alias.set(1, 2, 3) @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        AjiLexEnv_Clear(lex_env);
        (AjiTrv_Trav(ast, lex_env));
        assert(AjiAST_HasErrs(ast));
        assert(!strcmp(AjiAST_GetcFirstErrMsg(ast), "can't invoke alias.set. key is not string"));
    }
*/}

static void
test_trv_builtin_modules_alias_2(void) {
    /*
    AjiTkr_Parse(tkr, "{@ alias.set() @}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        AjiLexEnv_Clear(lex_env);
        (AjiTrv_Trav(ast, lex_env));
    }
*/}

static void
test_trv_builtin_modules_vec_0(void) {
    check_ok("{@\n"
"    vec = [1, 2]"
"    dst = []\n"
"    dst.push(vec[1])\n"
"@}{: dst[0] :}", "2");
}

static void
test_trv_builtin_modules_vec_1(void) {
    check_ok("{@\n"
"   vec = []\n"
"   for i = 0; i < 3; i += 1:\n"
"       vec.push(i)\n"
"   end\n"
"@}{: vec[0] :},{: vec[1] :},{: vec[2] :}", "0,1,2");
}

static void
test_trv_builtin_functions_type_dict(void) {
    check_ok("{@ d = {\"a\": 1} @}", "");
    check_ok("{@ def f(d): end \n f({\"a\": 1}) @}", "");
    check_ok("{@ def f(d): end @}{: f(1) :}", "nil");
    check_ok("{@ def f(d): end @}{: f({\"a\": 1}) :}", "nil");
    check_ok("{: type({ \"a\": 1 }) :}", "(type)");
}

static void
test_trv_builtin_functions_type(void) {
    check_ok("{: type(nil) :}", "(type)");
    check_ok("{: type(1) :}", "(type)");
    check_ok("{: type(true) :}", "(type)");
    check_ok("{: type(\"string\") :}", "(type)");
    check_ok("{: type([1, 2]) :}", "(type)");
    check_ok("{: type({ \"a\": 1 }) :}", "(type)");
    check_ok("{@ def f(): end @}{: type(f) :}", "(type)");
    check_ok("{@ import \"tests/lang/modules/hello.aji\" as mod @}{: type(mod) :}", "imported\n(type)");
    check_ok("{@ struct A: end @}{: type(A()) :}", "(struct)");
}

static void
test_trv_builtin_functions_exit(void) {
    // nothing to do
}

static void
test_trv_builtin_functions_cast(void) {
    // TODO
    // check_ok("{@ n, err = cast(\"float\", 1) @}{: n :}", "1.0");
    // check_ok("{@ n, err = cast(\"float\", 1.0) @}{: n :}", "1.0");
    // check_ok("{@ n, err = cast(\"float\", nil) @}{: err.what() :}", "can't cast nil to float");
}

static void
test_trv_builtin_functions_puts_0(void) {
    check_ok("{@ puts(1) @}", "1\n");
}

static void
test_trv_builtin_functions_len_0(void) {                        
    check_ok("{: len([1, 2]) :}", "2");
    check_ok("{: len([]) :}", "0");
    check_ok("{: len(\"12\") :}", "2");
    check_ok("{: len(\"\") :}", "0");
}

static void
test_trv_builtin_functions_copy_0(void) {
    check_ok("{@ a = 1 \n b = copy(a) @}{: id(a) != id(b) :}", "true");
}

static void
test_trv_builtin_functions_deepcopy_0(void) {
    check_ok("{@ a = 1 \n b = deepcopy(a) @}{: id(a) != id(b) :}", "true");
}

static void
test_trv_builtin_functions_assert_0(void) {
    check_fail("{@ assert(0) @}", "assertion error");
}

static void
test_trv_builtin_functions_assert_1(void) {
    check_ok("{@ assert(1) @}", "");
}

static void
test_trv_builtin_functions_extract_0(void) {
    check_ok("{@ struct I: a = 1 \n b = 2 \n end \n extract(I) @}{: a :},{: b :}", "1,2");
    check_ok("{@ struct I: a = 1 \n b = 2 \n end \n extract(I()) @}{: a :},{: b :}", "1,2");
    check_ok("{@ struct I: a = 1 \n b = 2 \n end \n struct A: extract(I) end @}{: A.a :},{: A.b :}", "1,2");
    check_ok("{@ struct I: a = 1 \n b = 2 \n end \n struct A: extract(I) end \n a = A() @}{: a.a :},{: a.b :}", "1,2");
}

static void
test_trv_builtin_functions_setattr_0(void) {
    check_ok("{@ struct A: end \n setattr(A, \"a\", 1) @}{: A.a :}", "1");
    check_ok("{@ struct A: end \n a = A() \n setattr(a, \"a\", 1) @}{: a.a :}", "1");

    check_fail("{@ struct A: end \n setattr(nil, nil, nil) @}", "unsupported object type");
    check_fail("{@ struct A: end \n setattr(A, nil, nil) @}", "invalid key");
}

static void
test_trv_builtin_functions_getattr_0(void) {
    check_ok("{@ struct A: end \n setattr(A, \"a\", 1) @}{: getattr(A, \"a\") :}", "1");
    check_ok("{@ struct A: end \n a = A() \n setattr(a, \"a\", 1) @}{: getattr(a, \"a\") :}", "1");

    check_fail("{@ getattr(nil, nil) @}", "invalid key");
    check_fail("{@ struct A: end \n getattr(A, nil) @}", "invalid key");
    check_fail("{@ getattr(nil, \"k\") @}", "unsupported object type 0");
}

static void
test_trv_builtin_functions_dance_0(void) {
    check_ok("{@ out, err = dance(\"{@ puts(1) @}\") @}{: out :},{: err :}", "1\n,nil");
    check_fail("{@ dance(\"{@ a = b @}\") @}", "\"b\" is not defined in asscalc ass idn");
    check_fail("{@ out, err = dance(\"{@ a = b @}\") @}{: out :},{: err :}", "\"b\" is not defined in asscalc ass idn");
    check_ok("{@ out, err = dance(\"{@ a = b @}{: a :}\", {\"b\": 1}) @}{: out :},{: err :}", "1,nil");
}

static void
test_trv_builtin_functions_ord_0(void) {
    check_ok("{: ord(\"a\") :}", "97");
    check_fail("{: ord() :}", "need one argument");
    check_fail("{: ord(\"\") :}", "empty strings");
    check_fail("{: ord(nil) :}", "invalid type");
}

static void
test_trv_builtin_functions_chr_0(void) {
    check_ok("{: chr(97) :}", "a");
    check_fail("{: chr() :}", "need one argument");
    check_fail("{: chr(\"a\") :}", "invalid type");
}

static void
test_travese(void) {
    /*******
    * test *
    *******/

    // digit or objects

    check_ok("{@ a = 0 or nil @}{: a :}", "nil");
    check_ok("{@ a = 0 or 1 @}{: a :}", "1");
    check_ok("{@ a = 0 or false @}{: a :}", "false");
    check_ok("{@ a = 0 or true @}{: a :}", "true");
    check_ok("{@ a = 0 or \"abc\" @}{: a :}", "abc");
    check_ok("{@ a = 0 or \"\" @}{: a :}", "");
    check_ok("{@ b = 1 \n a = 0 or b @}{: a :}", "1");
    check_ok("{@ def f(): end \n a = 0 or f @}{: a :}", "(function)");
    check_ok("{@ def f(): return true end \n a = 0 or f() @}{: a :}", "true");
    check_ok("{@ a = 0 or [] @}{: a :}", "[]");
    check_ok("{@ a = 0 or [1, 2] @}{: a :}", "[1, 2]");
    check_ok("{@ a = 1 or [] @}{: a :}", "1");
    check_ok("{@ a = 1 or [1, 2] @}{: a :}", "1");

    // bool or objects

    check_ok("{@ a = false or false @}{: a :}", "false");
    check_ok("{@ a = true or false @}{: a :}", "true");
    check_ok("{@ a = false or true @}{: a :}", "true");
    check_ok("{@ a = false or 1 @}{: a :}", "1");
    check_ok("{@ a = true or 1 @}{: a :}", "true");
    check_ok("{@ a = true or \"abc\" @}{: a :}", "true");
    check_ok("{@ a = false or \"abc\" @}{: a :}", "abc");
    check_ok("{@ a = false or \"\" @}{: a :}", "");
    check_ok("{@ def f(): end \n a = false or f @}{: a :}", "(function)");
    check_ok("{@ def f(): end \n a = true or f @}{: a :}", "true");
    check_ok("{@ def f(): return true end \n a = false or f() @}{: a :}", "true");
    check_ok("{@ def f(): return 0 end \n a = false or f() @}{: a :}", "0");
    check_ok("{@ a = false or [] @}{: a :}", "[]");
    check_ok("{@ a = false or [1, 2] @}{: a :}", "[1, 2]");
    check_ok("{@ a = true or [] @}{: a :}", "true");
    check_ok("{@ a = true or [1, 2] @}{: a :}", "true");

    // nil or objects

    check_ok("{@ a = nil or 0 @}{: a :}", "0");
    check_ok("{@ a = nil or 1 @}{: a :}", "1");
    check_ok("{@ a = nil or false @}{: a :}", "false");
    check_ok("{@ a = nil or true @}{: a :}", "true");
    check_ok("{@ a = nil or \"abc\" @}{: a :}", "abc");
    check_ok("{@ a = nil or \"\" @}{: a :}", "");
    check_ok("{@ b = 1 \n a = nil or b @}{: a :}", "1");
    check_ok("{@ def f(): end \n a = nil or f @}{: a :}", "(function)");
    check_ok("{@ def f(): return true end \n a = nil or f() @}{: a :}", "true");
    check_ok("{@ a = nil or [] @}{: a :}", "[]");
    check_ok("{@ a = nil or [1, 2] @}{: a :}", "[1, 2]");

    // string or objects

    check_ok("{@ a = \"abc\" or nil @}{: a :}", "abc");
    check_ok("{@ a = \"\" or nil @}{: a :}", "nil");
    check_ok("{@ a = \"abc\" or true @}{: a :}", "abc");
    check_ok("{@ a = \"abc\" or false @}{: a :}", "abc");
    check_ok("{@ a = \"\" or true @}{: a :}", "true");
    check_ok("{@ a = \"\" or false @}{: a :}", "false");
    check_ok("{@ a = \"\" or 1 @}{: a :}", "1");
    check_ok("{@ a = \"\" or 0 @}{: a :}", "0");
    check_ok("{@ a = \"abc\" or 1 @}{: a :}", "abc");
    check_ok("{@ a = \"abc\" or 0 @}{: a :}", "abc");
    check_ok("{@ a = \"\" or \"\" @}{: a :}", "");
    check_ok("{@ a = \"\" or \"abc\" @}{: a :}", "abc");
    check_ok("{@ a = \"def\" or \"abc\" @}{: a :}", "def");
    check_ok("{@ b = 1 \n a = \"abc\" or b @}{: a :}", "abc");
    check_ok("{@ b = 0 \n a = \"abc\" or b @}{: a :}", "abc");
    check_ok("{@ def f(): end \n a = \"abc\" or f @}{: a :}", "abc");
    check_ok("{@ def f(): end \n a = \"\" or f @}{: a :}", "(function)");
    check_ok("{@ def f(): return true end \n a = \"\" or f() @}{: a :}", "true");
    check_ok("{@ def f(): return nil end \n a = \"\" or f() @}{: a :}", "nil");
    check_ok("{@ def f(): return nil end \n a = \"abc\" or f() @}{: a :}", "abc");
    check_ok("{@ a = \"abc\" or [] @}{: a :}", "abc");
    check_ok("{@ a = \"abc\" or [1, 2] @}{: a :}", "abc");
    check_ok("{@ a = \"\" or [1, 2] @}{: a :}", "[1, 2]");
    check_ok("{@ a = \"\" or [] @}{: a :}", "[]");

    // vec or objects

    check_ok("{@ a = [1, 2] or nil @}{: a :}", "[1, 2]");
    check_ok("{@ a = [] or nil @}{: a :}", "nil");
    check_ok("{@ a = [1, 2] or 1 @}{: a :}", "[1, 2]");
    check_ok("{@ a = [] or 1 @}{: a :}", "1");
    check_ok("{@ a = [1, 2] or 0 @}{: a :}", "[1, 2]");
    check_ok("{@ a = [] or 0 @}{: a :}", "0");
    check_ok("{@ a = [1, 2] or true @}{: a :}", "[1, 2]");
    check_ok("{@ a = [] or true @}{: a :}", "true");
    check_ok("{@ a = [1, 2] or false @}{: a :}", "[1, 2]");
    check_ok("{@ a = [] or false @}{: a :}", "false");
    check_ok("{@ a = [1, 2] or \"\" @}{: a :}", "[1, 2]");
    check_ok("{@ a = [] or \"\" @}{: a :}", "");
    check_ok("{@ a = [1, 2] or \"abc\" @}{: a :}", "[1, 2]");
    check_ok("{@ a = [] or \"abc\" @}{: a :}", "abc");
    check_ok("{@ a = [1, 2] or [1, 2] @}{: a :}", "[1, 2]");
    check_ok("{@ a = [] or [1, 2] @}{: a :}", "[1, 2]");
    check_ok("{@ b = 1 \n a = [] or b @}{: a :}", "1");
    check_ok("{@ b = 0 \n a = [] or b @}{: a :}", "0");
    check_ok("{@ b = 1 \n a = [1, 2] or b @}{: a :}", "[1, 2]");
    check_ok("{@ def f(): end \n a = [] or f @}{: a :}", "(function)");
    check_ok("{@ def f(): end \n a = [1, 2] or f @}{: a :}", "[1, 2]");
    check_ok("{@ def f(): return 1 end \n a = [] or f() @}{: a :}", "1");
    check_ok("{@ def f(): return 0 end \n a = [] or f() @}{: a :}", "0");

    // func or objects

    check_ok("{@ def f(): end \n a = f or nil @}{: a :}", "(function)");
    check_ok("{@ def f(): end \n a = f or 0 @}{: a :}", "(function)");
    check_ok("{@ def f(): end \n a = f or 1 @}{: a :}", "(function)");
    check_ok("{@ def f(): end \n a = f or true @}{: a :}", "(function)");
    check_ok("{@ def f(): end \n a = f or false @}{: a :}", "(function)");
    check_ok("{@ def f(): end \n a = f or \"\" @}{: a :}", "(function)");
    check_ok("{@ def f(): end \n a = f or \"abc\" @}{: a :}", "(function)");
    check_ok("{@ def f(): end \n a = f or f @}{: a :}", "(function)");
    check_ok("{@ def f(): end \n a = f or [] @}{: a :}", "(function)");
    check_ok("{@ def f(): end \n a = f or [1, 2] @}{: a :}", "(function)");

    // other

    check_ok("{@ a = 0 or \"abc\" @}{: a :}", "abc");
    check_ok("{@ a = 1 \n b = 0 or a @}{: b :}", "1");
    check_ok("{@ def f(): end\na = 0 or f @}{: a :}", "(function)");
    check_ok("{@ a = 1 or 0 or 2 @}{: a :}", "1");
    check_ok("{@ a = 0 or \"abc\" @}{: a :}", "abc");
    check_ok("{@ a = not nil @}{: a :}", "true");
    check_ok("{@ a = not 0 @}{: a :}", "true");
    check_ok("{@ a = not 1 @}{: a :}", "false");
    check_ok("{@ a = not \"\" @}{: a :}", "true");
    check_ok("{@ a = not \"abc\" @}{: a :}", "false");
    check_ok("{@ def f(): end \n a = not f @}{: a :}", "false");

    /*******
    * expr *
    *******/

    check_ok("{@ a = 1 + 2 @}{: a :}", "3");
    check_ok("{@ a = 1 + 2 + 3 @}{: a :}", "6");
    check_ok("{@ a = 2 - 1 @}{: a :}", "1");
    check_ok("{@ a = 3 - 2 - 1 @}{: a :}", "0");
    check_ok("{@ a = 1 + 2 - 3 @}{: a :}", "0");
    check_ok("{@ a = \"abc\" + \"def\" @}{: a :}", "abcdef");
    check_ok("{@ a = \"123\" \n b = \"abc\" + a + \"def\" @}{: b :}", "abc123def");

    /*******
    * term *
    *******/

    check_ok("{@ a = 2 * 3 @}{: a :}", "6");
    check_ok("{@ a = 2 * 3 * 4 @}{: a :}", "24");
    check_ok("{@ a = 4 / 2 @}{: a :}", "2");
    check_ok("{@ a = 4 / 2 / 2 @}{: a :}", "1");
    check_ok("{@ a = 4 / (2 / 2) @}{: a :}", "4");
    check_ok("{@ a = 1 + ( 2 - 3 ) * 4 / 4 @}{: a :}", "0");

    /**********
    * asscalc *
    **********/

    check_fail("{@ a += 1 @}", "\"a\" is not defined");
    check_ok("{@ a = 0 \n a += 1 @}{: a :}", "1");
    check_ok("{@ a = 0 \n a += 1 + 1 @}{: a :}", "2");
    check_ok("{@ a = 0 \n b = 1 + 1 @}{: b :}", "2");
    check_ok("{@ a = 0 \n b = 1 + (a += 1) @}{: b :}", "2");
    check_ok("{@ a = 0 \n a += 1 @}{: a :}", "1");

    check_ok("{@ a = 0 \n a += 1 \n a += 2 @}{: a :}", "3");
    check_ok("{@ a = \"a\" a += \"b\" @}{: a :}", "ab");

    check_fail("{@\n"
    "def f():\n"
    "   a += 1\n"
    "end\n"
    "f()\n"
    "@}", "\"a\" is not defined");
    check_fail("{@\n"
    "a = \"x\"\n"
    "def f():\n"
    "   a += \"y\"\n"
    "end\n"
    "f()\n"
    "@}{: a :}", "\"a\" is not defined");

    check_ok("{@\n"
        "    def add(a):\n"
        "        a += \"x\"\n"
        "    end\n"
        "\n"
        "   a = \"\"\n"
        "   add(a)\n"
        "@}{: a :}", "");

    /*******************
    * import statement *
    *******************/

    check_fail("{@ import alias @}", "not found path in compile import as statement");
    check_fail("{@ import my.alias @}", "not found path in compile import as statement");

    /***************
    * if statement *
    ***************/

    check_ok("{@ if 1: a = 1 end @}{: a :}", "1");
    check_ok("{@ if 0: elif 1: a = 1 end @}{: a :}", "1");
    check_ok("{@ if 0: elif 0: else: a = 1 end @}{: a :}", "1");
    check_ok("{@ if 1: @}{@ end @}", "");
    check_ok("{@ if 1: @}abc{@ end @}", "abc");
    check_ok("abc{@ if 1: @}def{@ end @}ghi", "abcdefghi");
    check_ok("{@ if 1: @}{@ if 1: @}abc{@ end @}{@ end @}", "abc");
    check_ok("{@ if 1: @}abc{@ if 1: @}def{@ end @}ghi{@ end @}", "abcdefghi");
    check_ok("{@ if 0: @}abc{@ else: @}def{@ end @}", "def");
    check_ok("{@ if 0: @}abc{@ elif 1: @}def{@ end @}", "def");
    check_ok("{@ if 0: @}abc{@ elif 0: @}def{@ else: @}ghi{@ end @}", "ghi");

    /****************
    * for statement *
    ****************/

    check_fail(
        "{@\n"
        "    for a != 0:\n"
        "        break\n"
        "    end\n"
        "@}\n", "\"a\" is not defined");

    check_ok("{@ a = 0\n" 
        "for i = 0; i != 4; i += 1:\n"
        "   a += 1\n"
        "end @}{: a :}", "4");

    check_ok("{@ a = 0\n" 
        "for i = 0, j = 0; i != 4; i += 1, j += 1:\n"
        "   a += 1\n"
        "end @}{: a :} {: i :} {: j :}", "4 4 4");

    check_ok("{@ for i = 0; i != 4; i += 1: @}a{@ end @}", "aaaa");
   
    check_ok("{@ for i, j = 0, 0; i != 4; i += 1, j += 2: end @}{: i :},{: j :}", "4,8");
    check_ok("{@ i, a = 0, 0 \n for i != 4: a += i \n i += 1 end @}{: i :},{: a :}", "4,6");

    check_ok(
        "{@ for i = 0; i != 4; i += 1: @}"
        "hige\n"
        "{@ end @}", "hige\nhige\nhige\nhige\n");

    check_ok(
        "{@ i = 0 for i != 4: @}"
        "hige\n{@ i += 1 @}"
        "{@ end @}", "hige\nhige\nhige\nhige\n");

    check_ok(
        "{@ i = 0 for: @}"
        "{@ if i == 4: break end @}hige\n{@ i += 1 @}"
        "{@ end @}", "hige\nhige\nhige\nhige\n");

    /*******
    * jump *
    *******/

    check_ok("{@\n" 
        "for i=0; i!=4; i+=1:\n"
        "   break\n"
        "end @}{: i :}", "0");
    check_ok("{@\n"
        "for i=0; i!=4; i+=1:\n"
        "   if i == 2:\n"
        "       break\n"
        "   end\n"
        "end @}{: i :}", "2");

    check_ok("{@ a = 0\n" 
        "for i=0; i!=4; i+=1:\n"
        "   continue\n"
        "   a += 1\n"
        "end @}{: i :},{: a :}", "4,0");

    check_ok("{@ a = 0\n" 
        "for i=0; i!=4; i+=1:\n"
        "   if i == 2:\n"
        "       continue\n"
        "   end\n"
        "   a += 1\n"
        "end @}{: a :}", "3");

    check_ok("{@ a = 0\n" 
        "for i=0; i!=4; i+=1:\n"
        "   if i == 2:\n"
        "       continue\n"
        "   elif i == 3:\n"
        "       continue\n"
        "   end\n"
        "   a += 1\n"
        "end @}{: a :}", "2");

    check_ok("{@ a = 0, b = 0\n" 
        "for i=0; i!=4; i+=1:\n"
        "   a += 1"
        "   if i == 2:\n"
        "       continue\n"
        "   end\n"
        "   b += 1\n"
        "end @}{: a :},{: b :}", "4,3");

    check_ok("{@\n"
        "def func():\n"
        "   return\n"
        "end\n"
        "puts(func())"
        "@}", "nil\n");

    check_ok("{@\n" 
        "def func():\n"
        "   return 1\n"
        "end\n"
        "a = func()"
        "@}{: a :}", "1");

    check_ok("{@\n" 
        "def func():\n"
        "   return 1\n"
        "end\n"
        "puts(func())"
        "@}", "1\n");

    check_ok("{@\n" 
        "def func():\n"
        "   return 1, 2\n"
        "end\n"
        "puts(func())"
        "@}", "[1, 2]\n");

    check_ok("{@\n" 
        "def func():\n"
        "   puts(\"a\")\n"
        "   return 1\n"
        "   puts(\"b\")\n"
        "end\n"
        "puts(func())"
        "@}", "a\n1\n");

    check_ok("{@\n" 
        "def func():\n"
        "   a = 1\n"
        "   return a\n"
        "end\n"
        "x = func()\n"
        "@}", "");

    check_ok("{@\n" 
        "def func():\n"
        "   a = 1\n"
        "   if a == 1:\n"
        "       return a\n"
        "   end\n"
        "   puts(\"b\")\n"
        "end\n"
        "x = func()\n"
        "@}{: x :}", "1");

    check_ok("{@\n" 
        "def func():\n"
        "   a = 1\n"
        "   if a == 1:\n"
        "       return a\n"
        "   end\n"
        "   puts(\"b\")\n"
        "end\n"
        "puts(func())\n"
        "@}", "1\n");

    check_ok("{@\n" 
        "def func():\n"
        "   return 1, 2\n"
        "end\n"
        "a, b = func()\n"
        "@}{: a :},{: b :}", "1,2");

    check_ok("{@\n" 
        "def func():\n"
        "   return 1, 2\n"
        "end\n"
        "a = func()\n"
        "@}{: a :}", "[1, 2]");

    check_ok("{@\n" 
        "def func(a):\n"
        "   return a, a\n"
        "end\n"
        "a, b = func(1)\n"
        "@}{: a :},{: b :}", "1,1");

    /***********
    * func_def *
    ***********/

    check_ok("{@ def func(): end @}{: func :}", "(function)");

    check_ok("{@\n" 
        "def func():\n"
        "   a = 1\n"
        "end\n"
        "func()\n"
        "@}", "");

    check_fail("{: a :}", "\"a\" is not defined in ref block");
    check_fail("{@\n" 
        "def func():\n"
        "   a = 1\n"
        "end\n"
        "@}{: a :}", "\"a\" is not defined in ref block");

    check_fail("{@\n" 
        "def func():\n"
        "   a = 1\n"
        "end\n"
        "func()"
        "@}{: a :}", "\"a\" is not defined in ref block");

    check_fail("{@\n" 
        "def func(a):\n"
        "   b = a\n"
        "end\n"
        "func(1)"
        "@}{: a :},{: b :}", "\"a\" is not defined in ref block");

    check_fail("{@\n" 
        "def func(a, b):\n"
        "   c = a + b\n"
        "end\n"
        "func(1, 2)\n"
        "@}{: c :}", "\"c\" is not defined in ref block");

    check_ok("{@\n" 
        "c = 1\n"
        "def func(a, b):\n"
        "   c = a + b\n"
        "end\n"
        "func(1, 2)\n"
        "@}{: c :}", "1");

    check_ok("{@\n" 
        "c = 1\n"
        "def func(a, b):\n"
        "   puts(c)\n"
        "   c = a + b\n"
        "   puts(c)\n"
        "end\n"
        "func(1, 2)\n"
        "@}{: c :}", "1\n3\n1");

    /***********************
    * Aji_Escape character *
    ***********************/

    check_ok("{: \"abc\ndef\n\" :}", "abc\ndef\n");
    check_ok("{: \"\tabc\tdef\" :}", "\tabc\tdef");
}

/**
 * A test of assign to variable and refer variable
 * object is copy? or refer?
 */
static void
test_trv_assign_and_reference_0(void) {
    check_ok("{@\n"
    "   i = 0\n"
    "@}{: i :}", "0");
    
    check_ok("{@\n"
    "   i = j = 0\n"
    "@}{: i :},{: j :}", "0,0");

    check_ok("{@\n"
    "   i = j = 0\n"
    "@}{: id(i) == id(j) :}", "true");
}

static void
test_trv_assign_and_reference_1(void) {
    check_ok("{@\n"
    "   i = j = 0\n"
    "@}{: i :},{: j :},{: id(i) == id(j) :}", "0,0,true");
}

static void
test_trv_assign_and_reference_2(void) {
    check_ok("{@\n"
    "   i = 1\n"
    "   j = i\n"
    "@}{: i :},{: j :},{: id(i) == id(j) :}", "1,1,true");
}

static void
test_trv_assign_and_reference_3(void) {
    check_ok("{@\n"
    "   i, j = 1, 1\n"
    "@}{: i :},{: j :},{: id(i) != id(j) :}", "1,1,true");
}

static void
test_trv_assign_and_reference_4(void) {
    check_ok("{@\n"
    "   i = 1\n"
    "   j, k = i, i\n"
    "@}{: i :},{: j :},{: id(i) == id(j) :},{: id(i) == id(k) :},{: id(j) == id(k) :}"
    , "1,1,true,true,true");
}

static void
test_trv_assign_and_reference_5(void) {
    check_ok("{@\n"
    "   i = 1, 2\n"
    "   j = i\n"
    "@}{: i :},{: j :},{: id(i) == id(j) :}", "[1, 2],[1, 2],true");

    check_ok("{@\n"
    "   i = 1, 2\n"
    "   j = i\n"
    "@}", "");
}

static void
test_trv_assign_and_reference_6(void) {
    check_ok("{@\n"
    "   i = [[1]]\n"
    "@}{: i[0][0] :}"
    , "1");
    check_ok("{@\n"
    "   i = [[1]]\n"
    "   j = i[0][0]\n"
    "@}{: j :}"
    , "1");
    check_ok("{@\n"
    "   i = [1, 2]\n"
    "   j = 3\n"
    "   i[0] = j\n"
    "@}{: i[0] :}"
    , "3");
    check_ok("{@\n"
    "   i = [1, 2]\n"
    "   j = 3\n"
    "   i[0] = j\n"
    "@}{: i[0] :},{: i[1] :},{: j :},{: id(i[0]) == id(j) :}"
    , "3,2,3,true");
}

static void
test_trv_assign_and_reference_7(void) {
    check_ok("{@\n"
    "   i, j = [1, 2]\n"
    "@}{: i :},{: j :}", "1,2");
}

static void
test_trv_assign_and_reference_8(void) {
    check_ok("{@\n"
    "   i, j = k, l = 1, 2\n"
    "@}{: i :},{: j :},{: k :},{: l :}", "1,2,1,2");
}

static void
test_trv_assign_and_reference_9(void) {
    check_ok("{@\n"
    "   i = { \"a\": 1 }\n"
    "   j = i\n"
    "@}{: i :},{: j :},{: id(i) == id(j) :}"
    , "{\"a\": 1},{\"a\": 1},true");
}

static void
test_trv_assign_and_reference_10(void) {
    check_ok("{@\n"
    "   def f(a):\n"
    "       return a\n"
    "   end\n"
    "   i = f(1)"
    "@}{: i :}", "1");
}

static void
test_trv_assign_and_reference_11(void) {
    check_ok("{@\n"
    "   def f():\n"
    "       return 1, 2\n"
    "   end\n"
    "   i, j = f()\n"
    "@}{: i :},{: j :},{: id(i) != id(j) :}"
    , "1,2,true");
}

static void
test_trv_assign_and_reference_11_5(void) {
    check_ok("{@\n"
    "   def f(a):\n"
    "       return a\n"
    "   end\n"
    "   j = 1\n"
    "   k = f(j)\n"
    "@}{: id(k) != id(j) :}"
    , "true");
}

static void
test_trv_assign_and_reference_11_6(void) {
    check_ok("{@\n"
    "   def f():\n"
    "       return 1, 2\n"
    "   end\n"
    "   i, j = f()\n"
    "@}{: i :},{: j :}"
    , "1,2");
}

static void
test_trv_assign_and_reference_11_7(void) {
    check_ok("{@\n"
    "   def f(a):\n"
    "       return a, a\n"
    "   end\n"
    "   i, j = f(1)\n"
    "@}{: i :},{: j :}"
    , "1,1");
}

static void
test_trv_assign_and_reference_12(void) {
    check_ok("{@\n"
    "   def f(a):\n"
    "       return a, a\n"
    "   end\n"
    "   k = 1\n"
    "   i, j = f(k)\n"
    "@}{: i :},{: j :},{: id(i) == id(j) :},{: id(k) != id(i) :},{: id(k) != id(j) :}"
    , "1,1,true,true,true");
}

static void
test_trv_assign_and_reference_13(void) {
    check_ok("{@\n"
    "   i = j = 0\n"
    "@}{: id(i) == id(j) :}"
    , "true");
}

static void
test_trv_assign_and_reference_14(void) {
    check_ok("{@\n"
    "   i = 0\n"
    "@}{: i :}"
    , "0");
}

static void
test_trv_assign_and_reference_15(void) {
    check_ok("{@\n"
    "   i = j = 0\n"
    "@}{: id(i) == id(j) :}"
    , "true");
}

static void
test_trv_assign_and_reference_16(void) {
    check_ok("{@\n"
    "   def f(a):\n"
    "       return a\n"
    "   end\n"
    "   f(1)"
    "@}", "");
}

static void
test_trv_assign_and_reference_all(void) {
    test_trv_assign_and_reference_0();
    test_trv_assign_and_reference_1();
    test_trv_assign_and_reference_2();
    test_trv_assign_and_reference_3();
    test_trv_assign_and_reference_4();
    test_trv_assign_and_reference_5();
    test_trv_assign_and_reference_6();
    test_trv_assign_and_reference_7();
    test_trv_assign_and_reference_8();
    test_trv_assign_and_reference_9();
    test_trv_assign_and_reference_10();
    test_trv_assign_and_reference_11();
    test_trv_assign_and_reference_12();
    test_trv_assign_and_reference_13();
    test_trv_assign_and_reference_14();
    test_trv_assign_and_reference_15();
    test_trv_assign_and_reference_16();
}

static void
test_trv_assign_fail_0(void) {
    check_ok("", "");
    check_ok("{@ a @}", "");  // TODO: error? or ok?
    check_fail("{@ a @}{: a :}", "\"a\" is not defined in ref block"); 
    check_fail("{@ a = @}", "syntax error. not found rhs test in assign list");
    check_fail("{@ 1 = @}", "syntax error. not found rhs test in assign list");
    check_fail("{@ 1 = 1 @}", "invalid left hand operand (1) in calc assign");
    check_fail("{@ 1 = a @}", "invalid left hand operand (1) in calc assign");
    check_fail("{@ a = a @}", "\"a\" is not defined in asscalc ass idn");
    check_fail("{@ a, b = 1 @}", "invalid right operand (1)");
    check_fail("{@ a, @}", "syntax error. not found test in test list");
    check_fail("{@ a, = 1 @}", "syntax error. not found test in test list");
    check_fail("{@ ,a @}", "not found blocks");
    check_fail("{@ ,a = 1 @}", "not found blocks");
    check_fail("{@ a, b = 1, @}", "syntax error. not found test in test list");
    check_ok("{@ a = 1, 2 @}", "");  // ok
}

static void
test_trv_code_block(void) {
    check_ok("{@@}", "");
    check_ok("{@ \n @}", "");
    check_ok("{@ \n\n @}", "");
    check_ok("{@ \n\n1 @}", "");
    check_ok("{@ 1\n\n @}", "");
    check_ok("{@ \n\n1\n\n @}", "");
    check_ok("{@@}{@@}", "");
    check_ok("{@@}{@@}{@@}", "");
    check_ok("\n{@\n@}\n{@\n@}\n", "\n\n");
    check_ok("\n{@\n\n\n@}\n{@\n\n\n@}\n", "\n\n");
}

static void
test_trv_code_block_fail(void) {
    check_fail("{@", "syntax error. reached EOF in code block");
    check_fail("{@@", "syntax error. reached EOF in code block");
}

static void
test_trv_ref_block(void) {
    check_ok("{: 1 :}", "1");
    check_ok("{: 1\n :}", "1");
    check_ok("{: \n1 :}", "1");
    check_ok("\n{: 1 :}\n", "\n1");
    check_ok("{@@}{: 1 :}{@@}", "1");
    check_ok("{: 1 :}{@@}{: 2 :}", "12");
    check_ok("{: 2 * 3 + 1 :}", "7");
    check_ok("{: \"ab\" * 4 :}", "abababab");
}

static void
test_trv_ref_block_fail(void) {
    check_fail("{:", "not found blocks");
    check_fail("{: 1", "not found blocks");
    check_fail("{: 1 :", "syntax error. not found \":}\"");
    check_ok("{: 1 :}", "1");
    check_fail("{: :}", "not found blocks");
}

static void
test_trv_text_block(void) {
    check_ok("1", "1");
    check_ok("1{@@}2", "12");
    check_ok("1{@@}2{@@}3", "123");
    check_ok("1{: 2 :}3{: 4 :}5", "12345");
    check_ok("1{@@}{: 2 :}{@@}3", "123");                    
}

static void
test_trv_import_stmt_0(void) {
                        
    /**********************
    * import as statement *
    **********************/

    check_ok("{@ import \"tests/lang/modules/hello.aji\" as hello @}", "imported\n");
    check_fail("{@ import \n \"tests/lang/modules/hello.aji\" as hello @}", "not found path in compile import as statement");
    check_fail("{@ import \"tests/lang/modules/hello.aji\" \n as hello @}", "not found blocks");
    check_fail("{@ import \"tests/lang/modules/hello.aji\" as \n hello @}", "not found alias in compile import as statement");
    check_ok("{@ import \"tests/lang/modules/hello.aji\" as hello \n"
        "hello.world() @}"
        , "imported\nhello, world\n");
    check_ok("{@ import \"tests/lang/modules/count.aji\" as count \n"
"@}{: count.n :}", "45");

    /************************
    * from import statement *
    ************************/

    check_ok("{@ from \"tests/lang/modules/funcs.aji\" import f1 @}", "imported\n");
    check_ok("{@ from \"tests/lang/modules/funcs.aji\" import f1 \n f1() @}", "imported\nf1\n");
    check_ok("{@ from \"tests/lang/modules/funcs.aji\" import ( f1, f2 ) @}", "imported\n");
    check_ok("{@ from \"tests/lang/modules/funcs.aji\" import ( f1, f2, ) @}", "imported\n");
    check_ok("{@ from \"tests/lang/modules/funcs.aji\" import (\nf1,\nf2,\n) @}", "imported\n");
    check_ok("{@ from \"tests/lang/modules/funcs.aji\" import ( f1, f2 ) \n "
"   f1() \n f2() @}", "imported\nf1\nf2\n");
    check_fail("{@ from \n \"tests/lang/modules/funcs.aji\" import ( f1, f2 ) \n "
"   f1() \n f2() @}", "not found path in compile from import statement");
    check_fail("{@ from \"tests/lang/modules/funcs.aji\" \n import ( f1, f2 ) \n "
"   f1() \n f2() @}", "not found import in compile from import statement");
    check_fail("{@ from \"tests/lang/modules/funcs.aji\" import \n ( f1, f2 ) \n "
"   f1() \n f2() @}", "not found import variables in compile from import statement");                    
}

static void
test_trv_import_stmt_1(void) {
    check_ok("{@ import \"tests/lang/modules/count.aji\" as count \n"
"@}{: count.n :}", "45");
}

static void
test_trv_import_stmt_2(void) {
    check_ok("{@\n"
"   if 1:\n"
"       import \"tests/lang/modules/count.aji\" as count\n"
"   end"
"@}{: count.n :}", "45");
    check_ok("{@\n"
"   if 0:\n"
"   else:\n"
"       import \"tests/lang/modules/count.aji\" as count\n"
"   end"
"@}{: count.n :}", "45");
    check_ok("{@\n"
"   if 0:\n"
"   elif 1:\n"
"       import \"tests/lang/modules/count.aji\" as count\n"
"   end"
"@}{: count.n :}", "45");
    check_ok("{@\n"
"   for i = 0; i < 2; i += 1:\n"
"       import \"tests/lang/modules/count.aji\" as count\n"
"       puts(count.n)\n"
"   end"
"@}", "45\n45\n");
    check_ok("{@\n"
"   def func():\n"
"       import \"tests/lang/modules/count.aji\" as count\n"
"       puts(count.n)\n"
"   end\n"
"   func()\n"
"@}", "45\n");
    check_fail("{@\n"
"   def func():\n"
"       import \"tests/lang/modules/count.aji\" as count\n"
"   end"
"   func()\n"
"@}{: count :}", "\"count\" is not defined in ref block");
}

static void
test_trv_import_stmt_3(void) {
    check_ok("{@ import \"tests/lang/modules/hello.aji\" as hello @}", "imported\n");
}

static void
test_trv_import_stmt_4(void) {
    check_ok("{@ import \"tests/lang/modules/hello.aji\" as hello \n"
    "hello.world() @}"
    , "imported\nhello, world\n");
}

static void
test_trv_import_stmt_5(void) {
    check_ok(
    "{@ import \"tests/lang/modules/hello.aji\" as hello \n"
    "hello.world() @}"
    , "imported\nhello, world\n");
}

static void
test_trv_from_import_stmt_1(void) {
    check_ok("{@ from \"tests/lang/modules/funcs.aji\" import f1 \n f1() @}", "imported\nf1\n");                    
}

static void
test_trv_from_import_stmt_2(void) {
    check_ok("{@ import \"tests/lang/modules/hello.aji\" as hello \n"
"hello.world() @}", "imported\nhello, world\n");                    
}

static void
test_trv_from_import_stmt_3(void) {
    check_ok("{@ from \"tests/lang/modules/funcs.aji\" import ( f1, f2 ) \n "
"   f1() \n f2() @}", "imported\nf1\nf2\n");                    
}

static void
test_trv_if_stmt_0(void) {
    check_ok("{@ if 1: puts(1) end @}", "1\n");
    check_ok("{@ if 1:\n puts(1) end @}", "1\n");
    check_ok("{@ if 1: puts(1) \nend @}", "1\n");
    check_ok("{@ if 1:\n\n puts(1) \n\nend @}", "1\n");
    check_ok("{@ if \n1: puts(1) end @}", "1\n");
    check_ok("{@ if 1\n: puts(1) end @}", "1\n");
    check_ok("{@ \n if 1: puts(1) end @}", "1\n");
    check_ok("{@ if 1: puts(1) end \n @}", "1\n");                    
}

static void
test_trv_if_stmt_1(void) {
    check_ok("{@ if 1: @}1{@ end @}", "1");
    check_ok("{@ if 1: \n@}1{@ end @}", "1");
    check_ok("{@ if 1: \n\n@}1{@ end @}", "1");
    check_ok("{@ if 1: @}1{@ \nend @}", "1");
    check_ok("{@ if 1: @}1{@ \n\nend @}", "1");
}

static void
test_trv_if_stmt_2(void) {
    check_ok("{@ if 1: if 1: puts(1) end end @}", "1\n");
    check_ok("{@ if 1: \nif 1: puts(1) end end @}", "1\n");
    check_ok("{@ if 1: if 1: \nputs(1) end end @}", "1\n");
    check_ok("{@ if 1: if 1: puts(1) \nend end @}", "1\n");
    check_ok("{@ if 1: if 1: puts(1) end \nend @}", "1\n");
}

static void
test_trv_if_stmt_3(void) {
    check_ok("{@ if 1: @}{@ if 1: @}1{@ end @}{@ end @}", "1");
    check_ok("{@ \nif 1: @}{@ if 1: @}1{@ end @}{@ end @}", "1");
    check_ok("{@ if 1: \n@}{@ if 1: @}1{@ end @}{@ end @}", "1");
    check_ok("{@ if 1: @}{@ \nif 1: @}1{@ end @}{@ end @}", "1");
    check_ok("{@ if 1: @}{@ if 1: \n@}1{@ end @}{@ end @}", "1");
    check_ok("{@ if 1: @}{@ if 1: @}1{@ \nend @}{@ end @}", "1");
    check_ok("{@ if 1: @}{@ if 1: @}1{@ end \n@}{@ end @}", "1");
    check_ok("{@ if 1: @}{@ if 1: @}1{@ end @}{@ \nend @}", "1");
    check_ok("{@ if 1: @}{@ if 1: @}1{@ end @}{@ end \n@}", "1");
}

static void
test_trv_if_stmt_4(void) {
    check_ok("{@ if 1: puts(1) end @}", "1\n");
    check_ok("{@ i = 1 @}", "");
    check_ok("{@ i = 1 \n if i: end @}", "");
    check_ok("{@ i = 1 \n if i: puts(1) end @}", "1\n");
    check_ok("{@ i = 1 @}{@ if i: puts(1) end @}", "1\n");
    check_ok("{@\n"
"   def f():\n"
"       i = 1\n"
"       if i:\n"
"           puts(1)\n"
"       end\n"
"   end\n"
"   f()\n"
"@}", "1\n");
    check_ok("{@\n"
"   i = 1\n"
"   def f():\n"
"       if i:\n"
"           puts(1)\n"
"       end\n"
"   end\n"
"   f()\n"
"@}", "1\n");
}

static void
test_trv_if_stmt_5(void) {
    check_ok("{@\n"
"   import \"tests/lang/modules/if.aji\" as mod \n"
"@}", "1\n");
}

static void
test_trv_if_stmt_6(void) {
    check_ok("{@\n"
"   from \"tests/lang/modules/if-2.aji\" import f1 \n"
"   f1()\n"
"@}", "1\n");
}

static void
test_trv_if_stmt_7(void) {
    check_ok("{@\n"
"   from \"tests/lang/modules/if-2.aji\" import f2\n"
"   f2()\n"
"@}", "1\n");

}

static void
test_trv_if_stmt_8(void) {
    check_ok("{@\n"
"   import \"tests/lang/modules/if-3.aji\" as if3\n"
"@}", "1\n");
}

static void
test_trv_if_stmt_9(void) {
    check_ok("{@\n"
"   def f(n):\n"
"       puts(n)\n"
"       return n\n"
"   end\n"
"   if f(1):\n"
"       puts(2)\n"
"   end\n"
"@}", "1\n2\n");
    check_ok("{@\n"
"   if 2 * 3 + 1:\n"
"       puts(1)\n"
"   end\n"
"@}", "1\n");
    check_ok("{@\n"
"   i = 2\n"
"   if i:\n"
"       j = 3\n"
"       if j:\n"
"           puts(i * j)\n"
"       end\n"
"   end\n"
"@}", "6\n");
    check_ok("{@\n"
"   i = 2\n"
"   if i:\n"
"       j = 3\n"
"       if j:\n"
"           k = 4\n"
"           if k:\n"
"               puts(i * j * k)\n"
"           end\n"
"           k = 2\n"
"       end\n"
"       j = 10\n"
"   end\n"
"   i = 100\n"
"   puts(i * j * k)\n"
"@}", "24\n2000\n");
    check_ok("{@\n"
"   i = 2\n"
"   if i:\n"
"       j = 3\n"
"       if j:\n"
"           k = 4\n"
"           if k:\n"
"               def f(n):\n"
"                   if n:\n"
"                       puts(n)\n"
"                   end\n"
"               end\n"
"           end\n"
"           k = 2\n"
"       end\n"
"       j = 10\n"
"   end\n"
"   i = 100\n"
"   f(i * j * k)\n"
"@}", "2000\n");
    check_ok("{@\n"
"   i = 2\n"
"   if i:\n"
"       j = 3"
"       if j:\n"
"           k = 4\n"
"           if k:\n"
"               def f(n):\n"
"                   if n:\n"
"                       puts(n)\n"
"                   end\n"
"               end\n"
"           end\n"
"           k = 2\n"
"       end\n"
"       j = 10\n"
"   end\n"
"   i = 100\n"
"   f(i * j * k)\n"
"@}", "2000\n");
    check_ok("{@\n"
"   i = 2\n"
"   if i:\n"
"       j = 3"
"       if j:\n"
"           k = 4\n"
"           if k:\n"
"               def f(n):\n"
"                   if n:\n"
"                       puts(n)\n"
"                   end\n"
"               end\n"
"           end\n"
"           k = 2\n"
"       end\n"
"       j = 10\n"
"       if j:\n"
"           if k:\n"
"               puts(j * k)\n"
"           end\n"
"       end\n"
"   end\n"
"   i = 100\n"
"   f(i * j * k)\n"
"@}", "20\n2000\n");
    check_ok("{@\n"
"   if 1:\n"
"       i = 10\n"
"       j = i * 20\n"
"       if j:\n"
"           puts(j)\n"
"       end\n"
"   end\n"
"   i = 2\n"
"   if i:\n"
"       j = 3"
"       if j:\n"
"           k = 4\n"
"           if k:\n"
"               def f(n):\n"
"                   if n:\n"
"                       puts(n)\n"
"                   end\n"
"               end\n"
"           end\n"
"           k = 2\n"
"       end\n"
"       j = 10\n"
"       if j:\n"
"           if k:\n"
"               puts(j * k)\n"
"           end\n"
"       end\n"
"   end\n"
"   i = 100\n"
"   f(i * j * k)\n"
"@}", "200\n20\n2000\n");
}

static void
test_trv_if_stmt_10(void) {
    check_ok("{@ \nif\n0\n:\nend\n @}", "");
}

static void
test_trv_if_stmt_11(void) {
    check_ok("{@\n"
    "   if 1:\n"
    "       i = 0\n"
    "@}{: i :}{@"
    "       j = 1\n"
    "@}{: j :}{@\n"
    "       k = 2\n"
    "@}{: k :}{@\n"
    "   end\n"
    "@}", "012");
}

static void
test_trv_if_stmt_12(void) {
    check_ok("{@ if 0: @}123{@ elif 1: @}223{@ end @}", "223");
    check_ok("{@ if 0: @}123{@ elif 0: @}223{@ elif 1: @}323{@ end @}", "323");
    check_ok("{@ if 0: @}123{@ else: @}223{@ end @}", "223");
    check_ok("{@ if 0: @}123{@ elif 0: @}223{@ else: @}323{@ end @}", "323");
}

static void
test_trv_if_stmt_13(void) {
    check_ok("{@ path = \"/\" if path == \"/\": puts(1) elif path == \"/about\": puts(2) else: puts(3) end @}", "1\n");
    check_ok("{@ path = \"/about\" if path == \"/\": puts(1) elif path == \"/about\": puts(2) else: puts(3) end @}", "2\n");
}

static void
test_trv_if_stmt_fail_0(void) {
    check_fail("{@\n"
    "if 1:\n"
    "   puts(1)\n"
    "e"
    "@}", "reached EOF in if statement");

    check_fail("{@\n"
    "if 1:\n"
    "   puts(1)\n"
    "else:\n"
    "elif 1:\n"
    "end"
    "@}", "syntax error. invalid token");

    check_fail("{@ if 1: @}\n"
    "{@ else: @}\n"
    "{@ elif 1: @}\n"
    "{@ end @}", "syntax error. invalid token");
}

static void
test_trv_if_stmt_fail_1(void) {
    check_fail("{@\n"
    "if 1:\n"
    "el:"
    "end"
    "@}", "syntax error");

    check_fail("{@\n"
    "if 1:\n"
    "el def:"
    "end"
    "@}", "syntax error");
}

static void
test_trv_elif_stmt_0(void) {
    check_ok("{@ if 0: elif 1: puts(1) end @}", "1\n");
    check_ok("{@ if 0: \nelif 1: puts(1) end @}", "1\n");
    check_ok("{@ if 0: elif 1:\n puts(1) end @}", "1\n");
    check_ok("{@ if 0: elif 1: puts(1) \nend @}", "1\n");
    check_ok("{@ if 0: elif 1: puts(1) end \n@}", "1\n");
    check_ok("{@ if 0: elif \n1: puts(1) end @}", "1\n");
    check_ok("{@ if 0: elif 1\n: puts(1) end @}", "1\n");
}

static void
test_trv_elif_stmt_1(void) {
    check_ok("{@ if 0: @}{@ elif 1: @}1{@ end @}", "1");
    check_ok("{@ \nif 0: @}{@ elif 1: @}1{@ end @}", "1");
    check_ok("{@ if 0:\n @}{@ elif 1: @}1{@ end @}", "1");
    check_ok("{@ if 0: @}{@ \nelif 1: @}1{@ end @}", "1");
    check_ok("{@ if 0: @}{@ elif 1: \n@}1{@ end @}", "1");
    check_ok("{@ if 0: @}{@ elif 1: @}1{@ \nend @}", "1");
    check_ok("{@ if 0: @}{@ elif 1: @}1{@ end \n@}", "1");
    check_ok("{@ \nif 0: @}{@ elif 1: @}1{@ end \n@}", "1");
}

static void
test_trv_elif_stmt_2(void) {
    check_ok("{@ if 0: elif 1: if 1: puts(1) end end @}", "1\n");
    check_ok("{@ \nif 0: elif 1: if 1: puts(1) end end @}", "1\n");
    check_ok("{@ if 0: \nelif 1: if 1: puts(1) end end @}", "1\n");
    check_ok("{@ if 0: elif 1:\n if 1: puts(1) end end @}", "1\n");
    check_ok("{@ if 0: elif 1: if 1:\n puts(1) end end @}", "1\n");
    check_ok("{@ if 0: elif 1: if 1: puts(1)\n end end @}", "1\n");
    check_ok("{@ if 0: elif 1: if 1: puts(1) end\n end @}", "1\n");
    check_ok("{@ if 0: elif 1: if 1: puts(1) end end\n @}", "1\n");
}

static void
test_trv_elif_stmt_3(void) {
    check_ok("{@ if 0: @}{@ elif 1: @}{@ if 1: @}1{@ end @}{@ end @}", "1");
    check_ok("{@ if 0: @}{@ \nelif 1: @}{@ if 1: @}1{@ end @}{@ end @}", "1");
    check_ok("{@ if 0: @}{@ elif 1:\n @}{@ if 1: @}1{@ end @}{@ end @}", "1");
    check_ok("{@ if 0: @}{@ elif 1: @}{@ \nif 1: @}1{@ end @}{@ end @}", "1");
    check_ok("{@ if 0: @}{@ elif 1: @}{@ if 1:\n @}1{@ end @}{@ end @}", "1");
    check_ok("{@ if 0: @}{@ elif 1: @}{@ if 1: @}1{@ \nend @}{@ end @}", "1");
    check_ok("{@ if 0: @}{@ elif 1: @}{@ if 1: @}1{@ end \n@}{@ end @}", "1");
}

static void
test_trv_elif_stmt_4(void) {
    check_ok("{@\n"
"   if 0:\n"
"       i = 2 * 3\n"
"       if 1:\n"
"           puts(1)\n"
"       end\n"
"   elif 0:\n"
"       puts(2)\n"
"       j = 3 * 3\n"
"   elif 1:\n"
"       puts(3)\n"
"   end\n"
"@}", "3\n");
    check_fail("{@\n"
"   if i:\n"
"   end\n"
"@}", "\"i\" is not defined in if statement");
    check_fail("{@\n"
"   if 0:\n"
"   elif i:\n"
"   end\n"
"@}", "\"i\" is not defined in if statement");
    check_fail("{@\n"
"   if 1:\n"
"       puts(i)\n"
"   end\n"
"@}", "\"i\" is not defined");
    check_fail("{@\n"
"   if 0:\n"
"       i = 2 * 3\n"
"       if 1:\n"
"           puts(1)\n"
"       end\n"
"       j = 3 * 3\n"
"   elif 0:\n"
"       puts(2)\n"
"       j = 3 * 3\n"
"   elif 1:\n"
"       puts(i)\n"
"   end\n"
"@}", "\"i\" is not defined");
    check_ok("{@\n"
"   if 0:\n"
"       i = 2 * 3\n"
"       if 1:\n"
"           puts(1)\n"
"       end\n"
"       j = 3 * 3\n"
"   elif 0:\n"
"       puts(2)\n"
"       j = 3 * 3\n"
"   elif 9 * 9 - 1:\n"
"       puts(3)\n"
"   end\n"
"@}", "3\n");
    check_ok("{@\n"
"   i = 3\n"
"   j = 2\n"
"   if 0:\n"
"       i = 2 * 3\n"
"       if 1:\n"
"           puts(1)\n"
"       end\n"
"       j = 3 * 3\n"
"   elif i * j:\n"
"       puts(i * j)\n"
"       j = 3 * 3\n"
"   elif 9 * 9 - 1:\n"
"       puts(3)\n"
"   end\n"
"@}", "6\n");
    check_ok("{@\n"
"   i = 3\n"
"   j = 2\n"
"   if 0:\n"
"       i = 2 * 3\n"
"       if 1:\n"
"           puts(1)\n"
"       end\n"
"       j = 3 * 3\n"
"   elif i * j:\n"
"   elif 9 * 9 - 1:\n"
"       puts(3)\n"
"   end\n"
"@}", "");
    check_ok("{@\n"
"   i = 3\n"
"   j = 2\n"
"   if 0:\n"
"       i = 2 * 3\n"
"       if 1:\n"
"           puts(1)\n"
"       end\n"
"       j = 3 * 3\n"
"   elif i * j:\n"
"       if 0:\n"
"           puts(i * j)\n"
"       elif 1:\n"
"           puts(i * j)"
"       end\n"
"   elif 9 * 9 - 1:\n"
"       puts(3)\n"
"   end\n"
"@}", "6\n");
    check_ok("{@\n"
    "   i = 0\n"
    "   j = 0\n"
    "   if 0:\n"
    "       i = 2 * 3\n"
    "       if 1:\n"
    "           puts(1)\n"
    "       end\n"
    "       j = 3 * 3\n"
    "   elif i * j:\n"
    "       if 0:\n"
    "           puts(i * j)\n"
    "       elif 1:\n"
    "           puts(i * j)"
    "       end\n"
    "   elif j * i:\n"
    "       puts(3)\n"
    "   else:\n"
    "       if 0:\n"
    "           puts(123)\n"
    "       elif 2 * 3:\n"
    "           puts(10 * 123)\n"
    "       end\n"
    "   end\n"
    "@}", "1230\n");
}

static void
test_trv_elif_stmt_5(void) {

    check_fail("{@\n"
"   if 0:\n"
"       i = 2 * 3\n"
"       if 1:\n"
"           puts(1)\n"
"       end\n"
"       j = 3 * 3\n"
"   elif 0:\n"
"       puts(2)\n"
"       j = 3 * 3\n"
"   elif 1:\n"
"       puts(i)\n"
"   end\n"
"@}", "\"i\" is not defined");
}

static void
test_trv_elif_stmt_6(void) {
    check_ok("{@\n"
"   if 0:\n"
"       puts(1)\n"
"   elif 1:\n"
"       puts(2)\n"
"   elif 1:\n"
"       puts(3)\n"
"   end\n"
"@}", "2\n");
    check_ok("{@\n"
"   if 0:\n"
"       puts(1)\n"
"   elif 1:\n"
"       if 0:\n"
"           puts(21)\n"
"       elif 0:\n"
"           puts(22)\n"
"       elif 1:\n"
"           puts(23)\n"
"       end"
"   elif 1:\n"
"       puts(3)\n"
"   end\n"
"@}", "23\n");
    check_ok("{@\n"
"   if 0:\n"
"       puts(1)\n"
"   elif 1:\n"
"       if 0:\n"
"           puts(21)\n"
"       elif 0:\n"
"           puts(22)\n"
"       elif 1:\n"
"           if 0:\n"
"           elif 1:\n"
"               puts(31)\n"
"           end"
"       end"
"   elif 1:\n"
"       puts(3)\n"
"   end\n"
"@}", "31\n");
}

static void
test_trv_elif_stmt_7(void) {
    check_ok("{@\n"
"   if 0:\n"
"       puts(1)\n"
"   elif 1:\n"
"@}1{@\n"
"@}2{@\n"
"@}3{@\n"
"   end\n"
"@}", "123");
    check_ok("{@\n"
"   if 0:\n"
"       puts(1)\n"
"   elif 1:\n"
"@}1{@\n"
"       if 0:\n"
"       elif 1:\n"
"@}2{@"
"       end\n"
"@}3{@\n"
"   end\n"
"@}", "123");
    check_ok("{@\n"
"   if 0:\n"
"       puts(1)\n"
"   elif 1:\n"
"@}1{@\n"
"@}2{@\n"
"       if 0:\n"
"       elif 1:\n"
"@}3{@\n"
"@}4{@\n"
"       end\n"
"@}5{@\n"
"@}6{@\n"
"   end\n"
"@}", "123456");
}

static void
test_trv_else_stmt_0(void) {
    check_ok("{@ if 0: else: puts(1) end @}", "1\n");
    check_ok("{@ \nif 0: else: puts(1) end @}", "1\n");
    check_ok("{@ if 0: \nelse: puts(1) end @}", "1\n");
    check_ok("{@ if 0: else:\n puts(1) end @}", "1\n");
    check_ok("{@ if 0: else: puts(1) \nend @}", "1\n");
    check_ok("{@ if 0: else: puts(1) end \n@}", "1\n");
    check_ok("{@ if 0: else\n: puts(1) end @}", "1\n");    
}

static void
test_trv_else_stmt_1(void) {
    check_ok("{@ if 0: @}{@ else: @}1{@ end @}", "1");
    check_ok("{@ \nif 0: @}{@ else: @}1{@ end @}", "1");
    check_ok("{@ if 0:\n @}{@ else: @}1{@ end @}", "1");
    check_ok("{@ if 0: @}{@ \nelse: @}1{@ end @}", "1");
    check_ok("{@ if 0: @}{@ else\n: @}1{@ end @}", "1");
    check_ok("{@ if 0: @}{@ else: \n@}1{@ end @}", "1");
    check_ok("{@ if 0: @}{@ else: @}1{@ \nend @}", "1");
    check_ok("{@ if 0: @}{@ else: @}1{@ end \n@}", "1");
}

static void
test_trv_else_stmt_2(void) {
    check_ok("{@ if 0: else: if 1: puts(1) end end @}", "1\n");
    check_ok("{@ \nif 0: else: if 1: puts(1) end end @}", "1\n");
    check_ok("{@ if 0: \nelse: if 1: puts(1) end end @}", "1\n");
    check_ok("{@ if 0: else\n: if 1: puts(1) end end @}", "1\n");
    check_ok("{@ if 0: else: \nif 1: puts(1) end end @}", "1\n");
    check_ok("{@ if 0: else: if 1:\n puts(1) end end @}", "1\n");
    check_ok("{@ if 0: else: if 1: puts(1)\n end end @}", "1\n");
    check_ok("{@ if 0: else: if 1: puts(1) end \nend @}", "1\n");
    check_ok("{@ if 0: else: if 1: puts(1) end end \n@}", "1\n");
}

static void
test_trv_else_stmt_3(void) {
    check_ok("{@ if 0: @}{@ else: @}{@ if 1: @}1{@ end @}{@ end @}", "1");
    check_ok("{@ \nif 0: @}{@ else: @}{@ if 1: @}1{@ end @}{@ end @}", "1");
    check_ok("{@ if 0: \n@}{@ else: @}{@ if 1: @}1{@ end @}{@ end @}", "1");
    check_ok("{@ if 0: @}{@ \nelse: @}{@ if 1: @}1{@ end @}{@ end @}", "1");
    check_ok("{@ if 0: @}{@ else: \n@}{@ if 1: @}1{@ end @}{@ end @}", "1");
    check_ok("{@ if 0: @}{@ else: @}{@ \nif 1: @}1{@ end @}{@ end @}", "1");
    check_ok("{@ if 0: @}{@ else: @}{@ if 1:\n @}1{@ end @}{@ end @}", "1");
    check_ok("{@ if 0: @}{@ else: @}{@ if 1: @}1{@ \nend @}{@ end @}", "1");
    check_ok("{@ if 0: @}{@ else: @}{@ if 1: @}1{@ end \n@}{@ end @}", "1");
    check_ok("{@ if 0: @}{@ else: @}{@ if 1: @}1{@ end @}{@ \nend @}", "1");
    check_ok("{@ if 0: @}{@ else: @}{@ if 1: @}1{@ end @}{@ end \n@}", "1");
}

static void
test_trv_else_stmt_4(void) {
    check_ok("{@"
"   if 0:\n"
"   else:\n"
"@}1{@\n"
"@}2{@\n"
"@}3{@\n"
"   end\n"
"@}", "123");
}

static void
test_trv_for_stmt_0(void) {
    check_ok("{@ for i=0; i<2; i +=1: puts(i) end @}", "0\n1\n");
    check_ok("{@ size=0 for i=size; i<2; i += 1: puts(i) end @}", "0\n1\n");
    check_ok("{@ \nfor i=0; i<2; i +=1: puts(i) end @}", "0\n1\n");
    check_ok("{@ for i=0; i<2; i +=1: \nputs(i) end @}", "0\n1\n");
    check_ok("{@ for i=0; i<2; i +=1: puts(i)\n end @}", "0\n1\n");
    check_ok("{@ for i=0; i<2; i +=1: puts(i) end \n@}", "0\n1\n");
    check_ok("{@ for \ni=0; i<2; i +=1: puts(i) end @}", "0\n1\n");
    check_ok("{@ for i=0\n; i<2; i +=1: puts(i) end @}", "0\n1\n");
    check_ok("{@ for i=0; \ni<2; i +=1: puts(i) end @}", "0\n1\n");
    check_ok("{@ for i=0; i<2\n; i +=1: puts(i) end @}", "0\n1\n");
    check_ok("{@ for i=0; i<2; \ni +=1: puts(i) end @}", "0\n1\n");
    check_ok("{@ for i=0; i<2; i +=1\n: puts(i) end @}", "0\n1\n");    
}

static void
test_trv_for_stmt_1(void) {
    check_ok("{@ i=0 for i<2: puts(i)\ni+=1 end @}", "0\n1\n");
    check_ok("{@ i=0 for i<2: \nputs(i)\ni+=1 end @}", "0\n1\n");
    check_ok("{@ i=0 for i<2: puts(i)\ni+=1 \nend @}", "0\n1\n");
    check_ok("{@ i=0 for \ni<2: puts(i)\ni+=1 end @}", "0\n1\n");
    check_ok("{@ i=0 for i<2\n: puts(i)\ni+=1 end @}", "0\n1\n");    
}

static void
test_trv_for_stmt_2(void) {
    check_ok("{@ for i, j = 0, 0; i != 4; i += 1, j += 2: end @}{: i :},{: j :}", "4,8");
}

static void
test_trv_for_stmt_3(void) {
    check_ok("{@ for i = 0; i < 2; i += 1: @}{: i :},{@ end @}", "0,1,");
    check_ok("{@\n"
    "def func():\n"
    "   for i = 0; i < 2; i += 1: @}"
    "{: i :}\n"
    "{@ end \n"
    "end \n"
    "\n"
    " func() @}", "0\n1\n");
}

static void
test_trv_for_stmt_4(void) {
    check_ok("{@\n"
    "   def hiphop(rap, n):\n"
    "       puts(rap * n)\n"
    "   end\n"
    "\n"
    "   for i = 0; i < 3; i += 1:\n"
    "       hiphop(\"yo\", i)\n"
    "   end\n"
    "@}", "\nyo\nyoyo\n");
    check_ok("{@\n"
"   for i = 0; i < 3; i += 1:\n"
"       def hiphop(rap, n):\n"
"           puts(rap * n)\n"
"       end\n"
"\n"
"       hiphop(\"yo\", i)\n"
"   end\n"
"@}", "\nyo\nyoyo\n");
    check_ok("{@\n"
"   def hiphop(rap, n):\n"
"       for i = n-1; i >= 0; i -= 1:\n"
"           puts(rap * i)\n"
"       end\n"
"   end\n"
"\n"
"   hiphop(\"yo\", 3)"
"@}", "yoyo\nyo\n\n");
}

static void
test_trv_for_stmt_5(void) {
    check_ok("{@\n"
"   for i = 0; i < 3; i += 1:\n"
"@}{: i :}{@\n"
"   end\n"
"\n"
"@}", "012");
}

static void
test_trv_for_stmt_6(void) {
    check_ok("{@\n"
"    i = 0\n"
"    j = i\n"
"    a = [j, j+1, j+2]\n"
"@}{: a[0] :},{: a[1] :},{: a[2] :}", "0,1,2");
}

static void
test_trv_for_stmt_7(void) {
    check_ok("{@\n"
"for i = 0; i < 4; i += 1:\n"
"   j = i\n"
"end\n"
"@}", "");
}

static void
test_trv_for_stmt_8(void) {
    check_ok("{@\n"
"for i = 0; i < 2; i += 1:\n"
"   j = i\n"
"   k = i\n"
"   puts(j, k)\n"
"end\n"
"@}", "0 0\n1 1\n");
}

static void
test_trv_for_stmt_9(void) {
    check_ok("{@\n"
"for i = 0; i < 2; i += 1:\n"
"   j = i\n"
"   k = i\n"
"@}{: i :}{@"
"   l = i\n"
"   m = i\n"
"end\n"
"@}", "01");
    check_ok("{@\n"
"for i = 0; i < 2; i += 1:\n"
"   j = i\n"
"@}{: j :}{@\n"
"   k = i\n"
"@}{: k :}{@\n"
"   l = i\n"
"@}{: l :}{@\n"
"end\n"
"@}", "000111");
}

static void
test_trv_for_stmt_10(void) {
    check_ok("{@\n"
"for \n i = 0 \n ; \n i < 2 \n ; \n i += 1 \n : \n"
"end\n"
"@}", "");
    check_ok("{@\n"
"i = 0\n"
"for \n i < 2 \n : \n"
"   puts(i)\n"
"   i += 1\n"
"end\n"
"@}", "0\n1\n");
}

static void
test_trv_for_stmt_11(void) {
    // ?
}

static void
test_trv_for_stmt_12(void) {
    check_ok("{@\n"
"   def hiphop(rap, n):\n"
"       puts(rap * n)\n"
"   end\n"
"\n"
"   for i = 0; i < 3; i += 1:\n"
"       hiphop(\"yo\", i)\n"
"   end\n"
"@}", "\nyo\nyoyo\n");
}

static void
test_trv_break_stmt_0(void) {
    check_ok("{@ for: break end @}", "");
    check_ok("{@ for:\n break end @}", "");
    check_ok("{@ for: break \nend @}", "");
}

static void
test_trv_break_stmt_1(void) {
    check_ok("{@\n"
"   for i = 0; i < 2; i += 1:\n"
"       puts(i)\n"
"       break\n"
"       puts(10)\n"
"   end\n"
"@}", "0\n");
    check_ok("{@\n"
"   for i = 0; i < 2; i += 1:\n"
"       puts(i)\n"
"       for j = 4; j < 6; j += 1:\n"
"           puts(j)\n"
"           break\n"
"       end\n"
"       puts(10)\n"
"   end\n"
"@}", "0\n4\n10\n1\n4\n10\n");
}

static void
test_trv_break_stmt_2(void) {
    check_ok("{@\n"
"   for i = 0; i < 2; i += 1:\n"
"       puts(i)\n"
"       if 1:\n"
"           break\n"
"       end\n"
"       puts(10)\n"
"   end\n"
"@}", "0\n");
    check_ok("{@\n"
"   for i = 0; i < 2; i += 1:\n"
"       puts(i)\n"
"       if 0:\n"
"           puts(100)\n"
"       else:\n"
"           break\n"
"       end\n"
"       puts(10)\n"
"   end\n"
"@}", "0\n");
    check_ok("{@\n"
"   for i = 0; i < 2; i += 1:\n"
"       puts(i)\n"
"       if 0:\n"
"           puts(200)\n"
"       elif 1:\n"
"           break\n"
"       else:\n"
"           puts(100)\n"
"       end\n"
"       puts(10)\n"
"   end\n"
"@}", "0\n");
}

static void
test_trv_break_stmt_3(void) {
    check_fail("{@\n"
"   break\n"
"@}", "invalid break statement. not in loop");
    check_fail("{@\n"
"   def f():\n"
"       break\n"
"   end\n"
"\n"
"   for i = 0; i < 2; i += 1:\n"
"       puts(0)\n"
"       f()\n"
"       puts(1)\n"
"   end\n"
"@}", "invalid break statement. not in loop");
    check_fail("{@\n"
"   for i = 0; i < 2; i += 1:\n"
"       def f():\n"
"           break\n"
"       end\n"
"       puts(i)\n"
"   end\n"
"@}", "invalid break statement. not in loop");
}

static void
test_trv_continue_stmt_0(void) {
    check_ok("{@ j=0 for i=0; i<2; i+=1: continue\n j=i end @}{: j :}", "0");
}

static void
test_trv_continue_stmt_1(void) {
    check_ok("{@\n"
"   for i = 0; i < 2; i += 1:\n"
"       puts(0)\n"
"       continue\n"
"       puts(1)\n"
"   end\n"
"@}", "0\n0\n");
}

static void
test_trv_continue_stmt_2(void) {
    check_ok("{@\n"
"   for i = 0; i < 2; i += 1:\n"
"       puts(0)\n"
"       if 1:\n"
"           continue\n"
"       end\n"
"       puts(1)\n"
"   end\n"
"@}", "0\n0\n");
    check_ok("{@\n"
"   for i = 0; i < 2; i += 1:\n"
"       puts(0)\n"
"       if 0:\n"
"       elif 1:\n"
"           continue\n"
"       end\n"
"       puts(1)\n"
"   end\n"
"@}", "0\n0\n");
    check_ok("{@\n"
"   for i = 0; i < 2; i += 1:\n"
"       puts(0)\n"
"       if 0:\n"
"       else:\n"
"           continue\n"
"       end\n"
"       puts(1)\n"
"   end\n"
"@}", "0\n0\n");
}

static void
test_trv_continue_stmt_3(void) {
    check_ok("{@\n"
"   for i = 0; i < 2; i += 1:\n"
"       puts(0)\n"
"       for j = 0; j < 2; j += 1:\n"
"           puts(10)\n"
"           continue\n"
"           puts(20)\n"
"       end\n"
"       puts(1)\n"
"   end\n"
"@}", "0\n10\n10\n1\n0\n10\n10\n1\n");
}

static void
test_trv_continue_stmt_4(void) {
    check_fail("{@\n"
"   def f():\n"
"       continue\n"
"   end\n"
"\n"
"   for i = 0; i < 2; i += 1:\n"
"       puts(0)\n"
"       for j = 0; j < 2; j += 1:\n"
"           puts(10)\n"
"           f()\n"
"           puts(20)\n"
"       end\n"
"       puts(1)\n"
"   end\n"
"@}", "invalid continue statement. not in loop");
}

static void
test_trv_continue_stmt_5(void) {
    check_fail("{@\n"
"   continue\n"
"@}", "invalid continue statement. not in loop");
    check_fail("{@\n"
"   def f():\n"
"       continue\n"
"   end\n"
"\n"
"   for i = 0; i < 2; i += 1:\n"
"       puts(0)\n"
"       f()\n"
"       puts(1)\n"
"   end\n"
"@}", "invalid continue statement. not in loop");
    check_fail("{@\n"
"   for i = 0; i < 2; i += 1:\n"
"       def f():\n"
"           continue\n"
"       end\n"
"       puts(i)\n"
"   end\n"
"@}", "invalid continue statement. not in loop");
}

static void
test_trv_return_stmt_0(void) {
    check_ok("{@ def f(): return 1 end @}{: f() :}", "1");
}

static void
test_trv_return_stmt_1(void) {
    check_ok("{@\n"
"   def f():\n"
"       puts(1)\n"
"       return 2\n"
"       puts(3)\n"
"   end\n"
"@}{: f() :}", "1\n2");
}

static void
test_trv_return_stmt_2(void) {
    check_ok("{@\n"
"   def f():\n"
"       for i = 0; i < 2; i += 1:\n"
"           puts(i)\n"
"           return 1\n"
"           puts(i)\n"
"       end\n"
"   end\n"
"@}{: f() :}", "0\n1");
    check_ok("{@\n"
"   def f():\n"
"       for i = 0; i < 2; i += 1:\n"
"           puts(i)\n"
"           for j = 0; j < 2; j += 1:\n"
"               puts(j)\n"
"               return 1\n"
"           end\n"
"       end\n"
"   end\n"
"@}{: f() :}", "0\n0\n1");

}

static void
test_trv_return_stmt_3(void) {
    check_ok("{@\n"
"   def f():\n"
"       puts(0)\n"
"       if 1:\n"
"           puts(1)\n"
"           return 2\n"
"           puts(3)\n"
"       end\n"
"       puts(4)\n"
"   end\n"
"@}{: f() :}", "0\n1\n2");
    check_ok("{@\n"
"   def f():\n"
"       puts(0)\n"
"       if 0:\n"
"           puts(100)\n"
"       else:\n"
"           puts(1)\n"
"           return 2\n"
"           puts(3)\n"
"       end\n"
"       puts(4)\n"
"   end\n"
"@}{: f() :}", "0\n1\n2");
    check_ok("{@\n"
"   def f():\n"
"       puts(0)\n"
"       if 0:\n"
"           puts(100)\n"
"       elif 1:\n"
"           puts(1)\n"
"           return 2\n"
"           puts(3)\n"
"       else:\n"
"           puts(200)\n"
"       end\n"
"       puts(4)\n"
"   end\n"
"@}{: f() :}", "0\n1\n2");
}

static void
test_trv_return_stmt_4(void) {
    check_fail("{@\n"
"   return\n"
"@}", "invalid return statement. not in function");
    check_fail("{@\n"
"   if 1:\n"
"       return\n"
"   end\n"
"@}", "invalid return statement. not in function");
    check_fail("{@\n"
"   if 0:\n"
"   else:\n"
"       return\n"
"   end\n"
"@}", "invalid return statement. not in function");
    check_fail("{@\n"
"   if 0:\n"
"   elif 1:\n"
"       return\n"
"   end\n"
"@}", "invalid return statement. not in function");
    check_fail("{@\n"
"   for i = 0; i < 2; i += 1:\n"
"       return\n"
"   end\n"
"@}", "invalid return statement. not in function");
}

static void
test_trv_return_stmt_5(void) {
    check_ok("{@\n"
"   def func():\n"
"       if 1:\n"
"          return 1\n"
"       end\n"
"       return 2\n"
"   end\n"
"@}{: func() :}", "1");
}

static void
test_trv_return_stmt_6(void) {
    check_ok("{@\n"
"   def func():\n"
"       if 1:\n"
"          return\n"
"       end\n"
"       puts(1)\n"
"   end\n"
"@}{: func() :}", "nil");
}

static void
test_trv_block_stmt_0(void) {
    check_fail("{@\n"
"block aaa:\n"
"end\n"
"@}", "can't access to function node");
}

static void
test_trv_block_stmt_1(void) {
    check_fail("{@\n"
"block:\n"
"end\n"
"@}", "can't access to function node");
}

static void
test_trv_block_stmt_2(void) {
    check_ok("{@\n"
"def func():\n"
"   block aaa:\n"
"   end\n"
"end\n"
"@}", "");
}

static void
test_trv_inject_stmt_0(void) {
    check_fail("{@\n"
"inject aaa:\n"
"end\n"
"@}", "inject statement needs function");
}

static void
test_trv_inject_stmt_1(void) {
        check_fail("{@\n"
"inject:\n"
"end\n"
"@}", "not found identifier in inject statement");
}

static void
test_trv_inject_stmt_2(void) {
    check_ok("{@\n"
"def func():\n"
"   inject aaa:\n"
"   end\n"
"end\n"
"@}", "");
    }

static void
test_trv_struct_1(void) {
    check_ok("{@\n"
"struct Animal:\n"
"end\n"
"@}", "");
}

static void
test_trv_struct_2(void) {
    check_ok("{@\n"
"struct Animal:\n"
"   a = 1\n"
"   b = 2\n"
"   def aaa():\n"
"       puts(1)\n"
"   end\n"
"   struct Body:\n"
"       c = 3\n"
"   end\n"
"end\n"
"@}", "");
}

static void
test_trv_struct_3(void) {
    check_ok("{@\n"
"struct Animal:\n"
"   def aaa(): @}\n"
"       text\n"
"   {@ end\n"
"end\n"
"@}", "");
}

static void
test_trv_struct_4(void) {
    check_fail("{@\n"
    "struct Animal:\n"
    "   @}text{@\n"
    "end\n"
    "@}", "not found 'end'. found token is 5");
}

static void
test_trv_struct_5(void) {
    check_ok("{@\n"
    "struct Animal:\n"
    "   a = 1\n"
    "   b = 2\n"
    "end\n"
    "animal = Animal()\n"
    "@}{: type(animal) :}", "(struct)");
}

static void
test_trv_struct_6(void) {
    check_ok("{@\n"
"struct Animal:\n"
"   a = 1\n"
"end\n"
"animal = Animal()\n"
"@}{: animal.a :}", "1");
}

static void
test_trv_struct_7(void) {
    check_ok("{@\n"
    "struct Animal:\n"
    "   def func():\n"
    "       return 1\n"
    "   end\n"
    "end\n"
    "animal = Animal()\n"
    "@}{: animal.func() :}"
    , "1");
}

static void
test_trv_struct_8(void) {
    check_ok("{@\n"
"struct Animal:\n"
"   struct Body:\n"
"       a = 1\n"
"   end\n"
"end\n"
"animal = Animal()\n"
"@}", "");
    check_ok("{@\n"
"struct Animal:\n"
"   struct Body:\n"
"       a = 1\n"
"   end\n"
"   body = Body()\n"
"end\n"
"animal = Animal()\n"
"@}", "");
    check_ok("{@\n"
"struct Animal:\n"
"   struct Body:\n"
"       a = 1\n"
"   end\n"
"   body = 1\n"
"end\n"
"animal = Animal()\n"
"@}", "");
    check_ok("{@\n"
"struct Animal:\n"
"   struct Body:\n"
"       a = 1\n"
"   end\n"
"   body = Body()\n"
"end\n"
"animal = Animal()\n"
"@}{: animal.body.a :}", "1");
}

static void
test_trv_struct_9(void) {
    check_ok("{@\n"
"struct Animal:\n"
"end\n"
"animal = Animal()\n"
"animal.a = 1\n"
"@}{: animal.a :}", "1");
}

static void
test_trv_struct_10(void) {
    check_ok("{@\n"
"struct Animal:\n"
"   a = 1\n"
"end\n"
"animal = Animal()\n"
"animal.a = 2\n"
"@}{: animal.a :}", "2");
}

static void
test_trv_struct_11(void) {
    check_ok("{@\n"
"from \"tests/lang/modules/struct-1.aji\" import Animal\n"
"animal = Animal()"
"@}{: animal.a :}", "1");
    check_ok("{@\n"
"from \"tests/lang/modules/struct-2.aji\" import animal\n"
"@}{: animal.a :}", "1");
}

static void
test_trv_struct_12(void) {
    check_ok("{@\n"
"a = 1\n"
"struct Animal:\n"
"   b = a\n"
"end\n"
"animal = Animal()"
"@}{: animal.b :}", "1");
}

static void
test_trv_struct_13(void) {
    check_ok("{@\n"
"struct Animal:\n"
"   def func():\n"
"       puts(1)\n"
"   end\n"
"end\n"
"animal = Animal()\n"
"animal.func()"
"@}", "1\n");
}

static void
test_trv_struct_14(void) {
    check_ok("{@\n"
"def a():\n"
"   puts(1)\n"
"end\n"
"struct Animal:\n"
"   func = a\n"
"end\n"
"animal = Animal()\n"
"animal.func()"
"@}", "1\n");
}

static void
test_trv_struct_15(void) {
    check_ok("{@\n"
"def a():\n"
"   puts(1)\n"
"end\n"
"struct Animal:\n"
"   struct Body:\n"
"       func = a\n"
"   end\n"
"   body = Body()\n"
"end\n"
"animal = Animal()\n"
"animal.body.func()"
"@}", "1\n");
}

static void
test_trv_struct_16(void) {
    check_ok("{@\n"
"def func():\n"
"   puts(1)\n"
"end\n"
"struct Animal:\n"
"   struct Body:\n"
"       legs = func\n"
"   end\n"
"   body = Body()\n"
"end\n"
"struct Human:\n"
"   body = nil\n"
"end\n"
"animal = Animal()\n"
"human = Human()\n"
"human.body = animal.body\n"
"human.body.legs()\n"
"@}", "1\n");
}

static void
test_trv_struct_17(void) {
    check_ok("{@\n"
    "def func(n):\n"
    "   puts(n)\n"
    "end\n"
    "struct Animal:\n"
    "   struct Head:\n"
    "       eyes = 2\n"
    "   end\n"
    "   head = Head()\n"
    "   struct Body:\n"
    "       legs = func\n"
    "   end\n"
    "   body = Body()\n"
    "end\n"
    "struct Human:\n"
    "   head = nil\n"
    "   body = nil\n"
    "end\n"
    "animal = Animal()\n"
    "human = Human()\n"
    "human.head = animal.head\n"
    "human.body = animal.body\n"
    "human.body.legs(human.head.eyes)\n"
    "@}", "2\n");
}

static void
test_trv_struct_17_2(void) {
    check_ok("{@\n"
    "struct Head:\n"
    "end\n"
    "struct Animal:\n"
    "   head = Head()\n"
    "end\n"
    "animal = Animal()\n"
    "@}", "");
    check_ok("{@\n"
    "struct Num:\n"
    "   two = 2\n"
    "end\n"
    "struct Head:\n"
    "   eyes = Num.two\n"
    "end\n"
    // "Head()\n"
    "@}", "");
    check_ok("{@\n"
    "struct Num:\n"
    "   two = 2\n"
    "end\n"
    "struct Head:\n"
    "   eyes = Num.two\n"
    "end\n"
    "struct Animal:\n"
    "   head = Head()\n"
    "end\n"
    "animal = Animal()\n"
    "puts(animal.head.eyes)\n"
    "@}", "2\n");
}

static void
test_trv_struct_17_3(void) {
    check_ok("{@\n"
    "struct Num:\n"
    "   two = 2\n"
    "end\n"
    "struct Animal:\n"
    "   struct Head:\n"
    "       eyes = Num.two\n"
    "   end\n"
    "   head = Head()\n"
    "end\n"
    "animal = Animal()\n"
    "puts(animal.head.eyes)\n"
    "@}", "2\n");

    check_fail("{@\n"
    "struct Animal:\n"
    "   two = 2\n"
    "   struct Head:\n"
    "       eyes = Animal.two\n"
    "   end\n"
    "   head = Head()\n"
    "end\n"
    "animal = Animal()\n"
    "puts(animal.head.eyes)\n"
    "@}", "\"Animal\" is not defined");
}

static void
test_trv_struct_18(void) {
    check_ok("{@\n"
"struct Animal:\n"
"   sum = 0\n"
"   for i = 0; i < 10; i += 1:\n"
"       sum += i\n"
"   end\n"
"end\n"
"animal = Animal()\n"
"@}{: animal.sum :}", "45");
}

static void
test_trv_struct_19(void) {
    check_ok("{@\n"
"struct Animal:\n"
"   puts(1)\n"
"end\n"
"struct Human:\n"
"   puts(2)\n"
"end\n"
"Animal()\n"
"Human()\n"
"@}", "1\n2\n");
}

static void
test_trv_struct_20(void) {
    check_ok("{@\n"
"struct Animal:\n"
"   if true:\n"
"       puts(1)\n"
"   end\n"
"end\n"
"@}", "1\n");
}

static void
test_trv_struct_21(void) {
    check_ok("{@\n"
"def func():\n"
"   a = 1\n"
"   struct Animal:\n"
"       b = a\n"
"   end\n"
"   return Animal()\n"
"end\n"
"animal = func()\n"
"@}{: animal.b :}", "1");
}

static void
test_trv_struct_22(void) {
    check_ok("{@\n"
"struct Animal:\n"
"   a = 1 * 2\n"
"end\n"
"vec = [Animal(), Animal()]\n"
"@}{: vec[0].a :},{: vec[1].a :}", "2,2");
}

static void
test_trv_struct_23(void) {
    check_ok("{@\n"
"struct Animal:\n"
"   a = 1\n"
"end\n"
"d = {\"a\": Animal(), \"b\": Animal()}\n"
"@}{: d[\"a\"].a :},{: d[\"b\"].a :}", "1,1");
}

static void
test_trv_struct_24(void) {
    check_ok("{@\n"
"struct Animal:\n"
"   a = 1\n"
"end\n"
"def f1(a):\n"
"   a.a = 2\n"
"end\n"
"def f2(a):\n"
"   a.a = 3\n"
"end\n"
"animal = Animal()\n"
"puts(animal.a)\n"
"f1(animal)\n"
"puts(animal.a)\n"
"f2(animal)\n"
"puts(animal.a)\n"
"@}", "1\n2\n3\n");
}

static void
test_trv_struct_25(void) {
    check_ok("{@\n"
"struct File:\n"
"   fileno = nil\n"
"end\n"
"def fileNew(fileno):\n"
"   self = File()\n"
"   self.fileno = fileno\n"
"   return self\n"
"end\n"
"file = fileNew(1)\n"
"@}{: file.fileno :}", "1");
}

static void
test_trv_struct_26(void) {
    check_ok("{@\n"
"struct File:\n"
"   n = 1\n"
"end\n"
"@}{: File.n :}", "1");
}

static void
test_trv_struct_27(void) {
    check_ok("{@\n"
    "struct File:\n"
    "   n = 1\n"
    "end\n"
    "f = File()\n"
    "File.n = 2\n"
    "@}{: f.n :}", "1");
}

static void
test_trv_struct_28(void) {
    check_ok("{@\n"
    "struct File:\n"
    "   n = 1\n"
    "end\n"
    "f = File()\n"
    "f.n = 2\n"
    "@}{: File.n :}", "1");
}

static void
test_trv_struct_29(void) {
    check_ok("{@\n"
    "struct File:\n"
    "   n = 1\n"
    "   def f(self):\n"
    "       self.n += 1\n"
    "       puts(self.n)\n"
    "   end\n"
    "end\n"
    "f1 = File()\n"
    "f2 = File()\n"
    "f1.f(f1)\n"
    "f2.f(f2)\n"
    "@}", "2\n2\n");
}

static void
test_trv_struct_30(void) {
    check_ok("{@\n"
    "struct ns:\n"
    "   struct Animal:\n"
    "       n = 1\n"
    "   end\n"
    "end\n"
    "a = ns.Animal()\n"
    "@}{: a.n :}", "1");
}

static void
test_trv_struct_31(void) {
    check_ok("{@\n"
    "struct a:\n"
    "   struct b:\n"
    "       def c():\n"
    "           puts(1)\n"
    "       end\n"
    "   end\n"
    "end\n"
    "a.b.c()\n"
    "@}", "1\n");
}

static void
test_trv_struct_32(void) {
    check_ok("{@\n"
    "struct A:\n"
    "   def b():\n"
    "       return A()"
    "   end\n"
    "end\n"
    "@}{: A.b() :}", "(object)");
}

static void
test_trv_struct_33(void) {
    check_ok("{@\n"
    "import \"tests/lang/modules/struct-3.aji\" as sys\n"
    "struct File:\n"
    "   def read():\n"
    "       puts(sys)\n"
    "   end\n"
    "end\n"
    "File.read()\n"
    "@}", "(module)\n");
}

static void
test_trv_struct_34(void) {
    check_ok("{@\n"
    "import \"tests/lang/modules/struct-3.aji\" as mod\n"
    "struct File:\n"
    "   def read():\n"
    "       mod.test()\n"
    "   end\n"
    "end\n"
    "File.read()\n"
    "@}", "");
}

static void
test_trv_struct_35(void) {
    check_ok("{@\n"
    "import \"tests/lang/modules/struct-4.aji\" as mod\n"
    "struct File:\n"
    "   def read():\n"
    "       return mod.read()\n"
    "   end\n"
    "end\n"
    "@}{: File.read() :}", "readed");
}

static void
test_trv_struct_36(void) {
    check_ok("{@\n"
    "struct A:\n"
    "   def f():\n"
    "       a = nil\n"
    "       puts(a)\n"
    "   end\n"
    "end\n"
    "A.f()\n"
    "@}", "nil\n");
}

static void
test_trv_struct_37(void) {
    check_ok("{@\n"
    "struct A:\n"
    "   def a():\n"
    "       struct B:\n"
    "           def b():\n"
    "               struct C:\n"
    "                   def c():\n"
    "                       return 1\n"
    "                   end\n"
    "               end\n"
    "               return C.c()\n"
    "           end\n"
    "       end\n"
    "       return B.b()"
    "   end\n"
    "end\n"
    "@}{: A.a() :}", "1");
}

static void
test_trv_struct_38(void) {
    check_ok("{@\n"
    "struct A:\n"
    "   def init(self):\n"
    "       self.a = 1\n"
    "       self.b = 2\n"
    "       self.c = 3\n"
    "       return self\n"
    "   end\n"
    "   def dump(self):\n"
    "       puts(self.a, self.b, self.c)\n"
    "   end\n"
    "end\n"
    "a = A()\n"
    "A.init(a)\n"
    "A.dump(a)\n"
    "@}", "1 2 3\n");
}

static void
test_trv_struct_39(void) {
    check_ok(
"{@\n"
"struct Node:\n"
"    value = nil\n"
"    prev = nil\n"
"    next = nil\n"
"end\n"
"\n"
"struct List:\n"
"    head = nil\n"
"    tail = nil\n"
"\n"
"    def push(self, value):\n"
"        if not self.head:\n"
"            self.head = Node()\n"
"            self.head.value = value\n"
"            self.tail = self.head\n"
"            return\n"
"        end\n"
"\n"
"        tail = nil\n"
"        for cur = self.head; cur; cur = cur.next:\n"
"            tail = cur\n"
"        end\n"
"\n"
"        if tail:\n"
"            node = Node()\n"
"            node.value = value\n"
"            node.prev = tail\n"
"            tail.next = node\n"
"            self.tail = node\n"
"        end\n"
"    end\n"
"\n"
"    def pop(self):\n"
"        if not self.tail:\n"
"            return nil\n"
"        end\n"
"\n"
"        tail = self.tail\n"
"\n"
"        if tail.prev:\n"
"            tail.prev.next = nil\n"
"            self.tail = tail.prev\n"
"        else:\n"
"            self.head = nil\n"
"            self.tail = nil\n"
"        end\n"
"\n"
"        tail.prev = nil\n"
"        return tail.value\n"
"    end\n"
"end\n"
"\n"
"list = List()\n"
"List.push(list, 1)\n"
"List.push(list, 2)\n"
"List.push(list, 3)\n"
"puts(List.pop(list))\n"
"puts(List.pop(list))\n"
"puts(List.pop(list))\n"
"@}\n"
, "3\n2\n1\n");

    check_ok(
"{@\n"
"struct Node:\n"
"    value = nil\n"
"    prev = nil\n"
"    next = nil\n"
"end\n"
"\n"
"struct List:\n"
"    head = nil\n"
"    tail = nil\n"
"\n"
"    def dump(self):\n"
"        for cur = self.head; cur; cur = cur.next:\n"
"            puts(cur.value)\n"
"        end\n"
"    end\n"
"\n"
"    def push(self, value):\n"
"        if not self.head:\n"
"            self.head = Node()\n"
"            self.head.value = value\n"
"            self.tail = self.head\n"
"            return\n"
"        end\n"
"\n"
"        tail = nil\n"
"        for cur = self.head; cur; cur = cur.next:\n"
"            tail = cur\n"
"        end\n"
"\n"
"        if tail:\n"
"            node = Node()\n"
"            node.value = value\n"
"            node.prev = tail\n"
"            tail.next = node\n"
"            self.tail = node\n"
"        end\n"
"    end\n"
"\n"
"    def pop(self):\n"
"        if not self.tail:\n"
"            return nil\n"
"        end\n"
"\n"
"        tail = self.tail\n"
"\n"
"        if tail.prev:\n"
"            tail.prev.next = nil\n"
"            self.tail = tail.prev\n"
"        else:\n"
"            self.head = nil\n"
"            self.tail = nil\n"
"        end\n"
"\n"
"        tail.prev = nil\n"
"        return tail.value\n"
"    end\n"
"end\n"
"list = List()\n"
"List.push(list, 0)\n"
"List.push(list, 1)\n"
"List.push(list, 2)\n"
"puts(List.pop(list))\n"
"puts(List.pop(list))\n"
"puts(List.pop(list))\n"
"@}", "2\n1\n0\n");
}

static void
test_trv_struct_40(void) {
    check_ok(
"{@\n"
"struct Node:\n"
"    value = nil\n"
"    prev = nil\n"
"    next = nil\n"
"end\n"
"\n"
"struct List:\n"
"    head = nil\n"
"    tail = nil\n"
"\n"
"    def dump(self):\n"
"        for cur = self.head; cur; cur = cur.next:\n"
"            puts(cur.value)\n"
"        end\n"
"    end\n"
"\n"
"    def push(self, value):\n"
"        if not self.head:\n"
"            self.head = Node()\n"
"            self.head.value = value\n"
"            self.tail = self.head\n"
"            return\n"
"        end\n"
"\n"
"        tail = nil\n"
"        for cur = self.head; cur; cur = cur.next:\n"
"            tail = cur\n"
"        end\n"
"        puts(tail)\n"
"        if tail:\n"
"            node = Node()\n"
"            node.value = value\n"
"            node.prev = tail\n"
"            tail.next = node\n"
"            self.tail = node\n"
"        end\n"
"    end\n"
"\n"
"    def pop(self):\n"
"        if not self.tail:\n"
"            return nil\n"
"        end\n"
"\n"
"        tail = self.tail\n"
"\n"
"        if tail.prev:\n"
"            tail.prev.next = nil\n"
"            self.tail = tail.prev\n"
"        else:\n"
"            self.head = nil\n"
"            self.tail = nil\n"
"        end\n"
"\n"
"        tail.prev = nil\n"
"        return tail.value\n"
"    end\n"
"end\n"
"list = List()\n"
"List.push(list, 0)\n"
"List.push(list, 1)\n"
"List.push(list, 2)\n"
"puts(List.pop(list))\n"
"puts(List.pop(list))\n"
"puts(List.pop(list))\n"
"@}", "(object)\n(object)\n2\n1\n0\n");
}

static void
test_trv_struct_41(void) {
    check_ok(
"{@\n"
"struct Animal:\n"
"   a = 0\n"
"   b = 1\n"
"   c = 2\n"
"end\n"
"a = Animal(10, 11, 12)\n"
"@}{: a.a :},{: a.b :},{: a.c :}", "10,11,12");
}

static void
test_trv_struct_42(void) {
    check_ok(
"{@\n"
"struct Animal:\n"
"   a = 0\n"
"   b = 1\n"
"   c = 2\n"
"end\n"
"a = Animal(10, 11)\n"
"@}{: a.a :},{: a.b :},{: a.c :}", "10,11,2");

    check_ok(
"{@\n"
"struct Animal:\n"
"   a = 0\n"
"   b = 1\n"
"   c = 2\n"
"end\n"
"a = Animal(10)\n"
"@}{: a.a :},{: a.b :},{: a.c :}", "10,1,2");
}

static void
test_trv_struct_43(void) {
    check_ok(
"{@\n"
"struct Animal:\n"
"   a = 0\n"
"   b = 1\n"
"   c = 2\n"
"end\n"
"a = Animal(10, 11, 12, 13)\n"
"@}{: a.a :},{: a.b :},{: a.c :}", "10,11,12");
}

static void
test_trv_struct_44(void) {
    check_ok(
"{@\n"
"struct Body:\n"
"   legs = 4\n"
"end\n"
"body_ = Body()\n"
"struct Animal:\n"
"   body = body_\n"
"end\n"
"a = Animal()\n"
"@}{: a.body.legs :}", "4");
}

static void
test_trv_struct_45(void) {
    check_ok(
"{@\n"
"def func():\n"
"   def f():\n"
"   end\n"
"   puts(f)\n"
"end\n"
"func()\n"
"@}", "(function)\n");
}

static void
test_trv_struct_46(void) {
    check_ok(
"{@\n"
"def func():\n"
"   struct S:\n"
"   end\n"
"   puts(S)\n"
"end\n"
"func()\n"
"@}", "(struct)\n");
}

static void
test_trv_struct_47(void) {
//     check_ok_trace(
// "{@\n"
// "def func():\n"
// "   struct S:\n"
// "   end\n"
// "   puts(S)\n"
// "end\n"
// "vec = [func]\n"
// "f = vec[0]\n"
// "f()\n"
// "@}", "(struct)\n");
}


static void
test_trv_struct_48(void) {
    check_ok(
"{@\n"
"struct S:\n"
"   func = puts\n"
"end\n"
"puts(S.puts)\n"
"@}", "(builtin-function)\n");
}

static void
test_trv_struct_49(void) {
    check_ok(
"{@\n"
"struct S:\n"
"   func = puts\n"
"end\n"
"S.puts(1)\n"
"s = S()\n"
"s.puts(2)\n"
"@}", "1\n2\n");
}

static void
test_trv_struct_50(void) {
    check_ok(
"{@\n"
"struct S:\n"
"   a = 1\n"
"   def f1():\n"
"       return a\n"
"   end\n"
"end\n"
"s = S()\n"
"@}{: s.f1() :}", "1");
}

static void
test_trv_struct_51(void) {
    check_ok(
"{@\n"
"struct S:\n"
"   a = 1\n"
"   met inc(self):\n"
"       self.a += 1\n"
"   end\n"
"end\n"
"s = S()\n"
"s.inc()\n"
"@}{: s.a :}", "2");
}

static void
test_trv_struct_52(void) {
    check_ok(
"{@\n"
"def func():\n"
"   struct S:\n"
"       a = 1\n"
"       met inc(self):\n"
"           self.a += 1\n"
"       end\n"
"   end\n"
"   s = S()\n"
"   s.inc()\n"
"   puts(s.a)\n"
"end\n"
"func()\n"
"@}", "2\n");
}

static void
test_trv_struct_53(void) {
    check_ok(
"{@\n"
"from \"tests/lib/S.aji\" import S\n"
"o = S()\n"
"puts(o.f())\n"
"@}", "nil\n");
}

static void
test_trv_struct_54(void) {
    check_ok(
"{@\n"
"struct Able:\n"
"   a = 1\n"
"end\n"
"struct S:\n"
"   extract(Able)\n"
"end\n"
"puts(S.a)\n"
"@}", "1\n");
}

static void
test_trv_struct_55(void) {
    check_ok(
"{@\n"
"struct Able:\n"
"   met f(self):\n"
"   end\n"
"end\n"
"struct S:\n"
"   extract(Able)\n"
"end\n"
"@}", "");
    check_ok(
"{@\n"
"struct Able:\n"
"   met f(self):\n"
"       return 1\n"
"   end\n"
"end\n"
"struct S:\n"
"   extract(Able)\n"
"end\n"
"o = S()\n"
"puts(S.f())\n"
"puts(o.f())\n"
"@}", "1\n1\n");
}

static void
test_trv_struct_56(void) {
    check_ok(
"{@\n"
"def f():\n"
"   struct Able:\n"
"       met m(self):\n"
"           return 1\n"
"       end\n"
"   end\n"
"   struct S:\n"
"       extract(Able)\n"
"   end\n"
"   o = S()\n"
"   puts(S.m())\n"
"   puts(o.m())\n"
"end\n"
"f()\n"
"@}", "1\n1\n");
}

static void
test_trv_struct_57(void) {
    check_ok(
"{@\n"
"    struct Walkable:\n"
"        met walk(self):\n"
"            return self.name + \" walking\"\n"
"        end\n"
"    end\n"
"\n"
"    struct Animal:\n"
"        name = \"Tama\"\n"
"        extract(Walkable)\n"
"    end\n"
"\n"
"    animal = Animal()\n"
"    puts(animal.walk() == \"Tama walking\")\n"
"    puts(animal.name == \"Tama\")\n"
"\n"
"@}", "true\ntrue\n");
}

static void
test_trv_struct_58(void) {
    check_ok(
"{@\n"
"def case():\n"
"    struct Walkable:\n"
"        met walk(self):\n"
"            return self.name + \" walking\"\n"
"        end\n"
"    end\n"
"\n"
"    struct Animal:\n"
"        name = \"Tama\"\n"
"        extract(Walkable)\n"
"    end\n"
"\n"
"    animal = Animal()\n"
"    puts(animal.walk() == \"Tama walking\")\n"
"    puts(animal.name == \"Tama\")\n"
"end\n"
"cases = {\n"
"    \"case\": case,\n"
"}\n"
"def allTest():\n"
"    keys = cases.keys()\n"
"    for i = 0; i < len(keys); i += 1:\n"
"        cases[keys[i]]()\n"
"    end\n"
"end\n"
"def main():\n"
"   allTest()\n"
"end\n"
"main()\n"
"@}", "true\ntrue\n");
}

static void
test_trv_struct_59(void) {
    check_ok(
"{@\n"
"from \"tests/struct/struct_59.aji\" import main\n"
"puts(main())\n"
"@}\n", "1\n");
}

static void
test_trv_struct_60(void) {
    // TODO: 
    // Objects(Model) not working because Model is not found in
    // AjiLexEnv_FindVarDefault().
    // Modelが定義されている途中でModelを参照しているので、コンテキストにはまだ
    // Modelは存在しない。そのためModelが見つからない
    //
    // PythonではModelを参照できないのでエラーになる
    // 参照できるように実装したところAjiNode_DeepCopyで無限ループになった
    // おそらくコンテキストが循環しているためと思われる

    check_fail(
"{@\n"
"    struct Objects:\n"
"        T = nil\n"
"    end\n"
"\n"
"    struct Model:\n"
"        objects = Objects(Model)\n"
"    end\n"
"\n"
"    puts(Model.objects)\n"
"@}\n", "\"Model\" is not defined");
}

static void
test_trv_struct_fail_0(void) {
        check_fail(
"{@\n"
"st:\n"
"end\n"
"@}", "not found blocks");

    check_fail(
"{@\n"
"struct:\n"
"end\n"
"@}", "not found identifier");

    check_fail(
"{@\n"
"struct A:\n"
"@}", "not found 'end'. found token is 5");

    check_fail(
"{@\n"
"struct A:\n"
"en\n"
"@}", "not found 'end'. found token is 5");

    check_fail(
"{@\n"
"struct A:\n"
"   a\n"
"end\n"
"@}{: A.a :}", "not found \"a\"");

    check_fail(
"{@\n"
"struct A:\n"
"   a\n"
"end\n"
"@}{: A.a :}", "not found \"a\"");

    check_fail(
"{@\n"
"a = struct A:\n"
"end\n"
"@}", "syntax error. not found rhs test in assign list");

    check_fail(
"{@\n"
"[struct A: end]\n"
"@}", "not found ']' in vec");

    check_fail(
"{@\n"
"struct A: end = 1\n"
"@}", "not found blocks");
}

static void
test_trv_type_0(void) {
    check_ok("{@ Vec([1])[0] @}", "");
    check_ok("{: Vec([1, 2, 3])[1] :}", "2");
    check_ok("{: [1][0] :}", "1");
    check_ok("{: len(Vec()) :}", "0");
    check_ok("{: len(Vec([1, 2, 3])) :}", "3");

    check_ok("{@ i = 0 @}{: len(Vec([i])) :}", "1");
    check_ok("{@ i = 0 @}{: len([i]) :}", "1");

    check_ok("{@ i = 0 a = Vec([i]) @}{: len(a) :}", "1");
    
    check_ok("{@ i = 0 a = [i] @}{: id(a[0]) == id(i) :}", "true");
    check_ok("{@ i = 0 a = Vec([i]) @}{: id(a[0]) == id(i) :}", "true");

    check_ok("{@ i = 1.23 a = Vec([i]) @}{: id(a[0]) == id(i) :}", "true");
    check_ok("{@ i = \"abc\" a = Vec([i]) @}{: id(a[0]) == id(i) :}", "true");

    check_ok("{@ i = [0] a = Vec(i) @}{: id(a[0]) != id(i) :}", "true");
    check_ok("{@ i = {\"a\": 0} a = Vec([i]) @}{: id(a[0][\"a\"]) == id(i[\"a\"]) :}", "true");
}

static void
test_trv_type_1(void) {
    check_ok("{: Str() :}", "");
    check_ok("{: Str(\"abc\") :}", "abc");
    check_ok("{: Str(1) :}", "1");
    check_ok("{: Str(true) :}", "true");
    check_ok("{: Str([]) :}", "[]");
}

static void
test_trv_type_2(void) {
    check_ok("{: len(Dict()) :}", "0");
    check_ok("{: len(Dict({\"a\": 1})) :}", "1");
}

static void
test_trv_type_3(void) {
    check_ok("{: Int() :}", "0");
    check_ok("{: Int(1) :}", "1");
    check_ok("{: Int(\"2\") :}", "2");
    check_ok("{: Int(false) :}", "0");
}

static void
test_trv_type_4(void) {
    check_ok("{: Float() :}", "0.0");
    check_ok("{: Float(1) :}", "1.0");
    check_ok("{: Float(\"1.23\") :}", "1.23");
    check_ok("{: Float(false) :}", "0.0");
}

static void
test_trv_type_5(void) {
    check_ok("{: Bool() :}", "false");
    check_ok("{: Bool(0) :}", "false");
    check_ok("{: Bool(1) :}", "true");
    check_ok("{: Bool(\"abc\") :}", "true");
}

static void
test_trv_func_def_0(void) {
    check_ok("{@ def f(): end @}", "");                    
}

static void
test_trv_func_def_1(void) {
    check_ok("{@ def f(a, b): puts(a, b) end f(1, 2) @}", "1 2\n");                    
}

static void
test_trv_func_def_2(void) {
    trv_ready;

    AjiTkr_Parse(tkr, "{@\n"
        "def func(a, b):\n"
        "   c = a + b\n"
        "end\n"
        "@}{: c :}");
    {
        AjiAST_Clear(ast);
        (AjiCC_Compile(ast, AjiTkr_GetToks(tkr)));
        (AjiTrv_Trav(ast, lex_env));
        AjiObjDict *varmap = AjiLexEnv_GetVarmapAtCurScope(lex_env);
        assert(AjiObjDict_Get(varmap, "func"));
    }

    trv_cleanup;
}

static void
test_trv_func_def_3(void) {                        
    check_ok("{@ def f(): end \n a = not f @}", "");
}

static void
test_trv_func_def_4(void) {
    check_ok("{@ i = 1 \n def f(): puts(i) end \n f() @}", "1\n");                    
}

static void
test_trv_func_def_5(void) {                        
    check_ok("{@ def f(arg): end \n f() @}", "");
}

static void
test_trv_func_def_6(void) {
    check_ok("{@\n"
"   def f(n, desc):\n"
"       c = true\n"
"       indent = n * \"    \""
"@}{: indent :}abc{@"
"   end\n"
"   f(1, \"desc\")\n"
"@}", "    abc");
                    
}

static void
test_trv_func_def_7(void) {
    check_ok("{@\n"
"from \"tests/lang/modules/func-def.aji\" import draw\n"
"draw(1, \"desc\")\n"
"@}", "    program\n\n    comment\n");
}

static void
test_trv_func_def_8(void) {
    check_ok("{@\n"
    "   def f(vec):\n"
    "       puts(vec[0], vec[1], vec[2])\n"
    "   end\n"
    "   f([1, 2, 3])\n"
    "@}", "1 2 3\n");
}

static void
test_trv_func_def_9(void) {
    check_ok("{@\n"
"   def f(vec):\n"
"       puts(vec[0], vec[1], vec[2])\n"
"   end\n"
"   i = 0\n"
"   f([i, i+1, i+2])\n"
"@}", "0 1 2\n");
}

static void
test_trv_func_def_10(void) {
    check_ok("{@\n"
"   def f(vec):\n"
"       puts(vec[0], vec[1], vec[2])\n"
"   end\n"
"   for i = 0; i < 3; i += 1:\n"
"       f([i, i+1, i+2])\n"
"   end\n"
"@}", "0 1 2\n1 2 3\n2 3 4\n");
}

static void
test_trv_func_def_11(void) {
    check_ok("{@ def f(): end @}{: f() :}", "nil");
    check_ok("{@ def f(a): return a end @}{: f(1) :}", "1");
    check_ok("{@ def f(a, b): return a + b end @}{: f(1, 2) :}", "3");
    check_ok("{@ def f(): return true end @}{: f() :}", "true");
    check_ok("{@ def f(): return 0 end @}{: f() :}", "0");
    check_ok("{@ def f(): return 1 + 2 end @}{: f() :}", "3");
    check_ok("{@ def f(): @}abc{@ end @}{: f() :}", "abcnil");
    check_ok("{@ def f(): @}abc{@ a = 1 @}def{@ end @}{: f() :}", "abcdefnil");
    check_ok("{@ def f(): @}abc{@ a = 1 @}{: a :}{@ end @}{: f() :}", "abc1nil");
    check_ok("{@ def f(a): @}{: a :}{@ b = 123 @}{: b :}{@ end @}{: f(\"abc\") :}", "abc123nil");
    check_ok("{@\n"
"    def usage():\n"
"@}abc{@\n"
"    end\n"
"@}{: usage() :}", "abcnil");

    check_ok("{@\n"
"    def func():\n"
"        puts(\"hi\")\n"
"    end\n"
"\n"
"    d = { \"f\": func }\n"
"    f = d[\"f\"]\n"
"    f()\n"
"@}", "hi\n");

    check_ok(
        "{@\n"
        "    def func():\n"
        "        puts(\"hi\")\n"
        "    end\n"
        "\n"
        "    def func2(kwargs):\n"
        "        f = kwargs[\"f\"]\n"
        "        f()\n"
        "    end\n"
        "\n"
        "    func2({ \"f\": func })\n"
        "@}"
    , "hi\n");

    check_ok("{@\n"
"    def func():\n"
"       i = 0\n"
"@}{: i :},{@\n"
"       j = 1\n"
"@}{: j :}{@"
"    end\n"
"\n"
"    func()\n"
"@}", "0,1");
}

static void
test_trv_func_def_12(void) {
        // TODO: you need closure?
    // need 'nonlocal' stmti
    check_fail("{@\n"
    "def func():\n"
    "   a = 1\n"
    "   def inner():\n"
    "       a += 1\n"  // <- "a" is not defined
    "       puts(a)\n" // in Python same case
    "   end\n"
    "   inner()\n"
    "end\n"
    "func()\n"
    "@}", "\"a\" is not defined"
    );
}

static void
test_trv_func_def_13(void) {
    check_ok("{@\n"
    "def f1(a):\n"
    "   puts(a)\n"
    "end\n"
    "def f2(callback, a):\n"
    "   callback(a)\n"
    "end\n"
    "f2(f1, 1)\n"
    "@}", "1\n"
    );
    }

static void
test_trv_func_def_14(void) {
    check_ok("{@\n"
    "i = 1\n"
    "idval = id(i)\n"
    "def func(a):\n"
    "   puts(idval != id(a))\n"
    "end\n"
    "func(i)\n"
    "@}", "true\n"
    );

    check_ok("{@\n"
    "i = 1.23\n"
    "idval = id(i)\n"
    "def func(a):\n"
    "   puts(idval != id(a))\n"
    "end\n"
    "func(i)\n"
    "@}", "true\n"
    );

    check_ok("{@\n"
    "i = \"abc\"\n"
    "idval = id(i)\n"
    "def func(a):\n"
    "   puts(idval != id(a))\n"
    "end\n"
    "func(i)\n"
    "@}", "true\n"
    );
}

static void
test_trv_func_def_fail_0(void) {
    check_fail("{@ def @}", "not found blocks");
    check_fail("{@ def f @}", "not found blocks");
    check_fail("{@ def f() @}", "not found colon");
    check_fail("{@ def f(): @}", "not found 'end' in parse func def. token type is 5");
    check_fail("{@ def f(): en @}", "not found 'end' in parse func def. token type is 5");
    check_ok("{@ def f(): end @}", "");
    check_fail("{@ def f(a, ): end @}", "syntax error. not found identifier in func def args");
    check_fail("{@ def f(a): end f(1, 2) @}", "invalid arguments length");
}

static void
test_trv_func_met_0(void) {
    check_ok("{@\n"
    "struct A:\n"
    "   name = 1\n"
    "   met a(self):\n"
    "       puts(self.name)\n"
    "   end\n"
    "end\n"
    "a = A()\n"
    "a.a()"
    "@}", "1\n");
}

static void
test_trv_func_met_1(void) {
    check_ok("{@\n"
    "met a(self):\n"
    "    puts(self)\n"
    "end\n"
    "a()"
    "@}", "nil\n");
}

static void
test_trv_func_met_1_2(void) {
    check_ok("{@\n"
    "met a(self):\n"
    "    puts(self)\n"
    "end\n"
    "a(1)"
    "@}", "1\n");
}

static void
test_trv_func_met_2(void) {
    check_ok(
    "{@\n"
    "    struct Animal:\n"
    "        name = nil\n"
    "\n"
    "        def new(name):\n"
    "            animal = Animal()\n"
    "            animal.name = name\n"
    "            return animal\n"
    "        end\n"
    "\n"
    "        met show(self):\n"
    "            puts(self.name)\n"
    "        end\n"
    "    end\n"
    "\n"
    "    def test3():\n"
    "        animal = Animal.new(\"Mike\")\n"
    "        animal.show()\n"
    "    end\n"
    "\n"
    "    test3()\n"
    "@}", "Mike\n");
}

static void
test_trv_func_met_3(void) {
    check_ok(
    "{@\n"
    "    struct Animal:\n"
    "        age = \"aaa\"\n"
    "        met getAge(self):\n"
    "           return self.age\n"
    "        end\n"
    "    end\n"
    "    a = Animal()\n"
    "@}{: a.getAge() :}", "aaa");
}

static void
test_trv_func_met_4(void) {
    check_ok(
    "{@\n"
    "    struct Animal:\n"
    "        name = nil\n"
    "\n"
    "        def new(name):\n"
    "            animal = Animal()\n"
    "            animal.name = name\n"
    "            return animal\n"
    "        end\n"
    "\n"
    "        met decolate(self, head, tail):\n"
    "            return head + self.name + tail\n"
    "        end\n"
    "    end\n"
    "\n"
    "    def test():\n"
    "        animal = Animal.new(\"Mike\")\n"
    "        puts(animal.decolate(\"The \", \" Man\"))\n"
    "    end\n"
    "\n"
    "    test()\n"
    "@}", "The Mike Man\n");
}

static void
test_trv_func_met_5(void) {
    check_ok(
    "{@\n"
    "    struct Animal:\n"
    "        met show(self, msg):\n"
    "            puts(self, msg)\n"
    "        end\n"
    "    end\n"
    "    animal = Animal()\n"
    "    animal.show(1)\n"
    "@}", "(object) 1\n");
}

static void
test_trv_func_extends_0(void) {
    check_ok("{@\n"
"   def f1():\n"
"   end\n"
"   def f2() extends f1:\n"
"   end\n"
"   f2()\n"
"@}", "");
}

static void
test_trv_func_extends_fail_0(void) {
    check_fail("{@\n"
    "   def f2() extends:\n"
    "   end\n"
    "@}", "not found identifier in function extends");

    check_fail("{@\n"
    "   def f2() ext:\n"
    "   end\n"
    "@}", "not found colon");

    check_fail("{@\n"
    "   def f2() extends f1:\n"
    "   end\n"
    "@}", "not found \"f1\". can't extends");
}

static void
test_trv_func_super_0(void) {
    check_ok("{@\n"
"   def f1():\n"
"       puts(1)\n"
"   end\n"
"   def f2() extends f1:\n"
"       puts(2)\n"
"       super()\n"
"   end\n"
"   f2()\n"
"@}", "2\n1\n");
}

static void
test_trv_func_super_1(void) {
    check_ok("{@\n"
"   def f1():\n"
"       puts(1)\n"
"   end\n"
"   def f2() extends f1:\n"
"       puts(2)\n"
"       super()\n"
"   end\n"
"   def f3() extends f1:\n"
"       puts(3)\n"
"       super()\n"
"   end\n"
"   f2()\n"
"   f3()\n"
"@}", "2\n1\n3\n1\n");
}

static void
test_trv_func_super_2(void) {
    check_ok("{@\n"
    "   def f1():\n"
    "       puts(1)\n"
    "   end\n"
    "   def f2() extends f1:\n"
    "       puts(2)\n"
    "       super()\n"
    "   end\n"
    "   def f3() extends f2:\n"
    "       puts(3)\n"
    "       super()\n"
    "   end\n"
    "   f3()\n"
    "@}", "3\n2\n1\n");
    }

static void
test_trv_func_super_fail_0(void) {
        check_fail("{@\n"
    "   def f1():\n"
    "       super()\n"
    "   end\n"
    "   f1()\n"
    "@}", "\"super\" is not defined");

    check_fail("{@\n"
    "   def f1():\n"
    "   end\n"
    "   def f2() extends f1:\n"
    "       sup()\n"
    "   end\n"
    "   f2()\n"
    "@}", "\"sup\" is not defined");
}

static void
test_trv_block_stmt_3(void) {
    check_ok("{@\n"
    "   def f1():\n"
    "       block content:\n"
    "       end\n"
    "   end\n"
    "   f1()\n"
    "@}", "");
    check_ok("{@\n"
    "   def f1():\n"
    "       puts(1)\n"
    "       block content:\n"
    "           puts(2)\n"
    "       end\n"
    "       puts(3)\n"
    "   end\n"
    "   f1()\n"
    "@}", "1\n2\n3\n");
}

static void
test_trv_block_stmt_4(void) {
    check_ok("{@\n"
    "   def f1():\n"
    "       block content:\n"
    "       end\n"
    "   end\n"
    "   def f2() extends f1:\n"
    "   end\n"
    "@}", "");
    check_ok("{@\n"
    "   def f1():\n"
    "       block content:\n"
    "           puts(1)\n"
    "       end\n"
    "   end\n"
    "   def f2() extends f1:\n"
    "       super()\n"
    "   end\n"
    "   f1()\n"
    "@}", "1\n");
}

static void
test_trv_block_stmt_fail_0(void) {
    check_fail("{@\n"
    "   def f1():\n"
    "       block:\n"
    "       end\n"
    "   end\n"
    "   f1()\n"
    "@}", "not found identifier in block statement");

    check_fail("{@\n"
    "   def f1():\n"
    "       bl:\n"
    "       end\n"
    "   end\n"
    "   f1()\n"
    "@}", "not found 'end' in parse func def. token type is 10");

    check_fail("{@\n"
    "   def f1():\n"
    "       block content:\n"
    "   end\n"
    "@}", "not found 'end' in parse func def. token type is 5");

    check_fail("{@\n"
    "block content:\n"
    "end\n"
    "@}", "can't access to function node");
}

static void
test_trv_inject_stmt_3(void) {
    check_ok("{@\n"
"   def f1():\n"
"       block content:\n"
"           puts(1)\n"
"       end\n"
"   end\n"
"@}", "");
    check_ok("{@\n"
"   def f1():\n"
"       block content:\n"
"       end\n"
"   end\n"
"@}", "");
    check_ok("{@\n"
"   def f1():\n"
"       block content:\n"
"       end\n"
"   end\n"
"@}", "");
    check_ok("{@\n"
"   def f1():\n"
"       block content:\n"
"       end\n"
"   end\n"
"   def f2() extends f1:\n"
"       inject content:\n"
"       end\n"
"   end\n"
"   f2()\n"
"@}", "");
    check_ok("{@\n"
"   def f1():\n"
"       block content:\n"
"           puts(1)\n"
"       end\n"
"   end\n"
"   def f2() extends f1:\n"
"       inject content:\n"
"           puts(2)\n"
"       end\n"
"       super()\n"
"   end\n"
"   f2()\n"
"@}", "2\n");
}

static void
test_trv_inject_stmt_4(void) {
    check_ok("{@\n"
    "   def f1():\n"
    "       block content:\n"
    "           puts(1)\n"
    "       end\n"
    "   end\n"
    "   def f2() extends f1:\n"
    "       inject content:\n"
    "           puts(2)\n"
    "       end\n"
    "       super()\n"
    "   end\n"
    "   f1()\n"
    "   f2()\n"
    "   f1()\n"
    "@}", "1\n2\n1\n");
}

static void
test_trv_inject_stmt_5(void) {
    check_ok("{@\n"
"   def f1():\n"
"       block header:\n"
"           puts(1)\n"
"       end\n"
"       block content:\n"
"           puts(2)\n"
"       end\n"
"   end\n"
"   def f2() extends f1:\n"
"       inject header:\n"
"           puts(3)\n"
"       end\n"
"       inject content:\n"
"           puts(4)\n"
"       end\n"
"       super()\n"
"   end\n"
"   f2()\n"
"@}", "3\n4\n");
}

static void
test_trv_inject_stmt_6(void) {
    check_ok("{@\n"
"   def f1():\n"
"   end\n"
"   def f2() extends f1:\n"
"       super()\n"
"   end\n"
"   def f3() extends f2:\n"
"       super()\n"
"   end\n"
"   f3()\n"
"@}", "");
    check_ok("{@\n"
"   def f1():\n"
"       block content:\n"
"           puts(1)\n"
"       end\n"
"   end\n"
"   def f2() extends f1:\n"
"       super()\n"
"   end\n"
"   def f3() extends f2:\n"
"       inject content:\n"
"           puts(2)\n"
"       end\n"
"       super()\n"
"   end\n"
"   f3()\n"
"@}", "2\n");
}

static void
test_trv_inject_stmt_7(void) {
    check_ok("{@\n"
"   def f1():\n"
"       block header:\n"
"           puts(1)\n"
"       end\n"
"       block content:\n"
"           puts(2)\n"
"       end\n"
"   end\n"
"   def f2() extends f1:\n"
"       inject header:\n"
"           puts(3)\n"
"       end\n"
"       super()\n"
"   end\n"
"   def f3() extends f2:\n"
"       inject content:\n"
"           puts(4)\n"
"       end\n"
"       super()\n"
"   end\n"
"   f3()\n"
"@}", "3\n4\n");
}

static void
test_trv_inject_stmt_8(void) {
    check_ok("{@\n"
"   def f1():\n"
"       block content:\n"
"           puts(0)\n"
"       end\n"
"       block footer:\n"
"           puts(0)\n"
"       end\n"
"   end\n"
"   def f2() extends f1:\n"
"       block header:\n"
"           puts(1)\n"
"       end\n"
"       inject content:\n"
"           puts(2)\n"
"       end\n"
"       super()\n"
"   end\n"
"   def f3() extends f2:\n"
"       inject footer:\n"
"           puts(3)\n"
"       end\n"
"       super()\n"
"   end\n"
"   f3()\n"
"@}", "1\n2\n3\n");
}

static void
test_trv_inject_stmt_9(void) {
    check_ok("{@\n"
    "   def f1(k):\n"
    "       block content:\n"
    "           puts(k[\"b\"])\n"
    "       end\n"
    "   end\n"
    "   def f2(k) extends f1:\n"
    "       block header:\n"
    "           puts(k[\"a\"])\n"
    "       end\n"
    "       super(k)\n"
    "   end\n"
    "   f2({ \"a\": 1, \"b\": 2 })\n"
    "@}", "1\n2\n");
}

static void
test_trv_inject_stmt_10(void) {
    check_fail("{@\n"
"   def f1():\n"
"       block content:\n"
"       end\n"
"   end\n"
"   def f2():\n"
"       inject content:\n"
"       end\n"
"       super()\n"
"   end\n"
"   f2()\n"
"@}", "can't inject. not found extended function");
}

static void
test_trv_inject_stmt_11(void) {
    check_fail("{@\n"
    "   def f1():\n"
    "       puts(a)\n"
    "   end\n"
    "   def f2(a) extends f1:\n"
    "       super()\n"
    "   end\n"
    "   f2(1)\n"
    "@}", "\"a\" is not defined");
}

static void
test_trv_inject_stmt_12(void) {
    check_ok("{@\n"
"   def f1(b):\n"
"       block content:\n"
"           puts(2)\n"
"       end\n"
"       puts(b)\n"
"   end\n"
"   def f2(a) extends f1:\n"
"       inject content:\n"
"           puts(a)\n"
"       end\n"
"       super(3)\n"
"   end\n"
"   f2(1)\n"
"@}", "1\n3\n");
}

static void
test_trv_inject_stmt_13(void) {
    check_ok("{@\n"
"   def base():\n"
"       block header:\n"
"           @}<h1>Title</h1>{@\n"
"       end\n"
"   end\n"
"   def index() extends base:\n"
"       inject header:\n"
"           @}<h1>The title</h1>{@\n"
"       end\n"
"       super()\n"
"   end\n"
"   index()\n"
"@}", "<h1>The title</h1>");
}

static void
test_trv_inject_stmt_14(void) {
    check_ok("{@\n"
"   def base(a):\n"
"       block header:\n"
"           puts(a)"
"       end\n"
"   end\n"
"   def index(a) extends base:\n"
"       inject header:\n"
"           puts(a)\n"
"       end\n"
"       super(2)\n"
"   end\n"
"   index(1)\n"
"@}", "1\n");
}

static void
test_trv_inject_stmt_15(void) {
    check_ok("{@\n"
"   def base(a, b):\n"
"       block header:\n"
"       end\n"
"   end\n"
"   def index(a) extends base:\n"
"       inject header:\n"
"           puts(a, b)\n"
"       end\n"
"       super(2, 3)\n"
"   end\n"
"   index(1)\n"
"@}", "1 3\n");
}

static void
test_trv_inject_stmt_16(void) {
    check_ok("{@\n"
"   def base():\n"
"       block header:\n"
"       end\n"
"   end\n"
"   def index() extends base:\n"
"       i = 1\n"
"       inject header:\n"
"           puts(i)\n"
"       end\n"
"       super()\n"
"   end\n"
"   index()\n"
"@}", "1\n");
}

static void
test_trv_inject_stmt_17(void) {
    check_ok("{@\n"
"   def base():\n"
"       block header:\n"
"       end\n"
"   end\n"
"   def index() extends base:\n"
"       i = 1\n"
"       inject header: @}{: i :}{@ end\n"
"       super()\n"
"   end\n"
"   index()\n"
"@}", "1");
}

static void
test_trv_inject_stmt_18(void) {
    check_ok("{@\n"
"   from \"tests/lang/modules/base.aji\" import base\n"
"\n"
"   def index() extends base:\n"
"       i = 1\n"
"       inject contents:\n"
"           puts(i)\n"
"       end\n"
"       super()\n"
"   end\n"
"\n"
"   index()\n"
"@}", "1\n");
}

static void
test_trv_inject_stmt_19(void) {
    check_ok("{@\n"
    "   def f1():\n"
    "       block content:\n"
    "           puts(2)\n"
    "       end\n"
    "   end\n"
    "   def f2(a) extends f1:\n"
    "       inject content:\n"
    "           puts(a)\n"
    "       end\n"
    "       super()\n"
    "   end\n"
    "   f2(1)\n"
    "@}", "1\n");
    }

static void
test_trv_inject_stmt_20(void) {
    check_ok("{@\n"
    "a = 1\n"
    "def f1():\n"
    "   block content:\n"
    "       puts(a)\n"
    "   end\n"
    "end\n"
    "f1()\n"
    "@}", "1\n");

    check_ok("{@\n"
    "def f1():\n"
    "   a = 1\n"
    "   block content:\n"
    "       puts(a)\n"
    "   end\n"
    "end\n"
    "f1()\n"
    "@}", "1\n");

    check_ok(
    "{@\n"
    "    def f1():\n"
    "        block a:\n"
    "           block b:\n"
    "           end\n"
    "        end\n"
    "    end\n"
    "@}", "");
    check_ok(
    "{@\n"
    "    def f1():\n"
    "        puts(\"f1\")\n"
    "        block a:\n"
    "            puts(\"f1.a\")\n"
    "            block b:\n"
    "                puts(\"f1.b\")\n"
    "            end\n"
    "        end\n"
    "    end\n"
    "\n"
    "    def f2() extends f1:\n"
    "        inject b:\n"
    "            puts(1)\n"
    "        end\n"
    "        super()\n"
    "    end\n"
    "\n"
    "    f2()\n"
    "@}", "f1\nf1.a\n1\n");
}

static void
test_trv_inject_stmt_21(void) {
    check_ok(
    "{@\n"
    "    def f1():\n"
    "        puts(\"f1\")\n"
    "        block a:\n"
    "            puts(\"f1.a\")\n"
    "            block b:\n"
    "                puts(\"f1.b\")\n"
    "            end\n"
    "        end\n"
    "    end\n"
    "\n"
    "    def f2() extends f1:\n"
    "        puts(\"f2\")\n"
    "        block a:\n"
    "            puts(\"f2.a\")\n"
    "        end\n"
    "        super()\n"
    "    end\n"
    "\n"
    "    def f3() extends f2:\n"
    "        puts(\"f3\")"
    "        inject a:\n"
    "            puts(\"inject a from f3\")\n"
    "        end\n"
    "        inject b:\n"
    "            puts(\"inject b from f3\")\n"
    "        end\n"
    "        super()\n"
    "    end\n"
    "\n"
    "    f3()\n"
    "@}", "f3\nf2\ninject a from f3\nf1\nf1.a\ninject b from f3\n");

    check_ok(
    "{@\n"
    "    def f1():\n"
    "        puts(\"f1\")\n"
    "        block a:\n"
    "            puts(\"f1.a\")\n"
    "            block b:\n"
    "                puts(\"f1.b\")\n"
    "            end\n"
    "        end\n"
    "    end\n"
    "\n"
    "    def f2() extends f1:\n"
    "        puts(\"f2\")\n"
    "        block a:\n"
    "            puts(\"f2.a\")\n"
    "        end\n"
    "        super()\n"
    "    end\n"
    "\n"
    "    def f3() extends f2:\n"
    "        puts(\"f3\")\n"
    "        inject a:\n"
    "            puts(\"inject a from f3\")\n"
    "            inject b:\n"
    "                puts(\"inject b from f3\")\n"
    "            end\n"
    "        end\n"
    "        super()\n"
    "    end\n"
    "\n"
    "    f3()\n"
    "@}"
    , "f3\n"
    "f2\n"
    "inject a from f3\n"
    "f1\n"
    "f1.a\n"
    "inject b from f3\n");
}

static void
test_trv_inject_stmt_fail_0(void) {
        check_fail("{@\n"
    "def f1():\n"
    "   bl:\n"
    "   end\n"
    "end\n"
    "@}", "not found 'end' in parse func def. token type is 10");

    check_fail("{@\n"
    "def f1():\n"
    "   block:\n"
    "   end\n"
    "end\n"
    "@}", "not found identifier in block statement");

    check_fail(
    "{@\n"
    "    def f1():\n"
    "    end\n"
    "\n"
    "    def f2() extends f1:\n"
    "        inject a:\n"
    "            puts(\"f2.a\")\n"
    "        end\n"
    "        super()\n"
    "    end\n"
    "\n"
    "    f2()\n"
    "@}", "not found \"a\" block");
}

static void
test_trv_assign_list_0(void) {
    check_ok("{@ a = 1 @}{: a :}", "1");
}

static void
test_trv_assign_list_1(void) {
    check_ok("{@ a = 1, b = 2 @}{: a :},{: b :}", "1,2");
}

static void
test_trv_assign_list_2(void) {
    check_ok("{@ a = b = 1 @}{: a :},{: b :}", "1,1");
}

static void
test_trv_assign_list_3(void) {
    check_ok("{@ a = b = 1, c = 2 @}{: a :},{: b :},{: c :}", "1,1,2");
}

static void
test_trv_assign_list_fail_0(void) {
    check_fail("{@ = @}", "not found blocks");
    check_fail("{@ a = @}", "syntax error. not found rhs test in assign list");
    check_fail("{@ a = 1, @}", "syntax error. not found test in test list");
    check_fail("{@ a = 1, = @}", "syntax error. not found test in test list");
    check_fail("{@ a = 1, b = @}", "syntax error. not found rhs test in assign list");
}

static void
test_trv_multi_assign_0(void) {
    check_ok("{@ a, b = 1, 2 @}{: a :},{: b :}", "1,2");
}

static void
test_trv_multi_assign_1(void) {
    check_ok("{@ a, b = 1, 2 \n a, b = b, a @}{: a :},{: b :}", "2,1");
}

static void
test_trv_multi_assign_fail_0(void) {
    check_fail("{@ a, = 1, 2 @}", "syntax error. not found test in test list");
    check_fail("{@ a, b = 1, @}", "syntax error. not found test in test list");
    check_fail("{@ a, = 1, @}", "syntax error. not found test in test list");
    check_fail("{@ 1, 2 = 1, 2 @}", "invalid left hand operand (1) in calc assign");
}

static void
test_trv_or_test_0(void) {
    check_ok("{: 1 or 0 :}", "1");
}

static void
test_trv_or_test_fail_0(void) {
    check_fail("{: or :}", "not found blocks");
    check_fail("{: or 1 :}", "not found blocks");
    check_fail("{: 1 or :}", "syntax error. not found rhs operand in 'or' operator");
    check_fail("{: 1 or 2 o :}", "syntax error. not found \":}\"");
}

static void
test_trv_and_test_0(void) {
    check_ok("{: 1 and 1 :}", "1");
}

static void
test_trv_and_test_fail_0(void) {
    check_fail("{: and :}", "not found blocks");
    check_fail("{: and 1 :}", "not found blocks");
    check_fail("{: 1 and :}", "syntax error. not found rhs operand in 'and' operator");
    check_fail("{: 1 and 2 o :}", "syntax error. not found \":}\"");
}

static void
test_trv_not_test_0(void) {
    check_ok("{: not 0 :}", "true");
}

static void
test_trv_not_test_fail_0(void) {
    check_fail("{: not :}", "syntax error. not found operand in not operator");
    check_fail("{: 1 not :}", "syntax error. not found \":}\"");
    check_fail("{: not 1 2 :}", "syntax error. not found \":}\"");
}

static void
test_trv_comparison_0(void) {
    check_ok("{: 1 == 1 :}", "true");                    
}

static void
test_trv_comparison_1(void) {
    check_ok("{: 1 != 1 :}", "false");                    
}

static void
test_trv_comparison_2(void) {
    check_ok("{: 1 < 2 :}", "true");                    
}

static void
test_trv_comparison_3(void) {
    check_ok("{: 2 > 1 :}", "true");
}

static void
test_trv_comparison_4(void) {
    check_ok("{: 1 <= 2 :}", "true");
}

static void
test_trv_comparison_5(void) {                        
    check_ok("{: 2 >= 1 :}", "true");
}

static void
test_trv_comparison_6(void) {
    check_ok("{@\n"
    "struct A: end\n"
    "@}{: A() != nil :}", "true");
}

static void
test_trv_comparison_7(void) {
    check_ok("{@ c = \"a\" @}{: c == \">\" :}", "false");
    check_ok("{@ path = \"/\" @}{: path == \"/\" :}", "true");
    check_ok("{@ path = \"/about\" @}{: path == \"/about\" :}", "true");
}

static void
test_trv_asscalc_0(void) {
    /*****
    * ok *
    *****/

    check_ok("{@ a = 0 \n a += 1 @}{: a :}", "1");
    check_ok("{@ a = 0 \n b = 1 \n a += b @}{: a :}", "1");
    check_ok("{@ a = 0 \n a += true @}{: a :}", "1");
    check_ok("{@ a = 0 \n a += false @}{: a :}", "0");

    /*******
    * fail *
    *******/

    check_fail("{@ 0 += 1 @}", "invalid left hand operand (1)");
    check_fail("{@ true += 1 @}", "invalid left hand operand (3)");
    check_fail("{@ a = 0 \n a += \"b\" @}", "invalid right hand operand (5)");
}


static void
test_trv_asscalc_1(void) {
                        
    /*****
    * ok *
    *****/

    check_ok("{@ a = 0 \n a -= 1 @}{: a :}", "-1");
    check_ok("{@ a = 0 \n b = 1 \n a -= b @}{: a :}", "-1");
    check_ok("{@ a = 0 \n a -= true @}{: a :}", "-1");
    check_ok("{@ a = 0 \n a -= false @}{: a :}", "0");

    /*******
    * fail *
    *******/

    check_fail("{@ 1 -= 1 @}", "invalid left hand operand type (1)");
    check_fail("{@ true -= 1 @}", "invalid left hand operand type (3)");
    check_fail("{@ a = 0 \n a -= \"c\" @}", "invalid right hand operand type (5)");
}

static void
test_trv_asscalc_2(void) {
    /*****
    * ok *
    *****/

    check_ok("{@ a = 2 \n a *= 2 @}{: a :}", "4");
    check_ok("{@ a = 2 @}{: (a *= 2) :}", "4");
    check_ok("{@ a = 2 \n a *= true @}{: a :}", "2");
    check_ok("{@ a = 2 \n a *= false @}{: a :}", "0");
    check_ok("{@ a = \"ab\" \n a *= 2 @}{: a :}", "abab");
    check_ok("{@ a = \"ab\" \n a *= 0 @}{: a :}", "");
    check_ok("{@ a = \"ab\" \n a *= true @}{: a :}", "ab");
    check_ok("{@ a = \"ab\" \n a *= false @}{: a :}", "");

    /*******
    * fail *
    *******/

    check_fail("{@ a = \"ab\" \n a *= -1 @}{: a :}"
        , "can't mul by negative value");
    check_fail("{@ 1 *= 2 @}", "invalid left hand operand (1)");
    check_fail("{@ true *= 2 @}", "invalid left hand operand (3)");
    check_fail("{@ a = 2 \n a *= \"b\" @}", "invalid right hand operand (5)");
}

static void
test_trv_asscalc_3(void) {
    /*****
    * ok *
    *****/

    check_ok("{@ a = 4 \n a /= 2 @}{: a :}", "2");
    check_ok("{@ a = 4  @}{: (a /= 2) :}", "2");
    check_ok("{@ a = 4 \n a /= true @}{: a :}", "4");
    check_ok("{@ a = true \n a /= true @}{: a :}", "1.0");
    check_ok("{@ a = false \n a /= true @}{: a :}", "0.0");

    /*******
    * fail *
    *******/

    check_fail("{@ 4 /= 2 @}", "invalid left hand operand (1)");
    check_fail("{@ true /= 2 @}", "invalid left hand operand (3)");
    check_fail("{@ a = 4 \n a /= false @}", "zero division error");
    check_fail("{@ a = 4 \n a /= 0 @}", "zero division error");
    check_fail("{@ a = 4 \n a /= \"b\" @}", "invalid right hand operand (5)");
}

static void
test_trv_asscalc_4(void) {
    check_ok("{@\n"
    "   a = [1, 2]\n"
    "   a[0] += 1\n"
    "@}{: a[0] :}", "2");
}

static void
test_trv_asscalc_5(void) {
    /* this specification is different many languages.
       Aji select equal to 4 on this expression */

    check_ok("{@\n"
    "   a = [1, 2]\n"
    "   a[0] += a[0] += a[0]\n"
    "@}{: a[0] :}", "4");  // Ruby is eq 3, C/C++ is eq 4

    check_ok("{@\n"
    "   a = [1, 2]\n"
    "   a[0] += a[0] += 1\n"
    "@}{: a[0] :}", "4");  // Ruby is eq 3, C/C++ is eq 4

    check_ok("{@\n"
    "   a = [\"a\", \"b\"]\n"
    "   a[0] += a[0] += \"c\"\n"
    "@}{: a[0] :}", "acac");  // Ruby is eq 'aac'

    check_ok("{@\n"
    "   a = [[1, 2], [3, 4]]\n"
    "   a[0] += a[0] += [3, 4]\n"
    "@}{: a[0][0] :},{: a[0][1] :},{: a[0][2] :},{: a[0][3] :},{: a[0][4] :},{: a[0][5] :},{: a[0][6] :},{: a[0][7] :}", "1,2,3,4,1,2,3,4");  // Ruby is eq '1,2,1,2,3,4'
}

static void
test_trv_asscalc_6(void) {
    check_ok("{@\n"
    "   a = [\"aaa\", 2]\n"
    "   a[0] += \"bbb\"\n"
    "@}{: a[0] :}", "aaabbb");
}

static void
test_trv_asscalc_7(void) {
    check_ok("{@\n"
    "   a = [true, 2]\n"
    "   a[0] += 1\n"
    "@}{: a[0] :}", "2");
}

static void
test_trv_asscalc_8(void) {
    check_ok("{@\n"
    "   a = [true, 2]\n"
    "   a[0] += true\n"
    "@}{: a[0] :}", "2");
}

static void
test_trv_asscalc_9(void) {
    check_ok("{@\n"
    "   a = [1, 2]\n"
    "   a[0] -= 1\n"
    "@}{: a[0] :}", "0");
}

static void
test_trv_asscalc_10(void) {
    check_ok("{@\n"
    "   a = [true, 2]\n"
    "   a[0] -= true\n"
    "@}{: a[0] :}", "0");
}

static void
test_trv_asscalc_11(void) {
    check_ok("{@\n"
    "   a = [1, 2]\n"
    "   a[0] *= 2\n"
    "@}{: a[0] :}", "2");
}

static void
test_trv_asscalc_12(void) {
    check_ok("{@\n"
    "   a = [\"abc\", 2]\n"
    "   a[0] *= 2\n"
    "@}{: a[0] :}", "abcabc");
}

static void
test_trv_asscalc_13(void) {
    check_ok("{@\n"
    "   a = [true, 2]\n"
    "   a[0] *= 2\n"
    "@}{: a[0] :}", "2");
}

static void
test_trv_asscalc_14(void) {
    check_ok("{@\n"
"   a = [true, 2]\n"
"   a[0] *= true\n"
"@}{: a[0] :}", "1");
}

static void
test_trv_asscalc_15(void) {
    check_ok("{@\n"
"   a = [4, 2]\n"
"   a[0] /= 2\n"
"@}{: a[0] :}", "2");
}

static void
test_trv_asscalc_16(void) {
    check_fail("{@\n"
"   a = [4, 2]\n"
"   a[0] /= 0\n"
"@}{: a[0] :}", "zero division error");
}

static void
test_trv_asscalc_17(void) {
    check_fail("{@\n"
"   a = [4, 2]\n"
"   a[0] /= false\n"
"@}{: a[0] :}", "zero division error");
}

static void
test_trv_asscalc_18(void) {
    check_ok("{@\n"
"   a = [1, 2]\n"
"   a[0] /= true\n"
"@}{: a[0] :}", "1");
}

static void
test_trv_asscalc_19(void) {
    check_ok("{@\n"
"   a = [2, 2]\n"
"   a[0] %= 2\n"
"@}{: a[0] :}", "0");
}

static void
test_trv_asscalc_20(void) {
    check_ok("{@\n"
"   a = [2, 2]\n"
"   a[0] %= true\n"
"@}{: a[0] :}", "0");
}

static void
test_trv_asscalc_21(void) {
    check_fail("{@\n"
"   a = [2, 2]\n"
"   a[0] %= 0\n"
"@}{: a[0] :}", "zero division error");
}


static void
test_trv_asscalc_22(void) {
    check_fail("{@\n"
    "   a = [2, 2]\n"
    "   a[0] %= false\n"
    "@}{: a[0] :}", "zero division error");
}

static void
test_trv_asscalc_23(void) {
    check_ok("{@\n"
    "   a = \"ab\"\n"
    "   a *= 2\n"
    "@}{: a :}", "abab");
}

static void
test_trv_asscalc_24(void) {
    check_ok("{@\n"
    "   a = [\"ab\"]\n"
    "   a[0] *= 2\n"
    "@}{: a[0] :}", "abab");
}

static void
test_trv_asscalc_25(void) {
    check_ok("{@\n"
    "   a = {\"b\": \"cd\"}\n"
    "   a[\"b\"] *= 2\n"
    "@}{: a[\"b\"] :}", "cdcd");
}

static void
test_trv_asscalc_add_ass_string(void) {
    check_ok("{@ a = \"a\" a += \"b\" @}{: a :}", "ab");
    check_ok("{@ a = \"a\" b = [\"b\"] a += b[0] @}{: a :}", "ab");
    check_ok("{@ a = \"a\" aid = id(a) a += \"b\" @}{: aid != id(a) :}", "true");
}

static void
test_trv_asscalc_fail_0(void) {
    check_fail("{@ += @}", "not found blocks");
    check_fail("{@ -= @}", "not found blocks");
    check_fail("{@ *= @}", "not found blocks");
    check_fail("{@ /= @}", "not found blocks");
    check_fail("{@ %= @}", "not found blocks");
    check_fail("{@ a += @}", "syntax error. not found rhs operand in asscalc");
    check_fail("{@ a -= @}", "syntax error. not found rhs operand in asscalc");
    check_fail("{@ a *= @}", "syntax error. not found rhs operand in asscalc");
    check_fail("{@ a /= @}", "syntax error. not found rhs operand in asscalc");
    check_fail("{@ a %= @}", "syntax error. not found rhs operand in asscalc");
    check_fail("{@ 1 += 1 @}", "invalid left hand operand (1)");
    check_fail("{@ 1 -= 1 @}", "invalid left hand operand type (1)");
    check_fail("{@ 1 *= 1 @}", "invalid left hand operand (1)");
    check_fail("{@ 1 /= 1 @}", "invalid left hand operand (1)");
    check_fail("{@ 1 %= 1 @}", "invalid left hand operand (1)");
    check_fail("{@ a += b @}", "\"a\" is not defined");
    check_fail("{@ a -= b @}", "\"a\" is not defined");
    check_fail("{@ a *= b @}", "\"a\" is not defined");
    check_fail("{@ a /= b @}", "\"a\" is not defined");
    check_fail("{@ a %= b @}", "\"a\" is not defined");
}

static void
test_trv_expr_0(void) {
    check_ok("{: 1 + 1 :}", "2");
    check_ok("{@ a = 1 b = a @}{: b :}", "1");
}

static void
test_trv_expr_1(void) {
    check_ok("{: 1 - 1 :}", "0");
}

static void
test_trv_expr_2(void) {
    check_ok("{@ a = 1 \n b = a - 1 @}{: b :}", "0");
}

static void
test_trv_expr_3(void) {
    check_ok("{@\n"
"a = [1, 2, 3]\n"
"def f(arg):\n"
"   return arg\n"
"end\n"
"r = f(a)\n"
"@}{: r :}", "[1, 2, 3]");
}

static void
test_trv_expr_4(void) {
    check_ok("{@\n"
"a = [1, 2, 3]\n"
"def f(arg):\n"
"   return arg\n"
"end\n"
"r = f(a)[0]\n"
"@}{: r :}", "1");
}

static void
test_trv_expr_4a(void) {
    check_ok("{@\n"
"def f(arg):\n"
"   return arg\n"
"end\n"
"a = [f, 2, 3]\n"
"r = a[0](1)\n"
"@}{: r :}", "1");
}

static void
test_trv_expr_4b(void) {
    check_ok("{@\n"
"import \"tests/lang/modules/func.aji\" as mod\n"
"def f(arg):\n"
"   return arg\n"
"end\n"
"a = [mod, 2, 3]\n"
"r = a[0].vecMod.vec[0]\n"
"@}{: r :}", "0");
}

static void
test_trv_expr_4c(void) {
    check_ok("{@\n"
"import \"tests/lang/modules/func.aji\" as mod\n"
"def f(arg):\n"
"   return arg\n"
"end\n"
"a = [mod, 2, 3]\n"
"r = a[0].vecMod.funcVec[0](0)\n"
"@}{: r :}", "0");
}

static void
test_trv_expr_5(void) {
    check_ok("{@\n"
"a = [1, 2, 3]\n"
"d = { \"a\": 1, \"b\": 2 }\n"
"def f(arg):\n"
"   return arg\n"
"end\n"
"r = a[f(a)[0]]\n"
"@}{: r :}", "2");
    check_ok("{@\n"
"a = [1, 2, 3]\n"
"d = { \"a\": 1, \"b\": 2 }\n"
"def f(arg):\n"
"   return arg\n"
"end\n"
"r = a[f(a)[0] * 2] * 3 + f(10)\n"
"@}{: r :}", "19");
}


static void
test_trv_expr_6(void) {
    check_ok("{@\n"
    "import \"tests/lang/modules/vector\"\n"
    "@}", "");

    check_ok("{@\n"
    "import \"tests/lang/modules/vector.aji\" as mod\n"
    "\n"
    "r = mod.vec[0]\n"
    "@}{: r :}", "0");
}

static void
test_trv_expr_7(void) {
    check_ok("{@\n"
    "import \"tests/lang/modules/func.aji\" as mod\n"
    "\n"
    "r = mod.func(1)\n"
    "@}{: r :}", "1");
}

static void
test_trv_expr_8(void) {
    check_ok("{@\n"
"import \"tests/lang/modules/func.aji\" as mod\n"
"\n"
"r = mod.vecMod.vec[0]\n"
"@}{: r :}", "0");
}

static void
test_trv_expr_9(void) {
    /************************
    * theme: vec and expr *
    ************************/

    check_ok("{@\n"
    "   l = [1, 2]\n"
    "   l2 = l + l\n"
    "@}{: l2[0] :},{: l2[1] :},{: l2[2] :},{: l2[3] :},{: id(l2[0]) == id(l2[2]) :}"
    , "1,2,1,2,true");

    check_ok("{@\n"
    "   l1 = [1, 2]\n"
    "   l2 = [3, 4]\n"
    "   l3 = l1 + l2\n"
    "@}{: l3[0] :},{: l3[1] :},{: l3[2] :},{: l3[3] :}"
    , "1,2,3,4");
}

static void
test_trv_expr_float(void) {
    check_ok("{: 1.1 + 1.2 :}", "2.3");
    check_ok("{: 1 + 1.2 :}", "2.2");
    check_ok("{: 1.1 + true :}", "2.1");
    check_ok("{: 1.2 - 1.1 :}", "0.1");
    check_ok("{: 2 - 1.1 :}", "0.9");
    check_ok("{: 1.2 - true :}", "0.2");
    check_ok("{: 1.2 * 1.3 :}", "1.56");
    check_ok("{: 2 * 1.3 :}", "2.6");
    check_ok("{: 1.2 * true :}", "1.2");
    check_ok("{: 4.0 / 2.0 :}", "2.0");
    check_ok("{: 4 / 2.0 :}", "2.0");
    check_ok("{: 4.0 / true :}", "4.0");
    check_ok("{@ a = 1.1 b = a @}{: b :}", "1.1");

    check_fail("{: 1.0 + \"a\" :}", "can't add with float");
    check_fail("{: 1.0 - \"a\" :}", "can't sub with float");
    check_fail("{: 1.0 * \"a\" :}", "can't mul with float");
    check_fail("{: 1.0 / \"a\" :}", "invalid right hand operand");
    check_fail("{: \"a\" + 1.0 :}", "can't add 2 with string");
    check_fail("{: \"a\" - 1.0 :}", "can't sub");
    check_fail("{: \"a\" * 1.0 :}", "can't mul with string");
    check_fail("{: \"a\" / 1.0 :}", "can't division");

    check_ok("{@ a = 0.0 a += 0.1 @}{: a :}", "0.1");
    check_ok("{@ a = 0.0 a += 1 @}{: a :}", "1.0");
    check_ok("{@ a = 0.0 a += true @}{: a :}", "1.0");
    check_ok("{@ a = 0.0 a -= 0.1 @}{: a :}", "-0.1");
    check_ok("{@ a = 0.0 a -= 1 @}{: a :}", "-1.0");
    check_ok("{@ a = 0.0 a -= true @}{: a :}", "-1.0");
    check_ok("{@ a = 2.0 a *= 0.2 @}{: a :}", "0.4");
    check_ok("{@ a = 2.0 a *= 1 @}{: a :}", "2.0");
    check_ok("{@ a = 2.0 a *= true @}{: a :}", "2.0");
    check_ok("{@ a = 4.0 a /= 2.0 @}{: a :}", "2.0");
    check_ok("{@ a = 4.0 a /= 2 @}{: a :}", "2.0");
    check_ok("{@ a = 4.0 a /= true @}{: a :}", "4.0");

    check_fail("{@ a = 1.0 a += nil @}", "invalid right hand operand (0)");
    check_fail("{@ a = 1.0 a += \"a\" @}", "invalid right hand operand (5)");
    check_fail("{@ a = 1.0 a += [] @}", "invalid right hand operand (6)");
    check_fail("{@ a = 1.0 a += {} @}", "invalid right hand operand (7)");

    check_ok("{@ a = 1 a += 1.0 @}{: a :}", "2.0");
    check_ok("{@ a = 1 a -= 1.0 @}{: a :}", "0.0");
    check_ok("{@ a = 1 a *= 2.0 @}{: a :}", "2.0");
    check_ok("{@ a = 2 a /= 2.0 @}{: a :}", "1.0");

    check_ok("{@ a = true a += 1.0 @}{: a :}", "2.0");
    check_ok("{@ a = true a -= 1.0 @}{: a :}", "0.0");
    check_ok("{@ a = true a *= 2.0 @}{: a :}", "2.0");
    check_ok("{@ a = true a /= 2.0 @}{: a :}", "0.5");

    check_ok("{: 1 == 0.0 :}", "false");
    check_ok("{: 1.0 == 0.0 :}", "false");
    check_ok("{: true == 0.0 :}", "false");
    check_ok("{: 1 != 0.0 :}", "true");
    check_ok("{: 1.0 != 0.0 :}", "true");
    check_ok("{: true != 0.0 :}", "true");
    check_ok("{: 1 < 0.0 :}", "false");
    check_ok("{: 1.0 < 0.0 :}", "false");
    check_ok("{: true < 0.0 :}", "false");
    check_ok("{: 1 <= 0.0 :}", "false");
    check_ok("{: 1.0 <= 0.0 :}", "false");
    check_ok("{: true <= 0.0 :}", "false");
    check_ok("{: 1 > 0.0 :}", "true");
    check_ok("{: 1.0 > 0.0 :}", "true");
    check_ok("{: true > 0.0 :}", "true");
    check_ok("{: 1 >= 0.0 :}", "true");
    check_ok("{: 1.0 >= 0.0 :}", "true");
    check_ok("{: true >= 0.0 :}", "true");

    check_fail("{: 1.0 < nil :}", "can't compare lt with float");
    check_fail("{: 1.0 < \"a\" :}", "can't compare lt with float");
    check_fail("{: 1.0 < [] :}", "can't compare lt with float");
    check_fail("{: 1.0 < {} :}", "can't compare lt with float");
    check_fail("{@ def f(): end @}{: 1.0 < f :}", "can't compare lt with float");
    check_fail("{: 1.0 <= nil :}", "can't compare lte with float");
    check_fail("{: 1.0 <= \"a\" :}", "can't compare lte with float");
    check_fail("{: 1.0 <= [] :}", "can't compare lte with float");
    check_fail("{: 1.0 <= {} :}", "can't compare lte with float");
    check_fail("{@ def f(): end @}{: 1.0 <= f :}", "can't compare lte with float");
    check_fail("{: 1.0 > \"a\" :}", "can't compare gt with float");
    check_fail("{: 1.0 > nil :}", "can't compare gt with float");
    check_fail("{: 1.0 > [] :}", "can't compare gt with float");
    check_fail("{: 1.0 > {} :}", "can't compare gt with float");
    check_fail("{@ def f(): end @}{: 1.0 > f :}", "can't compare gt with float");
    check_fail("{: 1.0 >= nil :}", "can't compare gte with float");
    check_fail("{: 1.0 >= \"a\" :}", "can't compare gte with float");
    check_fail("{: 1.0 >= [] :}", "can't compare gte with float");
    check_fail("{: 1.0 >= {} :}", "can't compare gte with float");
    check_fail("{@ def f(): end @}{: 1.0 >= f :}", "can't compare gte with float");
    check_fail("{: nil < 1.0 :}", "can't compare with lt (0)");
    check_fail("{: \"a\" < 1.0 :}", "can't compare with lt (5)");
    check_fail("{: [] < 1.0 :}", "can't compare with lt (6)");
    check_fail("{: {} < 1.0 :}", "can't compare with lt (7)");
    check_fail("{@ def f(): end @}{: f < 1.0 :}", "can't compare with lt (11)");
    check_fail("{: nil <= 1.0 :}", "can't compare with lte");
    check_fail("{: \"a\" <= 1.0 :}", "can't compare with lte");
    check_fail("{: [] <= 1.0 :}", "can't compare with lte");
    check_fail("{: {} <= 1.0 :}", "can't compare with lte");
    check_fail("{@ def f(): end @}{: f <= 1.0 :}", "can't compare with lte");
    check_fail("{: nil > 1.0 :}", "can't compare with gt");
    check_fail("{: \"a\" > 1.0 :}", "can't compare with gt");
    check_fail("{: [] > 1.0 :}", "can't compare with gt");
    check_fail("{: {} > 1.0 :}", "can't compare with gt");
    check_fail("{@ def f(): end @}{: f > 1.0 :}", "can't compare with gt");
    check_fail("{: nil >= 1.0 :}", "can't compare with gte");
    check_fail("{: \"a\" >= 1.0 :}", "can't compare with gte");
    check_fail("{: [] >= 1.0 :}", "can't compare with gte");
    check_fail("{: {} >= 1.0 :}", "can't compare with gte");
    check_fail("{@ def f(): end @}{: f >= 1.0 :}", "can't compare with gte");

    check_ok("{: 1.0 == nil :}", "false");
    check_ok("{: 1.0 == \"a\" :}", "false");
    check_ok("{: 1.0 == [] :}", "false");
    check_ok("{: 1.0 == {} :}", "false");
    check_ok("{@ def f(): end @}{: 1.0 == f :}", "false");
    check_ok("{: nil == 1.0 :}", "false");
    check_ok("{: \"a\" == 1.0 :}", "false");
    check_ok("{: [] == 1.0 :}", "false");
    check_ok("{: {} == 1.0 :}", "false");
    check_ok("{@ def f(): end @}{: f == 1.0 :}", "false");
    check_ok("{: 1.0 != nil :}", "true");
    check_ok("{: 1.0 != \"a\" :}", "true");
    check_ok("{: 1.0 != [] :}", "true");
    check_ok("{: 1.0 != {} :}", "true");
    check_ok("{@ def f(): end @}{: 1.0 != f :}", "true");
    check_ok("{: nil != 1.0 :}", "true");
    check_ok("{: \"a\" != 1.0 :}", "true");
    check_ok("{: [] != 1.0 :}", "true");
    check_ok("{: {} != 1.0 :}", "true");
    check_ok("{@ def f(): end @}{: f != 1.0 :}", "true");

    check_fail("{@ a = 1.0 a /= 0 @}{: a :}", "zero division error");
    check_fail("{@ a = 1.0 a /= 0.0 @}{: a :}", "zero division error");
    check_fail("{@ a = 1.0 a /= false @}{: a :}", "zero division error");
    check_fail("{@ a = 1 a /= 0 @}{: a :}", "zero division error");
    check_fail("{@ a = 1 a /= 0.0 @}{: a :}", "zero division error");
    check_fail("{@ a = 1 a /= false @}{: a :}", "zero division error");
    check_fail("{@ a = true a /= 0 @}{: a :}", "zero division error");
    check_fail("{@ a = true a /= 0.0 @}{: a :}", "zero division error");
    check_fail("{@ a = true a /= false @}{: a :}", "zero division error");

    // chain-object and float
    // - add operator
    check_ok("{@ a = [1.0] @}{: a[0] + 1.0 :}", "2.0");
    check_ok("{@ a = [1.0] @}{: a[0] + 1 :}", "2.0");
    check_ok("{@ a = [1.0] @}{: a[0] + true :}", "2.0");
    check_fail("{@ a = [1.0] @}{: a[0] + nil :}", "can't add with float");
    check_fail("{@ a = [1.0] @}{: a[0] + [] :}", "can't add with float");
    check_fail("{@ a = [1.0] @}{: a[0] + {} :}", "can't add with float");
    check_fail("{@ a = [1.0] def f(): end @}{: a[0] + f :}", "can't add with float");

    check_ok("{@ a = [1.0] @}{: 1.0 + a[0] :}", "2.0");
    check_ok("{@ a = [1.0] @}{: 1 + a[0] :}", "2.0");
    check_ok("{@ a = [1.0] @}{: true + a[0] :}", "2.0");
    check_fail("{@ a = [1.0] @}{: nil + a[0] :}", "can't add");
    check_fail("{@ a = [1.0] @}{: [] + a[0] :}", "invalid right hand operand (2)");
    check_fail("{@ a = [1.0] @}{: {} + a[0] :}", "can't add");
    check_fail("{@ a = [1.0] def f(): end @}{: f + a[0] :}", "can't add");

    // - sub operator
    check_ok("{@ a = [1.0] @}{: a[0] - 1.0 :}", "0.0");
    check_ok("{@ a = [1.0] @}{: a[0] - 1 :}", "0.0");
    check_ok("{@ a = [1.0] @}{: a[0] - true :}", "0.0");
    check_fail("{@ a = [1.0] @}{: a[0] - nil :}", "can't sub with float");
    check_fail("{@ a = [1.0] @}{: a[0] - [] :}", "can't sub with float");
    check_fail("{@ a = [1.0] @}{: a[0] - {} :}", "can't sub with float");
    check_fail("{@ a = [1.0] def f(): end @}{: a[0] - f :}", "can't sub with float");

    check_ok("{@ a = [1.0] @}{: 1.0 - a[0] :}", "0.0");
    check_ok("{@ a = [1.0] @}{: 1 - a[0] :}", "0.0");
    check_ok("{@ a = [1.0] @}{: true - a[0] :}", "0.0");
    check_fail("{@ a = [1.0] @}{: nil - a[0] :}", "can't sub");
    check_fail("{@ a = [1.0] @}{: [] - a[0] :}", "can't sub");
    check_fail("{@ a = [1.0] @}{: {} - a[0] :}", "can't sub");
    check_fail("{@ a = [1.0] def f(): end @}{: f - a[0] :}", "can't sub");

    // - mul operator
    check_ok("{@ a = [2.0] @}{: a[0] * 2.0 :}", "4.0");
    check_ok("{@ a = [2.0] @}{: a[0] * 2 :}", "4.0");
    check_ok("{@ a = [2.0] @}{: a[0] * true :}", "2.0");
    check_fail("{@ a = [1.0] @}{: a[0] * nil :}", "can't mul with float");
    check_fail("{@ a = [1.0] @}{: a[0] * [] :}", "can't mul with float");
    check_fail("{@ a = [1.0] @}{: a[0] * {} :}", "can't mul with float");
    check_fail("{@ a = [1.0] def f(): end @}{: a[0] * f :}", "can't mul with float");

    check_ok("{@ a = [2.0] @}{: 2.0 * a[0] :}", "4.0");
    check_ok("{@ a = [2.0] @}{: 2 * a[0] :}", "4.0");
    check_ok("{@ a = [2.0] @}{: true * a[0] :}", "2.0");
    check_fail("{@ a = [1.0] @}{: nil * a[0] :}", "can't mul");
    check_fail("{@ a = [1.0] @}{: [] * a[0] :}", "can't mul");
    check_fail("{@ a = [1.0] @}{: {} * a[0] :}", "can't mul");
    check_fail("{@ a = [1.0] def f(): end @}{: f * a[0] :}", "can't mul");

    // - div operator
    check_ok("{@ a = [4.0] @}{: a[0] / 2.0 :}", "2.0");
    check_ok("{@ a = [4.0] @}{: a[0] / 2 :}", "2.0");
    check_ok("{@ a = [4.0] @}{: a[0] / true :}", "4.0");
    check_fail("{@ a = [1.0] @}{: a[0] / nil :}", "invalid right hand operand");
    check_fail("{@ a = [1.0] @}{: a[0] / [] :}", "invalid right hand operand");
    check_fail("{@ a = [1.0] @}{: a[0] / {} :}", "invalid right hand operand");
    check_fail("{@ a = [1.0] def f(): end @}{: a[0] / f :}", "invalid right hand operand");

    check_ok("{@ a = [2.0] @}{: 4.0 / a[0] :}", "2.0");
    check_ok("{@ a = [2.0] @}{: 4 / a[0] :}", "2.0");
    check_ok("{@ a = [2.0] @}{: true / a[0] :}", "0.5");
    check_fail("{@ a = [1.0] @}{: nil / a[0] :}", "can't division");
    check_fail("{@ a = [1.0] @}{: [] / a[0] :}", "can't division");
    check_fail("{@ a = [1.0] @}{: {} / a[0] :}", "can't division");
    check_fail("{@ a = [1.0] def f(): end @}{: f / a[0] :}", "can't division");

    // - assign operator
    check_ok("{@ a = [1.0] a[0] = 2.0 @}{: a[0] :}", "2.0");
    check_ok("{@ a = [1.0] a[0] = 2 @}{: a[0] :}", "2");
    check_ok("{@ a = [1.0] a[0] = true @}{: a[0] :}", "true");
    check_ok("{@ a = [1.0] @}{: a[0] = nil :}", "nil");
    check_ok("{@ a = [1.0] @}{: a[0] = [] :}", "[]");
    check_ok("{@ a = [1.0] @}{: a[0] = {} :}", "{}");
    check_ok("{@ a = [1.0] def f(): end @}{: a[0] = f :}", "(function)");

    check_ok("{@ a = [1] a[0] = 1.0 @}{: a[0] :}", "1.0");
    check_ok("{@ a = [true] a[0] = 1.0 @}{: a[0] :}", "1.0");
    check_ok("{@ a = [nil] a[0] = 1.0 @}{: a[0] :}", "1.0");
    check_ok("{@ a = [[]] a[0] = 1.0 @}{: a[0] :}", "1.0");
    check_ok("{@ a = [{}] a[0] = 1.0 @}{: a[0] :}", "1.0");
    check_ok("{@ def f(): end a = [f] a[0] = 1.0 @}{: a[0] :}", "1.0");

    // - add assign operator
    check_ok("{@ a = [1.0] a[0] += 1.0 @}{: a[0] :}", "2.0");
    check_ok("{@ a = [1.0] a[0] += 1 @}{: a[0] :}", "2.0");
    check_ok("{@ a = [1.0] a[0] += true @}{: a[0] :}", "2.0");
    check_fail("{@ a = [1.0] @}{: a[0] += nil :}", "invalid right hand operand (0)");
    check_fail("{@ a = [1.0] @}{: a[0] += [] :}", "invalid right hand operand (6)");
    check_fail("{@ a = [1.0] @}{: a[0] += {} :}", "invalid right hand operand (7)");
    check_fail("{@ a = [1.0] def f(): end @}{: a[0] += f :}", "invalid right hand operand (11)");

    check_ok("{@ a = [1] a[0] += 1.0 @}{: a[0] :}", "2.0");
    check_ok("{@ a = [true] a[0] += 1.0 @}{: a[0] :}", "2.0");
    check_fail("{@ a = [nil] a[0] += 1.0 @}{: a[0] :}", "invalid left hand operand (0)");
    check_fail("{@ a = [[]] a[0] += 1.0 @}{: a[0] :}", "invalid right hand operand (2)");
    check_fail("{@ a = [{}] a[0] += 1.0 @}{: a[0] :}", "invalid left hand operand (7)");
    check_fail("{@ def f(): end a = [f] a[0] += 1.0 @}{: a[0] :}", "invalid left hand operand (11)");

    // - sub assign operator
    check_ok("{@ a = [1.0] a[0] -= 1.0 @}{: a[0] :}", "0.0");
    check_ok("{@ a = [1.0] a[0] -= 1 @}{: a[0] :}", "0.0");
    check_ok("{@ a = [1.0] a[0] -= true @}{: a[0] :}", "0.0");
    check_fail("{@ a = [1.0] @}{: a[0] -= nil :}", "invalid right hand operand (0)");
    check_fail("{@ a = [1.0] @}{: a[0] -= [] :}", "invalid right hand operand (6)");
    check_fail("{@ a = [1.0] @}{: a[0] -= {} :}", "invalid right hand operand (7)");
    check_fail("{@ a = [1.0] def f(): end @}{: a[0] -= f :}", "invalid right hand operand (11)");

    check_ok("{@ a = [1] a[0] -= 1.0 @}{: a[0] :}", "0.0");
    check_ok("{@ a = [true] a[0] -= 1.0 @}{: a[0] :}", "0.0");
    check_fail("{@ a = [nil] a[0] -= 1.0 @}{: a[0] :}", "invalid left hand operand (0)");
    check_fail("{@ a = [[]] a[0] -= 1.0 @}{: a[0] :}", "invalid left hand operand (6)");
    check_fail("{@ a = [{}] a[0] -= 1.0 @}{: a[0] :}", "invalid left hand operand (7)");
    check_fail("{@ def f(): end a = [f] a[0] -= 1.0 @}{: a[0] :}", "invalid left hand operand (11)");

    // - mul assign operator
    check_ok("{@ a = [2.0] a[0] *= 2.0 @}{: a[0] :}", "4.0");
    check_ok("{@ a = [2.0] a[0] *= 2 @}{: a[0] :}", "4.0");
    check_ok("{@ a = [2.0] a[0] *= true @}{: a[0] :}", "2.0");
    check_fail("{@ a = [2.0] @}{: a[0] *= nil :}", "invalid right hand operand (0)");
    check_fail("{@ a = [2.0] @}{: a[0] *= [] :}", "invalid right hand operand (6)");
    check_fail("{@ a = [2.0] @}{: a[0] *= {} :}", "invalid right hand operand (7)");
    check_fail("{@ a = [2.0] def f(): end @}{: a[0] *= f :}", "invalid right hand operand (11)");

    check_ok("{@ a = [2] a[0] *= 2.0 @}{: a[0] :}", "4.0");
    check_ok("{@ a = [true] a[0] *= 2.0 @}{: a[0] :}", "2.0");
    check_fail("{@ a = [nil] a[0] *= 1.0 @}", "invalid left hand operand (0)");
    check_fail("{@ a = [[]] a[0] *= 1.0 @}", "invalid left hand operand (6)");
    check_fail("{@ a = [{}] a[0] *= 1.0 @}", "invalid left hand operand (7)");
    check_fail("{@ def f(): end a = [f] a[0] *= 1.0 @}", "invalid left hand operand (11)");

    // - div assign operator
    check_ok("{@ a = [4.0] a[0] /= 2.0 @}{: a[0] :}", "2.0");
    check_ok("{@ a = [4.0] a[0] /= 2 @}{: a[0] :}", "2.0");
    check_ok("{@ a = [4.0] a[0] /= true @}{: a[0] :}", "4.0");
    check_fail("{@ a = [4.0] @}{: a[0] /= nil :}", "invalid right hand operand (0)");
    check_fail("{@ a = [4.0] @}{: a[0] /= [] :}", "invalid right hand operand (6)");
    check_fail("{@ a = [4.0] @}{: a[0] /= {} :}", "invalid right hand operand (7)");
    check_fail("{@ a = [4.0] def f(): end @}{: a[0] /= f :}", "invalid right hand operand (11)");

    check_ok("{@ a = [4] a[0] /= 2.0 @}{: a[0] :}", "2.0");
    check_ok("{@ a = [true] a[0] /= 2.0 @}{: a[0] :}", "0.5");
    check_fail("{@ a = [nil] a[0] /= 1.0 @}", "invalid left hand operand (0)");
    check_fail("{@ a = [[]] a[0] /= 1.0 @}", "invalid left hand operand (6)");
    check_fail("{@ a = [{}] a[0] /= 1.0 @}", "invalid left hand operand (7)");
    check_fail("{@ def f(): end a = [f] a[0] /= 1.0 @}", "invalid left hand operand (11)");

    // - comparison operator
    // -- eq operator
    check_ok("{@ a = [1.0] @}{: a[0] == 1.0 :}", "true");
    check_ok("{@ a = [1] @}{: a[0] == 1.0 :}", "true");
    check_ok("{@ a = [true] @}{: a[0] == 1.0 :}", "true");
    check_ok("{@ a = [4.0] @}{: a[0] == nil :}", "false");
    check_ok("{@ a = [4.0] @}{: a[0] == [] :}", "false");
    check_ok("{@ a = [4.0] @}{: a[0] == {} :}", "false");
    check_ok("{@ a = [4.0] def f(): end @}{: a[0] == f :}", "false");

    check_ok("{@ a = [1.0] @}{: 1.0 == a[0] :}", "true");
    check_ok("{@ a = [1] @}{: 1.0 == a[0] :}", "true");
    check_ok("{@ a = [true] @}{: 1.0 == a[0] :}", "true");
    check_ok("{@ a = [nil] @}{: a[0] == 1.0 :}", "false");
    check_ok("{@ a = [[]] @}{: a[0] == 1.0 :}", "false");
    check_ok("{@ a = [{}] @}{: a[0] == 1.0 :}", "false");
    check_ok("{@ def f(): end a = [f] @}{: a[0] == 1.0 :}", "false");

    // -- neq operator
    check_ok("{@ a = [1.0] @}{: a[0] != 1.0 :}", "false");
    check_ok("{@ a = [1] @}{: a[0] != 1.0 :}", "false");
    check_ok("{@ a = [true] @}{: a[0] != 1.0 :}", "false");
    check_ok("{@ a = [nil] @}{: a[0] != 1.0 :}", "true");
    check_ok("{@ a = [[]] @}{: a[0] != 1.0 :}", "true");
    check_ok("{@ a = [{}] @}{: a[0] != 1.0 :}", "true");
    check_ok("{@ def f(): end a = [f] @}{: a[0] != 1.0 :}", "true");

    check_ok("{@ a = [1.0] @}{: 1.0 != a[0] :}", "false");
    check_ok("{@ a = [1] @}{: 1.0 != a[0] :}", "false");
    check_ok("{@ a = [true] @}{: 1.0 != a[0] :}", "false");
    check_ok("{@ a = [nil] @}{: 1.0 != a[0] :}", "true");
    check_ok("{@ a = [[]] @}{: 1.0 != a[0] :}", "true");
    check_ok("{@ a = [{}] @}{: 1.0 != a[0] :}", "true");
    check_ok("{@ def f(): end a = [f] @}{: 1.0 != a[0] :}", "true");

    // -- gt operator
    check_ok("{@ a = [1.0] @}{: a[0] > 0.5 :}", "true");
    check_ok("{@ a = [1] @}{: a[0] > 0.5 :}", "true");
    check_ok("{@ a = [true] @}{: a[0] > 0.5 :}", "true");
    check_fail("{@ a = [nil] @}{: a[0] > 0.5 :}", "can't compare with gt");
    check_fail("{@ a = [[]] @}{: a[0] > 0.5 :}", "can't compare with gt");
    check_fail("{@ a = [{}] @}{: a[0] > 0.5 :}", "can't compare with gt");
    check_fail("{@ def f(): end a = [f] @}{: a[0] > 0.5 :}", "can't compare with gt");

    check_ok("{@ a = [1.0] @}{: 0.5 > a[0] :}", "false");
    check_ok("{@ a = [1] @}{: 0.5 > a[0] :}", "false");
    check_ok("{@ a = [true] @}{: 0.5 > a[0] :}", "false");
    check_fail("{@ a = [nil] @}{: 0.5 > a[0] :}", "can't compare gt with float");
    check_fail("{@ a = [[]] @}{: 0.5 > a[0] :}", "can't compare gt with float");
    check_fail("{@ a = [{}] @}{: 0.5 > a[0] :}", "can't compare gt with float");
    check_fail("{@ def f(): end a = [f] @}{: 0.5 > a[0] :}", "can't compare gt with float");

    // -- gte operator
    check_ok("{@ a = [1.0] @}{: a[0] >= 0.5 :}", "true");
    check_ok("{@ a = [1] @}{: a[0] >= 0.5 :}", "true");
    check_ok("{@ a = [true] @}{: a[0] >= 0.5 :}", "true");
    check_fail("{@ a = [nil] @}{: a[0] >= 0.5 :}", "can't compare with gte");
    check_fail("{@ a = [[]] @}{: a[0] >= 0.5 :}", "can't compare with gte");
    check_fail("{@ a = [{}] @}{: a[0] >= 0.5 :}", "can't compare with gte");
    check_fail("{@ def f(): end a = [f] @}{: a[0] >= 0.5 :}", "can't compare with gte");

    check_ok("{@ a = [1.0] @}{: 0.5 >= a[0] :}", "false");
    check_ok("{@ a = [1] @}{: 0.5 >= a[0] :}", "false");
    check_ok("{@ a = [true] @}{: 0.5 >= a[0] :}", "false");
    check_fail("{@ a = [nil] @}{: 0.5 >= a[0] :}", "can't compare gte with float");
    check_fail("{@ a = [[]] @}{: 0.5 >= a[0] :}", "can't compare gte with float");
    check_fail("{@ a = [{}] @}{: 0.5 >= a[0] :}", "can't compare gte with float");
    check_fail("{@ def f(): end a = [f] @}{: 0.5 >= a[0] :}", "can't compare gte with float");

    // -- lt operator
    check_ok("{@ a = [1.0] @}{: a[0] < 1.5 :}", "true");
    check_ok("{@ a = [1] @}{: a[0] < 1.5 :}", "true");
    check_ok("{@ a = [true] @}{: a[0] < 1.5 :}", "true");
    check_fail("{@ a = [nil] @}{: a[0] < 1.5 :}", "can't compare with lt (0)");
    check_fail("{@ a = [[]] @}{: a[0] < 1.5 :}", "can't compare with lt (6)");
    check_fail("{@ a = [{}] @}{: a[0] < 1.5 :}", "can't compare with lt (7)");
    check_fail("{@ def f(): end a = [f] @}{: a[0] < 1.5 :}", "can't compare with lt (11)");

    check_ok("{@ a = [1.0] @}{: 1.5 < a[0] :}", "false");
    check_ok("{@ a = [1] @}{: 1.5 < a[0] :}", "false");
    check_ok("{@ a = [true] @}{: 1.5 < a[0] :}", "false");
    check_fail("{@ a = [nil] @}{: 1.5 < a[0] :}", "can't compare lt with float");
    check_fail("{@ a = [[]] @}{: 1.5 < a[0] :}", "can't compare lt with float");
    check_fail("{@ a = [{}] @}{: 1.5 < a[0] :}", "can't compare lt with float");
    check_fail("{@ def f(): end a = [f] @}{: 1.5 < a[0] :}", "can't compare lt with float");

    // -- lte operator
    check_ok("{@ a = [1.0] @}{: a[0] <= 1.5 :}", "true");
    check_ok("{@ a = [1] @}{: a[0] <= 1.5 :}", "true");
    check_ok("{@ a = [true] @}{: a[0] <= 1.5 :}", "true");
    check_fail("{@ a = [nil] @}{: a[0] <= 1.5 :}", "can't compare with lte");
    check_fail("{@ a = [[]] @}{: a[0] <= 1.5 :}", "can't compare with lte");
    check_fail("{@ a = [{}] @}{: a[0] <= 1.5 :}", "can't compare with lte");
    check_fail("{@ def f(): end a = [f] @}{: a[0] <= 1.5 :}", "can't compare with lte");

    check_ok("{@ a = [1.0] @}{: 1.5 <= a[0] :}", "false");
    check_ok("{@ a = [1] @}{: 1.5 <= a[0] :}", "false");
    check_ok("{@ a = [true] @}{: 1.5 <= a[0] :}", "false");
    check_fail("{@ a = [nil] @}{: 1.5 <= a[0] :}", "can't compare lte with float");
    check_fail("{@ a = [[]] @}{: 1.5 <= a[0] :}", "can't compare lte with float");
    check_fail("{@ a = [{}] @}{: 1.5 <= a[0] :}", "can't compare lte with float");
    check_fail("{@ def f(): end a = [f] @}{: 1.5 <= a[0] :}", "can't compare lte with float");
}

static void
test_trv_expr_string_0(void) {
    check_ok("{: \"abc\" + \"def\" :}", "abcdef");
}

static void
test_trv_expr_fail_0(void) {
    check_fail("{@ 1 + + @}", "syntax error. not found rhs operand in expr");
    check_fail("{@ + 1 + @}", "not found blocks");
    check_fail("{@ 1 + + @}", "syntax error. not found rhs operand in expr");
    check_fail("{@ 1 + [] @}", "can't add with int");
    check_fail("{@ [] + 1 @}", "invalid right hand operand (1)");
    check_fail("{@ 1 + {} @}", "can't add with int");
    check_fail("{@ {} + 1 @}", "can't add");
    check_fail("{@ 1 + \"a\" @}", "can't add with int");
    check_fail("{@ \"a\" + 1 @}", "can't add 1 with string");

    check_fail("{@ 1 - - @}", "syntax error. not found rhs operand in expr");
    check_fail("{@ - 1 - @}", "syntax error. not found rhs operand in expr");
    check_fail("{@ 1 - - @}", "syntax error. not found rhs operand in expr");
    check_fail("{@ 1 - [] @}", "can't sub with int");
    check_fail("{@ [] - 1 @}", "can't sub");
    check_fail("{@ 1 - {} @}", "can't sub with int");
    check_fail("{@ {} - 1 @}", "can't sub");
    check_fail("{@ 1 - \"a\" @}", "can't sub with int");
    check_fail("{@ \"a\" - 1 @}", "can't sub");
}

static void
test_trv_term_0(void) {
    check_ok("{: 2 * 2 :}", "4");
    check_ok("{: 2 * 2 * 3 :}", "12");
    check_ok("{: 2 * \"abc\" :}", "abcabc");
    check_ok("{: \"abc\" * 2 :}", "abcabc");
    check_ok("{: \"abc\" * 2 * 2 :}", "abcabcabcabc");
    check_ok("{: 0 * \"abc\" :}", "");
    check_fail("{: -1 * \"abc\" :}", "can't mul string by negative value");
}

static void
test_trv_term_1(void) {
    check_ok("{: 4 / 2 :}", "2");
    check_ok("{: 4 / true :}", "4");

    check_fail("{: 4 / 0 :}", "zero division error");
    check_fail("{: 4 / false :}", "zero division error");
    check_fail("{: 4 / \"aa\" :}", "invalid right hand operand");
    check_fail("{: 4 / [] :}", "invalid right hand operand");
    check_fail("{: 4 / {} :}", "invalid right hand operand");

    check_ok("{: true / 1 :}", "1");
    check_ok("{: false / 1 :}", "0");

    check_fail("{: \"aa\" / 1 :}", "can't division");
    check_fail("{: [] / 1 :}", "can't division");
    check_fail("{: {} / 1 :}", "can't division");
}

static void
test_trv_term_2(void) {
    check_ok("{: 4 % 2 :}", "0");
    check_ok("{: 3 % 2 :}", "1");
    check_ok("{: 1 % true :}", "0");

    check_fail("{: 1 % 0 :}", "zero division error");
    check_fail("{: 1 % false :}", "zero division error");
    check_fail("{: \"a\" % 1 :}", "invalid left hand operand (5)");
    check_fail("{: [] % 1 :}", "invalid left hand operand (6)");
    check_fail("{: {} % 1 :}", "invalid left hand operand (7)");

    check_fail("{: 1 % \"a\" :}", "invalid right hand operand (5)");
    check_fail("{: 1 % [] :}", "invalid right hand operand (6)");
    check_fail("{: 1 % {} :}", "invalid right hand operand (7)");

    check_fail("{: 4 % nil :}", "invalid right hand operand (0)");
    check_fail("{: nil % 2 :}", "invalid left hand operand (0)");
}

static void
test_trv_term_3(void) {
    check_ok("{: 2 * 2 / 4 % 2 :}", "1");
    check_ok("{: 4 / 2 * 2 % 2 :}", "0");
    check_ok("{: 3 % 2 * 3 / 3 :}", "1");
    check_ok("{: 3 * 2 / 3 * 3 :}", "6");
    check_ok("{: 4 / 2 * 2 / 2 :}", "2");
    check_ok("{: 3 % 2 * 2 % 2 :}", "0");
    check_ok("{: 3 * 2 % 2 * 2 :}", "0");
}

static void
test_trv_term_fail_0(void) {
    check_fail("{: * :}", "not found blocks");
    check_fail("{: 1 * :}", "syntax error. not found rhs operand in term");
    check_fail("{: * 1 :}", "operand is not a pointer");
    check_ok("{: 1 * true :}", "1");
    check_ok("{: false * 1 :}", "0");
    check_ok("{: 1 * \"a\" :}", "a");
    check_ok("{: \"a\" * 1 :}", "a");
    check_fail("{: 1 * [] :}", "can't mul with int");
    check_fail("{: [] * 1 :}", "can't mul");
    check_fail("{: 1 * {} :}", "can't mul with int");
    check_fail("{: {} * 1 :}", "can't mul");

    check_fail("{: / :}", "not found blocks");
    check_fail("{: 1 / :}", "syntax error. not found rhs operand in term");
    check_fail("{: / 1 :}", "not found blocks");
    check_ok("{: 1 / true :}", "1");
    check_ok("{: false / 1 :}", "0");
    check_fail("{: 1 / \"a\" :}", "invalid right hand operand");
    check_fail("{: \"a\" / 1 :}", "can't division");
    check_fail("{: 1 / [] :}", "invalid right hand operand");
    check_fail("{: [] / 1 :}", "can't division");
    check_fail("{: 1 / {} :}", "invalid right hand operand");
    check_fail("{: {} / 1 :}", "can't division");
}

static void
test_trv_call_0(void) {
    check_ok("{@ def f(): end f() @}", "");
}

static void
test_trv_call_1(void) {
    check_ok("{@\n"
"   def f(a):\n"
"       puts(a[0], a[1])\n"
"   end\n"
"   a = [1, 2]\n"
"   f(a)\n"
"@}", "1 2\n");
}

static void
test_trv_call_2(void) {
    check_ok("{@\n"
    "   def f(a):\n"
    "       puts(a[0], a[1])\n"
    "   end\n"
    "   for i = 0; i < 2; i += 1:\n"
    "       a = [i, i+1]\n"
    "       f(a)\n"
    "   end\n"
    "@}", "0 1\n1 2\n");
}

static void
test_trv_call_3(void) {
    check_ok("{@\n"
"   def f(a):\n"
"       puts(a[0], a[1], a[2])\n"
"   end\n"
"\n"
"   for i = 0; i < 2; i += 1:\n"
"       j = i\n"
"       a = [j, j+1, j+2]\n"
"       f(a)\n"
"   end\n"
"@}", "0 1 2\n1 2 3\n");
}

static void
test_trv_call_4(void) {
    check_ok("{@\n"
    "    for i = 0; i < 4; i +=1 :\n"
    "       j = i\n"
    "       a = [j, j+1, j+2]\n"
    "       puts(a)\n"
    "    end\n"
    "@}", "[0, 1, 2]\n[1, 2, 3]\n[2, 3, 4]\n[3, 4, 5]\n");

    check_ok("{@\n"
    "    from \"tests/lang/modules/list.aji\" import vecToUl\n"
    "\n"
    "    for i = 0; i < 4; i += 1:\n"
    "       j = i\n"
    "       a = [j, j+1, j+2]\n"
    "       vecToUl(a)\n"
    "    end\n"
    "@}",
        "<ul>\n"
        "    <li>0</li>\n"
        "    <li>1</li>\n"
        "    <li>2</li>\n"
        "</ul>\n"
        "<ul>\n"
        "    <li>1</li>\n"
        "    <li>2</li>\n"
        "    <li>3</li>\n"
        "</ul>\n"
        "<ul>\n"
        "    <li>2</li>\n"
        "    <li>3</li>\n"
        "    <li>4</li>\n"
        "</ul>\n"
        "<ul>\n"
        "    <li>3</li>\n"
        "    <li>4</li>\n"
        "    <li>5</li>\n"
        "</ul>\n"
    );
}

static void
test_trv_call_5(void) {
    check_ok("{@\n"
    "   def f1(a):\n"
    "       puts(a)\n"
    "       return a * 2\n"
    "   end\n"
    "   def f2(a):\n"
    "       return f1(a)\n"
    "   end\n"
    "   def f3(a):\n"
    "       return f2(a)\n"
    "   end\n"
    "   def f4(a):\n"
    "       return f3(a)\n"
    "   end\n"
    "@}{: f4(2) :}", "2\n4");
}

static void
test_trv_call_fail_0(void) {
    check_ok("{@\n"
    "def f(a):\n"
    "end\n"
    "f(1)\n"
    "@}"
    , "");

    check_fail("{@\n"
    "def f():\n"
    "end\n"
    "f(\n"
    "@}"
    , "not found ')'");

    check_fail("{@\n"
    "def f():\n"
    "end\n"
    "f)\n"
    "@}"
    , "not found blocks");

    check_fail("{@\n"
    "def f():\n"
    "end\n"
    "f(1)\n"
    "@}"
    , "invalid arguments length");

    check_fail("{@\n"
    "def f(a):\n"
    "end\n"
    "f(1, 2)\n"
    "@}"
    , "invalid arguments length");
}

static void
test_trv_call_fail_1(void) {
    check_fail("{@\n"
    "def f():\n"
    "end\n"
    "f(\n"
    "@}"
    , "not found ')'");
}


static void
test_trv_call_fail_2(void) {
    check_fail("{@\n"
    "def f():\n"
    "end\n"
    "f)\n"
    "@}"
    , "not found blocks");
}

static void
test_trv_call_fail_3(void) {
    check_fail("{@\n"
    "def f():\n"
    "end\n"
    "f(1)\n"
    "@}"
    , "invalid arguments length");
}

static void
test_trv_call_fail_4(void) {
    check_fail("{@\n"
    "def f(a):\n"
    "end\n"
    "f(1, 2)\n"
    "@}"
    , "invalid arguments length");
}

static void
test_trv_index_0(void) {
    check_ok("{@ a = [0, 1] @}{: a[0] :},{: a[1] :}", "0,1");
}

static void
test_trv_index_1(void) {
    check_ok("{@ a = [0, 1] @}{: a[0] :}", "0");
}

static void
test_trv_index_fail_0(void) {
    check_fail("{@ a = [0, 1] @}{: a[ :}", "not found expression");
    check_fail("{@ a = [0, 1] @}{: a] :}", "syntax error. not found \":}\"");
    check_fail("{@ a = [0, 1] @}{: a[] :}", "not found expression");
    check_fail("{@ a = [0, 1] @}{: a[\"a\"] :}", "index isn't integer");
    check_fail("{@ a = [0, 1] @}{: a[0][0] :}", "not indexable (1)");
}

static void
test_trv_vec_0(void) {
    check_ok("{@ a = [0, 1] @}", "");
}

static void
test_trv_vec_1(void) {
    check_ok("{@\n"
    "   i = 0\n"
    "   a = [i, 1]\n"
    "   a[0] += 1\n"
    "   puts(i)\n"
    "@}", "0\n");
}

static void
test_trv_vec_2(void) {
    check_ok("{@\n"
"   i = 0\n"
"   a = [i, 1]\n"
"   puts(i)\n"
"   puts(a[0])\n"
"   puts(id(i) == id(a[0]))"
"@}", "0\n0\ntrue\n");
}

static void
test_trv_vec_3(void) {
    
    check_ok("{@\n"
"   i = 0\n"
"   s = \"abc\"\n"
"   n = nil\n"
"   l = [0, 1, 2]\n"
"   d = {\"a\": 1, \"b\": 2}\n"
"   a = [i, s, n, l, d]\n"
"   puts(a[0], a[1], a[2], a[3][0], a[4][\"a\"])\n"
"   puts(id(i) == id(a[0]))\n"
"   puts(id(s) == id(a[1]))\n"
"   puts(id(n) == id(a[2]))\n"
"   puts(id(l) == id(a[3]))\n"
"   puts(id(d) == id(a[4]))\n"
"   l[0] = 3\n"
"   puts(l[0] == a[3][0])\n"
"   d[\"a\"] = 3\n"
"   puts(d[\"a\"] == a[4][\"a\"])\n"
"@}", "0 abc nil 0 1\ntrue\ntrue\ntrue\ntrue\ntrue\ntrue\ntrue\n");
}

static void
test_trv_vec_4(void) {
    
    check_ok("{@\n"
"   a = [\n"
"       1,\n"
"       2, 3,\n"
"       4,\n"
"   ]\n"
"@}{: a[0] :}{: a[1] :}{: a[2] :}{: a[3] :}", "1234");
}

static void
test_trv_vec_5(void) {
    check_ok("{@ a = [] @}{: a :}", "[]");
    check_ok("{@ a = [1] @}{: a :}", "[1]");
    check_ok("{@ a = [1] \n b = a @}{: a :},{: b :}", "[1],[1]");
    check_ok("{@ a = [1, 2] @}{: a :}", "[1, 2]");
    check_ok("{@ [b = 1] @}", "");
    check_ok("{@ a = [b = 1, c = 2] @}{: a :}", "[1, 2]");
    check_ok("{@ a = [1, b = 2] @}{: a :}", "[1, 2]");
}

static void
test_trv_vec_6(void) {
    check_ok("{@ a = [] @}{: a :}", "[]");
}

static void
test_trv_vec_7(void) {
    check_ok("{@\n"
    "a = [[0, 1], [2, 3]]\n"
    "@}{: a[0][0] :},{: a[0][1] :},{: a[1][0] :},{: a[1][1] :}"
    , "0,1,2,3");
}

static void
test_trv_vec_8(void) {
    check_ok("{@\n"
    "struct A:\n"
    "end\n"
    "A()\n"
    "@}", "");

    check_ok("{@\n"
    "struct A:\n"
    "end\n"
    "[A()]\n"
    "@}", "");

    check_ok("{@\n"
    "struct A:\n"
    "end\n"
    "a = [A()]\n"
    "@}", "");

    check_ok("{@\n"
    "struct A:\n"
    "end\n"
    "a = [A(), A()]\n"
    "@}{: a[0] :},{: a[1] :}", "(object),(object)");
}

static void
test_trv_vec_9(void) {
    check_ok("{@\n"
    "struct A:\n"
    "end\n"
    "a = [A(), A()]\n"
    "@}{: a[0] :},{: a[1] :}", "(object),(object)");
}

static void
test_trv_vec_10(void) {
    check_ok("{@\n"
    "struct A:\n"
    "end\n"
    "def f():\n"
    "end\n"
    "a = [A(), f]\n"
    "@}{: a[0] :},{: a[1] :}", "(object),(function)");
}

static void
test_trv_vec_11(void) {
    check_ok("{@\n"
    "d = {\"a\": 1, \"b\": 2}\n"
    "a = [d, d]\n"
    "@}{: a[0] :},{: a[1] :}", "{\"a\": 1, \"b\": 2},{\"a\": 1, \"b\": 2}");
}

static void
test_trv_vec_12(void) {
    check_ok("{@\n"
    "d = {\"a\": 1, \"b\": 2}\n"
    "a = [d, d]\n"
    "@}{: a[0][\"a\"] :},{: a[1][\"b\"] :}"
    , "1,2");
}

static void
test_trv_vec_13(void) {
    check_ok("{@\n"
    "d = {\"a\": 1, \"b\": 2}\n"
    "b = [2, d]\n"
    "a = [1, b, 3]\n"
    "@}{: a[1][1][\"b\"] :}", "2");
}

static void
test_trv_vec_14(void) {
    check_ok("{@\n"
    "a = Vec()\n"
    "@}{: len(a) :}", "0");
}

static void
test_trv_vec_fail_0(void) {
    check_fail("{@ a = [ @}", "not found ']' in vec");
    check_fail("{@ a = ] @}", "syntax error. not found rhs test in assign list");
    check_fail("{@ a = [,] @}", "not found ']' in vec");
    check_fail("{@ a = [,,] @}", "not found ']' in vec");
    check_ok("{@ a = [1,] @}", "");
    check_ok("{@ a = [1,2,] @}", "");
    check_fail("{@ a = [,1] @}", "not found ']' in vec");
}

static void
test_trv_nil(void) {
    check_ok("{: nil :}", "nil");
}

static void
test_trv_false(void) {
    check_ok("{: false :}", "false");
}

static void
test_trv_true(void) {
    check_ok("{: true :}", "true");
}

static void
test_trv_digit(void) {
    check_ok("{: 1 :}", "1");                    
}

static void
test_trv_string(void) {
    check_ok("{: \"abc\" :}", "abc");
    check_ok("{@ aaa = 1 @}{: \"aaa $aaa bbb\" :}", "aaa 1 bbb");
    check_ok("{@ aaa = 1 @}{: \"aaa $aaabbb\" :}", "aaa $aaabbb");
    check_ok("{@ aaa = \"AAA\" @}{: \"$aaa aaa bbb\" :}", "AAA aaa bbb");
    check_ok("{: \"aaa $$ bbb\" :}", "aaa $$ bbb");
    check_ok("{@ aaa = 1 @}{: \"aaa $$aaa bbb\" :}", "aaa $1 bbb");
    check_ok("{@ a = 1 b = 2 @}{: \"aaa $a bbb $b\" :}", "aaa 1 bbb 2");
    check_ok("{@ aaa = 1 @}{: \"aaa/$aaa/bbb\" :}", "aaa/1/bbb");
}

static void
test_trv_dict_0(void) {
    check_ok("{@ d = {\"a\":1, \"b\":2} @}", "");
}

static void
test_trv_dict_1(void) {
    check_ok("{@\n"
"   d = {\n"
"       \"a\" \n : \n 1 \n, \n"
"       \"b\" \n : \n 2 \n, \n"
"   }\n"
"@}{: d[\"a\"] :}{: d[\"b\"] :}", "12");
}

static void
test_trv_dict_2(void) {
    /*******
    * fail *
    *******/

    check_fail("{@\n"
"   d = {\"a\": 1}\n"
"@}{: d[\"b\"] :}", "not found key \"b\"");
}

static void
test_trv_dict_3(void) {
    /*******
    * fail *
    *******/

    check_fail("{@ a = { 1: 1 } @}", "key is not string in dict elem");
    check_fail("{@ a = { \"k\": 1 } \n a[0] @}", "index isn't string");
    check_fail("{@ k = 1 \n a = { k: 1 } @}", "invalid key");

    /*****
    * ok *
    *****/

    check_ok("{@ a = { \"key\": 1 } @}{: a :}", "{\"key\": 1}");
    check_ok("{@ a = { \"key\": 1 } @}{: a[\"key\"] :}", "1");
    check_ok("{@ a = { \"key\": \"val\" } @}{: a[\"key\"] :}", "val");
    check_ok("{@ a = { \"key\": [1, 2] } @}{: a[\"key\"] :}", "[1, 2]");
    check_ok("{@ a = { \"k1\": 1, \"k2\": 2 } @}{: a[\"k1\"] :},{: a[\"k2\"] :}", "1,2");
    check_ok("{@ a = { \"k1\": { \"k2\": 1 } } @}{: a[\"k1\"][\"k2\"] :}", "1");
    check_ok("{@ k = \"key\" \n a = { k: 1 } @}{: a[k] :}", "1");
    // check_ok("{@ a = { \"key\": 1 }[\"key\"] @}{: a :}", "1");
}

static void
test_trv_dict_4(void) {
    check_ok("{@ d = {\"a\": 1} d[\"a\"] @}", "");
    check_ok("{@ d = {\"a\": 1} d.pop(\"a\") @}", "");
    check_ok("{@ d = {\"a\": 1} @}{: d.pop(\"a\") :}", "1");
    check_fail("{@ d = {\"a\": 1} @}{: d.pop(\"b\") :}", "invalid key");
    check_ok("{@ d = {\"a\": 1} @}{: d.pop(\"b\", 2) :}", "2");
}

static void
test_trv_dict_5(void) {
    check_ok("{@ d = { aaa: 1 } @}{: d.aaa :}", "1");
    check_ok("{@ d = { aaa: 1, \"bbb\": 2 } @}{: d.aaa :},{: d[\"bbb\"] :}", "1,2");
    check_ok("{@ d = { aaa: 1 } puts(d.aaa) @}", "1\n");
    check_ok("{@ d = { aaa: 1 } d.aaa = 2 @}{: d.aaa :}", "2");
    check_ok("{@ d = { aaa: 1 } @}{: d.aaa + d.aaa :}", "2");
    check_ok("{@ aaa = \"bbb\" d = { aaa: 1 } @}{: d.bbb :}", "1");
}

static void
test_trv_dict_fail_0(void) {
    check_fail("{@ d = { @}", "not found right brace in parse dict");
    check_fail("{@ {1} @}", "not found colon in parse dict elem");
    check_fail("{@ {1:} @}", "not found value in parse dict elem");
    check_fail("{@ {1:2} @}", "key is not string in dict elem");
    check_fail("{@ {\"a\": 1, 2} @}", "not found colon in parse dict elem");
    check_ok("{@ {\"a\": 1, } @}", "");
    check_fail("{@ {\"a\": 1, \"b\"} @}", "not found colon in parse dict elem");
    check_fail("{@ {\"a\": 1, \"b\":} @}", "not found value in parse dict elem");
    check_fail("{@ {\"a\": b} @}", "\"b\" is not defined. can not store to dict elements");
    check_fail("{@ {[]} @}", "not found colon in parse dict elem");
    check_fail("{@ {{}} @}", "not found colon in parse dict elem");
}

static void
test_trv_identifier(void) {
    check_ok("{@ i = 1 @}{: i :}", "1");                    
}

static void
test_trv_builtin_vec_0(void) {
    check_ok("{@ vec = [1, 2] \n vec.push(3) @}{: len(vec) :}", "3");
    check_ok("{: len([1, 2].push(3)) :}", "3");
    check_ok("{@ a = [1, 2] @}{: a.pop() :}", "2");
    check_ok("{@ a = [] @}{: a.pop() :}", "nil");
    check_ok("{: [1, 2].pop() :}", "2");
}

static void
test_trv_builtin_dict_0(void) {
    check_fail("{@ d = {\"a\": 1} @}{: d.get(1) :}", "key is not found");
    check_ok("{@ d = {\"a\": 1} @}{: d.get(\"a\") :}", "1");
    check_ok("{@ d = {\"a\": 1} @}{: d.get(\"b\") :}", "nil");
    check_ok("{@ d = {\"a\": 1} @}{: d.get(\"b\", 2) :}", "2");
}

static void
test_trv_module_0(void) {
    check_ok("{@\n"
    "   import \"tests/lang/modules/module.aji\" as mod\n"
    "   puts(\"done\")\n"
    "@}", "imported\nimported module.aji\ndone\n");
}

static void
test_trv_chain_object(void) {
    /*****
    * ok *
    *****/

    check_ok("{@\n"
"   import \"tests/lang/modules/string.aji\" as string\n"
"   string.a = 1\n"
"@}{: string.a :}", "1");

    /*******
    * fail *
    *******/

    check_fail("{@\n"
"   import \"tests/lang/modules/string.aji\" as string\n"
"   string.a\n"
"@}", "\"a\" is not defined");
    check_fail("{@\n"
"   import \"tests/lang/modules/string.aji\" as string\n"
"@}{: string.a :}", "\"a\" is not defined");
    check_fail("{@\n"
"   import \"tests/lang/modules/string.aji\" as string\n"
"   string.a.b = 1\n"
"@}", "\"a\" is not defined");
    check_fail("{@\n"
"   import \"tests/lang/modules/string.aji\" as string\n"
"   string.a.b = 1\n"
"@}{: string.a :}", "\"a\" is not defined");
    check_fail("{@\n"
"   import \"tests/lang/modules/string.aji\" as string\n"
"   string.a.b = 1\n"
"@}{: string.a.b :}", "\"a\" is not defined");
    check_fail("{@\n"
"   import \"tests/lang/modules/string.aji\" as string\n"
"@}{: string.a.b :}", "\"a\" is not defined");
}

static void
test_trv_etc_0(void) {
    check_ok("{@\n"
"   def decolate(s):\n"
"       return \"***\" + s + \"***\"\n"
"   end\n"
"   s = decolate(\"i love life\")\n"
"   puts(s)\n"
"   for i = 0; i < len(s); i += 1:\n"
"       puts(s[i])\n"
"   end\n"
"@}", "***i love life***\n*\n*\n*\ni\n \nl\no\nv\ne\n \nl\ni\nf\ne\n*\n*\n*\n");
}

static void
test_trv_etc_1(void) {
    check_ok("{@\n"
"   def slice(vec, n):\n"
"       mat = []\n"
"       for i = 0; i < len(vec); i += n:\n"
"           row = []\n"
"           for j = 0; j < n and i + j < len(vec); j += 1:\n"
"               row.push(vec[i+j])\n"
"           end\n"
"           mat.push(row)\n"
"       end\n"
"       return mat\n"
"   end\n"
"   vec = [1, 2, 3, 4]\n"
"   mat = slice(vec, 2)\n"
"   for i = 0; i < len(mat); i += 1:\n"
"       row = mat[i]\n"
"       for j = 0; j < len(row); j += 1:\n"
"           puts(row[j])\n"
"       end\n"
"       puts(\",\")\n"
"   end\n"
"@}", "1\n2\n,\n3\n4\n,\n");
}

static void
test_trv_etc_2(void) {
    check_ok("{@\n"
"   def header(title):\n"
"@}<html>\n"
"<head>\n"
"<title>{: title :}</title>\n"
"</head>\n"
"{@\n"
"   end\n"
"\n"
"   def body(title, message):\n"
"@}<body>\n"
"<h1>{: title :}</h1>\n"
"<p>{: message :}</p>\n"
"</body>\n"
"{@\n"
"   end\n"
"\n"
"   def footer():\n"
"@}</html>\n"
"{@\n"
"   end\n"
"\n"
"   def index(kwargs):\n"
"       title = kwargs[\"title\"]\n"
"       message = kwargs[\"message\"]\n"
"       header(title)\n"
"       body(title, message)\n"
"       footer()\n"
"   end\n"
"\n"
"   index({\n"
"       \"title\": \"Good will hunting\",\n"
"       \"message\": \"I'm a robot\",\n"
"   })\n"
"@}", "<html>\n"
"<head>\n"
"<title>Good will hunting</title>\n"
"</head>\n"
"<body>\n"
"<h1>Good will hunting</h1>\n"
"<p>I'm a robot</p>\n"
"</body>\n"
"</html>\n");
}

static void
test_trv_etc_3(void) {
    check_ok("{@\n"
"   d = {\"a\": 1}"
"   a = [d, 2]\n"
"   a[0][\"a\"] += 1\n"
"@}{: a[0][\"a\"] :},{: d[\"a\"] :}", "2,2");
}

static void
test_trv_etc_4(void) {
    /***************************
    * theme: dict and function *
    ***************************/

    check_ok("{@\n"
"   def f(a):\n"
"       return a\n"
"   end\n"
"   a = f(f)\n"
"@}{: id(a) == id(f) :}", "true");
    check_ok("{@\n"
"   def f(arg):\n"
"       return arg[\"a\"]\n"
"   end\n"
"   d = {\"a\": f}"
"   d[\"a\"](d)\n"
"@}", "");

    // OK
    
    check_ok("{@\n"
    "def f(arg):\n"
    "   return arg\n"
    "end\n"
    "d = {\"a\": f}\n"
    "d[\"a\"](d)\n"
    "@}", "");

    // OK
    
    check_ok("{@\n"
    "def f():\n"
    "   d = {\"a\": f}\n"
    "   return d[\"a\"]\n"
    "end\n"
    "f()\n"
    "@}", "");

    // OK
    
    check_ok("{@\n"
    "def f(arg):\n"
    "   return arg[\"a\"]\n"
    "end\n"
    "d = {\"a\": f}\n"
    "f(d)\n"
    "@}", "");

    // OK

    check_ok("{@\n"
    "d = {\"a\": 1}\n"
    "def f():\n"
    "   return d[\"a\"]\n"
    "end\n"
    "f()\n"
    "@}", "");

    check_ok("{@\n"
    "def f(arg):\n"
    "   return arg[\"a\"]\n"
    "end\n"
    "d = {\"a\": f}\n"
    "d[\"a\"](d)\n"
    "@}", "");

    check_ok("{@\n"
"   def f(arg):\n"
"       return arg[\"a\"]\n"
"   end\n"
"   d = {\"a\": f}\n"
"   a = d[\"a\"](d)"
"@}{: a :},{: id(a) == id(f) :}", "(function),true");
}

static void
test_trv_etc_5(void) {

    /***************************
    * theme: dict and function *
    ***************************/

    check_ok("{@\n"
"   def f(arg):\n"
"       return arg[\"a\"] + arg[\"b\"]\n"
"   end\n"
"   a = 1, b = 2\n"
"   c = f({ \"a\" : a, \"b\": b })"
"@}{: c :}", "3");
    check_ok("{@\n"
"   def f(arg):\n"
"       return arg[\"d\"][\"a\"] + arg[\"c\"][\"b\"]\n"
"   end\n"
"   d = { \"a\": 1, \"b\": 2 }\n"
"   c = f({ \"d\" : d, \"c\": d })\n"
"@}{: c :}", "3");
    check_ok("{@\n"
"   def f(arg):\n"
"       arg[\"d\"][\"a\"] += 1\n"
"       arg[\"c\"][\"b\"] += 1\n"
"   end\n"
"   d = { \"a\": 1, \"b\": 2 }\n"
"   c = f({ \"d\" : d, \"c\": d })\n"
"@}{: d[\"a\"] :},{: d[\"b\"] :}", "2,3");
    check_ok("{@\n"
"   def f(arg):\n"
"       e = arg[\"d\"]\n"
"       e[\"a\"] += 1\n"
"   end\n"
"   d = { \"a\": 1 }\n"
"   c = f({ \"d\" : d })\n"
"@}{: d[\"a\"] :}", "2");
    check_ok("{@\n"
"   def f(arg):\n"
"       e = arg[\"d\"]\n"
"       e[\"a\"] += 1\n"
"   end\n"
"   d = { \"a\": 1 }\n"
"   c = f({ \"d\" : d })\n"
"@}{: d[\"a\"] :}", "2");
    check_ok("{@\n"
"   def f(arg):\n"
"       d = arg[\"d\"]\n"
"       l = d[\"a\"]\n"
"       l[0] += 1"
"   end\n"
"   l = [1, 2]\n"
"   d = { \"a\": l }\n"
"   c = f({ \"d\" : d })\n"
"@}{: l[0] :},{: l[1] :}", "2,2");
    check_ok("{@\n"
"   def f(arg):\n"
"       d = arg[\"d\"]\n"
"       l = d[\"a\"]\n"
"       return { \"a\": l[0]+1, \"b\": l[1]+1 }\n"
"   end\n"
"   l = [1, 2]\n"
"   d = { \"a\": l }\n"
"   c = f({ \"d\" : d })\n"
"@}{: c[\"a\"] :},{: c[\"b\"] :}", "2,3");
}

static void
test_trv_etc_6(void) {
    /***************************
    * theme: list and function *
    ***************************/

    check_ok("{@\n"
"   def f(l):\n"
"       l.push(3)\n"
"   end\n"
"   l = [1, 2]\n"
"   f(l)\n"
"@}{: l[2] :}", "3");
    check_ok("{@\n"
"   def f(l):\n"
"       l.push([3, 4])\n"
"       return l"
"   end\n"
"   l = [1, 2]\n"
"   l2 = f(l)\n"
"@}{: l2[2][0] :}", "3");
    check_ok("{@\n"
"   g = 3"
"   def f(l):\n"
"       l.push(g)\n"
"   end\n"
"   l = [1, 2]\n"
"   f(l)\n"
"@}{: l[2] :},{: id(l[2]) != id(g) :}", "3,true");
}

static void
test_trv_etc_7(void) {
    /*******************************
    * theme: for and if statements *
    *******************************/

    check_ok("{@\n"
"   for i = 0; i < 4; i += 1:\n"
"       if i % 2 == 0:\n"
"           puts(\"nyan\")\n"
"       end\n"
"   end\n"
"@}", "nyan\nnyan\n");
    check_ok("{@\n"
"   for i = 0; i < 4; i += 1:\n"
"       if i % 2 == 0:\n"
"           for j = 0; j < 2; j += 1:\n"
"               puts(j)"
"           end\n"
"       end\n"
"   end\n"
"@}", "0\n1\n0\n1\n");
    check_ok("{@\n"
"   m = 2\n"
"   if m == 2:\n"
"       for i = 0; i < 2; i += 1:\n"
"           puts(i)\n"
"       end\n"
"   end\n"
"@}", "0\n1\n");
    check_ok("{@\n"
"   m = 3\n"
"   if m == 2:\n"
"   elif m == 3:\n"
"       for i = 0; i < 2; i += 1:\n"
"           puts(i)\n"
"       end\n"
"   end\n"
"@}", "0\n1\n");
    check_ok("{@\n"
"   m = 4\n"
"   if m == 2:\n"
"   elif m == 3:\n"
"   else:\n"
"       for i = 0; i < 2; i += 1:\n"
"           puts(i)\n"
"       end\n"
"   end\n"
"@}", "0\n1\n");
}

static void
test_trv_etc_8(void) {
    /**************************
    * theme: function and for *
    **************************/

    check_ok("{@\n"
"   def f(n):\n"
"       for i = 0; i < n; i += 1:\n"
"           puts(i)\n"
"       end\n"
"   end\n"
"   \n"
"   for i = 0; i < 2; i += 1:\n"
"       f(i+1)\n"
"   end\n"
"@}", "0\n0\n1\n");
    check_ok("{@\n"
"   for i = 0; i < 2; i += 1:\n"
"       def f(n):\n"
"           for i = 0; i < n; i += 1:\n"
"               puts(i)\n"
"           end\n"
"       end\n"
"       f(i+1)\n"
"   end\n"
"@}", "0\n0\n1\n");
}

static void
test_trv_etc_9(void) {
    const char *s = "{@\n"
"vec = [3, 2, 4, 1]\n"
"for j = 0; j < 4; j += 1:\n"
"   for i = 0; i < len(vec) - 1; i += 1:\n"
"       if vec[i] > vec[i + 1]:\n"
"           tmp = vec[i]\n"
"           vec[i] = vec[i + 1]\n"
"           vec[i + 1] = tmp\n"
"       end\n"
"   end\n"
"end\n"
"puts(vec[0], vec[1], vec[2], vec[3])\n"
"@}";

    check_ok(s, "1 2 3 4\n");
}

static void
test_trv_etc_10(void) {
        const char *s = "{@\n"
"vec = [4, 1, 2, 3]\n"
"i = 0\n"
"tmp = vec[i]\n"
"vec[i] = vec[i + 1]\n"
"vec[i + 1] = tmp\n"
"i = 1\n"
"tmp = vec[i]\n"
"vec[i] = vec[i + 1]\n"
"vec[i + 1] = tmp\n"
"i = 2\n"
"tmp = vec[i]\n"
"vec[i] = vec[i + 1]\n"
"vec[i + 1] = tmp\n"
"i = 0\n"
"tmp = vec[i]\n"
"vec[i] = vec[i + 1]\n"
"vec[i + 1] = tmp\n"
"puts(vec[0], vec[1], vec[2], vec[3])\n"
"@}";

    check_ok(s, "2 1 3 4\n");
}

static void
test_trv_unicode_0(void) {
    check_ok("{@\n"
"   s = \"abc\""
"@}{: s[0] :}", "a");
}

static void
test_trv_unicode_1(void) {
    check_ok("{@\n"
    "   s = \"あいう\""
    "@}{: s[0] :}{: s[2] :}", "あう");
    }

static void
test_trv_scope_0(void) {
    check_ok("{@\n"
    "a = 0\n"
    "if true:\n"
    "   a = 1\n"
    "end\n"
    "@}{: a :}", "1");
    }

static void
test_trv_scope_1(void) {
    check_ok("{@\n"
    "a = 0\n"
    "def f():\n"
    "   a = 1\n"
    "end\n"
    "f()\n"
    "@}{: a :}", "0");
    }

static void
test_trv_scope_2(void) {
    check_ok("{@\n"
    "a = 0\n"
    "def f():\n"
    "   a = 1\n"
    "end\n"
    "f()\n"
    "@}{: a :}", "0");
    }

static void
test_trv_scope_3(void) {
    check_ok("{@\n"
    "a = 0\n"
    "def f():\n"
    "   puts(a)\n"
    "end\n"
    "f()\n"
    "@}", "0\n");
    }

static void
test_trv_scope_4(void) {    return;

    // TODO: change fail to ok
    // need 'nonlocal' stmt
    // this error happen too in python
    check_fail("{@\n"
    "def f(a):\n"
    "   def inner():\n"
    "       return a\n"
    "   end\n"
    "   return inner\n"
    "end\n"
    "c = f(1)\n"
    "@}{: c() :}", "\"a\" is not defined");
}

static void
test_trv_scope_5(void) {
    check_ok("{@\n"
    "a = 0\n"
    "def f():\n"
    "   return a\n"
    "end\n"
    "@}{: f() :}", "0");
    }

static void
test_trv_scope_6(void) {
    check_ok("{@\n"
    "a = 0\n"
    "def f():\n"
    "   return a + 1\n"
    "end\n"
    "@}{: f() :}", "1");

    check_ok("{@\n"
    "a = 0\n"
    "def f():\n"
    "end\n"
    "@}{: f() :}", "nil");
}

static void
test_trv_scope_7(void) {
        check_fail("{@\n"
    "a += 1\n"
    "@}", "\"a\" is not defined");

    check_fail("{@\n"
    "a = 0\n"
    "def f():\n"
    "   a += 1\n"
    "   return a\n"
    "end\n"
    "@}{: f() :}", "\"a\" is not defined");

    check_fail("{@\n"
    "def f():\n"
    "   a += 1\n"
    "end\n"
    "f() @}", "\"a\" is not defined");

    check_fail("{@\n"
    "def f():\n"
    "   return a\n"
    "end\n"
    "@}{: f() :}", "\"a\" is not defined");
}

static void 
test_trv_mutable_and_immutable(void) {
        /* Aji's mutable and immutable of object
     *
     * Mutable:
     *
     *   vec
     *   dict
     *
     * Immutable:
     *
     *   int
     *   float
     *   string
     * 
     * But int and float of function arguments copied and pass
     */
    // check_fail("{@ 0 1+= 1 @}", "invalid left hand operand (1)");

    // check_ok("{@ i = 0 iid = id(i) i += 1 @}{: iid != id(i) :},{: i :}", "true,1");
    // check_ok("{@ i = 0.12 iid = id(i) i += 0.01 @}{: iid != id(i) :},{: i :}", "true,0.13");
    // check_ok("{@ i = \"aaa\" iid = id(i) i += \"bbb\" @}{: iid != id(i) :},{: i :}", "true,aaabbb");
    // check_ok_trace("{@ def f(a): a += 1 end \n i = 0 \n f(i) @}{: i :}", "0")
    // check_ok_trace("{@ def f(a): a += 0.1 end \n i = 0.0 \n f(i) @}{: i :}", "0.0")
    // check_ok_trace("{@ def f(a): a += \"b\" end \n i = \"a\" \n f(i) @}{: i :}", "a")
    // check_ok_trace("{@ a = {\"b\": 0} aid = id(a[\"b\"]) a[\"b\"] += 1 @}{: aid != id(a[\"b\"]) :},{: a[\"b\"] :}", "true,1")

    check_ok("{@ a = [100] aid = id(a[0]) a[0] += 1 @}{: aid != id(a[0]) :},{: a[0] :}", "true,101")
    check_ok("{@ a = [100] aid = id(a[0]) a[0] += 1 @}", "")
    check_ok("{@ a = [100] @}{: a[0] :}", "100")
    check_ok("{@ a = [] @}", "")}

static void
test_trv_blt_funcs(void) {
    check_ok("{: dance :}", "(builtin-function)");
    check_ok("{@ puts(1) @}", "1\n");
    check_ok("{@ puts() @}", "\n");
    check_ok("{@ puts(dance) @}", "(builtin-function)\n");
}

static const struct testcase
traverser_1_tests[] = {
    {"blt_funcs", test_trv_blt_funcs},
    {"scope_0", test_trv_scope_0},
    {"scope_1", test_trv_scope_1},
    {"scope_2", test_trv_scope_2},
    {"scope_3", test_trv_scope_3},
    {"scope_4", test_trv_scope_4},
    {"scope_5", test_trv_scope_5},
    {"scope_6", test_trv_scope_6},
    {"scope_7", test_trv_scope_7},
    {"mutable_and_immutable", test_trv_mutable_and_immutable},
    {"assign_and_reference_0", test_trv_assign_and_reference_0},
    {"assign_and_reference_1", test_trv_assign_and_reference_1},
    {"assign_and_reference_2", test_trv_assign_and_reference_2},
    {"assign_and_reference_3", test_trv_assign_and_reference_3},
    {"assign_and_reference_4", test_trv_assign_and_reference_4},
    {"assign_and_reference_5", test_trv_assign_and_reference_5},
    {"assign_and_reference_6", test_trv_assign_and_reference_6},
    {"assign_and_reference_7", test_trv_assign_and_reference_7},
    {"assign_and_reference_8", test_trv_assign_and_reference_8},
    {"assign_and_reference_9", test_trv_assign_and_reference_9},
    {"assign_and_reference_10", test_trv_assign_and_reference_10},
    {"assign_and_reference_11", test_trv_assign_and_reference_11},
    {"assign_and_reference_11_5", test_trv_assign_and_reference_11_5},
    {"assign_and_reference_11_6", test_trv_assign_and_reference_11_6},
    {"assign_and_reference_11_7", test_trv_assign_and_reference_11_7},
    {"assign_and_reference_12", test_trv_assign_and_reference_12},
    {"assign_and_reference_13", test_trv_assign_and_reference_13},
    {"assign_and_reference_14", test_trv_assign_and_reference_14},
    {"assign_and_reference_15", test_trv_assign_and_reference_15},
    {"assign_and_reference_16", test_trv_assign_and_reference_16},
    {"assign_and_reference_all", test_trv_assign_and_reference_all},
    {"code_block", test_trv_code_block},
    {"code_block_fail", test_trv_code_block_fail},
    {"ref_block", test_trv_ref_block},
    {"ref_block_fail", test_trv_ref_block_fail},
    {"text_block", test_trv_text_block},
    {"type_0", test_trv_type_0},
    {"type_1", test_trv_type_1},
    {"type_2", test_trv_type_2},
    {"type_3", test_trv_type_3},
    {"type_4", test_trv_type_4},
    {"type_5", test_trv_type_5},
    {"assign_list_0", test_trv_assign_list_0},
    {"assign_list_1", test_trv_assign_list_1},
    {"assign_list_2", test_trv_assign_list_2},
    {"assign_list_3", test_trv_assign_list_3},
    {"assign_list_fail_0", test_trv_assign_list_fail_0},
    {"multi_assign_0", test_trv_multi_assign_0},
    {"multi_assign_1", test_trv_multi_assign_1},
    {"multi_assign_2", test_trv_multi_assign_2},
    {"multi_assign_fail_0", test_trv_multi_assign_fail_0},
    {"or_test_0", test_trv_or_test_0},
    {"or_test_fail_0", test_trv_or_test_fail_0},
    {"and_test_0", test_trv_and_test_0},
    {"and_test_fail_0", test_trv_and_test_fail_0},
    {"not_test_0", test_trv_not_test_0},
    {"not_test_fail_0", test_trv_not_test_fail_0},
    {"asscalc_0", test_trv_asscalc_0},
    {"asscalc_1", test_trv_asscalc_1},
    {"asscalc_2", test_trv_asscalc_2},
    {"asscalc_3", test_trv_asscalc_3},
    {"asscalc_4", test_trv_asscalc_4},
    {"asscalc_5", test_trv_asscalc_5},
    {"asscalc_6", test_trv_asscalc_6},
    {"asscalc_7", test_trv_asscalc_7},
    {"asscalc_8", test_trv_asscalc_8},
    {"asscalc_9", test_trv_asscalc_9},
    {"asscalc_10", test_trv_asscalc_10},
    {"asscalc_11", test_trv_asscalc_11},
    {"asscalc_12", test_trv_asscalc_12},
    {"asscalc_13", test_trv_asscalc_13},
    {"asscalc_14", test_trv_asscalc_14},
    {"asscalc_15", test_trv_asscalc_15},
    {"asscalc_16", test_trv_asscalc_16},
    {"asscalc_17", test_trv_asscalc_17},
    {"asscalc_18", test_trv_asscalc_18},
    {"asscalc_19", test_trv_asscalc_19},
    {"asscalc_20", test_trv_asscalc_20},
    {"asscalc_21", test_trv_asscalc_21},
    {"asscalc_22", test_trv_asscalc_22},
    {"asscalc_23", test_trv_asscalc_23},
    {"asscalc_24", test_trv_asscalc_24},
    {"asscalc_25", test_trv_asscalc_25},
    {"asscalc_add_ass_string", test_trv_asscalc_add_ass_string},
    {"asscalc_fail_0", test_trv_asscalc_fail_0},
    {"expr_0", test_trv_expr_0},
    {"expr_1", test_trv_expr_1},
    {"expr_2", test_trv_expr_2},
    {"expr_3", test_trv_expr_3},
    {"expr_4", test_trv_expr_4},
    {"expr_4a", test_trv_expr_4a},
    {"expr_5", test_trv_expr_5},
    {"expr_6", test_trv_expr_6},
    {"expr_7", test_trv_expr_7},
    {"expr_8", test_trv_expr_8},
    {"expr_9", test_trv_expr_9},
    {"expr_float", test_trv_expr_float},
    {"expr_string_0", test_trv_expr_string_0},
    {"expr_fail_0", test_trv_expr_fail_0},
    {"term_0", test_trv_term_0},
    {"term_1", test_trv_term_1},
    {"term_2", test_trv_term_2},
    {"term_3", test_trv_term_3},
    {"term_fail_0", test_trv_term_fail_0},
    {"call_0", test_trv_call_0},
    {"call_1", test_trv_call_1},
    {"call_2", test_trv_call_2},
    {"call_3", test_trv_call_3},
    {"call_4", test_trv_call_4},
    {"call_5", test_trv_call_5},
    {"call_fail_0", test_trv_call_fail_0},
    {"call_fail_1", test_trv_call_fail_1},
    {"call_fail_2", test_trv_call_fail_2},
    {"call_fail_3", test_trv_call_fail_3},
    {"call_fail_4", test_trv_call_fail_4},
    {"index_0", test_trv_index_0},
    {"index_1", test_trv_index_1},
    {"index_fail_0", test_trv_index_fail_0},
    {0},
};

/**************
* traverser_2 *
**************/

static const struct testcase
traverser_2_tests[] = {
    {"vec_0", test_trv_vec_0},
    {"vec_1", test_trv_vec_1},
    {"vec_2", test_trv_vec_2},
    {"vec_3", test_trv_vec_3},
    {"vec_4", test_trv_vec_4},
    {"vec_5", test_trv_vec_5},
    {"vec_6", test_trv_vec_6},
    {"vec_7", test_trv_vec_7},
    {"vec_8", test_trv_vec_8},
    {"vec_9", test_trv_vec_9},
    {"vec_10", test_trv_vec_10},
    {"vec_11", test_trv_vec_11},
    {"vec_12", test_trv_vec_12},
    {"vec_13", test_trv_vec_13},
    {"vec_fail_0", test_trv_vec_fail_0},
    {"nil", test_trv_nil},
    {"false", test_trv_false},
    {"true", test_trv_true},
    {"digit", test_trv_digit},
    {"string", test_trv_string},
    {"dict_0", test_trv_dict_0},
    {"dict_1", test_trv_dict_1},
    {"dict_2", test_trv_dict_2},
    {"dict_3", test_trv_dict_3},
    {"dict_4", test_trv_dict_4},
    {"dict_5", test_trv_dict_5},
    {"dict_fail_0", test_trv_dict_fail_0},
    {"identifier", test_trv_identifier},
    {"traverse", test_travese},
    {"long_code", test_trv_long_code},
    {"comparison", test_trv_comparison},
    {"comparison_0", test_trv_comparison_0},
    {"comparison_1", test_trv_comparison_1},
    {"comparison_2", test_trv_comparison_2},
    {"comparison_3", test_trv_comparison_3},
    {"comparison_4", test_trv_comparison_4},
    {"comparison_5", test_trv_comparison_5},
    {"comparison_6", test_trv_comparison_6},
    {"comparison_7", test_trv_comparison_7},
    {"vec_index", test_trv_vec_index},
    {"text_block_old", test_trv_text_block_old},
    {"ref_block_old", test_trv_ref_block_old},
    {"assign_0", test_trv_assign_0},
    {"assign_1", test_trv_assign_1},
    {"assign_2", test_trv_assign_2},
    {"assign_3", test_trv_assign_3},
    {"assign_4", test_trv_assign_4},
    {"assign_5", test_trv_assign_5},
    {"assign_fail_0", test_trv_assign_fail_0},
    {"atom_0", test_trv_atom_0},
    {"index", test_trv_index},
    {"string_index", test_trv_string_index},
    {"multi_assign", test_trv_multi_assign},
    {"and_test", test_trv_and_test},
    {"assign_list", test_trv_assign_list},
    {"test_list", test_trv_test_list},
    {"dot_0", test_trv_dot_0},
    {"dot_1", test_trv_dot_1},
    {"dot_2", test_trv_dot_2},
    {"dot_3", test_trv_dot_3},
    {"dot_4", test_trv_dot_4},
    {"dot_5", test_trv_dot_5},
    {"dot_6", test_trv_dot_6},
    {"negative_0", test_trv_negative_0},
    {"call", test_trv_call},
    {"module_0", test_trv_module_0},
    {"chain_object", test_trv_chain_object},
    {"etc_0", test_trv_etc_0},
    {"etc_1", test_trv_etc_1},
    {"etc_2", test_trv_etc_2},
    {"etc_3", test_trv_etc_3},
    {"etc_4", test_trv_etc_4},
    {"etc_5", test_trv_etc_5},
    {"etc_6", test_trv_etc_6},
    {"etc_7", test_trv_etc_7},
    {"etc_8", test_trv_etc_8},
    {"etc_9", test_trv_etc_9},
    {"etc_10", test_trv_etc_10},
    {"unicode_0", test_trv_unicode_0},
    {"unicode_1", test_trv_unicode_1},
    {"import_stmt_0", test_trv_import_stmt_0},
    {"import_stmt_1", test_trv_import_stmt_1},
    {"import_stmt_2", test_trv_import_stmt_2},
    {"import_stmt_3", test_trv_import_stmt_3},
    {"import_stmt_4", test_trv_import_stmt_4},
    {"import_stmt_5", test_trv_import_stmt_5},
    {"from_import_stmt_1", test_trv_from_import_stmt_1},
    {"from_import_stmt_2", test_trv_from_import_stmt_2},
    {"from_import_stmt_3", test_trv_from_import_stmt_3},
    {0},
};


/**************
* traverser_3 *
**************/

static void
test_trv_builtin_open_0(void) {
    check_ok("{@ file = open(\"tests/open/file1.txt\", \"r\") @}{: file :}{@ file.close() @}", "(file)");
    check_ok("{@ file = open(\"tests/open/file1.txt\", \"r\") @}{: file.read() :}{@ file.close() @}", "aaa\nbbb\nccc\n");

    check_ok("{@ file = open(\"tests/open/file.w.txt\", \"w\")\n"
        "file.write(\"abc\") file.close() @}", "");
    char *s = AjiFile_ReadCopyFromPath("tests/open/file.w.txt");
    assert(!strcmp(s, "abc"));
    AjiFile_Remove("tests/open/file.w.txt");
    free(s);
}

static const struct testcase
traverser_3_tests[] = {
    {"if_stmt_0", test_trv_if_stmt_0},
    {"if_stmt_1", test_trv_if_stmt_1},
    {"if_stmt_2", test_trv_if_stmt_2},
    {"if_stmt_3", test_trv_if_stmt_3},
    {"if_stmt_4", test_trv_if_stmt_4},
    {"if_stmt_5", test_trv_if_stmt_5},
    {"if_stmt_6", test_trv_if_stmt_6},
    {"if_stmt_7", test_trv_if_stmt_7},
    {"if_stmt_8", test_trv_if_stmt_8},
    {"if_stmt_9", test_trv_if_stmt_9},
    {"if_stmt_10", test_trv_if_stmt_10},
    {"if_stmt_11", test_trv_if_stmt_11},
    {"if_stmt_12", test_trv_if_stmt_12},
    {"if_stmt_13", test_trv_if_stmt_13},
    {"if_stmt_fail_0", test_trv_if_stmt_fail_0},
    {"if_stmt_fail_1", test_trv_if_stmt_fail_1},
    {"elif_stmt_0", test_trv_elif_stmt_0},
    {"elif_stmt_1", test_trv_elif_stmt_1},
    {"elif_stmt_2", test_trv_elif_stmt_2},
    {"elif_stmt_3", test_trv_elif_stmt_3},
    {"elif_stmt_4", test_trv_elif_stmt_4},
    {"elif_stmt_5", test_trv_elif_stmt_5},
    {"elif_stmt_6", test_trv_elif_stmt_6},
    {"elif_stmt_7", test_trv_elif_stmt_7},
    {"else_stmt_0", test_trv_else_stmt_0},
    {"else_stmt_1", test_trv_else_stmt_1},
    {"else_stmt_2", test_trv_else_stmt_2},
    {"else_stmt_3", test_trv_else_stmt_3},
    {"else_stmt_4", test_trv_else_stmt_4},
    {"for_stmt_0", test_trv_for_stmt_0},
    {"for_stmt_1", test_trv_for_stmt_1},
    {"for_stmt_2", test_trv_for_stmt_2},
    {"for_stmt_3", test_trv_for_stmt_3},
    {"for_stmt_4", test_trv_for_stmt_4},
    {"for_stmt_5", test_trv_for_stmt_5},
    {"for_stmt_6", test_trv_for_stmt_6},
    {"for_stmt_7", test_trv_for_stmt_7},
    {"for_stmt_8", test_trv_for_stmt_8},
    {"for_stmt_9", test_trv_for_stmt_9},
    {"for_stmt_10", test_trv_for_stmt_10},
    {"for_stmt_11", test_trv_for_stmt_11},
    {"for_stmt_12", test_trv_for_stmt_12},
    {"break_stmt_0", test_trv_break_stmt_0},
    {"break_stmt_1", test_trv_break_stmt_1},
    {"break_stmt_2", test_trv_break_stmt_2},
    {"break_stmt_3", test_trv_break_stmt_3},
    {"continue_stmt_0", test_trv_continue_stmt_0},
    {"continue_stmt_1", test_trv_continue_stmt_1},
    {"continue_stmt_2", test_trv_continue_stmt_2},
    {"continue_stmt_3", test_trv_continue_stmt_3},
    {"continue_stmt_4", test_trv_continue_stmt_4},
    {"continue_stmt_5", test_trv_continue_stmt_5},
    {"return_stmt_0", test_trv_return_stmt_0},
    {"return_stmt_1", test_trv_return_stmt_1},
    {"return_stmt_2", test_trv_return_stmt_2},
    {"return_stmt_3", test_trv_return_stmt_3},
    {"return_stmt_4", test_trv_return_stmt_4},
    {"return_stmt_5", test_trv_return_stmt_5},
    {"return_stmt_6", test_trv_return_stmt_6},
    {"block_stmt_0", test_trv_block_stmt_0},
    {"block_stmt_1", test_trv_block_stmt_1},
    {"block_stmt_2", test_trv_block_stmt_2},
    {"block_stmt_3", test_trv_block_stmt_3},
    {"block_stmt_4", test_trv_block_stmt_4},
    {"block_stmt_fail_0", test_trv_block_stmt_fail_0},
    {"inject_stmt_0", test_trv_inject_stmt_0},
    {"inject_stmt_1", test_trv_inject_stmt_1},
    {"inject_stmt_2", test_trv_inject_stmt_2},
    {"inject_stmt_3", test_trv_inject_stmt_3},
    {"inject_stmt_4", test_trv_inject_stmt_4},
    {"inject_stmt_5", test_trv_inject_stmt_5},
    {"inject_stmt_6", test_trv_inject_stmt_6},
    {"inject_stmt_7", test_trv_inject_stmt_7},
    {"inject_stmt_8", test_trv_inject_stmt_8},
    {"inject_stmt_9", test_trv_inject_stmt_9},
    {"inject_stmt_10", test_trv_inject_stmt_10},
    {"inject_stmt_11", test_trv_inject_stmt_11},
    {"inject_stmt_12", test_trv_inject_stmt_12},
    {"inject_stmt_13", test_trv_inject_stmt_13},
    {"inject_stmt_14", test_trv_inject_stmt_14},
    {"inject_stmt_15", test_trv_inject_stmt_15},
    {"inject_stmt_16", test_trv_inject_stmt_16},
    {"inject_stmt_17", test_trv_inject_stmt_17},
    {"inject_stmt_18", test_trv_inject_stmt_18},
    {"inject_stmt_19", test_trv_inject_stmt_19},
    {"inject_stmt_20", test_trv_inject_stmt_20},
    {"inject_stmt_21", test_trv_inject_stmt_21},
    {"inject_stmt_fail_0", test_trv_inject_stmt_fail_0},
    {"func_def_0", test_trv_func_def_0},
    {"func_def_1", test_trv_func_def_1},
    {"func_def_2", test_trv_func_def_2},
    {"func_def_3", test_trv_func_def_3},
    {"func_def_4", test_trv_func_def_4},
    {"func_def_5", test_trv_func_def_5},
    {"func_def_6", test_trv_func_def_6},
    {"func_def_7", test_trv_func_def_7},
    {"func_def_8", test_trv_func_def_8},
    {"func_def_9", test_trv_func_def_9},
    {"func_def_10", test_trv_func_def_10},
    {"func_def_11", test_trv_func_def_11},
    {"func_def_12", test_trv_func_def_12},
    {"func_def_13", test_trv_func_def_13},
    {"func_def_14", test_trv_func_def_14},
    {"func_def_fail_0", test_trv_func_def_fail_0},
    {"func_met_0", test_trv_func_met_0},
    {"func_met_1", test_trv_func_met_1},
    {"func_met_1_2", test_trv_func_met_1_2},
    {"func_met_2", test_trv_func_met_2},
    {"func_met_3", test_trv_func_met_3},
    {"func_met_4", test_trv_func_met_4},
    {"func_met_5", test_trv_func_met_5},
    {"func_extends_0", test_trv_func_extends_0},
    {"func_extends_fail_0", test_trv_func_extends_fail_0},
    {"func_super_0", test_trv_func_super_0},
    {"func_super_1", test_trv_func_super_1},
    {"func_super_2", test_trv_func_super_2},
    {"func_super_fail_0", test_trv_func_super_fail_0},
    {"struct_1", test_trv_struct_1},
    {"struct_2", test_trv_struct_2},
    {"struct_3", test_trv_struct_3},
    {"struct_4", test_trv_struct_4},
    {"struct_5", test_trv_struct_5},
    {"struct_6", test_trv_struct_6},
    {"struct_7", test_trv_struct_7},
    {"struct_8", test_trv_struct_8},
    {"struct_9", test_trv_struct_9},
    {"struct_10", test_trv_struct_10},
    {"struct_11", test_trv_struct_11},
    {"struct_12", test_trv_struct_12},
    {"struct_13", test_trv_struct_13},
    {"struct_14", test_trv_struct_14},
    {"struct_15", test_trv_struct_15},
    {"struct_16", test_trv_struct_16},
    {"struct_17", test_trv_struct_17},
    {"struct_17_2", test_trv_struct_17_2},
    {"struct_17_3", test_trv_struct_17_3},
    {"struct_18", test_trv_struct_18},
    {"struct_19", test_trv_struct_19},
    {"struct_20", test_trv_struct_20},
    {"struct_21", test_trv_struct_21},
    {"struct_22", test_trv_struct_22},
    {"struct_23", test_trv_struct_23},
    {"struct_24", test_trv_struct_24},
    {"struct_25", test_trv_struct_25},
    {"struct_26", test_trv_struct_26},
    {"struct_27", test_trv_struct_27},
    {"struct_28", test_trv_struct_28},
    {"struct_29", test_trv_struct_29},
    {"struct_30", test_trv_struct_30},
    {"struct_31", test_trv_struct_31},
    {"struct_32", test_trv_struct_32},
    {"struct_33", test_trv_struct_33},
    {"struct_34", test_trv_struct_34},
    {"struct_35", test_trv_struct_35},
    {"struct_36", test_trv_struct_36},
    {"struct_37", test_trv_struct_37},
    {"struct_38", test_trv_struct_38},
    {"struct_39", test_trv_struct_39},
    {"struct_40", test_trv_struct_40},
    {"struct_41", test_trv_struct_41},
    {"struct_42", test_trv_struct_42},
    {"struct_43", test_trv_struct_43},
    {"struct_44", test_trv_struct_44},
    {"struct_45", test_trv_struct_45},
    {"struct_46", test_trv_struct_46},
    {"struct_47", test_trv_struct_47},
    {"struct_48", test_trv_struct_48},
    {"struct_49", test_trv_struct_49},
    {"struct_50", test_trv_struct_50},
    {"struct_51", test_trv_struct_51},
    {"struct_52", test_trv_struct_52},
    {"struct_53", test_trv_struct_53},
    {"struct_54", test_trv_struct_54},
    {"struct_55", test_trv_struct_55},
    {"struct_56", test_trv_struct_56},
    {"struct_57", test_trv_struct_57},
    {"struct_58", test_trv_struct_58},
    {"struct_59", test_trv_struct_59},
    {"struct_60", test_trv_struct_60},
    {"struct_fail_0", test_trv_struct_fail_0},
    {"builtin_modules_opts_0", test_trv_builtin_modules_opts_0},
    {"builtin_modules_alias_0", test_trv_builtin_modules_alias_0},
    {"builtin_modules_alias_1", test_trv_builtin_modules_alias_1},
    {"builtin_modules_alias_2", test_trv_builtin_modules_alias_2},
    {"builtin_modules_vec_0", test_trv_builtin_modules_vec_0},
    {"builtin_modules_vec_1", test_trv_builtin_modules_vec_1},
    {"builtin_functions", test_trv_builtin_functions},
    {"builtin_functions_puts_0", test_trv_builtin_functions_puts_0},
    {"builtin_functions_len_0", test_trv_builtin_functions_len_0},
    {"builtin_functions_type", test_trv_builtin_functions_type},
    {"builtin_functions_cast", test_trv_builtin_functions_cast},
    {"builtin_functions_type_dict", test_trv_builtin_functions_type_dict},
    {"builtin_functions_copy_0", test_trv_builtin_functions_copy_0},
    {"builtin_functions_deepcopy_0", test_trv_builtin_functions_deepcopy_0},
    {"builtin_functions_assert_0", test_trv_builtin_functions_assert_0},
    {"builtin_functions_assert_1", test_trv_builtin_functions_assert_1},
    {"builtin_functions_extract_0", test_trv_builtin_functions_extract_0},
    {"builtin_functions_setattr_0", test_trv_builtin_functions_setattr_0},
    {"builtin_functions_getattr_0", test_trv_builtin_functions_getattr_0},
    {"builtin_functions_dance_0", test_trv_builtin_functions_dance_0},
    {"builtin_functions_ord_0", test_trv_builtin_functions_ord_0},
    {"builtin_functions_chr_0", test_trv_builtin_functions_chr_0},
    {"builtin_string", test_trv_builtin_string},
    {"builtin_unicode_split", test_trv_builtin_unicode_split},
    {"builtin_unicode_rstrip", test_trv_builtin_unicode_rstrip},
    {"builtin_unicode_lstrip", test_trv_builtin_unicode_lstrip},
    {"builtin_unicode_strip", test_trv_builtin_unicode_strip},
    {"builtin_unicode_isdigit", test_trv_builtin_unicode_isdigit},
    {"builtin_unicode_isalpha", test_trv_builtin_unicode_isalpha},
    {"builtin_unicode_isspace", test_trv_builtin_unicode_isspace},
    {"builtin_vec_0", test_trv_builtin_vec_0},
    {"builtin_dict_0", test_trv_builtin_dict_0},
    {"builtin_open_0", test_trv_builtin_open_0},
    {0},
};

/**************
* traverser_4 *
**************/

static void
test_try_catch_stmt_1(void) {
    check_ok("{@ try: catch: end @}", "");
    check_ok("{@ try: a = 1 catch: end @}", "");
    check_ok("{@ try: a = 1 catch: b = 2 end @}", "");
    check_ok("{@ try: a = 1 b = 2 catch: c = 3 d = 4 end @}", "");
}

static void
test_try_catch_stmt_2(void) {
    check_ok("{@ try: 1 += 1 catch: end @}", "");
    check_ok("{@ try: 1 += 1 catch: puts(1) end @}", "1\n");
}

static void
test_try_catch_stmt_3(void) {
    check_ok("{@ try: 1 += 1 catch TypeError: puts(1) end @}", "1\n");
    check_ok("{@ try: 1 += 1 catch TypeError as e: end @}", "");
    check_ok("{@ try: 1 += 1 catch TypeError as e: puts(e) end @}", "(object)\n");
    check_ok("{@ try: 1 += 1 catch TypeError as e: puts(e.what()) end @}", "invalid left hand operand (1)\n");
}

static void
test_try_catch_stmt_4(void) {
    check_ok("{@ def func(): try: puts(1) catch: end end func() @}", "1\n");
    }

static void
test_try_catch_stmt_5(void) {
    check_ok("{@ def func(): try: 1 += 1 catch TypeError: puts(1) end end func() @}", "1\n");
    check_ok("{@ def func(): try: 1 += 1 catch: puts(1) end end func() @}", "1\n");
    check_ok("{@ def func(): try: puts(1) catch TypeError: end end func() @}", "1\n");
    check_ok("{@ def func(): puts(1) end func() @}", "1\n");
    check_ok("{@ def func(): try: 1 += 1 catch TypeError: puts(1) end end puts(func) @}", "(function)\n");
    check_ok("{@ def func(): try: 1 += 1 catch: puts(1) end end func() @}", "1\n");
    check_ok_trace("{@ def func(): try: 1 += 1 catch TypeError as e: puts(e.what()) end end func() @}", "invalid left hand operand (1)\n");
}

static void
test_try_catch_stmt_6(void) {
    check_ok("{@\n"
    "def func():\n"
    "   try:\n"
    "       1 += 1\n"
    "   catch TypeError:\n"
    "       puts(1)\n"
    "   end\n"
    "end\n"
    "a = 2\n"
    "if a == 1:\n"
    "elif a == 2:\n"
    "   func()\n"
    "end\n"
    "@}", "1\n");
    }

static void
test_try_catch_stmt_7(void) {
     check_ok("{@ def case2():\n"
"    try:\n"
"        2 += 2\n"
"    catch TypeError:\n"
"        puts(1)\n"
"    end\n"
"end\n"
"\n"
"def main(case):\n"
"    if case == \"all\":\n"
"    elif case == \"case1\":\n"
"    elif case == \"case2\":\n"
"        case2()\n"
"    elif case == \"case3\":\n"
"    end\n"
"end\n"
"\n"
"main(\"case2\") @}\n", "1\n");
    }

static void
test_try_catch_stmt_8(void) {
     check_fail("{@\n"
"try:\n"
"    1 += 1\n"
"catch TypeError as e:\n"
"    2 += 2\n"
"end\n"
"\n"
"@}\n", "invalid left hand operand (1)");

    check_fail("{@\n"
"try:\n"
"    1 += 1\n"
"catch TypeError as e:\n"
"    try:\n"
"        a += 2\n"
"    catch ValueError as e:\n"
"    end\n"
"end\n"
"\n"
"@}\n", "\"a\" is not defined");

    check_fail("{@\n"
"try:\n"
"    1 += 1\n"
"catch TypeError as e:\n"
"    try:\n"
"        2 += 2\n"
"    catch TypeError as e:\n"
"    end\n"
"    try:\n"
"        a += 3\n"
"    catch ValueError as e:\n"
"    end\n"
"end\n"
"\n"
"@}\n", "\"a\" is not defined");

    check_ok("{@\n"
"try:\n"
"    1 += 1\n"
"catch TypeError as e:\n"
"    try:\n"
"        2 += 2\n"
"    catch TypeError as e:\n"
"        puts(1)\n"
"    end\n"
"    try:\n"
"        2 += 2\n"
"    catch TypeError as e:\n"
"        puts(2)\n"
"    end\n"
"end\n"
"\n"
"@}\n", "1\n2\n");
}

static void
test_try_catch_stmt_9(void) {
        check_fail("{@\n"
"try:\n"
"    a += 1\n"
"catch ValueError:\n"
"catch RuntimeError:\n"
"end\n"
"\n"
"@}\n", "\"a\" is not defined");

    check_ok("{@\n"
"try:\n"
"    a += 1\n"
"catch ValueError:\n"
"catch RuntimeError:\n"
"catch LookUpError as e:\n"
"    puts(e.what())\n"
"end\n"
"\n"
"@}\n", "\"a\" is not defined\n");
}

static void
test_try_catch_stmt_10(void) {
    check_ok("{@\n"
"try:\n"
"    throw ValueError(\"error\")\n"
"catch ValueError as e:\n"
"    puts(1)\n"
"end\n"
"\n"
"@}\n", "1\n");

    check_ok("{@\n"
"try:\n"
"    throw ValueError(\"error\")\n"
"catch ValueError as e:\n"
"    puts(e.what())\n"
"end\n"
"\n"
"@}\n", "error\n");

    check_ok("{@\n"
"try:\n"
"    throw ValueError(\"error1\")\n"
"    throw ValueError(\"error2\")\n"
"catch ValueError as e:\n"
"    puts(e.what())\n"
"end\n"
"\n"
"@}\n", "error1\n");
}

static void
test_try_catch_stmt_11(void) {
    check_fail("{@\n"
"    throw ValueError(\"error\")\n"
"@}\n", "error");
    check_ok("{@\n"
"try:\n"
"    throw ValueError(\"error\")\n"
"catch ValueError as e:\n"
"end\n"
"@}\n", "");
    check_fail("{@\n"
"try:\n"
"    throw ValueError(\"error\")\n"
"catch ValueError as e:\n"
"    throw e\n"
"end\n"
"@}\n", "error");
}

static void
test_try_catch_stmt_12(void) {
    check_ok("{@\n"
"try:\n"
"    throw ValueError(\"error\")\n"
"catch (ValueError, TypeError) as e:\n"
"    puts(e.what())\n"
"end\n"
"@}\n", "error\n");

    check_ok("{@\n"
"try:\n"
"    throw ValueError(\"error\")\n"
"catch (RuntimeError, TypeError) as e:\n"
"    puts(e.what())\n"
"catch (TypeError, ValueError):\n"
"    puts(1)\n"
"end\n"
"@}\n", "1\n");

    check_ok("{@\n"
"try:\n"
"    throw ValueError(\"error\")\n"
"catch (RuntimeError, TypeError) as e:\n"
"    puts(e.what())\n"
"catch (TypeError, LookUpError):\n"
"    puts(1)\n"
"catch ValueError:\n"
"    puts(2)\n"
"end\n"
"@}\n", "2\n");
}

static void
test_try_catch_stmt_13(void) {
    check_ok("{@\n"
"struct MyException:\n"
"    message = nil\n"
"end\n"
"try:\n"
"    throw MyException(\"error\")\n"
"catch MyException as e:\n"
"    puts(e.message)\n"
"end\n"
"@}\n", "error\n");

    check_ok("{@\n"
"struct MyException:\n"
"    message = nil\n"
"end\n"
"try:\n"
"    throw MyException(\"error\")\n"
"catch ValueError:\n"
"    puts(1)\n"
"catch MyException as e:\n"
"    puts(e.message)\n"
"end\n"
"@}\n", "error\n");
}

static void
test_try_catch_stmt_14(void) {
    check_fail("{@\n"
"throw a\n"
"@}\n", "\"a\" is not defined");
}

static void
test_try_catch_stmt_15(void) {
    check_ok("{@\n"
"try:\n"
"   a = 1\n"
"catch:\n"
"end\n"
"@}{: a :}\n", "1");

    check_ok("{@\n"
"def func():\n"
"    try:\n"
"        a = 1\n"
"    catch:\n"
"    end\n"
"    puts(a)\n"
"end\n"
"func()\n"
"@}", "1\n");

    check_ok("{@\n"
"try:\n"
"    fin = open(\"tests/open/file1.txt\", \"r\")\n"
"catch:\n"
"end\n"
"fin.close()\n"
"@}{: fin :}", "(file)");

    check_ok("{@\n"
"met method(self):\n"
"    try:\n"
"        fin = open(\"tests/open/file1.txt\", \"r\")\n"
"    catch:\n"
"    end\n"
"    fin.close()\n"
"    puts(fin)\n"
"end\n"
"method(1)\n"
"@}", "(file)\n");
}

static void
test_try_catch_stmt_16(void) {
    check_ok("{@\n"
"struct S:\n"
"   def f():\n"
"       try:\n"
"           puts(1)\n"
"       catch:\n"
"       end\n"
"   end\n"
"end\n"
"S.f()\n"
"@}", "1\n");

    check_ok("{@\n"
"struct S:\n"
"   def f():\n"
"       try:\n"
"           puts(1)\n"
"       catch:\n"
"       end\n"
"   end\n"
"end\n"
"s = S()\n"
"s.f()\n"
"@}", "1\n");

    check_ok("{@\n"
"struct S:\n"
"   def f():\n"
"       try:\n"
"           fin = open(\"tests/open/file1.txt\", \"r\")"
"           puts(1)\n"
"       catch:\n"
"       end\n"
"       fin.close()\n"
"   end\n"
"end\n"
"s = S()\n"
"s.f()\n"
"@}", "1\n");

    check_ok("{@\n"
"def f():\n"
"    try:\n"
"        fin = open(\"tests/open/file1.txt\", \"r\")"
"        puts(1)\n"
"    catch:\n"
"    end\n"
"    fin.close()\n"
"end\n"
"f()\n"
"@}", "1\n");

    check_ok("{@\n"
"struct S:\n"
"    try:\n"
"       fin = open(\"tests/open/file1.txt\", \"r\")"
"       puts(1)\n"
"    catch:\n"
"    end\n"
"    fin.close()\n"
"end\n"
"s = S()\n"
"@}", "1\n");

    check_ok("{@\n"
"struct S:\n"
"   met f(self):\n"
"       try:\n"
"           fin = open(\"tests/open/file1.txt\", \"r\")"
"           puts(1)\n"
"       catch:\n"
"       end\n"
"       fin.close()\n"
"   end\n"
"end\n"
"s = S()\n"
"s.f()\n"
"@}", "1\n");
}

static void
test_scope_test(void) {
        check_fail("{@\n"
    "def func():\n"
    "   puts(key)\n"
    "end\n"
    "def main():\n"
    "   key = \"hige\"\n"
    "   func()"
    "end\n"
    "main()\n"
    "@}", "\"key\" is not defined");
    }

static const struct testcase
traverser_4_tests[] = {
    {"try_catch_stmt_1", test_try_catch_stmt_1},
    {"try_catch_stmt_2", test_try_catch_stmt_2},
    {"try_catch_stmt_3", test_try_catch_stmt_3},
    {"try_catch_stmt_4", test_try_catch_stmt_4},
    {"try_catch_stmt_5", test_try_catch_stmt_5},
    {"try_catch_stmt_6", test_try_catch_stmt_6},
    {"try_catch_stmt_7", test_try_catch_stmt_7},
    {"try_catch_stmt_8", test_try_catch_stmt_8},
    {"try_catch_stmt_9", test_try_catch_stmt_9},
    {"try_catch_stmt_10", test_try_catch_stmt_10},
    {"try_catch_stmt_11", test_try_catch_stmt_11},
    {"try_catch_stmt_12", test_try_catch_stmt_12},
    {"try_catch_stmt_13", test_try_catch_stmt_13},
    {"try_catch_stmt_14", test_try_catch_stmt_14},
    {"try_catch_stmt_15", test_try_catch_stmt_15},
    {"try_catch_stmt_16", test_try_catch_stmt_16},
    {"test_scope_test", test_scope_test},
    {0},
};

/**************
* error_stack *
**************/

static void
test_AjiErrStack_New(void) {
    AjiErrStack *stack = AjiErrStack_New();
    assert(stack);
    AjiErrStack_Del(stack);
}

static void
test_AjiErrStack_PushBack(void) {
    AjiErrStack *stack = AjiErrStack_New();

    assert(AjiErrStack_Len(stack) == 0);
    assert(_AjiErrStack_PushBack(stack, 0, NULL, 0, NULL, 0, "file1", 1, "func1", "this is %s", "message1"));
    assert(_AjiErrStack_PushBack(stack, 0, NULL, 0, NULL, 0, "file2", 2, "func2", "this is %s", "message2"));
    assert(AjiErrStack_Len(stack) == 2);

    const AjiErrElem *elem = AjiErrStack_Getc(stack, 0);
    assert(elem);
    assert(!strcmp(elem->filename, "file1"));
    assert(elem->lineno == 1);
    assert(!strcmp(elem->funcname, "func1"));
    assert(!strcmp(elem->message, "this is message1"));

    elem = AjiErrStack_Getc(stack, 1);
    assert(elem);
    assert(!strcmp(elem->filename, "file2"));
    assert(elem->lineno == 2);
    assert(!strcmp(elem->funcname, "func2"));
    assert(!strcmp(elem->message, "this is message2"));

    assert(AjiErrStack_Getc(stack, 2) == NULL);

    AjiErrStack_Del(stack);
}

static void
test_errstack_resize(void) {
    AjiErrStack *stack = AjiErrStack_New();

    assert(AjiErrStack_Len(stack) == 0);
    assert(_AjiErrStack_PushBack(stack, 0, NULL, 0, NULL, 0, "file1", 1, "func1", "this is %s", "message1"));
    assert(_AjiErrStack_PushBack(stack, 0, NULL, 0, NULL, 0, "file2", 2, "func2", "this is %s", "message2"));
    assert(_AjiErrStack_PushBack(stack, 0, NULL, 0, NULL, 0, "file3", 3, "func3", "this is %s", "message3"));
    assert(_AjiErrStack_PushBack(stack, 0, NULL, 0, NULL, 0, "file4", 4, "func4", "this is %s", "message4"));
    assert(_AjiErrStack_PushBack(stack, 0, NULL, 0, NULL, 0, "file5", 5, "func5", "this is %s", "message5"));
    assert(AjiErrStack_Len(stack) == 5);

    const AjiErrElem *elem = AjiErrStack_Getc(stack, 0);
    assert(elem);
    assert(!strcmp(elem->filename, "file1"));
    assert(elem->lineno == 1);
    assert(!strcmp(elem->funcname, "func1"));
    assert(!strcmp(elem->message, "this is message1"));

    elem = AjiErrStack_Getc(stack, 1);
    assert(elem);
    assert(!strcmp(elem->filename, "file2"));
    assert(elem->lineno == 2);
    assert(!strcmp(elem->funcname, "func2"));
    assert(!strcmp(elem->message, "this is message2"));

    elem = AjiErrStack_Getc(stack, 2);
    assert(elem);
    assert(!strcmp(elem->filename, "file3"));
    assert(elem->lineno == 3);
    assert(!strcmp(elem->funcname, "func3"));
    assert(!strcmp(elem->message, "this is message3"));

    elem = AjiErrStack_Getc(stack, 3);
    assert(elem);
    assert(!strcmp(elem->filename, "file4"));
    assert(elem->lineno == 4);
    assert(!strcmp(elem->funcname, "func4"));
    assert(!strcmp(elem->message, "this is message4"));

    elem = AjiErrStack_Getc(stack, 4);
    assert(elem);
    assert(!strcmp(elem->filename, "file5"));
    assert(elem->lineno == 5);
    assert(!strcmp(elem->funcname, "func5"));
    assert(!strcmp(elem->message, "this is message5"));

    AjiErrStack_Del(stack);
}

static void
test_AjiErrStack_Trace(void) {
    AjiErrStack *stack = AjiErrStack_New();

    assert(_AjiErrStack_PushBack(stack, 0, NULL, 0, NULL, 0, "file1", 1, "func1", "this is %s", "message1"));
    assert(_AjiErrStack_PushBack(stack, 0, NULL, 0, NULL, 0, "file2", 2, "func2", "this is %s", "message2"));

    char buf[BUFSIZ] = {0};
    setbuf(stderr, buf);

    fseek(stderr, 0, SEEK_SET);
    AjiErrStack_Trace(stack, stderr);
    fseek(stderr, 0, SEEK_SET);
    setbuf(stderr, NULL);

    // printf("buf[%s]\n", buf);
#ifdef AJI_TESTS__WINDOWS
    assert(!strcmp(buf, "Stack trace:\n"
        "    UsersDefinition: (unknown module): 0: This is message2.\n"
        "    UsersDefinition: (unknown module): 0: This is message1.\n\n"));
#endif

    AjiErrStack_Del(stack);
}

static void
test_AjiErrElem_Show(void) {
    AjiErrStack *stack = AjiErrStack_New();

    assert(_AjiErrStack_PushBack(stack, 0, NULL, 0, NULL, 0, "file1", 1, "func1", "this is %s", "message1"));
    assert(_AjiErrStack_PushBack(stack, 0, NULL, 0, NULL, 0, "file2", 2, "func2", "this is %s", "message2"));

    char buf[BUFSIZ] = {0};
    setbuf(stderr, buf);

    const AjiErrElem *elem = AjiErrStack_Getc(stack, 0);
    AjiErrElem_Show(elem, stderr);
    // assert(!strcmp(buf, "file1: 1: func1: This is message1.\n"));

    fseek(stderr, 0, SEEK_SET);
    buf[0] = '\0';

    elem = AjiErrStack_Getc(stack, 1);
    AjiErrElem_Show(elem, stderr);
    // assert(!strcmp(buf, "file2: 2: func2: This is message2.\n"));

    setbuf(stderr, NULL);
    AjiErrStack_Del(stack);
}

static void
test_AjiErrStack_ExtendFrontOther_0(void) {
    AjiErrStack *stack = AjiErrStack_New();
    AjiErrStack *other = AjiErrStack_New();

    assert(_AjiErrStack_PushBack(stack, 0, NULL, 0, NULL, 0, "file3", 3, "func3", "this is %s", "message3"));
    assert(_AjiErrStack_PushBack(stack, 0, NULL, 0, NULL, 0, "file4", 4, "func4", "this is %s", "message4"));

    assert(_AjiErrStack_PushBack(other, 0, NULL, 0, NULL, 0, "file1", 1, "func1", "this is %s", "message1"));
    assert(_AjiErrStack_PushBack(other, 0,
     NULL, 0, NULL, 0, "file2", 2, "func2", "this is %s", "message2"));

    assert(AjiErrStack_Len(stack) == 2);
    assert(AjiErrStack_Len(other) == 2);

    assert(AjiErrStack_ExtendFrontOther(stack, other));
    assert(AjiErrStack_Len(stack) == 4);
    assert(AjiErrStack_Len(other) == 2);

    AjiErrStack_Del(stack);
    AjiErrStack_Del(other);
}

static void
test_AjiErrStack_ExtendFrontOther_1(void) {
    AjiErrStack *stack = AjiErrStack_New();
    AjiErrStack *other = AjiErrStack_New();

    assert(_AjiErrStack_PushBack(stack, 0, NULL, 0, NULL, 0, "file5", 5, "func5", "this is %s", "message5"));
    assert(_AjiErrStack_PushBack(stack, 0, NULL, 0, NULL, 0, "file6", 6, "func6", "this is %s", "message6"));

    assert(_AjiErrStack_PushBack(other, 0, NULL, 0, NULL, 0, "file1", 1, "func1", "this is %s", "message1"));
    assert(_AjiErrStack_PushBack(other, 0, NULL, 0, NULL, 0, "file2", 2, "func2", "this is %s", "message2"));
    assert(_AjiErrStack_PushBack(other, 0, NULL, 0, NULL, 0, "file3", 3, "func3", "this is %s", "message3"));
    assert(_AjiErrStack_PushBack(other, 0, NULL, 0, NULL, 0, "file4", 4, "func4", "this is %s", "message4"));

    assert(AjiErrStack_Len(stack) == 2);
    assert(AjiErrStack_Len(other) == 4);

    assert(AjiErrStack_ExtendFrontOther(stack, other));
    assert(AjiErrStack_Len(stack) == 6);
    assert(AjiErrStack_Len(other) == 4);

    assert(AjiErrStack_ExtendFrontOther(stack, other));
    assert(AjiErrStack_Len(stack) == 10);
    assert(AjiErrStack_Len(other) == 4);

    AjiErrStack_Del(stack);
    AjiErrStack_Del(other);
}

static void
test_AjiErrStack_ExtendFrontOther_2(void) {
    AjiErrStack *stack = AjiErrStack_New();

    assert(_AjiErrStack_PushBack(stack, 0, NULL, 0, NULL, 0, "file1", 1, "func1", "this is %s", "message1"));
    assert(_AjiErrStack_PushBack(stack, 0, NULL, 0, NULL, 0, "file2", 2, "func2", "this is %s", "message2"));
    assert(_AjiErrStack_PushBack(stack, 0, NULL, 0, NULL, 0, "file3", 3, "func3", "this is %s", "message3"));
    assert(_AjiErrStack_PushBack(stack, 0, NULL, 0, NULL, 0, "file4", 4, "func4", "this is %s", "message4"));

    assert(AjiErrStack_Len(stack) == 4);

    assert(AjiErrStack_ExtendFrontOther(stack, stack));
    assert(AjiErrStack_Len(stack) == 8);

    AjiErrStack_Del(stack);
}

static void
test_AjiErrStack_ExtendBackOther_0(void) {
    AjiErrStack *stack = AjiErrStack_New();
    AjiErrStack *other = AjiErrStack_New();

    assert(_AjiErrStack_PushBack(stack, 0, NULL, 0, NULL, 0, "file3", 3, "func3", "this is %s", "message3"));
    assert(_AjiErrStack_PushBack(stack, 0, NULL, 0, NULL, 0, "file4", 4, "func4", "this is %s", "message4"));

    assert(_AjiErrStack_PushBack(other, 0, NULL, 0, NULL, 0, "file1", 1, "func1", "this is %s", "message1"));
    assert(_AjiErrStack_PushBack(other, 0, NULL, 0, NULL, 0, "file2", 2, "func2", "this is %s", "message2"));

    assert(AjiErrStack_Len(stack) == 2);
    assert(AjiErrStack_Len(other) == 2);

    assert(AjiErrStack_ExtendBackOther(stack, other));
    assert(AjiErrStack_Len(stack) == 4);
    assert(AjiErrStack_Len(other) == 2);

    AjiErrStack_Del(stack);
    AjiErrStack_Del(other);
}

static void
test_AjiErrStack_ExtendBackOther_1(void) {
    AjiErrStack *stack = AjiErrStack_New();
    AjiErrStack *other = AjiErrStack_New();

    assert(_AjiErrStack_PushBack(stack, 0, NULL, 0, NULL, 0, "file5", 5, "func5", "this is %s", "message5"));
    assert(_AjiErrStack_PushBack(stack, 0, NULL, 0, NULL, 0, "file6", 4, "func6", "this is %s", "message6"));

    assert(_AjiErrStack_PushBack(other, 0, NULL, 0, NULL, 0, "file1", 1, "func1", "this is %s", "message1"));
    assert(_AjiErrStack_PushBack(other, 0, NULL, 0, NULL, 0, "file2", 2, "func2", "this is %s", "message2"));
    assert(_AjiErrStack_PushBack(other, 0, NULL, 0, NULL, 0, "file3", 3, "func3", "this is %s", "message3"));
    assert(_AjiErrStack_PushBack(other, 0, NULL, 0, NULL, 0, "file4", 4, "func4", "this is %s", "message4"));

    assert(AjiErrStack_Len(stack) == 2);
    assert(AjiErrStack_Len(other) == 4);

    assert(AjiErrStack_ExtendBackOther(stack, other));
    assert(AjiErrStack_Len(stack) == 6);
    assert(AjiErrStack_Len(other) == 4);

    AjiErrStack_Del(stack);
    AjiErrStack_Del(other);
}

static void
test_AjiErrStack_ExtendBackOther_2(void) {
    AjiErrStack *stack = AjiErrStack_New();

    assert(_AjiErrStack_PushBack(stack, 0, NULL, 0, NULL, 0, "file1", 1, "func1", "this is %s", "message1"));
    assert(_AjiErrStack_PushBack(stack, 0, NULL, 0, NULL, 0, "file2", 2, "func2", "this is %s", "message2"));
    assert(_AjiErrStack_PushBack(stack, 0, NULL, 0, NULL, 0, "file3", 3, "func3", "this is %s", "message3"));
    assert(_AjiErrStack_PushBack(stack, 0, NULL, 0, NULL, 0, "file4", 4, "func4", "this is %s", "message4"));

    assert(AjiErrStack_Len(stack) == 4);

    assert(AjiErrStack_ExtendBackOther(stack, stack));
    assert(AjiErrStack_Len(stack) == 8);

    AjiErrStack_Del(stack);
}

static void
test_AjiErrStack_TrimAround(void) {
    AjiErrStack *stack = AjiErrStack_New();

    const char *src = "the source strings";
    int32_t pos = 10;
    _AjiErrStack_PushBack(stack, 0, NULL, 0, src, pos, "file1", 1, "func1", "this is dog");
    _AjiErrStack_PushBack(stack, 0, NULL, 0, src, pos, "file1", 1, "func1", "this is bird");

    AjiStr *s = AjiErrStack_TrimAround(src, pos);
    assert(s);
    assert(!strcmp(AjiStr_Getc(s),
"the source strings\n"
"         ^"));
    AjiStr_Del(s);

    AjiErrStack_Del(stack);
}

static const struct testcase
error_stack_tests[] = {
    {"AjiErrElem_Show", test_AjiErrElem_Show},
    {"AjiErrStack_New", test_AjiErrStack_New},
    {"AjiErrStack_PushBack", test_AjiErrStack_PushBack},
    {"errstack_resize", test_errstack_resize},
    {"AjiErrStack_Trace", test_AjiErrStack_Trace},
    {"AjiErrStack_ExtendFrontOther_0", test_AjiErrStack_ExtendFrontOther_0},
    {"AjiErrStack_ExtendFrontOther_1", test_AjiErrStack_ExtendFrontOther_1},
    {"AjiErrStack_ExtendFrontOther_2", test_AjiErrStack_ExtendFrontOther_2},
    {"AjiErrStack_ExtendBackOther_0", test_AjiErrStack_ExtendBackOther_0},
    {"AjiErrStack_ExtendBackOther_1", test_AjiErrStack_ExtendBackOther_1},
    {"AjiErrStack_ExtendBackOther_2", test_AjiErrStack_ExtendBackOther_2},
    {"AjiErrStack_TrimAround", test_AjiErrStack_TrimAround},
    {0},
};

/**********
* lang/gc *
**********/

static void
test_lang_AjiGC_New(void) {
    AjiGC *gc = AjiGC_New();
    assert(gc);
    AjiGC_Del(gc);
}

static void
test_lang_AjiGC_Alloc(void) {
    AjiGC *gc = AjiGC_New();
    assert(gc);

    AjiGCItem item = {0};
    AjiGC_Alloc(gc, &item, 100);

    assert(item.ptr);
    assert(item.ref_counts == 0);

    item.ref_counts++;
    item.ref_counts++;

    AjiGC_Free(gc, &item);
    assert(item.ptr);
    assert(item.ref_counts == 2);

    item.ref_counts--;
    AjiGC_Free(gc, &item);
    assert(item.ptr);
    assert(item.ref_counts == 1);

    item.ref_counts--;
    AjiGC_Free(gc, &item);
    assert(item.ptr == NULL);
    assert(item.ref_counts == 0);

    AjiGC_Del(gc);
}

static const struct testcase
gc_tests[] = {
    {"AjiGC_New", test_lang_AjiGC_New},
    {"AjiGC_Alloc", test_lang_AjiGC_Alloc},
    {0},
};

/*******************
* lang/object_dict *
*******************/

static void
test_lang_AjiObjDict_Move(void) {
    AjiConfig *config = AjiConfig_New();
    AjiGlobal_Init(config);
    AjiGC *gc = AjiGC_New();
    AjiObjDict *d = AjiObjDict_New(gc);

    AjiObj *obj1 = AjiObj_NewInt(gc, 1);
    AjiObjDict_Move(d, "abc", obj1);
    assert(AjiObjDict_Len(d) == 1);

    AjiObj *obj2 = AjiObj_NewInt(gc, 1);
    AjiObjDict_Move(d, "def", obj2);
    assert(AjiObjDict_Len(d) == 2);

    AjiObjDictItem *item1 = AjiObjDict_Get(d, "abc");
    assert(obj1 == item1->value);

    AjiObjDictItem *item2 = AjiObjDict_Get(d, "def");
    assert(obj2 == item2->value);

    AjiObjDict_Del(d);
    AjiGlobal_Destroy();
    AjiConfig_Del(config);
    AjiGC_Del(gc);
}

static void
test_lang_AjiObjDict_Set(void) {
    AjiConfig *config = AjiConfig_New();
    AjiGlobal_Init(config);
    AjiGC *gc = AjiGC_New();
    AjiObjDict *d = AjiObjDict_New(gc);

    AjiObj *obj1 = AjiObj_NewInt(gc, 1);
    AjiObjDict_Move(d, "abc", obj1);
    assert(AjiObjDict_Len(d) == 1);

    AjiObj *obj2 = AjiObj_NewInt(gc, 1);
    AjiObjDict_Move(d, "def", obj2);
    assert(AjiObjDict_Len(d) == 2);

    AjiObjDictItem *item1 = AjiObjDict_Get(d, "abc");
    assert(obj1 == item1->value);

    AjiObjDictItem *item2 = AjiObjDict_Get(d, "def");
    assert(obj2 == item2->value);

    AjiObjDict_Del(d);
    AjiGlobal_Destroy();
    AjiConfig_Del(config);
    AjiGC_Del(gc);
}

static void
test_lang_AjiObjDict_Pop(void) {
    /**********
    * pop one *
    **********/

    AjiConfig *config = AjiConfig_New();
    AjiGlobal_Init(config);
    AjiGC *gc = AjiGC_New();
    AjiObjDict *d = AjiObjDict_New(gc);
    AjiObj *obj = AjiObj_NewInt(gc, 0);

    AjiObjDict_Move(d, "abc", obj);
    assert(AjiObjDict_Len(d) == 1);
    AjiObj *popped = AjiObjDict_Pop(d, "abc");
    assert(popped);
    assert(AjiObjDict_Len(d) == 0);
    assert(obj == popped);

    AjiObjDict_Del(d);
    AjiGlobal_Destroy();
    AjiConfig_Del(config);
    AjiGC_Del(gc);

    /***********
    * pop many *
    ***********/

    config = AjiConfig_New();
    AjiGlobal_Init(config);
    gc = AjiGC_New();
    d = AjiObjDict_New(gc);

    for (int32_t i = 0; i < 10; ++i) {
        AjiObj *obj = AjiObj_NewInt(gc, i);
        char key[10];
        snprintf(key, sizeof key, "obj%d", i);
        AjiObjDict_Move(d, key, obj);
    }
    assert(AjiObjDict_Len(d) == 10);

    for (int32_t i = 0; i < 10; ++i) {
        char key[10];
        snprintf(key, sizeof key, "obj%d", i);
        AjiObj *popped = AjiObjDict_Pop(d, key);
        assert(popped);
    }
    assert(AjiObjDict_Len(d) == 0);

    AjiObjDict_Del(d);
    AjiGlobal_Destroy();
    AjiConfig_Del(config);
    AjiGC_Del(gc);
}

static const struct testcase
objdict_tests[] = {
    {"move", test_lang_AjiObjDict_Move},
    {"set", test_lang_AjiObjDict_Set},
    {"pop", test_lang_AjiObjDict_Pop},
    {0},
};

/*********
* memman *
*********/

static void
test_memman_AjiMem_Calloc(void) {

    return;  // TODO

    void *p;

    p = AjiMemman_Calloc(10, 4);
    AjiMemman_Free(p);
    p = AjiMemman_Calloc(10, 4);
    AjiMemman_Free(p);
    p = AjiMemman_Calloc(10, 4);
    AjiMemman_Free(p);

    void *p1 = AjiMemman_Calloc(10, 4);
    void *p2 = AjiMemman_Calloc(10, 4);
    void *p3 = AjiMemman_Calloc(10, 4);
    void *p4 = AjiMemman_Calloc(10, 4);
    void *p5 = AjiMemman_Calloc(10, 4);

    AjiMemman_Free(p1);
    AjiMemman_Free(p2);
    AjiMemman_Free(p3);
    AjiMemman_Free(p4);
    AjiMemman_Free(p5);

    p = AjiMemman_Calloc(1, 1000);
    AjiMemman_Free(p);

    const size_t n = 10000;
    void *pary[n];

    for (size_t i = 0; i < n; i += 1) {
        size_t size = 1 + rand() % 1000;
        pary[i] = AjiMemman_Calloc(1, size);
        memset(pary[i], 0, size);

        if (rand() % 5 == 0) {
            AjiMemman_Free(pary[i]);
            pary[i] = NULL;
        }
    }

    for (size_t i = 0; i < n; i += 1) {
        AjiMemman_Free(pary[i]);
    }

    AjiMemman_Destroy();
}

static void
test_memman_AjiMem_Realloc(void) {

    return; // TODO

    void *p;

    p = AjiMemman_Calloc(10, 4);
    p = AjiMemman_Realloc(p, 80);
    AjiMemman_Free(p);

    p = AjiMemman_Calloc(5, 1);
    strcpy(p, "hello");
    assert(!strcmp(p, "hello"));
    p = AjiMemman_Realloc(p, 10);
    assert(!strcmp(p, "hello"));
    AjiMemman_Free(p);

    AjiMemman_Destroy();
}

static const struct testcase
memman_tests[] = {
    {"AjiMem_Calloc", test_memman_AjiMem_Calloc},
    {"AjiMem_Realloc", test_memman_AjiMem_Realloc},
    {0},
};

/***********
* lib/list *
***********/

static void
test_lang_lib_list_dump(void) {
    check_ok("{@\n"
        "from \"lib/list.aji\" import List\n"
        "list = List()\n"
        "list.push(1)\n"
        "list.push(2)\n"
        "list.push(3)\n"
        "list.dump()\n"
        "@}", "1\n2\n3\n");
    }

static void
test_lang_lib_list_push(void) {
    // @see list_dump
}

static void
test_lang_lib_list_pop(void) {
    /*
    check_ok("{@\n"
        "from \"lib/list.aji\" import List\n"
        "list = List.new()\n"
        "list.push(1)\n"
        "list.push(2)\n"
        "val = list.pop()\n"
        "@}{: val :}", "2");
*/}

static const struct testcase
lib_list_tests[] = {
    {"dump", test_lang_lib_list_dump},
    {"push", test_lang_lib_list_push},
    {"pop", test_lang_lib_list_pop},
    {0},
};

/****************
* lib/stream *
****************/

static void
test_lang_lib_stream_new(void) {
    check_ok("{@\n"
        "from \"lib/stream.aji\" import Stream\n"
        "s = Stream.new(\"abc\")\n"
        "@}{: s.buf :},{: s.index :},{: s.length :}", "abc,0,3");
    }

static void
test_lang_lib_stream_get(void) {
    check_ok("{@\n"
        "from \"lib/stream.aji\" import Stream\n"
        "s = Stream.new(\"abc\")\n"
        "puts(s.eof())\n"
        "puts(s.get())\n"
        "puts(s.get())\n"
        "puts(s.get())\n"
        "puts(s.get())\n"
        "puts(s.eof())\n"
        "@}", "false\na\nb\nc\nnil\ntrue\n");
    }

static void
test_lang_lib_stream_next(void) {
    check_ok("{@\n"
        "from \"lib/stream.aji\" import Stream\n"
        "s = Stream.new(\"abc\")\n"
        "puts(s.cur(0))\n"
        "s.next()\n"
        "puts(s.cur(0))\n"
        "s.next()\n"
        "puts(s.cur(0))\n"
        "s.next()\n"
        "puts(s.cur(0))\n"
        "s.next()\n"
        "puts(s.index)\n"
        "@}", "a\nb\nc\nnil\n3\n");
    }

static void
test_lang_lib_stream_prev(void) {
    check_ok("{@\n"
        "from \"lib/stream.aji\" import Stream\n"
        "src = \"abc\""
        "s = Stream.new(src)\n"
        "s.index = len(src)\n"
        "puts(s.cur(0))\n"
        "s.prev()\n"
        "puts(s.cur(0))\n"
        "s.prev()\n"
        "puts(s.cur(0))\n"
        "s.prev()\n"
        "puts(s.cur(0))\n"
        "s.prev()\n"
        "puts(s.cur(0))\n"
        "@}", "nil\nc\nb\na\na\n");
    }

static void
test_lang_lib_stream_cur(void) {
    check_ok("{@\n"
        "from \"lib/stream.aji\" import Stream\n"
        "s = Stream.new(\"abc\")\n"
        "puts(s.cur(0))\n"
        "@}", "a\n");
    }

static const struct testcase
lib_stream_tests[] = {
    {"new", test_lang_lib_stream_new},
    {"get", test_lang_lib_stream_get},
    {"next", test_lang_lib_stream_next},
    {"prev", test_lang_lib_stream_prev},
    {"cur", test_lang_lib_stream_cur},
    {0},
};

static void
test_dict_default(void) {
    AjiDict *d = AjiDict_New(2);
    assert(d);

    assert(AjiDict_Set(d, "aaa", "111"));
    assert(AjiDict_Set(d, "bbb", "222"));
    assert(AjiDict_Set(d, "ccc", "333"));
    assert(AjiDict_Set(d, "ddd", "444"));
    assert(AjiDict_Set(d, "eee", "555"));

    const AjiDictItem *i;
    i = AjiDict_Getc(d, "bbb");
    assert(i);
    assert(strcmp(i->key, "bbb") == 0);
    assert(strcmp(i->value, "222") == 0);

    i = AjiDict_Getc(d, "ddd");
    assert(i);
    assert(strcmp(i->key, "ddd") == 0);
    assert(strcmp(i->value, "444") == 0);

    i = AjiDict_Getc(d, "???");
    assert(i == NULL);
    
    AjiDict_Del(d);
}

static const struct testcase
dict_tests[] = {
    {"default", test_dict_default},
    {0},
};

static void
test_void_dict_default(void) {
    AjiVoidDict *d = AjiVoidDict_New();
    assert(d);

    assert(AjiVoidDict_Move(d, "aaa", (void *)111));
    assert(AjiVoidDict_Move(d, "bbb", (void *)222));
    assert(AjiVoidDict_Move(d, "ccc", (void *)333));
    assert(AjiVoidDict_Move(d, "ddd", (void *)444));
    assert(AjiVoidDict_Move(d, "eee", (void *)555));
    assert(AjiVoidDict_Move(d, "fff", (void *)666));
    assert(AjiVoidDict_Move(d, "ggg", (void *)777));
    assert(AjiVoidDict_Move(d, "hhh", (void *)888));
    assert(AjiVoidDict_Move(d, "iii", (void *)999));
    assert(AjiVoidDict_Move(d, "jjj", (void *)1000));

    const AjiVoidDictItem *i;
    i = AjiVoidDict_Getc(d, "aaa");
    assert(AjiCStr_Eq(i->key, "aaa"));
    assert(i->value == (void *)111);
    i = AjiVoidDict_Getc(d, "ggg");
    assert(AjiCStr_Eq(i->key, "ggg"));
    assert(i->value == (void *)777);
    assert(AjiVoidDict_Getc(d, "???") == NULL);

    AjiVoidDict_Del(d);
}

static const struct testcase
void_dict_tests[] = {
    {"default", test_void_dict_default},
    {0},
};

static void 
uni_deleter(void *ptr) {
    AjiUni *u = ptr;
    AjiUni_Del(u);
}

static void *
uni_deep_copy(const void *ptr) {
    const AjiUni *u = ptr;
    return AjiUni_DeepCopy(u);
}

static void *
uni_shallow_copy(const void *ptr) {
    const AjiUni *u = ptr;
    return AjiUni_ShallowCopy(u);
}

static int
uni_sort_compare(const void *lhs, const void *rhs) {
    const AjiUni *lu = lhs;
    const AjiUni *ru = rhs;
    return AjiUni_Compare(lu, ru);
}

static AjiVoidVec *
new_uni_void_vector(void) {
    return AjiVoidVec_New(
        uni_deleter,
        uni_deep_copy,
        uni_shallow_copy,
        uni_sort_compare,
        NULL
    );
}

static void
test_void_vector_new(void) {
    AjiVoidVec *vec = new_uni_void_vector();
    AjiVoidVec_Del(vec);
}

static void
test_void_vector_esc_del(void) {
    AjiVoidVec *vec = new_uni_void_vector();
    AjiUni *u = AjiUni_New();
    AjiUni_SetMB(u, "aaa");
    AjiVoidVec_PushBack(vec, u);
    AjiVoidVec_PushBack(vec, u);
    AjiVoidVec_PushBack(vec, u);
    AjiUni_Del(u);
    
    void **pvec = AjiVoidVec_EscDel(vec);
    assert(pvec);
    for (void **p = pvec; *p; p += 1) {
        u = *p;
        assert(!strcmp(AjiUni_GetcMB(u), "aaa"));
        AjiUni_Del(u);
    }

    free(pvec);
}

static void
test_void_vector_move_back(void) {
    AjiVoidVec *vec = new_uni_void_vector();
    AjiUni *u;

    char s[100];
    for (int i = 0; i < 20; i += 1) {
        snprintf(s, sizeof s, "%d", i);
        u = AjiUni_New();
        AjiUni_SetMB(u, s);
        AjiVoidVec_MoveBack(vec, u);
        assert(AjiVoidVec_Len(vec) == i + 1);
    }
    
    AjiVoidVec_Del(vec);
}

static void
test_void_vector_pop_move(void) {
    AjiVoidVec *vec = new_uni_void_vector();
    AjiUni *u = AjiUni_New();
    AjiUni_SetMB(u, "aaa");
    AjiVoidVec_PushBack(vec, u);
    AjiVoidVec_PushBack(vec, u);
    AjiUni_Del(u);

    u = AjiVoidVec_PopMove(vec);
    assert(!strcmp(AjiUni_GetcMB(u), "aaa"));
    AjiUni_Del(u);

    u = AjiVoidVec_PopMove(vec);
    assert(!strcmp(AjiUni_GetcMB(u), "aaa"));
    AjiUni_Del(u);

    u = AjiVoidVec_PopMove(vec);
    assert(!u);

    AjiVoidVec_Del(vec);
}

static void
test_void_vector_getc(void) {
    AjiVoidVec *vec = new_uni_void_vector();
    AjiUni *u;

    u = AjiUni_New();
    AjiUni_SetMB(u, "aaa");
    AjiVoidVec_PushBack(vec, u);
    AjiUni_Del(u);

    u = AjiUni_New();
    AjiUni_SetMB(u, "bbb");
    AjiVoidVec_PushBack(vec, u);
    AjiUni_Del(u);

    assert(!AjiVoidVec_Getc(vec, 100));
    assert(!AjiVoidVec_Getc(vec, -100));

    u = AjiVoidVec_Get(vec, 0);
    assert(!strcmp(AjiUni_GetcMB(u), "aaa"));
    u = AjiVoidVec_Get(vec, 1);
    assert(!strcmp(AjiUni_GetcMB(u), "bbb"));

    AjiVoidVec_Del(vec);
}

static void
test_void_vector_clear(void) {
    AjiVoidVec *vec = new_uni_void_vector();
    AjiUni *u;

    u = AjiUni_New();
    AjiUni_SetMB(u, "aaa");
    AjiVoidVec_PushBack(vec, u);
    AjiUni_Del(u);

    u = AjiUni_New();
    AjiUni_SetMB(u, "bbb");
    AjiVoidVec_PushBack(vec, u);
    AjiUni_Del(u);

    assert(AjiVoidVec_Len(vec) == 2);
    AjiVoidVec_Clear(vec);
    assert(AjiVoidVec_Len(vec) == 0);

    AjiVoidVec_Del(vec);
}

static void
test_void_vector_sort(void) {
    // TODO: fix me! sort logic
    // This sort function in this case is not working for string-vec
    // Need to fix the comparison function of sort but I don't it because I tired
    return;

    AjiVoidVec *vec = new_uni_void_vector();
    AjiUni *u;

    u = AjiUni_New();
    AjiUni_SetMB(u, "bbb");
    AjiVoidVec_PushBack(vec, u);
    u = AjiUni_New();
    AjiUni_SetMB(u, "aaa");
    AjiVoidVec_PushBack(vec, u);
    AjiUni_Del(u);

    AjiVoidVec_Sort(vec);
    u = AjiVoidVec_Get(vec, 0);
    assert(!strcmp(AjiUni_GetcMB(u), "bbb"));  // Wha?
    u = AjiVoidVec_Get(vec, 1);
    assert(!strcmp(AjiUni_GetcMB(u), "aaa"));  // Hm?

    AjiVoidVec_Clear(vec);

    u = AjiUni_New();
    AjiUni_SetMB(u, "aaa");
    AjiVoidVec_PushBack(vec, u);
    u = AjiUni_New();
    AjiUni_SetMB(u, "bbb");
    AjiVoidVec_PushBack(vec, u);

    AjiVoidVec_Sort(vec);
    u = AjiVoidVec_Get(vec, 0);
    assert(!strcmp(AjiUni_GetcMB(u), "aaa"));  // Fa?
    u = AjiVoidVec_Get(vec, 1);
    assert(!strcmp(AjiUni_GetcMB(u), "bbb"));  // Un?

    AjiVoidVec_Del(vec);
}

static void
find_deleter(void *p) {
    // nothing todo
}

static int
find_compar(const void *l, const void *r) {
    int a = *(int *) l;
    int b = *(int *) r;
    printf("a[%d] b[%d]\n", a, b);
    return a - b;
}

static void
test_void_vector_find(void) {

    return;  // AjiVoidVec_Find() not working. since 2021/11/13
             // maybe, void pointer is not findable because 
             // this pointer address can't sort before bsearch()

    AjiVoidVec *vec = AjiVoidVec_New(
        find_deleter,
        NULL,
        NULL,
        NULL,
        find_compar
    );
    int *a = AjiMem_Calloc(1, sizeof(int));
    *a = 1;
    int *b = AjiMem_Calloc(1, sizeof(int));
    *b = 2;
    int *c = AjiMem_Calloc(1, sizeof(int));
    *c = 3;
    printf("a[%p] b[%p] c[%p]\n", a, b, c);
    assert(AjiVoidVec_MoveBack(vec, (void *) a));
    assert(AjiVoidVec_MoveBack(vec, (void *) b));
    assert(AjiVoidVec_MoveBack(vec, (void *) c));

    void *found = AjiVoidVec_Find(vec, (void *)b);
    intptr_t p = (intptr_t) found;
    printf("found[%p]\n", found);
    assert(p == 2);

    AjiVoidVec_Del(vec);
    free(a);
    free(b);
    free(c);
}

static const struct testcase
void_vector_tests[] = {
    {"AjiVoidVec_New", test_void_vector_new},
    {"AjiVoidVec_EscDel", test_void_vector_esc_del},
    {"AjiVoidVec_MoveBack", test_void_vector_move_back},
    {"AjiVoidVec_PopMove", test_void_vector_pop_move},
    {"AjiVoidVec_Getc", test_void_vector_getc},
    {"AjiVoidVec_Clear", test_void_vector_clear},
    {"AjiVoidVec_Sort", test_void_vector_sort},
    {"AjiVoidVec_Find", test_void_vector_find},
    {0},
};

/*******
* main *
*******/

static const struct testmodule
test_modules[] = {
    // lib
    {"cstring_vector", cstrvec_tests},
    {"cstring", cstring_tests},
    {"string", string_tests},
    {"unicode", unicode_tests},
    {"bytes", bytes_tests},
    {"file", file_tests},
    {"cl", cl_tests},
    {"cmdline", cmdline_tests},
    {"error", error_tests},
    {"util", util_tests},
    {"path", path_tests},
    {"unicode_path", unicode_path_tests},
    {"dict", dict_tests},
    {"void_dict", void_dict_tests},
    {"void_vector", void_vector_tests},
    {"tokenizer", tokenizer_tests},
    {"compiler", compiler_tests},
    {"traverser_1", traverser_1_tests},
    {"traverser_2", traverser_2_tests},
    {"traverser_3", traverser_3_tests},
    {"traverser_4", traverser_4_tests},
    {"lib.list", lib_list_tests},
    {"lib.stream", lib_stream_tests},
    {"error_stack", error_stack_tests},
    {"gc", gc_tests},
    {"objdict", objdict_tests},
    {"memman", memman_tests},
    {0},
};

struct AjiOpts {
    bool is_help;
    bool is_unused_global;
    int32_t argc;
    char **argv;
    int32_t optind;
};

static int32_t
parseopts(struct AjiOpts *opts, int argc, char *argv[]) {
    // Init opts
    *opts = (struct AjiOpts) {0};
    optind = 0;
    opterr = 0;

    // Parse options
    static struct option longopts[] = {
        {"help", no_argument, 0, 'h'},
        {"unused-global", no_argument, 0, 'g'},
        {0},
    };

    for (;;) {
        int optsindex;
        int cur = getopt_long(argc, argv, "hg", longopts, &optsindex);
        if (cur == -1) {
            break;
        }

        switch (cur) {
        case 0: /* Long option only */ break;
        case 'h': opts->is_help = true; break;
        case 'g': opts->is_unused_global = true; break;
        case '?':
        default: die("unknown option"); break;
        }
    }

    if (argc < optind) {
        die("failed to parse option");
    }

    opts->argc = argc;
    opts->optind = optind;
    opts->argv = argv;

    return 0;
}

static int32_t
modtest(const char *modname) {
    int32_t ntest = 0;
    const struct testmodule *fndmod = NULL;

    for (const struct testmodule *m = test_modules; m->name; ++m) {
        if (strcmp(modname, m->name) == 0) {
            fndmod = m;
        }
    }
    if (!fndmod) {
        return 0;
    }

    printf("\n* module '%s'\n", fndmod->name);

    for (const struct testcase *t = fndmod->tests; t->name; ++t) {
        printf("- testing '%s'\n", t->name);
        t->test();
        ++ntest;
    }

    return ntest;
}

static int32_t
methtest(const char *modname, const char *methname) {
    const struct testmodule *fndmod = NULL;

    for (const struct testmodule *m = test_modules; m->name; ++m) {
        if (strcmp(modname, m->name) == 0) {
            fndmod = m;
        }
    }
    if (!fndmod) {
        return 0;
    }

    printf("\n* module '%s'\n", fndmod->name);

    const struct testcase *fndt = NULL;
    for (const struct testcase *t = fndmod->tests; t->name; ++t) {
        if (!strcmp(t->name, methname)) {
            fndt = t;
            break;
        }
    }
    if (!fndt) {
        return 0;
    }

    printf("* method '%s'\n", fndt->name);
    fndt->test();

    return 1;
}

static int32_t
fulltests(void) {
    int32_t ntest = 0;

    for (const struct testmodule *m = test_modules; m->name; ++m) {
        printf("\n* module '%s'\n", m->name);
        for (const struct testcase *t = m->tests; t->name; ++t) {
            printf("- testing '%s'\n", t->name);
            t->test();
            ++ntest;
        }
    }

    return ntest;
}

static void
run(const struct AjiOpts *opts) {
    AjiConfig *config;
    if (!opts->is_unused_global) {
        config = AjiConfig_New();
        AjiGlobal_Init(config);
    }

    int32_t ntest = 0;
    clock_t start;
    clock_t end;

    if (opts->argc - opts->optind == 1) {
        start = clock();
        ntest = modtest(opts->argv[opts->optind]);
        end = clock();
    } else if (opts->argc - opts->optind >= 2) {
        start = clock();
        ntest = methtest(opts->argv[opts->optind], opts->argv[opts->optind+1]);
        end = clock();
    } else {
        start = clock();
        ntest = fulltests();
        end = clock();
    }

    fflush(stdout);
    fprintf(stderr, "\nRun %d test in %0.3lfs.\n", ntest, (double)(end-start)/CLOCKS_PER_SEC);
    fprintf(stderr, "\n");
    fprintf(stderr, "OK\n");

    fflush(stderr);

    if (!opts->is_unused_global) {
        AjiGlobal_Destroy();
        AjiConfig_Del(config);
    }
}

static void
cleanup(void) {
    remove_test_file();
}

static void
test_global(void) {
    puts("- testing 'global'");
    AjiConfig *config = AjiConfig_New();
    AjiGlobal_Init(config);
    AjiGlobal_Destroy();
    AjiConfig_Del(config);
    puts("OK");
}

int
main(int argc, char *argv[]) {
    setlocale(LC_CTYPE, "");

    bool do_global_test = argc >= 2 && !strcmp(argv[1], "global");
    if (do_global_test) {
        test_global();
        return 0;
    }

    struct AjiOpts opts;
    if (parseopts(&opts, argc, argv) != 0) {
        die("failed to parse options");
    }

    run(&opts);
    cleanup();

    return 0;
}

