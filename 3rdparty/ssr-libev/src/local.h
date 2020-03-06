/*
 * local.h - Define the client's buffers and callbacks
 *
 * Copyright (C) 2013 - 2016, Max Lv <max.c.lv@gmail.com>
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

#ifndef _LOCAL_H
#define _LOCAL_H

#ifdef __cplusplus
extern "C" {
#endif
#include <ev.h>
#include <libcork/ds.h>

#include "encrypt.h"
#include "jconf.h"
#include "protocol.h"

#include "common.h"

// use this as a profile or environment
typedef struct listen_ctx{
    ev_io io;
    ss_addr_t tunnel_addr;

    struct cork_dllist_item entries; // for inactive profile list
    struct cork_dllist connections_eden; // For connections just created but not attach to a server

    char *iface;
    int timeout;
    int fd;
    int mptcp;

    int server_num;
    server_def_t servers[MAX_SERVER_NUM];
} listen_ctx_t;

typedef struct server_ctx {
    ev_io io;
    int connected;
    struct server *server;
} server_ctx_t;

typedef struct remote_ctx {
    ev_io io;
    ev_timer watcher;
    int connected;
    struct remote *remote;
} remote_ctx_t;

typedef struct remote {
    int fd;
    buffer_t *buf;
    remote_ctx_t *recv_ctx;
    remote_ctx_t *send_ctx;
    uint32_t counter;
    struct server *server;

    int direct;
    struct { // direct = 1
        struct sockaddr_storage addr;
        int addr_len;
    } direct_addr;
} remote_t;

typedef struct server {
    int fd;
    char stage;
    enc_ctx_t *e_ctx;
    enc_ctx_t *d_ctx;
    server_ctx_t *recv_ctx;
    server_ctx_t *send_ctx;
    listen_ctx_t *listener;
    remote_t *remote;

    buffer_t *buf;

    struct cork_dllist_item entries;
    struct cork_dllist_item entries_all; // for all_connections

    server_def_t *server_env;

    // SSR
    obfs *protocol;
    obfs *obfs;
} server_t;

#ifdef __cplusplus
}
#endif
#endif // _LOCAL_H
