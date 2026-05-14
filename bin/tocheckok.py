#!/usr/bin/python
"""
    AjiTkr_Parse(tkr, "{: 1 :}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        AjiLexEnv_Clear(lex_env);
        AjiTrv_Trav(ast, lex_env);
        assert(!AjiAST_HasErrs(ast));
        assert(!strcmp(AjiLexEnv_GetcStdoutBufSuperGlobal(lex_env), "1"));
    }

    AjiTkr_Parse(tkr, "{@\n"
    "   import \"tests/lang/modules/string.aji\" as string\n"
    "@}{: string.variable.upper() :}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        AjiLexEnv_Clear(lex_env);
        AjiTrv_Trav(ast, lex_env);
        assert(!AjiAST_HasErrs(ast));
        assert(!strcmp(AjiLexEnv_GetcStdoutBufSuperGlobal(lex_env), "STRING"));
    }



static void
test_trv_if_stmt_6(void) {
        AjiTkr_Parse(tkr, "{@\n"
    "   from \"tests/lang/modules/if-2.aji\" import f1 \n"
    "   f1()\n"
    "@}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        AjiLexEnv_Clear(lex_env);
        AjiTrv_Trav(ast, lex_env);
        assert(!AjiAST_HasErrs(ast));
        assert(!strcmp(AjiLexEnv_GetcStdoutBufSuperGlobal(lex_env), "1\n"));
    }
}

static void
test_trv_if_stmt_7(void) {
        AjiTkr_Parse(tkr, "{@\n"
    "   from \"tests/lang/modules/if-2.aji\" import f2\n"
    "   f2()\n"
    "@}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        AjiLexEnv_Clear(lex_env);
        AjiTrv_Trav(ast, lex_env);
        assert(!AjiAST_HasErrs(ast));
        assert(!strcmp(AjiLexEnv_GetcStdoutBufSuperGlobal(lex_env), "1\n"));
    }
}

"""
import sys
import unittest


def parse(s, i, debug=False):
    save_i = i
    m = 0
    buf = ''
    in_str = ''
    out_str = ''
    ok = False

    while i < len(s):
        c = s[i]

        if debug:
            print(f'{m} [{c}]')

        if m == 0:
            if c == 'P':
                if s[i:i+12] == 'AjiTkr_Parse':
                    i += 11
                    m = 10
                    buf = ''
        elif m == 10:  # found AjiTkr_Parse
            if c == '"':
                m = 20
                buf += c
            elif c == ';':
                m = 0
        elif m == 20:  # found "
            if c == '\\':
                buf += c 
                i += 1
                if i < len(s):
                    buf += s[i]
            elif c == '"':
                m = 30
                buf += c
            else:
                buf += c
        elif m == 30:  # wait for ;
            if c == ';':
                m = 100
                ok = False
                in_str = buf
                buf = ''
            elif c == '"':
                buf += '<!>'
                buf += c
                m = 20
        elif m == 100:  # wait output string
            if c == 'a' and s[i:i+22] == 'assert(!AjiAST_HasErrs':
                ok = True
            elif c == 'a' and s[i:i+6] == 'assert':
                m = 105
            elif c == '}':
                out_str = '""'
                buf = ''
                return (i, ok, in_str, out_str)
        elif m == 105:
            t = 'AjiLexEnv_GetcStdoutBufSuperGlobal'
            if c == 'P' and s[i:i+len(t)] == t:
                m = 108
            t = 'AjiLexEnv_GetcStderrBufSuperGlobal'
            if c == 'P' and s[i:i+len(t)] == t:
                m = 108
            t = 'AjiAST_GetcFirstErrMsg'
            if c == 'P' and s[i:i+len(t)] == t:
                m = 108
        elif m == 108:
            if c == '"':
                m = 110
                buf += c
            elif c == '}':
                out_str = buf
                buf = ''
                return (i, ok, in_str, out_str)
        elif m == 110:  # found "
            if c == '\\':
                buf += c
                i += 1
                if i < len(s):
                    buf += s[i]
            elif c == '"':
                m = 120
                buf += c
            else:
                buf += c
        elif m == 120:
            if c == ';':
                out_str = buf
                buf = ''
                return (i, ok, in_str, out_str)
            elif c == '"':
                buf += '<!>'
                buf += c
                m = 110

        i += 1

    return save_i, False, '', ''


def conv_obj(ok, in_str, out_str):
    s = ''
    if ok:
        s = 'check_ok('
    else:
        s = 'check_fail('

    for ins in in_str.split('<!>'):
        s += ins + '\n'

    s = s.rstrip()
    s += ', '

    for outs in out_str.split('<!>'):
        s += outs + '\n'

    s = s.rstrip()
    s += ');'

    return s

