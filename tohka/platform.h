//
// Created by li on 2022/3/5.
//

#ifndef TOHKA_TOHKA_PLATFORM_H
#define TOHKA_TOHKA_PLATFORM_H

// modified from https://github.com/ithewei/libhv/blob/master/base/hplatform.h
//  OS
#if defined(WIN64) || defined(_WIN64)
#define OS_WIN64
#define OS_WIN32
#elif defined(WIN32) || defined(_WIN32)
#define OS_WIN32
#elif defined(ANDROID) || defined(__ANDROID__)
#define OS_ANDROID
#define OS_LINUX
#elif defined(linux) || defined(__linux) || defined(__linux__)
#define OS_LINUX
#elif defined(__APPLE__) || defined(__DARWIN__)
#define OS_MAC
#else
#warning "Untested operating system platform!"
#endif

#if defined(OS_WIN32) || defined(OS_WIN64)
#undef OS_UNIX
#define OS_WIN
#else
#define OS_UNIX
#endif

// headers
#ifdef OS_WIN
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <direct.h>   // for mkdir,rmdir,chdir,getcwd
#include <io.h>       // for open,Close,read,write,lseek,tell
#include <process.h>  // for getpid,exec
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>  // for inet_pton,inet_ntop

// access
#ifndef F_OK
#define F_OK 0 /* test for existence of file */
#endif
#ifndef X_OK
#define X_OK (1 << 0) /* test for execute or search permission */
#endif
#ifndef W_OK
#define W_OK (1 << 1) /* test for write permission */
#endif
#ifndef R_OK
#define R_OK (1 << 2) /* test for read permission */
#endif

// stat
#ifndef S_ISREG
#define S_ISREG(st_mode) (((st_mode)&S_IFMT) == S_IFREG)
#endif
#ifndef S_ISDIR
#define S_ISDIR(st_mode) (((st_mode)&S_IFMT) == S_IFDIR)
#endif
#else

// unix
#include <signal.h>
#include <sys/fcntl.h>
#include <sys/uio.h>
#include <unistd.h>

// socket
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#endif

// ANSI C
#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
// c++ header
#include <algorithm>
#include <any>
#include <chrono>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#if HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#if HAVE_FCNTL_H
#include <fcntl.h>
#endif

#endif  // TOHKA_TOHKA_PLATFORM_H
