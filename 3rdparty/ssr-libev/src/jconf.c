/*
 * jconf.c - Parse the JSON format config file
 *
 * Copyright (C) 2013 - 2016, Max Lv <max.c.lv@gmail.com>
 *
 * This file is part of the shadowsocks-libev.
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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "utils.h"
#include "jconf.h"
#include "json.h"
#include "string.h"

#include <libcork/core.h>

#define check_json_value_type(value, expected_type, message) \
    do { \
        if ((value)->type != (expected_type)) \
            FATAL((message)); \
    } while(0)

static char *
to_string(const json_value *value)
{
    if (value->type == json_string) {
        return ss_strndup(value->u.string.ptr, value->u.string.length);
    } else if (value->type == json_integer) {
        return strdup(ss_itoa(value->u.integer));
    } else if (value->type == json_null) {
        return "null";
    } else {
        LOGE("%d", value->type);
        FATAL("Invalid config format.");
    }
    return 0;
}

void
free_addr(ss_addr_t *addr)
{
    ss_free(addr->host);
    ss_free(addr->port);
}

void
parse_addr(const char *str, ss_addr_t *addr)
{
    int ipv6 = 0, ret = -1, n = 0;
    char *pch;

    struct cork_ip ip;
    if (cork_ip_init(&ip, str) != -1) {
        addr->host = strdup(str);
        addr->port = NULL;
        return;
    }

    pch = strchr(str, ':');
    while (pch != NULL) {
        n++;
        ret = pch - str;
        pch = strchr(pch + 1, ':');
    }
    if (n > 1) {
        ipv6 = 1;
        if (str[ret - 1] != ']') {
            ret = -1;
        }
    }

    if (ret == -1) {
        if (ipv6) {
            addr->host = ss_strndup(str + 1, strlen(str) - 2);
        } else {
            addr->host = strdup(str);
        }
        addr->port = NULL;
    } else {
        if (ipv6) {
            addr->host = ss_strndup(str + 1, ret - 2);
        } else {
            addr->host = ss_strndup(str, ret);
        }
        addr->port = strdup(str + ret + 1);
    }
}

void parse_ss_server(ss_server_t *server, json_value* json) {
    unsigned int i;

    // TODO: set default value

    for (i = 0; i < json->u.object.length; i++) {
        char *name = json->u.object.values[i].name;
        json_value *value = json->u.object.values[i].value;

        if (strcmp(name, "id") == 0) {
            server->id = to_string(value);
        } else if (strcmp(name, "server") == 0) {
            server->server = to_string(value);
        } else if (strcmp(name, "server_port") == 0) {
            check_json_value_type(value, json_integer,
                                  "invalid config file: option 'server_port' must be an integer");
            server->server_port = value->u.integer;
        } else if (strcmp(name, "server_udp_port") == 0) { // SSR
            check_json_value_type(value, json_integer,
                                  "invalid config file: option 'server_udp_port' must be an integer");
            server->server_udp_port = value->u.integer;
        } else if (strcmp(name, "password") == 0) {
            server->password = to_string(value);
        } else if (strcmp(name, "method") == 0) {
            server->method = to_string(value);
        } else if (strcmp(name, "protocol") == 0) { // SSR
            server->protocol = to_string(value);
        } else if (strcmp(name, "protocol_param") == 0) { //SSR
            server->protocol_param = to_string(value);
        } else if (strcmp(name, "obfs") == 0) { // SSR
            server->obfs = to_string(value);
        } else if (strcmp(name, "obfs_param") == 0) { // SSR
            server->obfs_param = to_string(value);
        } else if (strcmp(name, "group") == 0) { // SSR
            server->group = to_string(value);
        } else if (strcmp(name, "enable") == 0) { // SSR
            check_json_value_type(value, json_boolean,
                                  "invalid config file: option 'enable' must be an boolean");
            server->enable = value->u.boolean;
        } else if (strcmp(name, "udp_over_tcp") == 0) { // SSR
            check_json_value_type(value, json_boolean,
                                  "invalid config file: option 'udp_over_tcp' must be an boolean");
            server->udp_over_tcp = value->u.boolean;
        }
    }
}

jconf_t *
read_jconf(const char *file)
{
    static jconf_t conf;

    memset(&conf, 0, sizeof(jconf_t));

    conf.conf_ver = CONF_VER_LEGACY; // try legacy version first

    char *buf;
    json_value *obj;

    FILE *f = fopen(file, "rb");
    if (f == NULL) {
        FATAL("Invalid config path.");
    }

    fseek(f, 0, SEEK_END);
    long pos = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (pos >= MAX_CONF_SIZE) {
        FATAL("Too large config file.");
    }

    buf = ss_malloc(pos + 1);
    if (buf == NULL) {
        FATAL("No enough memory.");
    }

    int nread = fread(buf, pos, 1, f);
    if (!nread) {
        FATAL("Failed to read the config file.");
    }
    fclose(f);

    buf[pos] = '\0'; // end of string

    json_settings settings = { 0UL, 0, NULL, NULL, NULL };
    char error_buf[512];
    obj = json_parse_ex(&settings, buf, pos, error_buf);

    if (obj == NULL) {
        FATAL(error_buf);
    }

    if (obj->type == json_object) {
        unsigned int i, j;
        for (i = 0; i < obj->u.object.length; i++) {
            char *name = obj->u.object.values[i].name;
            json_value *value = obj->u.object.values[i].value;

            int match = 1;

            // Legacy server config format
            if (conf.conf_ver == CONF_VER_LEGACY) {
                if (strcmp(name, "server") == 0) {
                    if (value->type == json_array) {
                        for (j = 0; j < value->u.array.length; j++) {
                            if (j >= MAX_REMOTE_NUM) {
                                break;
                            }
                            json_value *v = value->u.array.values[j];
                            char *addr_str = to_string(v);
                            parse_addr(addr_str, conf.server_legacy.remote_addr + j);
                            ss_free(addr_str);
                            conf.server_legacy.remote_num = j + 1;
                        }
                    } else if (value->type == json_string) {
                        conf.server_legacy.remote_addr[0].host = to_string(value);
                        conf.server_legacy.remote_addr[0].port = NULL;
                        conf.server_legacy.remote_num = 1;
                    }
                } else if (strcmp(name, "port_password") == 0) {
                    if (value->type == json_object) {
                        for (j = 0; j < value->u.object.length; j++) {
                            if (j >= MAX_PORT_NUM) {
                                break;
                            }
                            json_value *v = value->u.object.values[j].value;
                            if (v->type == json_string) {
                                conf.server_legacy.port_password[j].port = ss_strndup(value->u.object.values[j].name,
                                                                        value->u.object.values[j].name_length);
                                conf.server_legacy.port_password[j].password = to_string(v);
                                conf.server_legacy.port_password_num = j + 1;
                            }
                        }
                    }
                } else if (strcmp(name, "server_port") == 0) {
                    conf.server_legacy.remote_port = to_string(value);
                } else if (strcmp(name, "local_address") == 0) {
                    conf.server_legacy.local_addr = to_string(value);
                } else if (strcmp(name, "local_port") == 0) {
                    conf.server_legacy.local_port = to_string(value);
                } else if (strcmp(name, "password") == 0) {
                    conf.server_legacy.password = to_string(value);
                } else if (strcmp(name, "auth") == 0) {
                    LOGI("auth is deprecated, ignored");
                } else if (strcmp(name, "protocol") == 0) { // SSR
                    conf.server_legacy.protocol = to_string(value);
                } else if (strcmp(name, "protocol_param") == 0) { //SSR
                    conf.server_legacy.protocol_param = to_string(value);
                } else if (strcmp(name, "method") == 0) {
                    conf.server_legacy.method = to_string(value);
                } else if (strcmp(name, "obfs") == 0) { // SSR
                    conf.server_legacy.obfs = to_string(value);
                } else if (strcmp(name, "obfs_param") == 0) { // SSR
                    conf.server_legacy.obfs_param = to_string(value);
                } else {
                    match = 0;
                }
            }
            if (!match) {
                if(strcmp(name, "servers") == 0) {
                    if(conf.conf_ver == CONF_VER_LEGACY) {
                        memset(&conf.server_new_1, 0, sizeof(conf.server_new_1));
                        conf.conf_ver = CONF_VER_1;
                    }

                    if (value->type == json_array) {
                        for (j = 0; j < value->u.array.length; j++) {
                            if (conf.server_new_1.server_num >= MAX_SERVER_NUM) {
                                LOGI("Max servers exceed, ignore remain server defines.");
                                break;
                            }
                            json_value *v = value->u.array.values[j];

                            if(v->type == json_object) {
                                parse_ss_server(&conf.server_new_1.servers[conf.server_new_1.server_num], v);
                                conf.server_new_1.server_num++;
                            }
                        }
                    }
                } else if (strcmp(name, "timeout") == 0) {
                    conf.timeout = to_string(value);
                } else if (strcmp(name, "user") == 0) {
                    conf.user = to_string(value);
                } else if (strcmp(name, "fast_open") == 0) {
                    check_json_value_type(value, json_boolean,
                                          "invalid config file: option 'fast_open' must be a boolean");
                    conf.fast_open = value->u.boolean;
                } else if (strcmp(name, "nofile") == 0) {
                    check_json_value_type(value, json_integer,
                                          "invalid config file: option 'nofile' must be an integer");
                    conf.nofile = value->u.integer;
                } else if (strcmp(name, "nameserver") == 0) {
                    conf.nameserver = to_string(value);
                } else if (strcmp(name, "tunnel_address") == 0) {
                    conf.tunnel_address = to_string(value);
                } else if (strcmp(name, "mode") == 0) {
                    char *mode_str = to_string(value);

                    if (strcmp(mode_str, "tcp_only") == 0)
                        conf.mode = TCP_ONLY;
                    else if (strcmp(mode_str, "tcp_and_udp") == 0)
                        conf.mode = TCP_AND_UDP;
                    else if (strcmp(mode_str, "udp_only") == 0)
                        conf.mode = UDP_ONLY;
                    else
                        LOGI("ignore unknown mode: %s, use tcp_only as fallback",
                             mode_str);
                    ss_free(mode_str);
                } else if (strcmp(name, "mtu") == 0) {
                    check_json_value_type(value, json_integer,
                                          "invalid config file: option 'mtu' must be an integer");
                    conf.mtu = value->u.integer;
                } else if (strcmp(name, "mptcp") == 0) {
                    check_json_value_type(value, json_boolean,
                                          "invalid config file: option 'mptcp' must be a boolean");
                    conf.mptcp = value->u.boolean;
                } else if (strcmp(name, "ipv6_first") == 0) {
                    check_json_value_type(value, json_boolean,
                                          "invalid config file: option 'ipv6_first' must be a boolean");
                    conf.ipv6_first = value->u.boolean;
                }
            }
        }
    } else {
        FATAL("Invalid config file");
    }

    ss_free(buf);
    json_value_free(obj);
    return &conf;
}

void free_jconf(jconf_t *conf) {
    int i;

    if (!conf) {
        return;
    }

    ss_free(conf->timeout);
    ss_free(conf->user);
    ss_free(conf->nameserver);
    ss_free(conf->tunnel_address);

    if(conf->conf_ver == CONF_VER_LEGACY){
        ss_server_legacy_t *legacy = &conf->server_legacy;
        for(i = 0; i < legacy->remote_num; i++){
            free_addr(&legacy->remote_addr[i]);
        }
        for(i = 0; i < legacy->port_password_num; i++){
            ss_free(legacy->port_password[i].port);
            ss_free(legacy->port_password[i].password);
        }
        ss_free(legacy->remote_port);
        ss_free(legacy->local_addr);
        ss_free(legacy->local_port);
        ss_free(legacy->password);
        ss_free(legacy->protocol);
        ss_free(legacy->protocol_param);
        ss_free(legacy->method);
        ss_free(legacy->obfs);
        ss_free(legacy->obfs_param);
    } else {
        ss_server_new_1_t *ss_server_new_1 = &conf->server_new_1;
        for(i = 0; i < ss_server_new_1->server_num; i++){
            ss_server_t *serv = &ss_server_new_1->servers[i];

            ss_free(serv->server);
            ss_free(serv->password);
            ss_free(serv->method);
            ss_free(serv->protocol);
            ss_free(serv->protocol_param);
            ss_free(serv->obfs);
            ss_free(serv->obfs_param);
            ss_free(serv->id);
            ss_free(serv->group);
        }
    }
}
