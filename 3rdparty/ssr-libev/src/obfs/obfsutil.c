#include <stdint.h>
#include <time.h>

#include "obfsutil.h"
#include "encrypt.h"

int get_head_size(char *plaindata, int size, int def_size) {
    if (plaindata == NULL || size < 2)
        return def_size;
    int head_type = plaindata[0] & 0x7;
    if (head_type == 1)
        return 7;
    if (head_type == 4)
        return 19;
    if (head_type == 3)
        return 4 + plaindata[1];
    return def_size;
}

static int shift128plus_init_flag = 0;
static uint64_t shift128plus_s[2] = {0x10000000, 0xFFFFFFFF};

void init_shift128plus(void) {
    if (shift128plus_init_flag == 0) {
        shift128plus_init_flag = 1;
        uint32_t seed = (uint32_t)time(NULL);
        shift128plus_s[0] = seed | 0x100000000L;
        shift128plus_s[1] = ((uint64_t)seed << 32) | 0x1;
    }
}

uint64_t xorshift128plus(void) {
    uint64_t x = shift128plus_s[0];
    uint64_t const y = shift128plus_s[1];
    shift128plus_s[0] = y;
    x ^= x << 23; // a
    x ^= x >> 17; // b
    x ^= y ^ (y >> 26); // c
    shift128plus_s[1] = x;
    return x + y;
}

int ss_md5_hmac(char *auth, char *msg, int msg_len, uint8_t *iv, int enc_iv_len, uint8_t *enc_key, int enc_key_len)
{
    uint8_t auth_key[MAX_IV_LENGTH + MAX_KEY_LENGTH];
    memcpy(auth_key, iv, enc_iv_len);
    memcpy(auth_key + enc_iv_len, enc_key, enc_key_len);
    return ss_md5_hmac_with_key(auth, msg, msg_len, auth_key, enc_iv_len + enc_key_len);
}

int ss_sha1_hmac(char *auth, char *msg, int msg_len, uint8_t *iv, int enc_iv_len, uint8_t *enc_key, int enc_key_len)
{
    uint8_t auth_key[MAX_IV_LENGTH + MAX_KEY_LENGTH];
    memcpy(auth_key, iv, enc_iv_len);
    memcpy(auth_key + enc_iv_len, enc_key, enc_key_len);
    return ss_sha1_hmac_with_key(auth, msg, msg_len, auth_key, enc_iv_len + enc_key_len);
}

void memintcopy_lt(void *mem, uint32_t val) {
    ((uint8_t *)mem)[0] = (uint8_t)(val);
    ((uint8_t *)mem)[1] = (uint8_t)(val >> 8);
    ((uint8_t *)mem)[2] = (uint8_t)(val >> 16);
    ((uint8_t *)mem)[3] = (uint8_t)(val >> 24);
}

