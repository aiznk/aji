/**
 * parse command line with pipe (|) and operator (&&, &) and redirect (>)
 *
 * @example
 *
 *     AjiCmdline *cmdline = AjiCmdline_New();
 *     AjiCmdline_Parse(cmdline, "/bin/date-line.py | /bin/lstab.py");
 *     AjiCmdline_Del(cmdline);
 */
#pragma once

#include <stdint.h>

#include <aji/lib/memory.h>
#include <aji/lib/cl.h>
#include <aji/lib/string.h>

/**
 * number of type of cmdline objects
 *
 * the command line
 *
 *     cat text | cat text && make
 *
 * structure is
 *
 *     CMD PIPE CMD AND CMD
 *
 * command line objects
 */
typedef enum {
    AJI_CMDLINE_OBJ_TYPE__CMD,
    AJI_CMDLINE_OBJ_TYPE__PIPE,
    AJI_CMDLINE_OBJ_TYPE__AND,
    AJI_CMDLINE_OBJ_TYPE__REDIRECT,
} AjiCmdlineObjType;

/**
 * structure of AjiCmdlineObj
 * this is element in vec of cmdline object
 * DO NOT DELETE command, and cl object
 */
struct AjiCmdlineObj {
    AjiCmdlineObjType type;
    AjiStr *command;
    AjiCL *cl;
};

struct AjiCmdlineObj;
typedef struct AjiCmdlineObj AjiCmdlineObj;

struct AjiCmdline;
typedef struct AjiCmdline AjiCmdline;

/*****************
* AjiCmdlineObj *
*****************/

/**
 * destruct AjiObj
 *
 * @param[in] *self pointer to AjiCmdlineObj dynamic allocate memory
 */
void
AjiCmdlineObj_Del(AjiCmdlineObj *self);

/**
 * construct AjiObj
 *
 * @param[in] type number of type of object
 *
 * @return pointer to AjiCmdlineObj dynamic allocate memory
 */
AjiCmdlineObj *
AjiCmdlineObj_New(AjiCmdlineObjType type);

/**
 * parse text line
 *
 * @param[in] *self pointer to AjiCmdlineObj dynamic allocate memory
 * @param[in] *line pointer to strings
 *
 * @return success to pointer to AjiCmdlineObj dynamic allocate memory
 * @return failed to pointer to NULL
 */
AjiCmdlineObj *
AjiCmdlineObj_Parse(AjiCmdlineObj *self, const char *line);

void
AjiCmdlineObj_Show(const AjiCmdlineObj *self, FILE *fout);

/**********
* cmdline *
**********/

/**
 * destruct AjiObj
 *
 * @param[in] *self pointer to AjiCmdline dynamic allocate memory
 */
void
AjiCmdline_Del(AjiCmdline *self);

/**
 * construct AjiObj
 *
 * @return pointer to AjiCmdline dynamic allocate memory
 */
AjiCmdline *
AjiCmdline_New(void);

/**
 * resize objects vec
 *
 * @param[in] *self pointer to AjiCmdline dynamic allocate memory
 * @param[in] capa  number of new capacity
 *
 * @return success to pointer to AjiCmdline dynamic allocate memory
 * @return failed to pointer to NULL
 */
AjiCmdline *
AjiCmdline_Resize(AjiCmdline *self, int32_t capa);

/**
 * move back object at vec
 *
 * @param[in] *self     pointer to AjiCmdline dynamic allocate memory
 * @param[in] *move_obj pointer to object with move semantics
 *
 * @return success to pointer to AjiCmdline dynamic allocate memory
 * @return failed to pointer to NULL
 */
AjiCmdline *
AjiCmdline_MoveBack(AjiCmdline *self, AjiCmdlineObj *move_obj);

/**
 * if object has error then return true
 *
 * @param[in] *self pointer to AjiCmdline dynamic allocate memory
 *
 * @return if has error then true
 * @return if not has error then false
 */
bool
AjiCmdline_HasErr(const AjiCmdline *self);

/**
 * get length of objects vec
 *
 * @param[in] *self pointer to AjiCmdline dynamic allocate memory
 *
 * @return number of length of vec
 */
int32_t
AjiCmdline_Len(const AjiCmdline *self);

/**
 * get read-only object from vec
 *
 * @param[in] *self pointer to AjiCmdline dynamic allocate memory
 * @param[in] index number of index of vec
 *
 * @return success to pointer to AjiCmdlineObj dynamic allocate memory
 * @return failed to pointer to NULL
 */
const AjiCmdlineObj *
AjiCmdline_Getc(const AjiCmdline *self, int32_t index);

/**
 * parse text line
 *
 * @param[in] *self pointer to AjiCmdline dynamic allocate memory
 * @param[in] *line pointer to strings
 *
 * @return success to pointer to AjiCmdline dynamic allocate memory
 * @return failed to pointer to NULL
 */
AjiCmdline *
AjiCmdline_Parse(AjiCmdline *self, const char *line);

/**
 * clear state
 *
 * @param[in] *self pointer to AjiCmdline dynamic allocate memory
 */
void 
AjiCmdline_Clear(AjiCmdline *self);

void
AjiCmdline_Show(AjiCmdline *self, FILE *fout);
