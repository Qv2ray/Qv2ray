/*
 * local.c - Setup a socks5 proxy through remote shadowsocks server
 *
 * Copyright (C) 2013 - 2015, Max Lv <max.c.lv@gmail.com>
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <locale.h>
#include <signal.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <getopt.h>

#ifndef __MINGW32__
#include <errno.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#endif

#ifdef LIB_ONLY
#include <pthread.h>
#include "shadowsocks.h"
#endif

#if defined(HAVE_SYS_IOCTL_H) && defined(HAVE_NET_IF_H) && defined(__linux__)
#include <net/if.h>
#include <sys/ioctl.h>
#define SET_INTERFACE
#endif
#include <libcork/core.h>
#include <udns.h>

#ifdef __MINGW32__
#include "win32.h"
#endif

#include "netutils.h"
#include "utils.h"
#include "socks5.h"
#include "acl.h"
#include "http.h"
#include "tls.h"
#include "local.h"

#ifndef LIB_ONLY
#ifdef __APPLE__
#include <AvailabilityMacros.h>
#if defined(MAC_OS_X_VERSION_10_10) && MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_10
#include <launch.h>
#define HAVE_LAUNCHD
#endif
#endif
#endif

#ifndef EAGAIN
#define EAGAIN EWOULDBLOCK
#endif

#ifndef EWOULDBLOCK
#define EWOULDBLOCK EAGAIN
#endif

#ifndef BUF_SIZE
#define BUF_SIZE 2048
#endif

int verbose = 0;
int keep_resolving = 1;

static int log_tx_rx  = 1;
static uint64_t tx    = 0;
static uint64_t rx    = 0;
static ev_tstamp last = 0;

#include "obfs/auth.h"
#include "obfs/auth_chain.h"
#include "obfs/base64.h"
#include "obfs/crc32.h"
#include "obfs/http_simple.h"
#include "obfs/obfsutil.h"
#include "obfs/tls1.2_ticket.h"
#include "obfs/verify.h"
#include "jconf.h"
#include "obfs/obfs.h"

static int acl       = 0;
static int mode = TCP_ONLY;
static int ipv6first = 0;
static bool stop=false;
static int fast_open = 0;
static struct ev_loop* loop=NULL;
static struct ev_timer* stop_watcher=NULL;
#ifdef HAVE_SETRLIMIT
#ifndef LIB_ONLY
static int nofile = 0;
#endif
#endif

static void server_recv_cb(EV_P_ ev_io *w, int revents);
static void server_send_cb(EV_P_ ev_io *w, int revents);
static void remote_recv_cb(EV_P_ ev_io *w, int revents);
static void remote_send_cb(EV_P_ ev_io *w, int revents);
static void accept_cb(EV_P_ ev_io *w, int revents);
static void signal_cb(EV_P_ ev_signal *w, int revents);

static int create_and_bind(const char *addr, const char *port);
#ifdef HAVE_LAUNCHD
static int launch_or_create(const char *addr, const char *port);
#endif
static remote_t *create_remote(listen_ctx_t *listener, struct sockaddr *addr);
static void free_remote(remote_t *remote);
static void close_and_free_remote(EV_P_ remote_t *remote);
static void free_server(server_t *server);
static void close_and_free_server(EV_P_ server_t *server);

static remote_t *new_remote(int fd, int timeout);
static server_t *new_server(int fd, listen_ctx_t* profile);

static listen_ctx_t *current_profile;
static struct cork_dllist all_connections;

#ifndef __MINGW32__
int
setnonblocking(int fd)
{
    int flags;
    if (-1 == (flags = fcntl(fd, F_GETFL, 0))) {
        flags = 0;
    }
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

#endif

void
ev_io_remote_send(EV_P_ server_t* server, remote_t* remote)
{
    ev_io_stop(EV_A_ & remote->send_ctx->io);
    ev_io_start(EV_A_ & server->recv_ctx->io);
}

void
ev_io_remote_recv(EV_P_ server_t* server, remote_t* remote)
{
    ev_io_stop(EV_A_ & remote->recv_ctx->io);
    ev_io_start(EV_A_ & server->send_ctx->io);
}

void
ev_io_server_send(EV_P_ server_t* server, remote_t* remote)
{
    ev_io_stop(EV_A_ & server->send_ctx->io);
    ev_io_start(EV_A_ & remote->recv_ctx->io);
}

void
ev_io_server_recv(EV_P_ server_t* server, remote_t* remote)
{
    ev_io_stop(EV_A_ & server->recv_ctx->io);
    ev_io_start(EV_A_ & remote->send_ctx->io);
}

int
create_and_bind(const char *addr, const char *port)
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int s, listen_sock;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family   = AF_UNSPEC;   /* Return IPv4 and IPv6 choices */
    hints.ai_socktype = SOCK_STREAM; /* We want a TCP socket */

    s = getaddrinfo(addr, port, &hints, &result);
    if (s != 0) {
        LOGI("getaddrinfo: %s", gai_strerror(s));
        return -1;
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        listen_sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (listen_sock == -1) {
            continue;
        }

        int opt = 1;
        setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#ifdef SO_NOSIGPIPE
        setsockopt(listen_sock, SOL_SOCKET, SO_NOSIGPIPE, &opt, sizeof(opt));
#endif
        int err = set_reuseport(listen_sock);
        if (err == 0) {
            LOGI("tcp port reuse enabled");
        }

        s = bind(listen_sock, rp->ai_addr, rp->ai_addrlen);
        if (s == 0) {
            /* We managed to bind successfully! */
            break;
        } else {
            ERROR("bind");
        }

        close(listen_sock);
    }

    if (rp == NULL) {
        LOGE("Could not bind");
        return -1;
    }

    freeaddrinfo(result);

    return listen_sock;
}

#ifdef HAVE_LAUNCHD
int
launch_or_create(const char *addr, const char *port)
{
    int *fds;
    size_t cnt;
    int error = launch_activate_socket("Listeners", &fds, &cnt);
    if (error == 0) {
        if (cnt == 1) {
            return fds[0];
        } else {
            FATAL("please don't specify multi entry");
        }
    } else if (error == ESRCH || error == ENOENT) {
        /* ESRCH:  The calling process is not managed by launchd(8).
         * ENOENT: The socket name specified does not exist
         *          in the caller's launchd.plist(5).
         */
        if (port == NULL) {
            usage();
            exit(EXIT_FAILURE);
        }
        return create_and_bind(addr, port);
    } else {
        FATAL("launch_activate_socket() error");
    }
    return -1;
}
#endif

