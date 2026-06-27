/**
 * Aji
 *
 * License: MIT
 *  Author: noname
 *   Since: 2016
 */
#pragma once

#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdbool.h>

#include <aji/lib/windows.h>
#include <aji/lib/memory.h>

#if defined(_WIN32) || defined(_WIN64)
	#define AJI_SOCK__WINDOWS 1
#endif

#ifdef AJI_SOCK__WINDOWS
	#pragma comment(lib, "ws2_32.dll")
#endif

struct AjiSock;
typedef struct AjiSock AjiSock;

/**
 * dump parameters of socket to stream
 *
 * @param[in] self
 */
void
AjiSock_Dump(const AjiSock *self, FILE *fout);

/**
 * close socket
 * if self is NULL then don't anything
 *
 * @param[in] self
 *
 * @return success to number of zero
 * @return failed to under number of zero
 */
int32_t
AjiSock_Close(AjiSock *self);

/**
 * open socket by source and mode like a fopen(3)
 * the open modes are:
 *
 * 		tcp-client
 * 		tcp-server
 *
 * @param[in] src  format of "host:port" of C string
 * @param[in] mode open mode
 *
 * @return pointer to dynamic allocate memory of struct cap_socket
 */
AjiSock *
AjiSock_Open(const char *src, const char *mode);

/**
 * get host of C string by socket
 *
 * @param[in] self
 *
 * @return pointer to memory of C string
 */
const char *
AjiSock_GetcHost(const AjiSock *self);

/**
 * get port of C string by socket
 *
 * @param[in] self
 *
 * @return pointer to memory of C string
 */
const char *
AjiSock_GetcPort(const AjiSock *self);

/**
 * wrapper of accept(2)
 * get new socket by self socket
 *
 * @param[in] self
 *
 * @return pointer to dynamic allocate memory of struct cap_socket of client
 */
AjiSock *
AjiSock_Accept(AjiSock *self);

/**
 * wrapper of recv(2)
 * recv string from socket
 *
 * @param[in] self
 * @param[in] dst pointer to memory of destination buffer
 * @param[in] dstsz number of size of destination buffer
 *
 * @return success to number of recv size
 * @return failed to number of under of zero
 */
int32_t
AjiSock_RecvStr(AjiSock *self, char *dst, int32_t dstsz);

int32_t
AjiSock_Recv(AjiSock *self, void *dst, int32_t dstsz);

/**
 * wrapper of send(2)
 * send string to socket
 *
 * @param[in] self
 * @param[in] str send C string
 *
 * @return success to number of send size
 * @return failed to number of under of zero
 */
int32_t
AjiSock_SendStr(AjiSock *self, const char *str);

/**
 * wrapper of send(2)
 * send bytes to socket
 *
 * @param[in] self
 * @param[in] bytes pointer to memory of bytes
 * @param[in] size number of size of bytes
 *
 * @return success to number of send size
 * @return failed to number of under of zero
 */
int32_t
AjiSock_Send(AjiSock *self, void *data, int32_t datasz);

/**
 * set error
 *
 * @param[in] *self
 * @param[in] *fmt
 */
void
AjiSock_SetErr(AjiSock *self, const char *fmt, ...);

/**
 * get error message
 *
 * @param[in] *self
 *
 * @return poitner to C strings
 */
const char *
AjiSock_GetcErr(const AjiSock *self);

/**
 * if AjiSock has error then return true else return false
 *
 * @param[in] *self
 *
 * @return true or false
 */
bool
AjiSock_HasErr(const AjiSock *self);
