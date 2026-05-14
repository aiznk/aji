#include <aji/lang/excepts.h>

const char *
AjiExc_ToStr(AjiExc exc) {
    switch (exc) {
    case AJI_EXC__USERS_DEF: return "UsersDefinition"; break;
    case AJI_EXC__EXCEPTION: return "Exception"; break;
    case AJI_EXC__VALUE_ERR: return "ValueError"; break;
    case AJI_EXC__TYPE_ERR: return "TypeError"; break;
    case AJI_EXC__SYNTAX_ERR: return "SyntaxError"; break;
    case AJI_EXC__LOOK_UP_ERR: return "LookUpError"; break;
    case AJI_EXC__CONSTRUCT_ERR: return "ConstructError"; break;
    case AJI_EXC__IMPORT_ERR: return "ImportError"; break;
    case AJI_EXC__INJECT_ERR: return "InjectError"; break;
    case AJI_EXC__INTERNAL_ERR: return "InternalError"; break;
    case AJI_EXC__ATTR_ERR: return "AttributeError"; break;
    case AJI_EXC__REFER_ERR: return "ReferError"; break;
    case AJI_EXC__INDEX_ERR: return "IndexError"; break;
    case AJI_EXC__ZERO_DIV_ERR: return "ZeroDivisionError"; break;
    case AJI_EXC__KEY_ERR: return "KeyError"; break;
    case AJI_EXC__NAME_ERR: return "NameError"; break;
    case AJI_EXC__ARGS_ERR: return "ArgumentsError"; break;
    case AJI_EXC__ASSERT_ERR: return "AssertionError"; break;
    case AJI_EXC__INVOKE_ERR: return "InvokeError"; break;
    case AJI_EXC__OS_ERR: return "OSError"; break;
    case AJI_EXC__FILE_NOT_FOUND_ERR: return "FileNotFoundError"; break;
    case AJI_EXC__RUNTIME_ERR: return "RuntimeError"; break;
    case AJI_EXC__EXIT_ERR: return "ExitError"; break;
    case AJI_EXC__CONST_ERR: return "ConstError"; break;
    case AJI_EXC__STACK_OVERFLOW_ERR: return "StackOverflowError"; break;
    case AJI_EXC__NIL_PTR_ERR: return "NilPointerError"; break;
    }
    return NULL;
}

AjiExc
AjiExc_StrToExc(const char *str) {
    if (AjiCStr_Eq(str, "ValueError")) {
        return AJI_EXC__VALUE_ERR;
    } else if (AjiCStr_Eq(str, "Exception")) {
        return AJI_EXC__EXCEPTION;
    } else if (AjiCStr_Eq(str, "TypeError")) {
        return AJI_EXC__TYPE_ERR;
    } else if (AjiCStr_Eq(str, "SyntaxError")) {
        return AJI_EXC__SYNTAX_ERR;
    } else if (AjiCStr_Eq(str, "LookUpError")) {
        return AJI_EXC__LOOK_UP_ERR;
    } else if (AjiCStr_Eq(str, "ConstructError")) {
        return AJI_EXC__CONSTRUCT_ERR;
    } else if (AjiCStr_Eq(str, "ImportError")) {
        return AJI_EXC__IMPORT_ERR;
    } else if (AjiCStr_Eq(str, "InjectError")) {
        return AJI_EXC__INJECT_ERR;
    } else if (AjiCStr_Eq(str, "InternalError")) {
        return AJI_EXC__INTERNAL_ERR;
    } else if (AjiCStr_Eq(str, "AttributeError")) {
        return AJI_EXC__ATTR_ERR;
    } else if (AjiCStr_Eq(str, "ReferError")) {
        return AJI_EXC__REFER_ERR;
    } else if (AjiCStr_Eq(str, "IndexError")) {
        return AJI_EXC__INDEX_ERR;
    } else if (AjiCStr_Eq(str, "ZeroDivisionError")) {
        return AJI_EXC__ZERO_DIV_ERR;
    } else if (AjiCStr_Eq(str, "KeyError")) {
        return AJI_EXC__KEY_ERR;
    } else if (AjiCStr_Eq(str, "NameError")) {
        return AJI_EXC__NAME_ERR;
    } else if (AjiCStr_Eq(str, "ArgumentsError")) {
        return AJI_EXC__ARGS_ERR;
    } else if (AjiCStr_Eq(str, "AssertionError")) {
        return AJI_EXC__ASSERT_ERR;
    } else if (AjiCStr_Eq(str, "InvokeError")) {
        return AJI_EXC__INVOKE_ERR;
    } else if (AjiCStr_Eq(str, "OSError")) {
        return AJI_EXC__OS_ERR;
    } else if (AjiCStr_Eq(str, "FileNotFoundError")) {
        return AJI_EXC__FILE_NOT_FOUND_ERR;
    } else if (AjiCStr_Eq(str, "RuntimeError")) {
        return AJI_EXC__RUNTIME_ERR;
    } else if (AjiCStr_Eq(str, "ExitError")) {
        return AJI_EXC__EXIT_ERR;
    } else if (AjiCStr_Eq(str, "ConstError")) {
        return AJI_EXC__CONST_ERR;
    } else if (AjiCStr_Eq(str, "StackOverflowError")) {
        return AJI_EXC__STACK_OVERFLOW_ERR;
    } else if (AjiCStr_Eq(str, "NilPointerError")) {
        return AJI_EXC__NIL_PTR_ERR;
    } else {
        return AJI_EXC__USERS_DEF;
    }
}