static void
free_connections(struct ev_loop *loop)
{
    struct cork_dllist_item *curr, *next;
    cork_dllist_foreach_void(&all_connections, curr, next) {
        server_t *server = cork_container_of(curr, server_t, entries_all);
        remote_t *remote = server->remote;
        close_and_free_remote(loop, remote);
        close_and_free_server(loop, server);
    }
}

static void
server_recv_cb(EV_P_ ev_io *w, int revents)
{
    server_ctx_t *server_recv_ctx = (server_ctx_t *)w;
    server_t *server              = server_recv_ctx->server;
    remote_t *remote              = server->remote;
    buffer_t *buf;
    ssize_t r;

    if (remote == NULL) {
        buf = server->buf;
    } else {
        buf = remote->buf;
    }

    r = recv(server->fd, buf->array + buf->len, BUF_SIZE - buf->len, 0);

    if (r == 0) {
        // connection closed
        close_and_free_remote(EV_A_ remote);
        close_and_free_server(EV_A_ server);
        return;
    } else if (r == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // no data
            // continue to wait for recv
            return;
        } else {
            if (verbose)
                ERROR("server_recv_cb_recv");
            close_and_free_remote(EV_A_ remote);
            close_and_free_server(EV_A_ server);
            return;
        }
    }

    buf->len += r;

    if (server->stage == STAGE_INIT) {
        char *host = server->listener->tunnel_addr.host;
        char *port = server->listener->tunnel_addr.port;
        if (host && port) {
            server->stage = STAGE_PARSE;
            int addr_len = strlen(host);
            int header_len = addr_len + 3 + 4;
            int port_num = atoi(port);
            memmove(buf->array + header_len, buf->array, buf->len);
            buf->len += header_len;
            buf->array[0] = 5;
            buf->array[1] = 1;
            buf->array[2] = 0;
            buf->array[3] = 3;
            buf->array[4] = addr_len;
            memcpy(buf->array + 5, host, addr_len);
            buf->array[addr_len + 5] = port_num >> 8;
            buf->array[addr_len + 6] = port_num;
        }
    }
    while (1) {
        // local socks5 server
        if (server->stage == STAGE_STREAM) {
            if (remote == NULL) {
                LOGE("invalid remote");
                close_and_free_server(EV_A_ server);
                return;
            }

            // insert shadowsocks header
            if (!remote->direct) {
                server_def_t *server_env = server->server_env;
                // SSR beg
                if (server_env->protocol_plugin) {
                    obfs_class *protocol_plugin = server_env->protocol_plugin;
                    if (protocol_plugin->client_pre_encrypt) {
                        remote->buf->len = protocol_plugin->client_pre_encrypt(server->protocol, &remote->buf->array, remote->buf->len, &remote->buf->capacity);
                    }
                }
                int err = ss_encrypt(&server_env->cipher, remote->buf, server->e_ctx, BUF_SIZE);

                if (err) {
                    LOGE("server invalid password or cipher");
                    close_and_free_remote(EV_A_ remote);
                    close_and_free_server(EV_A_ server);
                    return;
                }

                if (server_env->obfs_plugin) {
                    obfs_class *obfs_plugin = server_env->obfs_plugin;
                    if (obfs_plugin->client_encode) {
                        remote->buf->len = obfs_plugin->client_encode(server->obfs, &remote->buf->array, remote->buf->len, &remote->buf->capacity);
                    }
                }
                // SSR end
                if (log_tx_rx)
                    tx += buf->len;
            }

            if (!remote->send_ctx->connected) {
#ifdef ANDROID
                if (vpn) {
                    int not_protect = 0;
                    if (remote->direct_addr.addr.ss_family == AF_INET) {
                        struct sockaddr_in *s = (struct sockaddr_in *)&remote->direct_addr.addr;
                        if (s->sin_addr.s_addr == inet_addr("127.0.0.1"))
                            not_protect = 1;
                    }
                    if (!not_protect) {
                        if (protect_socket(remote->fd) == -1) {
                            ERROR("protect_socket");
                            close_and_free_remote(EV_A_ remote);
                            close_and_free_server(EV_A_ server);
                            return;
                        }
                    }
                }
#endif

                remote->buf->idx = 0;

                if (!fast_open || remote->direct) {
                    // connecting, wait until connected
                    int r = connect(remote->fd, (struct sockaddr *)&(remote->direct_addr.addr), remote->direct_addr.addr_len);

                    if (r == -1 && errno != CONNECT_IN_PROGRESS) {
                        ERROR("connect");
                        close_and_free_remote(EV_A_ remote);
                        close_and_free_server(EV_A_ server);
                        return;
                    }

                    // wait on remote connected event
                    ev_io_server_recv(EV_A_ server, remote);
                    ev_timer_start(EV_A_ & remote->send_ctx->watcher);
                } else {
#ifdef TCP_FASTOPEN
#ifdef __APPLE__
                    ((struct sockaddr_in *)&(remote->direct_addr.addr))->sin_len = sizeof(struct sockaddr_in);
                    sa_endpoints_t endpoints;
                    memset((char *)&endpoints, 0, sizeof(endpoints));
                    endpoints.sae_dstaddr    = (struct sockaddr *)&(remote->direct_addr.addr);
                    endpoints.sae_dstaddrlen = remote->direct_addr.addr_len;

                    int s = connectx(remote->fd, &endpoints, SAE_ASSOCID_ANY,
                                     CONNECT_RESUME_ON_READ_WRITE | CONNECT_DATA_IDEMPOTENT,
                                     NULL, 0, NULL, NULL);
                    if (s == 0) {
                        s = send(remote->fd, remote->buf->array, remote->buf->len, 0);
                    }
#else
                    int s = sendto(remote->fd, remote->buf->array, remote->buf->len, MSG_FASTOPEN,
                                   (struct sockaddr *)&(remote->direct_addr.addr), remote->direct_addr.addr_len);
#endif
                    if (s == -1) {
                        if (errno == CONNECT_IN_PROGRESS) {
                            // in progress, wait until connected
                            remote->buf->idx = 0;
                            ev_io_server_recv(EV_A_ server, remote);
                            return;
                        } else {
                            ERROR("sendto");
                            if (errno == ENOTCONN) {
                                LOGE("fast open is not supported on this platform");
                                // just turn it off
                                fast_open = 0;
                            }
                            close_and_free_remote(EV_A_ remote);
                            close_and_free_server(EV_A_ server);
                            return;
                        }
                    } else if (s < (int)(remote->buf->len)) {
                        remote->buf->len -= s;
                        remote->buf->idx  = s;

                        ev_io_server_recv(EV_A_ server, remote);
                        ev_timer_start(EV_A_ & remote->send_ctx->watcher);
                        return;
                    } else {
                        // Just connected
                        remote->buf->idx = 0;
                        remote->buf->len = 0;
#ifdef __APPLE__
                        ev_io_server_recv(EV_A_ server, remote);
                        ev_timer_start(EV_A_ & remote->send_ctx->watcher);
#else
                        remote->send_ctx->connected = 1;
                        ev_timer_stop(EV_A_ & remote->send_ctx->watcher);
                        ev_timer_start(EV_A_ & remote->recv_ctx->watcher);
                        ev_io_start(EV_A_ & remote->recv_ctx->io);
                        return;
#endif
                    }
#else
                    // if TCP_FASTOPEN is not defined, fast_open will always be 0
                    LOGE("can't come here");
                    exit(1);
#endif
                }
            } else {
                if (r > 0 && remote->buf->len == 0) {
                    remote->buf->idx = 0;
                    ev_io_stop(EV_A_ & server_recv_ctx->io);
                    return;
                }
                int s = send(remote->fd, remote->buf->array, remote->buf->len, 0);
                if (s == -1) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        // no data, wait for send
                        remote->buf->idx = 0;
                        ev_io_server_recv(EV_A_ server, remote);
                        return;
                    } else {
                        ERROR("server_recv_cb_send");
                        close_and_free_remote(EV_A_ remote);
                        close_and_free_server(EV_A_ server);
                        return;
                    }
                } else if (s < (int)(remote->buf->len)) {
                    remote->buf->len -= s;
                    remote->buf->idx  = s;
                    ev_io_server_recv(EV_A_ server, remote);
                    return;
                } else {
                    remote->buf->idx = 0;
                    remote->buf->len = 0;
                }
            }

            // all processed
            return;
        } else if (server->stage == STAGE_INIT) {
            struct method_select_response response;
            response.ver    = SVERSION;
            response.method = 0;
            char *send_buf = (char *)&response;
            send(server->fd, send_buf, sizeof(response), 0);
            server->stage = STAGE_HANDSHAKE;

            int off = (buf->array[1] & 0xff) + 2;
            if (buf->array[0] == 0x05 && off < (int)(buf->len)) {
                memmove(buf->array, buf->array + off, buf->len - off);
                buf->len -= off;
                continue;
            }

            buf->len = 0;

            return;
        } else if (server->stage == STAGE_HANDSHAKE || server->stage == STAGE_PARSE) {
            struct socks5_request *request = (struct socks5_request *)buf->array;
            struct sockaddr_in sock_addr;
            memset(&sock_addr, 0, sizeof(sock_addr));

            int udp_assc = 0;

            if (request->cmd == 3) {
                udp_assc = 1;
                socklen_t addr_len = sizeof(sock_addr);
                getsockname(server->fd, (struct sockaddr *)&sock_addr,
                            &addr_len);
                if (verbose) {
                    LOGI("udp assc request accepted");
                }
            } else if (request->cmd != 1) {
                LOGE("unsupported cmd: %d", request->cmd);
                struct socks5_response response;
                response.ver  = SVERSION;
                response.rep  = CMD_NOT_SUPPORTED;
                response.rsv  = 0;
                response.atyp = 1;
                char *send_buf = (char *)&response;
                send(server->fd, send_buf, 4, 0);
                close_and_free_remote(EV_A_ remote);
                close_and_free_server(EV_A_ server);
                return;
            }

            // Fake reply
            if (server->stage == STAGE_HANDSHAKE) {
                struct socks5_response response;
                response.ver  = SVERSION;
                response.rep  = 0;
                response.rsv  = 0;
                response.atyp = 1;

                buffer_t resp_to_send;
                buffer_t *resp_buf = &resp_to_send;
                balloc(resp_buf, BUF_SIZE);

                memcpy(resp_buf->array, &response, sizeof(struct socks5_response));
                memcpy(resp_buf->array + sizeof(struct socks5_response),
                       &sock_addr.sin_addr, sizeof(sock_addr.sin_addr));
                memcpy(resp_buf->array + sizeof(struct socks5_response) +
                       sizeof(sock_addr.sin_addr),
                       &sock_addr.sin_port, sizeof(sock_addr.sin_port));

                int reply_size = sizeof(struct socks5_response) +
                                 sizeof(sock_addr.sin_addr) + sizeof(sock_addr.sin_port);

                int s = send(server->fd, resp_buf->array, reply_size, 0);

                bfree(resp_buf);

                if (s < reply_size) {
                    LOGE("failed to send fake reply");
                    close_and_free_remote(EV_A_ remote);
                    close_and_free_server(EV_A_ server);
                    return;
                }
                if (udp_assc) {
                    // Wait until client closes the connection
                    return;
                }
            }

            char host[257], ip[INET6_ADDRSTRLEN], port[16];

            buffer_t ss_addr_to_send;
            buffer_t *abuf = &ss_addr_to_send;
            balloc(abuf, BUF_SIZE);

            abuf->array[abuf->len++] = request->atyp;
            int atyp = request->atyp;

            // get remote addr and port
            if (atyp == 1) {
                // IP V4
                size_t in_addr_len = sizeof(struct in_addr);
                memcpy(abuf->array + abuf->len, buf->array + 4, in_addr_len + 2);
                abuf->len += in_addr_len + 2;

                if (acl || verbose) {
                    uint16_t p = ntohs(*(uint16_t *)(buf->array + 4 + in_addr_len));
                    dns_ntop(AF_INET, (const void *)(buf->array + 4),
                         ip, INET_ADDRSTRLEN);
                    sprintf(port, "%d", p);
                }
            } else if (atyp == 3) {
                // Domain name
                uint8_t name_len = *(uint8_t *)(buf->array + 4);
                abuf->array[abuf->len++] = name_len;
                memcpy(abuf->array + abuf->len, buf->array + 4 + 1, name_len + 2);
                abuf->len += name_len + 2;

                if (acl || verbose) {
                    uint16_t p =
                        ntohs(*(uint16_t *)(buf->array + 4 + 1 + name_len));
                    memcpy(host, buf->array + 4 + 1, name_len);
                    host[name_len] = '\0';
                    sprintf(port, "%d", p);
                }
            } else if (atyp == 4) {
                // IP V6
                size_t in6_addr_len = sizeof(struct in6_addr);
                memcpy(abuf->array + abuf->len, buf->array + 4, in6_addr_len + 2);
                abuf->len += in6_addr_len + 2;

                if (acl || verbose) {
                    uint16_t p = ntohs(*(uint16_t *)(buf->array + 4 + in6_addr_len));
                    dns_ntop(AF_INET6, (const void *)(buf->array + 4),
                         ip, INET6_ADDRSTRLEN);
                    sprintf(port, "%d", p);
                }
            } else {
                bfree(abuf);
                LOGE("unsupported addrtype: %d", request->atyp);
                close_and_free_remote(EV_A_ remote);
                close_and_free_server(EV_A_ server);
                return;
            }

            size_t abuf_len  = abuf->len;
            int sni_detected = 0;

            server->stage = STAGE_STREAM;

            buf->len -= (3 + abuf_len);
            if (buf->len > 0) {
                memmove(buf->array, buf->array + 3 + abuf_len, buf->len);
            }

            if (acl) {
                if (outbound_block_match_host(host) == 1) {
                    if (verbose)
                        LOGI("outbound blocked %s", host);
                    close_and_free_remote(EV_A_ remote);
                    close_and_free_server(EV_A_ server);
                    return;
                }

                int host_match = acl_match_host(host);
                int bypass = 0;
                int resolved = 0;
                struct sockaddr_storage storage;
                memset(&storage, 0, sizeof(struct sockaddr_storage));
                int err;

                if (verbose)
                    LOGI("acl_match_host %s result %d", host, host_match);
                if (host_match > 0)
                    bypass = 0;                 // bypass hostnames in black list
                else if (host_match < 0)
                    bypass = 1;                 // proxy hostnames in white list
                else {
                    if (atyp == 3) {            // resolve domain so we can bypass domain with geoip
                        err = get_sockaddr(host, port, &storage, 0, ipv6first);
                        if ( err != -1) {
                            resolved = 1;
                            switch(((struct sockaddr*)&storage)->sa_family) {
                                case AF_INET: {
                                    struct sockaddr_in *addr_in = (struct sockaddr_in *)&storage;
                                    dns_ntop(AF_INET, &(addr_in->sin_addr), ip, INET_ADDRSTRLEN);
                                    break;
                                }
                                case AF_INET6: {
                                    struct sockaddr_in6 *addr_in6 = (struct sockaddr_in6 *)&storage;
                                    dns_ntop(AF_INET6, &(addr_in6->sin6_addr), ip, INET6_ADDRSTRLEN);
                                    break;
                                }
                                default:
                                    break;
                            }
                        }
                    }

                    if (outbound_block_match_host(ip) == 1) {
                        if (verbose)
                            LOGI("outbound blocked %s", ip);
                        close_and_free_remote(EV_A_ remote);
                        close_and_free_server(EV_A_ server);
                        return;
                    }

                    int ip_match = acl_match_host(ip);// -1 if IP in white list or 1 if IP in black list
                    if (verbose)
                        LOGI("acl_match_host ip %s result %d mode %d", ip, ip_match, get_acl_mode());
                    if (ip_match < 0)
                        bypass = 1;
                    else if (ip_match > 0)
                        bypass = 0;
                    else
                        bypass = (get_acl_mode() == BLACK_LIST);
                }

                if (bypass) {
                    if (verbose) {
                        if (sni_detected || atyp == 3)
                            LOGI("bypass %s:%s", host, port);
                        else if (atyp == 1)
                            LOGI("bypass %s:%s", ip, port);
                        else if (atyp == 4)
                            LOGI("bypass [%s]:%s", ip, port);
                    }
                    struct sockaddr_storage storage;
                    memset(&storage, 0, sizeof(struct sockaddr_storage));
                    int err;
                    if (atyp == 3 && resolved != 1)
                        err = get_sockaddr(host, port, &storage, 0, ipv6first);
                    else
                        err = get_sockaddr(ip, port, &storage, 0, ipv6first);
                    if (err != -1) {
                        remote = create_remote(server->listener, (struct sockaddr *)&storage);
                        if (remote != NULL) remote->direct = 1;
                    }
                }
            }

            // Not match ACL
            if (remote == NULL) {
                // pick a server
                listen_ctx_t *profile = server->listener;
                int index = rand() % profile->server_num;
                server_def_t *server_env = &profile->servers[index];

                if (verbose) {
                    if (sni_detected || atyp == 3)
                        LOGI("connect to %s:%s via %s:%d", host, port, server_env->host, server_env->port);
                    else if (atyp == 1)
                        LOGI("connect to %s:%s via %s:%d", ip, port, server_env->host, server_env->port);
                    else if (atyp == 4)
                        LOGI("connect to [%s]:%s via %s:%d", ip, port, server_env->host, server_env->port);
                }

                server->server_env = server_env;

                remote = create_remote(profile, (struct sockaddr *) server_env->addr);
            }

            if (remote == NULL) {
                bfree(abuf);
                LOGE("invalid remote addr");
                close_and_free_server(EV_A_ server);
                return;
            }

            if (!remote->direct) {
                server_def_t *server_env = server->server_env;

                // expelled from eden
                cork_dllist_remove(&server->entries);
                cork_dllist_add(&server_env->connections, &server->entries);

                // init server cipher
                if (server_env->cipher.enc_method > TABLE) {
                    server->e_ctx = ss_malloc(sizeof(struct enc_ctx));
                    server->d_ctx = ss_malloc(sizeof(struct enc_ctx));
                    enc_ctx_init(&server_env->cipher, server->e_ctx, 1);
                    enc_ctx_init(&server_env->cipher, server->d_ctx, 0);
                } else {
                    server->e_ctx = NULL;
                    server->d_ctx = NULL;
                }
                // SSR beg
                server_info _server_info;
                memset(&_server_info, 0, sizeof(server_info));
                if (server_env->hostname)
                    strcpy(_server_info.host, server_env->hostname);
                else
                    strcpy(_server_info.host, server_env->host);
                if (verbose) {
                    LOGI("server_info host %s", _server_info.host);
                }
                _server_info.port = server_env->port;
                _server_info.param = server_env->obfs_param;
                _server_info.g_data = server_env->obfs_global;
                _server_info.head_len = get_head_size(ss_addr_to_send.array, 320, 30);
                _server_info.iv = server->e_ctx->evp.iv;
                _server_info.iv_len = enc_get_iv_len(&server_env->cipher);
                _server_info.key = enc_get_key(&server_env->cipher);
                _server_info.key_len = enc_get_key_len(&server_env->cipher);
                _server_info.tcp_mss = 1452;
                _server_info.buffer_size = BUF_SIZE;
                _server_info.cipher_env = &server_env->cipher;

                if (server_env->obfs_plugin) {
                    server->obfs = server_env->obfs_plugin->new_obfs();
                    server_env->obfs_plugin->set_server_info(server->obfs, &_server_info);
                }

                _server_info.param = server_env->protocol_param;
                _server_info.g_data = server_env->protocol_global;

                if (server_env->protocol_plugin) {
                    server->protocol = server_env->protocol_plugin->new_obfs();
                    _server_info.overhead = server_env->protocol_plugin->get_overhead(server->protocol)
                        + (server_env->obfs_plugin ? server_env->obfs_plugin->get_overhead(server->obfs) : 0);
                    server_env->protocol_plugin->set_server_info(server->protocol, &_server_info);
                }
                // SSR end

                brealloc(remote->buf, buf->len + abuf->len, BUF_SIZE);
                memcpy(remote->buf->array, abuf->array, abuf->len);
                remote->buf->len = buf->len + abuf->len;

                if (buf->len > 0) {
                    memcpy(remote->buf->array + abuf->len, buf->array, buf->len);
                }
            } else {
                if (buf->len > 0) {
                    memcpy(remote->buf->array, buf->array, buf->len);
                    remote->buf->len = buf->len;
                }
            }

            server->remote = remote;
            remote->server = server;

            bfree(abuf);
        }
    }
}


