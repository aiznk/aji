#pragma once

#if defined(_WIN32) || defined(_WIN64)
# undef AJI__WINDOWS
# define AJI__WINDOWS 1
#endif

#if defined(AJI__WINDOWS)
// lang/builtin/modules/socket.h
# include <winsock2.h>
# include <ws2tcpip.h>
# include <windows.h>
// lib/term.h
# include <wincon.h>
#else
# undef _DEFAULT_SOURCE
# define _DEFAULT_SOURCE 1 /* aji: lib/windows.h: netdb.h */
# undef __USE_POSIX
# define __USE_POSIX 1 /* aji: lib/windows.h: netdb.h */
# undef __USE_XOPEN2K
# define __USE_XOPEN2K 1 /* aji: lib/windows.h: netdb.h */
// lang/builtin/modules/socket.h
# include <netdb.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
#endif
