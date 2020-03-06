/*
 * verify.h - Define shadowsocksR server's buffers and callbacks
 *
 * Copyright (C) 2015 - 2016, Break Wa11 <mmgac001@gmail.com>
 */

#ifndef _OBFS_VERIFY_H
#define _OBFS_VERIFY_H

#include "obfs.h"

#ifdef __cplusplus
extern "C" {
#endif
obfs * verify_simple_new_obfs();
void verify_simple_dispose(obfs *self);

int verify_simple_client_pre_encrypt(obfs *self, char **pplaindata, int datalength, size_t* capacity);
int verify_simple_client_post_decrypt(obfs *self, char **pplaindata, int datalength, size_t* capacity);
#ifdef __cplusplus
}
#endif

#endif // _OBFS_VERIFY_H