static void
server_send_cb(EV_P_ ev_io *w, int revents)
{
    server_ctx_t *server_send_ctx = (server_ctx_t *)w;
    server_t *server              = server_send_ctx->server;
    remote_t *remote              = server->remote;
    if (server->buf->len == 0) {
        // close and free
        close_and_free_remote(EV_A_ remote);
        close_and_free_server(EV_A_ server);
        return;
    } else {
        // has data to send
        ssize_t s = send(server->fd, server->buf->array + server->buf->idx,
                         server->buf->len, 0);
        if (s == -1) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                ERROR("server_send_cb_send");
                close_and_free_remote(EV_A_ remote);
                close_and_free_server(EV_A_ server);
            }
            return;
        } else if (s < (ssize_t)(server->buf->len)) {
            // partly sent, move memory, wait for the next time to send
            server->buf->len -= s;
            server->buf->idx += s;
            return;
        } else {
            // all sent out, wait for reading
            server->buf->len = 0;
            server->buf->idx = 0;
            ev_io_server_send(EV_A_ server, remote);
            return;
        }
    }
}

static void
stat_update_cb()
{
    if (log_tx_rx) {
        ev_tstamp now = ev_time();
        if (now - last > 1.0) {
            send_traffic_stat(tx, rx);
            last = now;
        }
    }
}


