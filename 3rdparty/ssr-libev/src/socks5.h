/*
 * socks5.h - Define SOCKS5's header
 *
 * Copyright (C) 2013, clowwindy <clowwindy42@gmail.com>
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

#ifndef _SOCKS5_H
#define _SOCKS5_H

#ifdef __cplusplus
extern "C" {
#endif
#define SVERSION 0x05
#define CONNECT 0x01
#define IPV4 0x01
#define DOMAIN 0x03
#define IPV6 0x04
#define CMD_NOT_SUPPORTED 0x07

#pragma pack(push)
#pragma pack(1)

struct method_select_request {
    char ver;
    char nmethods;
    char methods[255];
};

struct method_select_response {
    char ver;
    char method;
};

struct socks5_request {
    char ver;
    char cmd;
    char rsv;
    char atyp;
};

struct socks5_response {
    char ver;
    char rep;
    char rsv;
    char atyp;
};

#pragma pack(pop)

#ifdef __cplusplus
}
#endif
#endif // _SOCKS5_H