"""
    AjiConfig *config = AjiConfig_New();
    AjiTkrOpt *opt = AjiTkrOpt_New();
    AjiTkr *tkr = AjiTkr_New(AjiMem_Move(opt));
    AjiAST *ast = AjiAST_New(config);
    AjiGC *gc = AjiGC_New();
    AjiLexEnv *lex_env = AjiLexEnv_New(AJI_LEX_ENV_TYPE__ROOT, gc, NULL);

    AjiGC_Del(gc);
    AjiAST_Del(ast);
    AjiTkr_Del(tkr);
    AjiConfig_Del(config);
"""
def conv(s):
    i = 0
    m = 0
    buf = ''

    while i < len(s):
        c = s[i]
        # print(f'{m} [{c}]')
        if m == 0:
            if c == 'P':
                if s[i:i+12] == 'AjiTkr_Parse':
                    ii, ok, in_str, out_str = parse(s, i)
                    if i == ii:
                        buf += c
                    else:
                        buf = buf.rstrip() + '\n    '
                        buf += conv_obj(ok, in_str, out_str)
                        i = ii
                        m = 10
                elif s[i:i+17] == 'AjiConfig *config' or \
                     s[i:i+14] == 'AjiTkrOpt *opt' or \
                     s[i:i+11] == 'AjiTkr *tkr' or \
                     s[i:i+11] == 'AjiAST *ast' or \
                     s[i:i+9] == 'AjiGC *gc' or \
                     s[i:i+18] == 'AjiLexEnv *lex_env' or \
                     s[i:i+9] == 'AjiGC_Del' or \
                     s[i:i+10] == 'AjiAST_Del' or \
                     s[i:i+10] == 'AjiTkr_Del' or \
                     s[i:i+13] == 'AjiConfig_Del' or \
                     s[i:i+23] == 'AjiLexEnv_DelWithChilds':
                    m = 5000
                else:
                    buf += c
            elif c == 't':
                if s[i:i+10] == 'trv_ready;':
                    i += 9
                    m = 5000
                elif s[i:i+12] == 'trv_cleanup;':
                    i += 11
                    m = 5000
                else:
                    buf += c
            elif c == '#':
                m = 1000
                buf += c
            else:
                buf += c
        elif m == 10:  # wait for }
            if c == '}':
                m = 5000
        elif m == 1000:  # found #
            buf +=c 
            if c == '\\':
                i += 1
                if i < len(s):
                    buf += s[i]
            elif c == '\n':
                m = 0
        elif m == 5000:  # wait for \n
            if c == '\n':
                buf += c
                m = 0

        i += 1

    return buf


def main():
    print(conv(sys.stdin.read()))


if __name__ == '__main__':
    main()