static void
remote_timeout_cb(EV_P_ ev_timer *watcher, int revents)
{
    remote_ctx_t *remote_ctx
        = cork_container_of(watcher, remote_ctx_t, watcher);

    remote_t *remote = remote_ctx->remote;
    server_t *server = remote->server;

    if (verbose) {
        LOGI("TCP connection timeout");
    }

    close_and_free_remote(EV_A_ remote);
    close_and_free_server(EV_A_ server);
}

static void
remote_recv_cb(EV_P_ ev_io *w, int revents)
{
    remote_ctx_t *remote_recv_ctx = (remote_ctx_t *)w;
    remote_t *remote              = remote_recv_ctx->remote;
    server_t *server              = remote->server;
    server_def_t *server_env      = server->server_env;

    ev_timer_again(EV_A_ & remote->recv_ctx->watcher);

    stat_update_cb();

    ssize_t r = recv(remote->fd, server->buf->array, BUF_SIZE, 0);

    if (r == 0) {
        // connection closed
        close_and_free_remote(EV_A_ remote);
        close_and_free_server(EV_A_ server);
        return;
    } else if (r == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // no data
            // continue to wait for recv
            return;
        } else {
            ERROR("remote_recv_cb_recv");
            close_and_free_remote(EV_A_ remote);
            close_and_free_server(EV_A_ server);
            return;
        }
    }

    server->buf->len = r;

    if (!remote->direct) {
        if (log_tx_rx)
            rx += server->buf->len;
        if ( r == 0 )
            return;
        // SSR beg
        if (server_env->obfs_plugin) {
            obfs_class *obfs_plugin = server_env->obfs_plugin;
            if (obfs_plugin->client_decode) {
                int needsendback;
                server->buf->len = obfs_plugin->client_decode(server->obfs, &server->buf->array, server->buf->len, &server->buf->capacity, &needsendback);
                if ((int)server->buf->len < 0) {
                    LOGE("client_decode");
                    close_and_free_remote(EV_A_ remote);
                    close_and_free_server(EV_A_ server);
                    return;
                }
                if (needsendback) {
                    if (obfs_plugin->client_encode) {
                        remote->buf->len = obfs_plugin->client_encode(server->obfs, &remote->buf->array, 0, &remote->buf->capacity);
                        ssize_t s = send(remote->fd, remote->buf->array, remote->buf->len, 0);
                        if (s == -1) {
                            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                                ERROR("remote_recv_cb_send");
                                // close and free
                                close_and_free_remote(EV_A_ remote);
                                close_and_free_server(EV_A_ server);
                            }
                            return;
                        } else if (s < (ssize_t)(remote->buf->len)) {
                            // partly sent, move memory, wait for the next time to send
                            remote->buf->len -= s;
                            remote->buf->idx += s;
                            return;
                        } else {
                            // all sent out, wait for reading
                            remote->buf->len = 0;
                            remote->buf->idx = 0;
                            ev_io_remote_send(EV_A_ server, remote);
                        }
                    }
                }
            }
        }
        if (server->buf->len > 0) {
        int err = ss_decrypt(&server_env->cipher, server->buf, server->d_ctx, BUF_SIZE);
            if (err) {
                LOGE("remote invalid password or cipher");
                close_and_free_remote(EV_A_ remote);
                close_and_free_server(EV_A_ server);
                return;
            }
        }
        if (server_env->protocol_plugin) {
            obfs_class *protocol_plugin = server_env->protocol_plugin;
            if (protocol_plugin->client_post_decrypt) {
                server->buf->len = protocol_plugin->client_post_decrypt(server->protocol, &server->buf->array, server->buf->len, &server->buf->capacity);
                if ((int)server->buf->len < 0) {
                    LOGE("client_post_decrypt");
                    close_and_free_remote(EV_A_ remote);
                    close_and_free_server(EV_A_ server);
                    return;
                }
                if ( server->buf->len == 0 )
                    return;
            }
        }
        // SSR end
    }

    int s = send(server->fd, server->buf->array, server->buf->len, 0);

    if (s == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // no data, wait for send
            server->buf->idx = 0;
            ev_io_remote_recv(EV_A_ server, remote);
        } else {
            ERROR("remote_recv_cb_send");
            close_and_free_remote(EV_A_ remote);
            close_and_free_server(EV_A_ server);
        }
    } else if (s < (int)(server->buf->len)) {
        server->buf->len -= s;
        server->buf->idx  = s;
        ev_io_remote_recv(EV_A_ server, remote);
    }
}

