#pragma once
#include <stdint.h>

struct AjiObjDict;
typedef struct AjiObjDict AjiObjDict;

struct AjiObjVec;
typedef struct AjiObjVec AjiObjVec;

struct AjiObj;
typedef struct AjiObj AjiObj;

struct AjiFuncObj;
typedef struct AjiFuncObj AjiFuncObj;

struct AjiIndexObj;
typedef struct AjiIndexObj AjiIndexObj;

struct AjiModObj;
typedef struct AjiModObj AjiModObj;

struct AjiIdentObj;
typedef struct AjiIdentObj AjiIdentObj;

struct AjiDefStructObj;
typedef struct AjiDefStructObj AjiDefStructObj;

struct AjiDefEnumObj;
typedef struct AjiDefEnumObj AjiDefEnumObj;

struct AjiObjObj;
typedef struct AjiObjObj AjiObjObj;

struct AjiRingObj;
typedef struct AjiRingObj AjiRingObj;

struct AjiOwnsMethodObj;
typedef struct AjiOwnsMethodObj AjiOwnsMethodObj;

struct AjiTypeObj;
typedef struct AjiTypeObj AjiTypeObj;

struct AjiBltFuncObj;
typedef struct AjiBltFuncObj AjiBltFuncObj;

struct AjiFileObj;
typedef struct AjiFileObj AjiFileObj;

struct AjiPtrObj;
typedef struct AjiPtrObj AjiPtrObj;

struct AjiSockObj;
typedef struct AjiSockObj AjiSockObj;

struct AjiNodeVec;
typedef struct AjiNodeVec AjiNodeVec;

struct AjiNodeDict;
typedef struct AjiNodeDict AjiNodeDict;

struct AjiTkr;
typedef struct AjiTkr AjiTkr;

struct AjiCtx;
typedef struct AjiCtx AjiCtx;

struct AjiScope;
typedef struct AjiScope AjiScope;

struct AjiAST;
typedef struct AjiAST AjiAST;

struct AjiCCArgs;
typedef struct AjiCCArgs AjiCCArgs;

struct AjiTrvArgs;
typedef struct AjiTrvArgs AjiTrvArgs;

struct AjiBltFuncArgs;
typedef struct AjiBltFuncArgs AjiBltFuncArgs;

struct AjiUtilsArgs;
typedef struct AjiUtilsArgs AjiUtilsArgs;

typedef AjiObj *(*AjiBltFunc)(AjiBltFuncArgs *args);

struct AjiBltFuncInfo;
typedef struct AjiBltFuncInfo AjiBltFuncInfo;

struct AjiBltFuncInfoVec;
typedef struct AjiBltFuncInfoVec AjiBltFuncInfoVec;

struct AjiGC;
typedef struct AjiGC AjiGC;

struct AjiGCItem;
typedef struct AjiGCItem AjiGCItem;

typedef long AjiIntObj;
typedef double AjiFloatObj;

// number of depth type for function recursion
typedef int32_t AjiDepth;

struct AjiChainNode;
typedef struct AjiChainNode AjiChainNode;

struct AjiChainNodes;
typedef struct AjiChainNodes AjiChainNodes;

struct AjiChainObj;
typedef struct AjiChainObj AjiChainObj;

struct AjiChainObjs;
typedef struct AjiChainObjs AjiChainObjs;

struct AjiNode;
typedef struct AjiNode AjiNode;

struct AjiKit;
typedef struct AjiKit AjiKit;

struct AjiImporter;
typedef struct AjiImporter AjiImporter;

typedef char *(* AjiImporterFixPathFunc)(AjiImporter *, char *, int32_t, const char *);

typedef char *(* AjiOpenFixPathFunc)(AjiBltFuncArgs *, char *, int32_t, const char *);

struct AjiLexEnv;
typedef struct AjiLexEnv AjiLexEnv;

struct AjiLexEnvList;
typedef struct AjiLexEnvList AjiLexEnvList;

struct AjiLexEnvListItem;
typedef struct AjiLexEnvListItem AjiLexEnvListItem;

struct AjiReferResult;
typedef struct AjiReferResult AjiReferResult;