class Test(unittest.TestCase):
    def test_conv(self):
        s = '''
static void
test_trv_dot_1(void) {
    trv_ready;

    AjiTkr_Parse(tkr, "{@\\n"
    "   import \\"tests/lang/modules/string.aji\\" as string\\n"
    "@}{: string.variable.upper() :}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        AjiLexEnv_Clear(lex_env);
        AjiTrv_Trav(ast, lex_env);
        assert(!AjiAST_HasErrs(ast));
        assert(!strcmp(AjiLexEnv_GetcStdoutBufSuperGlobal(lex_env), "STRING"));
    }

    trv_cleanup;
}

static void
test_trv_dot_5(void) {
    AjiConfig *config = AjiConfig_New();
    AjiTkrOpt *opt = AjiTkrOpt_New();
    AjiTkr *tkr = AjiTkr_New(AjiMem_Move(opt));
    AjiAST *ast = AjiAST_New(config);
    AjiGC *gc = AjiGC_New();
    AjiLexEnv *lex_env = AjiLexEnv_New(AJI_LEX_ENV_TYPE__ROOT, gc, NULL);

    AjiTkr_Parse(tkr, "{@\\n"
    "    vec = [[[[[[[[1, 2]]]]]]]]\\n"
    "    dst = []\\n"
    "    n = dst.push(vec.pop().pop().pop().pop().pop().pop().pop().pop()).pop()\\n"
    "@}{: n :}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        AjiLexEnv_Clear(lex_env);
        (AjiTrv_Trav(ast, lex_env));
        assert(!AjiAST_HasErrs(ast));
        assert(!strcmp(AjiLexEnv_GetcStdoutBufSuperGlobal(lex_env), "2"));
    }

    check_fail("hige1", "hoge1");

    AjiTkr_Parse(tkr, "{: \\"abc\\".upper() :}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        AjiLexEnv_Clear(lex_env);
        AjiTrv_Trav(ast, lex_env);
        printf("hige");
        assert(!AjiAST_HasErrs(ast));
        assert(!strcmp(AjiLexEnv_GetcStdoutBufSuperGlobal(lex_env), "ABC"));
    }

    check_fail("hige2", "hoge2");

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

    AjiGC_Del(gc);
    AjiAST_Del(ast);
    AjiTkr_Del(tkr);
    AjiConfig_Del(config);
}
'''
        s = '''
static void
test_trv_if_stmt_6(void) {
        AjiTkr_Parse(tkr, "{@\n"
    "   from \"tests/lang/modules/if-2.aji\" import f1 \n"
    "   f1()\n"
    "@}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        AjiLexEnv_Clear(lex_env);
        AjiTrv_Trav(ast, lex_env);
        assert(!AjiAST_HasErrs(ast));
        assert(!strcmp(AjiLexEnv_GetcStdoutBufSuperGlobal(lex_env), "1\n"));
    }
}

static void
test_trv_if_stmt_7(void) {
        AjiTkr_Parse(tkr, "{@\n"
    "   from \"tests/lang/modules/if-2.aji\" import f2\n"
    "   f2()\n"
    "@}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        AjiLexEnv_Clear(lex_env);
        AjiTrv_Trav(ast, lex_env);
        assert(!AjiAST_HasErrs(ast));
        assert(!strcmp(AjiLexEnv_GetcStdoutBufSuperGlobal(lex_env), "1\n"));
    }
}
'''
        print(conv(s))

    def test_parse(self):
        s = '''
    AjiTkr_Parse(tkr, "{: 1 :}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        AjiLexEnv_Clear(lex_env);
        AjiTrv_Trav(ast, lex_env);
        assert(!AjiAST_HasErrs(ast));
        assert(!strcmp(AjiLexEnv_GetcStdoutBufSuperGlobal(lex_env), "1"));
    }
        '''
        el = parse(s, 0)
        self.assertEqual(el[1], True)
        self.assertEqual(el[2], '"{: 1 :}"')
        self.assertEqual(el[3], '"1"')

        s = '''
    AjiTkr_Parse(tkr, "{: 1 :}"
        "hige\thige\n");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        AjiLexEnv_Clear(lex_env);
        AjiTrv_Trav(ast, lex_env);
        assert(!AjiAST_HasErrs(ast));
        assert(!strcmp(AjiLexEnv_GetcStdoutBufSuperGlobal(lex_env), "111"
            "222\n333\t"));
    }
        '''
        el = parse(s, 0)
        self.assertEqual(el[1], True)
        self.assertEqual(el[2], '"{: 1 :}"<!>"hige\thige\n"')
        self.assertEqual(el[3], '"111"<!>"222\n333\t"')

        s = '''
    AjiTkr_Parse(tkr, "{: 1 :}"
        "hige\thige\n");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        AjiLexEnv_Clear(lex_env);
        AjiTrv_Trav(ast, lex_env);
        assert(AjiAST_HasErrs(ast));
        assert(!strcmp(AjiLexEnv_GetcStdoutBufSuperGlobal(lex_env), "111"
            "222\n333\t"));
    }
        '''
        el = parse(s, 0)
        self.assertEqual(el[1], False)
        self.assertEqual(el[2], '"{: 1 :}"<!>"hige\thige\n"')
        self.assertEqual(el[3], '"111"<!>"222\n333\t"')

        s = '''
    AjiTkr_Parse(tkr, "{: 1 :}"
        "hige\thige\n");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        AjiLexEnv_Clear(lex_env);
        AjiTrv_Trav(ast, lex_env);
        assert(AjiAST_HasErrs(ast));
        assert(!strcmp(AjiLexEnv_GetcStdoutBufSuperGlobal(lex_env), "111"
            "222\n333\t"));
    }
        '''
        el = parse(s, 0)
        self.assertEqual(el[1], False)
        self.assertEqual(el[2], '"{: 1 :}"<!>"hige\thige\n"')
        self.assertEqual(el[3], '"111"<!>"222\n333\t"')

        s = '''
    AjiTkr_Parse(tkr, "{@\n"
    "   import \\"abc\\"\n"
    "@}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        AjiLexEnv_Clear(lex_env);
        AjiTrv_Trav(ast, lex_env);
        assert(!AjiAST_HasErrs(ast));
        assert(!strcmp(AjiLexEnv_GetcStdoutBufSuperGlobal(lex_env), "STRING"));
    }
        '''

        el = parse(s, 0)
        self.assertEqual(el[1], True)
        self.assertEqual(el[2], '"{@\n"<!>"   import \\"abc\\"\n"<!>"@}"')
        self.assertEqual(el[3], '"STRING"')

        s = '''
    AjiTkr_Parse(tkr, "{@\n"
    "   import \\"tests/lang/modules/string.aji\\" as string\n"
    "@}{: string.variable.upper() :}");
    {
        AjiAST_Clear(ast);
        AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
        AjiLexEnv_Clear(lex_env);
        AjiTrv_Trav(ast, lex_env);
        assert(!AjiAST_HasErrs(ast));
        assert(!strcmp(AjiLexEnv_GetcStdoutBufSuperGlobal(lex_env), "STRING"));
    }
        '''
        el = parse(s, 0)
        self.assertEqual(el[1], True)
        self.assertEqual(el[2], '"{@\n"<!>"   import \\"tests/lang/modules/string.aji\\" as string\n"<!>"@}{: string.variable.upper() :}"')
        self.assertEqual(el[3], '"STRING"')
