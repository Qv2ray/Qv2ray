/*
 * auth.h - Define shadowsocksR server's buffers and callbacks
 *
 * Copyright (C) 2015 - 2016, Break Wa11 <mmgac001@gmail.com>
 */

#ifndef _OBFS_AUTH_CHAIN_H
#define _OBFS_AUTH_CHAIN_H

#include "obfs.h"

#ifdef __cplusplus
extern "C" {
#endif
void * auth_chain_a_init_data();
obfs * auth_chain_a_new_obfs();
void auth_chain_a_dispose(obfs *self);


int auth_chain_a_client_pre_encrypt(obfs *self, char **pplaindata, int datalength, size_t* capacity);
int auth_chain_a_client_post_decrypt(obfs *self, char **pplaindata, int datalength, size_t* capacity);

int auth_chain_a_client_udp_pre_encrypt(obfs *self, char **pplaindata, int datalength, size_t* capacity);
int auth_chain_a_client_udp_post_decrypt(obfs *self, char **pplaindata, int datalength, size_t* capacity);
void auth_chain_b_set_server_info(obfs *self, server_info *server);

int auth_chain_a_get_overhead(obfs *self);

obfs * auth_chain_b_new_obfs();
#ifdef __cplusplus
}
#endif
#endif // _OBFS_AUTH_CHAIN_H