static void
remote_send_cb(EV_P_ ev_io *w, int revents)
{
    remote_ctx_t *remote_send_ctx = (remote_ctx_t *)w;
    remote_t *remote              = remote_send_ctx->remote;
    server_t *server              = remote->server;

    if (!remote_send_ctx->connected) {
        int err_no = 0;
        socklen_t len = sizeof err_no;
#ifdef __MINGW32__
        int r = getsockopt(remote->fd, SOL_SOCKET, SO_ERROR, (char *)&err_no, &len);
#else
        int r = getsockopt(remote->fd, SOL_SOCKET, SO_ERROR, &err_no, &len);
#endif
        if (r == 0 && err_no == 0) {
            remote_send_ctx->connected = 1;
            ev_timer_stop(EV_A_ & remote_send_ctx->watcher);
            ev_timer_start(EV_A_ & remote->recv_ctx->watcher);
            ev_io_start(EV_A_ & remote->recv_ctx->io);

            // no need to send any data
            if (remote->buf->len == 0) {
                ev_io_remote_send(EV_A_ server, remote);
                return;
            }
        } else {
            // not connected
            LOGE("getsockopt error code %d %d", r, err_no);
            ERROR("getsockopt");
            close_and_free_remote(EV_A_ remote);
            close_and_free_server(EV_A_ server);
            return;
        }
    }

    if (remote->buf->len == 0) {
        // close and free
        close_and_free_remote(EV_A_ remote);
        close_and_free_server(EV_A_ server);
        return;
    } else {
        // has data to send
        ssize_t s = send(remote->fd, remote->buf->array + remote->buf->idx,
                         remote->buf->len, 0);
        if (s == -1) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                ERROR("remote_send_cb_send");
                // close and free
                close_and_free_remote(EV_A_ remote);
                close_and_free_server(EV_A_ server);
            }
            return;
        } else if (s < (ssize_t)(remote->buf->len)) {
            // partly sent, move memory, wait for the next time to send
            remote->buf->len -= s;
            remote->buf->idx += s;
            return;
        } else {
            // all sent out, wait for reading
            remote->buf->len = 0;
            remote->buf->idx = 0;
            ev_io_remote_send(EV_A_ server, remote);
        }
    }
}

