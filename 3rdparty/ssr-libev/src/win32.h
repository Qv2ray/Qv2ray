/*
 * win32.h - Win32 port helpers
 *
 * Copyright (C) 2014, Linus Yang <linusyang@gmail.com>
 *
 * This file is part of the shadowsocks-libev.
 *
 * shadowsocks-libev is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * shadowsocks-libev is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with shadowsocks-libev; see the file COPYING. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef _WIN32_H
#define _WIN32_H

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif

#define _WIN32_WINNT 0x0501

#include <winsock2.h>
#include <ws2tcpip.h>

#ifdef EWOULDBLOCK
#undef EWOULDBLOCK
#endif

#ifdef errno
#undef errno
#endif

#ifdef ERROR
#undef ERROR
#endif

#ifndef AI_ALL
#define AI_ALL 0x00000100
#endif

#ifndef AI_ADDRCONFIG
#define AI_ADDRCONFIG 0x00000400
#endif

#ifndef AI_V4MAPPED
#define AI_V4MAPPED 0x00000800
#endif

#ifndef IPV6_V6ONLY
#define IPV6_V6ONLY 27 // Treat wildcard bind as AF_INET6-only.
#endif

#define EWOULDBLOCK WSAEWOULDBLOCK
#define errno WSAGetLastError()
#define close(fd) closesocket(fd)
#define ERROR(s) ss_error(s)
#define setsockopt(a, b, c, d, e) setsockopt(a, b, c, (char *)(d), e)

void winsock_init(void);
void winsock_cleanup(void);
void ss_error(const char *s);
size_t strnlen(const char *s, size_t maxlen);
int setnonblocking(int fd);
const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);

#endif
