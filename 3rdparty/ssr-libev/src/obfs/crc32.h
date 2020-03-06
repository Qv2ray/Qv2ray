#ifndef _OBFS_CRC32_H
#define _OBFS_CRC32_H

#include "obfs.h"
#ifdef __cplusplus
extern "C" {
#endif
void init_crc32_table(void);

uint32_t crc32_ssr(unsigned char *buffer, unsigned int size);

void fillcrc32to(unsigned char *buffer, unsigned int size, unsigned char *outbuffer);

void fillcrc32(unsigned char *buffer, unsigned int size);

void filladler32(unsigned char *buffer, unsigned int size);

int checkadler32(unsigned char *buffer, unsigned int size);
#ifdef __cplusplus
}
#endif
#endif // _OBFS_CRC32_H