static remote_t *
new_remote(int fd, int timeout)
{
    remote_t *remote;
    remote = ss_malloc(sizeof(remote_t));

    memset(remote, 0, sizeof(remote_t));

    remote->buf                 = ss_malloc(sizeof(buffer_t));
    remote->recv_ctx            = ss_malloc(sizeof(remote_ctx_t));
    remote->send_ctx            = ss_malloc(sizeof(remote_ctx_t));
    balloc(remote->buf, BUF_SIZE);
    memset(remote->recv_ctx, 0, sizeof(remote_ctx_t));
    memset(remote->send_ctx, 0, sizeof(remote_ctx_t));
    remote->recv_ctx->connected = 0;
    remote->send_ctx->connected = 0;
    remote->fd                  = fd;
    remote->recv_ctx->remote    = remote;
    remote->send_ctx->remote    = remote;

    ev_io_init(&remote->recv_ctx->io, remote_recv_cb, fd, EV_READ);
    ev_io_init(&remote->send_ctx->io, remote_send_cb, fd, EV_WRITE);
    ev_timer_init(&remote->send_ctx->watcher, remote_timeout_cb,
                  min(MAX_CONNECT_TIMEOUT, timeout), 0);
    ev_timer_init(&remote->recv_ctx->watcher, remote_timeout_cb,
                  timeout, timeout);

    return remote;
}

static void
free_remote(remote_t *remote)
{
    if (remote->server != NULL) {
        remote->server->remote = NULL;
    }
    if (remote->buf != NULL) {
        bfree(remote->buf);
        ss_free(remote->buf);
    }
    ss_free(remote->recv_ctx);
    ss_free(remote->send_ctx);
    ss_free(remote);
}

static void
close_and_free_remote(EV_P_ remote_t *remote)
{
    if (remote != NULL) {
        ev_timer_stop(EV_A_ & remote->send_ctx->watcher);
        ev_timer_stop(EV_A_ & remote->recv_ctx->watcher);
        ev_io_stop(EV_A_ & remote->send_ctx->io);
        ev_io_stop(EV_A_ & remote->recv_ctx->io);
        close(remote->fd);
        free_remote(remote);
    }
}

static server_t *
new_server(int fd, listen_ctx_t* profile)
{
    server_t *server;
    server = ss_malloc(sizeof(server_t));

    memset(server, 0, sizeof(server_t));

    server->listener = profile;
    server->recv_ctx            = ss_malloc(sizeof(server_ctx_t));
    server->send_ctx            = ss_malloc(sizeof(server_ctx_t));
    server->buf                 = ss_malloc(sizeof(buffer_t));
    balloc(server->buf, BUF_SIZE);
    memset(server->recv_ctx, 0, sizeof(server_ctx_t));
    memset(server->send_ctx, 0, sizeof(server_ctx_t));
    server->stage               = STAGE_INIT;
    server->recv_ctx->connected = 0;
    server->send_ctx->connected = 0;
    server->fd                  = fd;
    server->recv_ctx->server    = server;
    server->send_ctx->server    = server;

    ev_io_init(&server->recv_ctx->io, server_recv_cb, fd, EV_READ);
    ev_io_init(&server->send_ctx->io, server_send_cb, fd, EV_WRITE);

    cork_dllist_add(&profile->connections_eden, &server->entries);
    cork_dllist_add(&all_connections, &server->entries_all);

    return server;
}

