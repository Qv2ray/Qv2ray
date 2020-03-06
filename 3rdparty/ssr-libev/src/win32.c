/*
 * win32.c - Win32 port helpers
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

#include "win32.h"
#include "utils.h"

#ifdef setsockopt
#undef setsockopt
#endif

void
winsock_init(void)
{
    WORD wVersionRequested;
    WSADATA wsaData;
    int ret;
    wVersionRequested = MAKEWORD(1, 1);
    ret               = WSAStartup(wVersionRequested, &wsaData);
    if (ret != 0) {
        FATAL("Could not initialize winsock");
    }
    if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1) {
        WSACleanup();
        FATAL("Could not find a usable version of winsock");
    }
}

void
winsock_cleanup(void)
{
    WSACleanup();
}

void
ss_error(const char *s)
{
    LPVOID *msg = NULL;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&msg, 0, NULL);
    if (msg != NULL) {
        LOGE("%s: %s", s, (char *)msg);
        LocalFree(msg);
    }
}

int
setnonblocking(int fd)
{
    u_long iMode = 1;
    long int iResult;
    iResult = ioctlsocket(fd, FIONBIO, &iMode);
    if (iResult != NO_ERROR) {
        LOGE("ioctlsocket failed with error: %ld\n", iResult);
    }
    return iResult;
}

size_t
strnlen(const char *s, size_t maxlen)
{
    const char *end = memchr(s, 0, maxlen);
    return end ? (size_t)(end - s) : maxlen;
}

const char *
inet_ntop(int af, const void *src, char *dst, socklen_t size)
{
    struct sockaddr_storage ss;
    unsigned long s = size;
    ZeroMemory(&ss, sizeof(ss));
    ss.ss_family = af;
    switch (af) {
    case AF_INET:
        ((struct sockaddr_in *)&ss)->sin_addr = *(struct in_addr *)src;
        break;
    case AF_INET6:
        ((struct sockaddr_in6 *)&ss)->sin6_addr = *(struct in6_addr *)src;
        break;
    default:
        return NULL;
    }
    return (WSAAddressToString((struct sockaddr *)&ss, sizeof(ss), NULL, dst,
                               &s) == 0) ? dst : NULL;
}
