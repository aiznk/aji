l = [
    'AJI_OBJ_TYPE__NIL',
    'AJI_OBJ_TYPE__INT',
    'AJI_OBJ_TYPE__FLOAT',
    'AJI_OBJ_TYPE__BOOL',
    'AJI_OBJ_TYPE__IDENT',
    'AJI_OBJ_TYPE__UNICODE',
    'AJI_OBJ_TYPE__VECTOR',
    'AJI_OBJ_TYPE__DICT',
    'AJI_OBJ_TYPE__DEF_STRUCT',
    'AJI_OBJ_TYPE__DEF_ENUM',
    'AJI_OBJ_TYPE__OBJECT',
    'AJI_OBJ_TYPE__FUNC',
    'AJI_OBJ_TYPE__RING',
    'AJI_OBJ_TYPE__MODULE',
    'AJI_OBJ_TYPE__OWNERS_METHOD',
    'AJI_OBJ_TYPE__TYPE',
    'AJI_OBJ_TYPE__BLTIN_FUNC',
    'AJI_OBJ_TYPE__FILE',
    'AJI_OBJ_TYPE__PTR',
    'AJI_OBJ_TYPE__SOCKET',
]

import sys

s = sys.stdin.read()
for t in l:
    if t not in s:
        print(t)
        