static void
release_profile(listen_ctx_t *profile)
{
    int i;

    ss_free(profile->iface);

    for(i = 0; i < profile->server_num; i++)
    {
        server_def_t *server_env = &profile->servers[i];

        ss_free(server_env->host);

        if(server_env->addr != server_env->addr_udp)
        {
            ss_free(server_env->addr_udp);
        }
        ss_free(server_env->addr);

        ss_free(server_env->psw);

        ss_free(server_env->protocol_name);
        ss_free(server_env->obfs_name);
        ss_free(server_env->protocol_param);
        ss_free(server_env->obfs_param);
        ss_free(server_env->protocol_global);
        ss_free(server_env->obfs_global);
        if(server_env->protocol_plugin){
            free_obfs_class(server_env->protocol_plugin);
        }
        if(server_env->obfs_plugin){
            free_obfs_class(server_env->obfs_plugin);
        }
        ss_free(server_env->id);
        ss_free(server_env->group);

        enc_release(&server_env->cipher);
    }
    ss_free(profile);
}

static void
check_and_free_profile(listen_ctx_t *profile)
{
    int i;

    if(profile == current_profile)
    {
        return;
    }
    // if this connection is created from an inactive profile, then we need to free the profile
    // when the last connection of that profile is colsed
    if(!cork_dllist_is_empty(&profile->connections_eden))
    {
        return;
    }

    for(i = 0; i < profile->server_num; i++)
    {
        if(!cork_dllist_is_empty(&profile->servers[i].connections))
        {
            return;
        }
    }

    // No connections anymore
    cork_dllist_remove(&profile->entries);
    release_profile(profile);
}

static void
free_server(server_t *server)
{
    listen_ctx_t *profile = server->listener;
    server_def_t *server_env = server->server_env;

    cork_dllist_remove(&server->entries);
    cork_dllist_remove(&server->entries_all);

    if (server->remote != NULL) {
        server->remote->server = NULL;
    }
    if (server->buf != NULL) {
        bfree(server->buf);
        ss_free(server->buf);
    }

    if(server_env)
    {
        if (server->e_ctx != NULL) {
            enc_ctx_release(&server_env->cipher, server->e_ctx);
            ss_free(server->e_ctx);
        }
        if (server->d_ctx != NULL) {
            enc_ctx_release(&server_env->cipher, server->d_ctx);
            ss_free(server->d_ctx);
        }
        // SSR beg
        if (server_env->obfs_plugin) {
            server_env->obfs_plugin->dispose(server->obfs);
            server->obfs = NULL;
        }
        if (server_env->protocol_plugin) {
            server_env->protocol_plugin->dispose(server->protocol);
            server->protocol = NULL;
        }
        // SSR end
    }

    ss_free(server->recv_ctx);
    ss_free(server->send_ctx);
    ss_free(server);

    // after free server, we need to check the profile
    check_and_free_profile(profile);
}

static void
close_and_free_server(EV_P_ server_t *server)
{
    if (server != NULL) {
        ev_io_stop(EV_A_ & server->send_ctx->io);
        ev_io_stop(EV_A_ & server->recv_ctx->io);
        close(server->fd);
        free_server(server);
    }
}

static remote_t *
create_remote(listen_ctx_t *profile, struct sockaddr *addr)
{
    int remotefd = socket(addr->sa_family, SOCK_STREAM, IPPROTO_TCP);

    if (remotefd == -1) {
        ERROR("socket");
        return NULL;
    }

    int opt = 1;
    setsockopt(remotefd, SOL_TCP, TCP_NODELAY, &opt, sizeof(opt));
#ifdef SO_NOSIGPIPE
    setsockopt(remotefd, SOL_SOCKET, SO_NOSIGPIPE, &opt, sizeof(opt));
#endif

    if (profile->mptcp == 1) {
        int err = setsockopt(remotefd, SOL_TCP, MPTCP_ENABLED, &opt, sizeof(opt));
        if (err == -1) {
            ERROR("failed to enable multipath TCP");
        }
    }

    // Setup
    setnonblocking(remotefd);
#ifdef SET_INTERFACE
    if (profile->iface) {
        if (setinterface(remotefd, profile->iface) == -1)
            ERROR("setinterface");
    }
#endif

    remote_t *remote = new_remote(remotefd, profile->timeout);
    remote->direct_addr.addr_len = get_sockaddr_len(addr);
    memcpy(&(remote->direct_addr.addr), addr, remote->direct_addr.addr_len);
//    remote->direct_addr.remote_index = index;

    return remote;
}

static void
signal_cb(EV_P_ ev_signal *w, int revents)
{
    if (revents & EV_SIGNAL) {
        switch (w->signum) {
        case SIGINT:
        case SIGTERM:
#ifndef __MINGW32__
        case SIGUSR1:
#endif
            ev_unloop(EV_A_ EVUNLOOP_ALL);
        }
    }
}

void
accept_cb(EV_P_ ev_io *w, int revents)
{
    listen_ctx_t *listener = (listen_ctx_t *)w;
    int serverfd           = accept(listener->fd, NULL, NULL);
    if (serverfd == -1) {
        ERROR("accept");
        return;
    }
    setnonblocking(serverfd);
    int opt = 1;
    setsockopt(serverfd, SOL_TCP, TCP_NODELAY, &opt, sizeof(opt));
#ifdef SO_NOSIGPIPE
    setsockopt(serverfd, SOL_SOCKET, SO_NOSIGPIPE, &opt, sizeof(opt));
#endif

    server_t *server = new_server(serverfd, listener);

    ev_io_start(EV_A_ & server->recv_ctx->io);
}

void
resolve_int_cb(int dummy)
{
    keep_resolving = 0;
}

static void
init_obfs(server_def_t *serv, char *protocol, char *protocol_param, char *obfs, char *obfs_param)
{
    serv->protocol_name = protocol;
    serv->protocol_param = protocol_param;
    serv->protocol_plugin = new_obfs_class(protocol);
    serv->obfs_name = obfs;
    serv->obfs_param = obfs_param;
    serv->obfs_plugin = new_obfs_class(obfs);

    if (serv->obfs_plugin) {
        serv->obfs_global = serv->obfs_plugin->init_data();
    }
    if (serv->protocol_plugin) {
        serv->protocol_global = serv->protocol_plugin->init_data();
    }
}

int
stop_ss_local_server()
{
stop=true;
}

void
stop_timer_cb(EV_P_ ev_timer *w, int revents)
{
    if(stop) {
        ev_timer_stop(loop,stop_watcher);
        ev_unloop(EV_A_ EVUNLOOP_ALL);
    } else {
        stop_watcher->repeat=0.5;
        ev_timer_again(loop,stop_watcher);
    }
}

