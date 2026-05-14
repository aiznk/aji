#pragma once

#include <stdio.h>

#include <aji/lib/error.h>
#include <aji/lib/cstring.h>

typedef enum {
    AJI_EXC__USERS_DEF,
    AJI_EXC__EXCEPTION,
    AJI_EXC__VALUE_ERR,
    AJI_EXC__TYPE_ERR,
    AJI_EXC__SYNTAX_ERR,
    AJI_EXC__RUNTIME_ERR,
    AJI_EXC__LOOK_UP_ERR,
    AJI_EXC__CONSTRUCT_ERR,
    AJI_EXC__IMPORT_ERR,
    AJI_EXC__INJECT_ERR,
    AJI_EXC__INTERNAL_ERR,
    AJI_EXC__ATTR_ERR,
    AJI_EXC__REFER_ERR,
    AJI_EXC__INDEX_ERR,
    AJI_EXC__ZERO_DIV_ERR,
    AJI_EXC__KEY_ERR,
    AJI_EXC__NAME_ERR,
    AJI_EXC__ARGS_ERR,
    AJI_EXC__ASSERT_ERR,
    AJI_EXC__INVOKE_ERR,
    AJI_EXC__OS_ERR,
    AJI_EXC__FILE_NOT_FOUND_ERR,
    AJI_EXC__EXIT_ERR,
    AJI_EXC__CONST_ERR,
    AJI_EXC__STACK_OVERFLOW_ERR,
    AJI_EXC__NIL_PTR_ERR,
} AjiExc;

const char *
AjiExc_ToStr(AjiExc exc);

AjiExc
AjiExc_StrToExc(const char *str);