int
start_ss_local_server(profile_t profile)
{
    srand(time(NULL));
    ev_timer stop_timer;
    stop_watcher=&stop_timer;
    stop=false;
    char *remote_host = profile.remote_host;
    char *local_addr  = profile.local_addr;
    char *method      = profile.method;
    char *password    = profile.password;
    char *log         = profile.log;
    int remote_port   = profile.remote_port;
    int local_port    = profile.local_port;
    int timeout       = profile.timeout;
    char *obfs        = profile.obfs;           // ssr
    char *obfs_param  = profile.obfs_param;    // ssr
    char *protocol    = profile.protocol;       // ssr
    char *proto_param = profile.protocol_param;// ssr
    int mtu           = 0;
    int mptcp         = 0;

    ss_addr_t tunnel_addr = { .host = NULL, .port = NULL };

    mode      = profile.mode;
    fast_open = profile.fast_open;
    verbose   = profile.verbose;
    mtu       = profile.mtu;
    mptcp     = profile.mptcp;

    char local_port_str[16];
    char remote_port_str[16];
    sprintf(local_port_str, "%d", local_port);
    sprintf(remote_port_str, "%d", remote_port);


    if (profile.acl != NULL) {
        acl = !init_acl(profile.acl);
    }

    if (local_addr == NULL) {
        local_addr = "127.0.0.1";
    }

#ifdef __MINGW32__
    winsock_init();
#else
    // ignore SIGPIPE
//    signal(SIGPIPE, SIG_IGN);
//    signal(SIGABRT, SIG_IGN);
#endif

//    struct ev_signal sigint_watcher;
//    struct ev_signal sigterm_watcher;
//    ev_signal_init(&sigint_watcher, signal_cb, SIGINT);
//    ev_signal_init(&sigterm_watcher, signal_cb, SIGTERM);
//    ev_signal_start(EV_DEFAULT, &sigint_watcher);
//    ev_signal_start(EV_DEFAULT, &sigterm_watcher);
//#ifndef __MINGW32__
//    struct ev_signal sigusr1_watcher;
//    ev_signal_init(&sigusr1_watcher, signal_cb, SIGUSR1);
//    ev_signal_start(EV_DEFAULT, &sigusr1_watcher);
//#endif

    struct sockaddr_storage *storage = ss_malloc(sizeof(struct sockaddr_storage));
    memset(storage, 0, sizeof(struct sockaddr_storage));
    if (get_sockaddr(remote_host, remote_port_str, storage, 0, ipv6first) == -1) {
        return -1;
    }

    // Setup proxy context
    loop = ev_loop_new(0);
    listen_ctx_t *listen_ctx = (listen_ctx_t *)ss_malloc(sizeof(listen_ctx_t));
    memset(listen_ctx, 0, sizeof(listen_ctx_t));
    current_profile=listen_ctx;
    listen_ctx->server_num     = 1;
    server_def_t *serv = &listen_ctx->servers[0];
    serv->host=ss_strdup(remote_host);
    serv->port=remote_port;
    serv->psw=ss_strdup(password);
    ss_server_t server_cfg;
    ss_server_t *serv_cfg = &server_cfg;
    server_cfg.protocol = protocol;
    server_cfg.protocol_param = proto_param;
    server_cfg.obfs = obfs;
    server_cfg.obfs_param = obfs_param;
    serv->enable=1;
    cork_dllist_init(&listen_ctx->connections_eden);
    serv->addr = serv->addr_udp = storage;
    serv->addr_len = serv->addr_udp_len = get_sockaddr_len((struct sockaddr *) storage);
    listen_ctx->timeout        = timeout;
    listen_ctx->iface          = NULL;
    listen_ctx->mptcp          = mptcp;

    if (mode != UDP_ONLY) {
        // Setup socket
        int listenfd;
        listenfd = create_and_bind(local_addr, local_port_str);
        if (listenfd == -1) {
            ERROR("bind()");
            return -1;
        }
        if (listen(listenfd, SOMAXCONN) == -1) {
            ERROR("listen()");
            return -1;
        }
        setnonblocking(listenfd);

        listen_ctx->fd = listenfd;
        ev_timer_init(stop_watcher,stop_timer_cb,0.5,0);
        ev_timer_start(loop,stop_watcher);
        ev_io_init(&listen_ctx->io, accept_cb, listenfd, EV_READ);
        ev_io_start(loop, &listen_ctx->io);
    }

    // TODO Currently we don't support UDP relay.
    // Setup UDP
    if (mode != TCP_ONLY) {
        LOGI("udprelay enabled");
        init_udprelay(local_addr, local_port_str, (struct sockaddr*)listen_ctx->servers[0].addr_udp,
                      listen_ctx->servers[0].addr_udp_len, tunnel_addr, mtu, listen_ctx->timeout, listen_ctx->iface, &listen_ctx->servers[0].cipher, listen_ctx->servers[0].protocol_name, listen_ctx->servers[0].protocol_param);
    }

    if (strcmp(local_addr, ":") > 0)
        LOGI("listening at [%s]:%s", local_addr, local_port_str);
    else
        LOGI("listening at %s:%s", local_addr, local_port_str);

    // Setup keys
    LOGI("initializing ciphers... %s", method);
    enc_init(&serv->cipher, password, method);

    // init obfs
    init_obfs(serv, ss_strdup(serv_cfg->protocol), ss_strdup(serv_cfg->protocol_param), ss_strdup(serv_cfg->obfs), ss_strdup(serv_cfg->obfs_param));

    // Init connections
    cork_dllist_init(&serv->connections);
    cork_dllist_init(&all_connections);


    // Enter the loop
    ev_run(loop, 0);

    if (verbose) {
        LOGI("closed gracefully");
    }

    // Clean up
    if (mode != TCP_ONLY) {
        free_udprelay();
    }

    if (mode != UDP_ONLY) {
        ev_io_stop(loop, &listen_ctx->io);
        free_connections(loop);
        close(listen_ctx->fd);
        release_profile(current_profile);
    }

    ss_free(serv->addr);

#ifdef __MINGW32__
    winsock_cleanup();
#endif

//    ev_signal_stop(EV_DEFAULT, &sigint_watcher);
//    ev_signal_stop(EV_DEFAULT, &sigterm_watcher);
//#ifndef __MINGW32__
//    ev_signal_stop(EV_DEFAULT, &sigusr1_watcher);
//#endif

    // cannot reach here
    return 0;
}

