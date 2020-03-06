#include <string.h>
#include <time.h>
#include "auth.h"
#include "obfsutil.h"
#include "crc32.h"
#include "base64.h"
#include "encrypt.h"

uint32_t g_endian_test = 1;

typedef struct shift128plus_ctx {
    uint64_t v[2];
}shift128plus_ctx;

void swap(int *x, int *y) {
	int t = *x;
	*x = *y;
	*y = t;
}

//https://zh.wikipedia.org/wiki/%E5%BF%AB%E9%80%9F%E6%8E%92%E5%BA%8F
void quick_sort_recursive(int arr[], int start, int end) {
	if (start >= end)
		return;//這是為了防止宣告堆疊陣列時當機
	int mid = arr[end];
	int left = start, right = end - 1;
	while (left < right) {
		while (arr[left] < mid && left < right)
			left++;
		while (arr[right] >= mid && left < right)
			right--;
		swap(&arr[left], &arr[right]);
	}
	if (arr[left] >= arr[end])
		swap(&arr[left], &arr[end]);
	else
		left++;
    if (left) {
        quick_sort_recursive(arr, start, left - 1);
    }
    quick_sort_recursive(arr, left + 1, end);
}

void quick_sort(int arr[], int len) {
	 quick_sort_recursive(arr, 0, len - 1);
}

int array_bin_search(int start, int end, int array[], int value)
{
  	int mid = (start + end) / 2;

  	if(start > end) {
        if (value > array[start]) {
            return start + 1;
        }
        else {
            return start;
        }
  	}

  	if(array[mid] > value) {
  		  return array_bin_search(start, mid - 1, array, value);
  	} else {
  		  return array_bin_search(mid + 1, end, array, value);
  	}
}

uint64_t shift128plus_next(shift128plus_ctx* ctx) {
    uint64_t x = ctx->v[0];
    uint64_t y = ctx->v[1];
    ctx->v[0] = y;
    x ^= x << 23;
    x ^= (y ^ (x >> 17) ^ (y >> 26));
    ctx->v[1] = x;
    return x + y;
}

void i64_memcpy(uint8_t* target, uint8_t* source)
{
    for (int i = 0; i < 8; ++i)
        target[i] = source[7 - i];
}

void shift128plus_init_from_bin(shift128plus_ctx* ctx, uint8_t* bin, int bin_size) {
    uint8_t fill_bin[16] = {0};
    memcpy(fill_bin, bin, bin_size);
    if (*(uint8_t*)&g_endian_test == 1) {
        memcpy(ctx, fill_bin, 16);
    } else {
        i64_memcpy((uint8_t*)ctx, fill_bin);
        i64_memcpy((uint8_t*)ctx + 8, fill_bin + 8);
    }
}

void shift128plus_init_from_bin_datalen(shift128plus_ctx* ctx, uint8_t* bin, int bin_size, int datalen) {
    uint8_t fill_bin[16] = {0};
    memcpy(fill_bin, bin, bin_size);
    fill_bin[0] = datalen;
    fill_bin[1] = datalen >> 8;
    if (*(uint8_t*)&g_endian_test == 1) {
        memcpy(ctx, fill_bin, 16);
    } else {
        i64_memcpy((uint8_t*)ctx, fill_bin);
        i64_memcpy((uint8_t*)ctx + 8, fill_bin + 8);
    }
    for (int i = 0; i < 4; ++i) {
        shift128plus_next(ctx);
    }
}

typedef struct auth_chain_global_data {
    uint8_t local_client_id[4];
    uint32_t connection_id;
}auth_chain_global_data;

typedef unsigned int (*rnd_func)(int datalength, shift128plus_ctx* random, uint8_t* last_hash, int data_size_list[], int data_size_list_len, int data_size_list2[], int data_size_list2_len, int overhead);

typedef struct auth_chain_local_data {
    int has_sent_header;
    char * recv_buffer;
    int recv_buffer_size;
    uint32_t recv_id;
    uint32_t pack_id;
    char * salt;
    uint8_t * user_key;
    char uid[4];
    int user_key_len;
    int last_data_len;
    uint8_t last_client_hash[16];
    uint8_t last_server_hash[16];
    shift128plus_ctx random_client;
    shift128plus_ctx random_server;
    int cipher_init_flag;
    cipher_env_t cipher;
    enc_ctx_t* cipher_client_ctx;
    enc_ctx_t* cipher_server_ctx;
    int *data_size_list;
    int data_size_list_len;
    rnd_func rnd;
    int *data_size_list2;
    int data_size_list2_len;
}auth_chain_local_data;

void auth_chain_local_data_init(auth_chain_local_data* local) {
    local->has_sent_header = 0;
    local->recv_buffer = (char*)malloc(16384);
    local->recv_buffer_size = 0;
    local->recv_id = 1;
    local->pack_id = 1;
    local->salt = "";
    local->user_key = 0;
    local->user_key_len = 0;
    local->cipher_init_flag = 0;
    local->cipher_client_ctx = 0;
    local->cipher_server_ctx = 0;
    local->rnd = 0;
    local->data_size_list = NULL;
    local->data_size_list_len = 0;
    local->data_size_list2 = NULL;
    local->data_size_list2_len = 0;//Thanks to @hiboy
}

void * auth_chain_a_init_data() {
    auth_chain_global_data *global = (auth_chain_global_data*)malloc(sizeof(auth_chain_global_data));
    rand_bytes(global->local_client_id, 4);
    rand_bytes((uint8_t*)&global->connection_id, 4);
    global->connection_id &= 0xFFFFFF;
    return global;
}

unsigned int auth_chain_a_get_rand_len(int datalength, shift128plus_ctx* random, uint8_t* last_hash, int data_size_list[], int data_size_list_len, int data_size_list2[], int data_size_list2_len, int overhead) {
    if (datalength > 1440)
        return 0;
    shift128plus_init_from_bin_datalen(random, last_hash, 16, datalength);
    if (datalength > 1300)
        return shift128plus_next(random) % 31;
    if (datalength > 900)
        return shift128plus_next(random) % 127;
    if (datalength > 400)
        return shift128plus_next(random) % 521;
    return shift128plus_next(random) % 1021;
}

obfs * auth_chain_a_new_obfs() {
    obfs * self = new_obfs();
    self->l_data = malloc(sizeof(auth_chain_local_data));
    auth_chain_local_data_init((auth_chain_local_data*)self->l_data);
    ((auth_chain_local_data*)self->l_data)->salt = "auth_chain_a";
    ((auth_chain_local_data*)self->l_data)->rnd = auth_chain_a_get_rand_len;
    return self;
}

int auth_chain_a_get_overhead(obfs *self) {
    return 4;
}

void auth_chain_a_dispose(obfs *self) {
    auth_chain_local_data *local = (auth_chain_local_data*)self->l_data;
    if (local->recv_buffer != NULL) {
        free(local->recv_buffer);
        local->recv_buffer = NULL;
    }
    if (local->user_key != NULL) {
        free(local->user_key);
        local->user_key = NULL;
    }
    if (local->cipher_init_flag) {
        if (local->cipher_client_ctx) {
            enc_ctx_release(&local->cipher, local->cipher_client_ctx);
        }
        if (local->cipher_server_ctx) {
            enc_ctx_release(&local->cipher, local->cipher_server_ctx);
        }
        enc_release(&local->cipher);
        local->cipher_init_flag = 0;
    }
    if(local->data_size_list != NULL) {
        free(local->data_size_list);
    }
    if(local->data_size_list2 != NULL) {
        free(local->data_size_list2);
    }
    free(local);
    self->l_data = NULL;
    dispose_obfs(self);
}

void auth_chain_set_server_info(obfs *self, server_info *server) {
    server->overhead = 4;
    memmove(&self->server, server, sizeof(server_info));
}

unsigned int udp_get_rand_len(shift128plus_ctx* random, uint8_t* last_hash) {
    shift128plus_init_from_bin(random, last_hash, 16);
    return shift128plus_next(random) % 127;
}

unsigned int get_rand_start_pos(int rand_len, shift128plus_ctx* random) {
    if (rand_len > 0)
        return shift128plus_next(random) % 8589934609 % rand_len;
    return 0;
}

unsigned int get_client_rand_len(auth_chain_local_data *local, int datalength, int overhead) {
    return local->rnd(datalength, &local->random_client, local->last_client_hash, local->data_size_list, local->data_size_list_len, local->data_size_list2, local->data_size_list2_len, overhead);
}

unsigned int get_server_rand_len(auth_chain_local_data *local, int datalength, int overhead) {
    return local->rnd(datalength, &local->random_server, local->last_server_hash, local->data_size_list, local->data_size_list_len, local->data_size_list2, local->data_size_list2_len, overhead);
}

int auth_chain_a_pack_data(char *data, int datalength, char *outdata, auth_chain_local_data *local, server_info *server) {
    unsigned int rand_len = get_client_rand_len(local, datalength, server->overhead);
    int out_size = (int)rand_len + datalength + 2;
    outdata[0] = (char)((uint8_t)datalength ^ local->last_client_hash[14]);
    outdata[1] = (char)((uint8_t)(datalength >> 8) ^ local->last_client_hash[15]);

    {
        uint8_t rnd_data[rand_len];
        rand_bytes(rnd_data, (int)rand_len);
        if (datalength > 0) {
            int start_pos = get_rand_start_pos(rand_len, &local->random_client);
            size_t out_len;
            ss_encrypt_buffer(&local->cipher, local->cipher_client_ctx,
                    data, datalength, &outdata[2 + start_pos], &out_len);
            memcpy(outdata + 2, rnd_data, start_pos);
            memcpy(outdata + 2 + start_pos + datalength, rnd_data + start_pos, rand_len - start_pos);
        } else {
            memcpy(outdata + 2, rnd_data, rand_len);
        }
    }

    uint8_t key_len = (uint8_t)(local->user_key_len + 4);
    uint8_t key[key_len];
    memcpy(key, local->user_key, local->user_key_len);
    memintcopy_lt(key + key_len - 4, local->pack_id);
    ++local->pack_id;

    ss_md5_hmac_with_key((char*)local->last_client_hash, outdata, out_size, key, key_len);
    memcpy(outdata + out_size, local->last_client_hash, 2);

    return out_size + 2;
}

int auth_chain_a_pack_auth_data(auth_chain_global_data *global, server_info *server, auth_chain_local_data *local, char *data, int datalength, char *outdata) {
    const int authhead_len = 4 + 8 + 4 + 16 + 4;
    const char* salt = local->salt;
    int out_size = authhead_len;

    ++global->connection_id;
    if (global->connection_id > 0xFF000000) {
        rand_bytes(global->local_client_id, 8);
        rand_bytes((uint8_t*)&global->connection_id, 4);
        global->connection_id &= 0xFFFFFF;
    }

    char encrypt[20];

    uint8_t key[server->iv_len + server->key_len];
    uint8_t key_len = (uint8_t)(server->iv_len + server->key_len);
    memcpy(key, server->iv, server->iv_len);
    memcpy(key + server->iv_len, server->key, server->key_len);

    time_t t = time(NULL);
    memintcopy_lt(encrypt, (uint32_t)t);
    memcpy(encrypt + 4, global->local_client_id, 4);
    memintcopy_lt(encrypt + 8, global->connection_id);
    encrypt[12] = (char)server->overhead;
    encrypt[13] = (char)(server->overhead >> 8);
    encrypt[14] = 0;
    encrypt[15] = 0;

    // first 12 bytes
    {
        rand_bytes((uint8_t*)outdata, 4);
        ss_md5_hmac_with_key((char*)local->last_client_hash, (char*)outdata, 4, key, key_len);
        memcpy(outdata + 4, local->last_client_hash, 8);
    }
    // uid & 16 bytes auth data
    {
        uint8_t uid[4];
        if (local->user_key == NULL) {
            if(server->param != NULL && server->param[0] != 0) {
                char *param = server->param;
                char *delim = strchr(param, ':');
                if(delim != NULL) {
                    char uid_str[16] = {};
                    strncpy(uid_str, param, delim - param);
                    char key_str[128];
                    strcpy(key_str, delim + 1);
                    long uid_long = strtol(uid_str, NULL, 10);
                    memintcopy_lt((char*)local->uid, (uint32_t)uid_long);

                    local->user_key_len = (int)strlen(key_str);
                    local->user_key = (uint8_t*)malloc((size_t)local->user_key_len);
                    memcpy(local->user_key, key_str, local->user_key_len);
                }
            }
            if (local->user_key == NULL) {
                rand_bytes((uint8_t*)local->uid, 4);

                local->user_key_len = (int)server->key_len;
                local->user_key = (uint8_t*)malloc((size_t)local->user_key_len);
                memcpy(local->user_key, server->key, local->user_key_len);
            }
        }
        for (int i = 0; i < 4; ++i)
        {
            uid[i] = local->uid[i] ^ local->last_client_hash[8 + i];
        }

        char encrypt_key_base64[256] = {0};
        unsigned char encrypt_key[local->user_key_len];
        memcpy(encrypt_key, local->user_key, local->user_key_len);
        base64_encode(encrypt_key, (unsigned int)local->user_key_len, encrypt_key_base64);

        int salt_len = strlen(salt);
        int base64_len = (local->user_key_len + 2) / 3 * 4;
        memcpy(encrypt_key_base64 + base64_len, salt, salt_len);

        char enc_key[16];
        int enc_key_len = base64_len + salt_len;
        bytes_to_key_with_size(encrypt_key_base64, (size_t)enc_key_len, (uint8_t*)enc_key, 16);
        char encrypt_data[16];
        ss_aes_128_cbc(encrypt, encrypt_data, enc_key);
        memcpy(encrypt, uid, 4);
        memcpy(encrypt + 4, encrypt_data, 16);
    }
    // final HMAC
    {
        ss_md5_hmac_with_key((char*)local->last_server_hash, encrypt, 20, local->user_key, local->user_key_len);
        memcpy(outdata + 12, encrypt, 20);
        memcpy(outdata + 12 + 20, local->last_server_hash, 4);
    }

    char password[256] = {0};
    base64_encode(local->user_key, local->user_key_len, password);
    base64_encode(local->last_client_hash, 16, password + strlen(password));
    local->cipher_init_flag = 1;
    enc_init(&local->cipher, password, "rc4");
    local->cipher_client_ctx = malloc(sizeof(enc_ctx_t));
    local->cipher_server_ctx = malloc(sizeof(enc_ctx_t));
    enc_ctx_init(&local->cipher, local->cipher_client_ctx, 1);
    enc_ctx_init(&local->cipher, local->cipher_server_ctx, 0);

    out_size += auth_chain_a_pack_data(data, datalength, outdata + out_size, local, server);

    return out_size;
}

int auth_chain_a_client_pre_encrypt(obfs *self, char **pplaindata, int datalength, size_t* capacity) {
    char *plaindata = *pplaindata;
    server_info *server = (server_info*)&self->server;
    auth_chain_local_data *local = (auth_chain_local_data*)self->l_data;
    char * out_buffer = (char*)malloc((size_t)(datalength * 2 + 4096));
    char * buffer = out_buffer;
    char * data = plaindata;
    int len = datalength;
    int pack_len;
    if (len > 0 && local->has_sent_header == 0) {
        int head_size = 1200;
        if (head_size > datalength)
            head_size = datalength;
        pack_len = auth_chain_a_pack_auth_data((auth_chain_global_data *)self->server.g_data, &self->server, local, data, head_size, buffer);
        buffer += pack_len;
        data += head_size;
        len -= head_size;
        local->has_sent_header = 1;
    }
    int unit_size = server->tcp_mss - server->overhead;
    while ( len > unit_size ) {
        pack_len = auth_chain_a_pack_data(data, unit_size, buffer, local, &self->server);
        buffer += pack_len;
        data += unit_size;
        len -= unit_size;
    }
    if (len > 0) {
        pack_len = auth_chain_a_pack_data(data, len, buffer, local, &self->server);
        buffer += pack_len;
    }
    len = (int)(buffer - out_buffer);
    if ((int)*capacity < len) {
        *pplaindata = (char*)realloc(*pplaindata, *capacity = (size_t)(len * 2));
        plaindata = *pplaindata;
    }
    local->last_data_len = datalength;
    memmove(plaindata, out_buffer, len);
    free(out_buffer);
    return len;
}

int auth_chain_a_client_post_decrypt(obfs *self, char **pplaindata, int datalength, size_t* capacity) {
    char *plaindata = *pplaindata;
    auth_chain_local_data *local = (auth_chain_local_data*)self->l_data;
    server_info *server = (server_info*)&self->server;
    uint8_t * recv_buffer = (uint8_t *)local->recv_buffer;
    if (local->recv_buffer_size + datalength > 16384)
        return -1;
    memmove(recv_buffer + local->recv_buffer_size, plaindata, datalength);
    local->recv_buffer_size += datalength;

    int key_len = local->user_key_len + 4;
    uint8_t *key = (uint8_t*)malloc((size_t)key_len);
    memcpy(key, local->user_key, local->user_key_len);

    char * out_buffer = (char*)malloc((size_t)local->recv_buffer_size);
    char * buffer = out_buffer;
    char error = 0;
    while (local->recv_buffer_size > 4) {
        memintcopy_lt(key + key_len - 4, local->recv_id);

        int data_len = (int)(((unsigned)(recv_buffer[1] ^ local->last_server_hash[15]) << 8) + (recv_buffer[0] ^ local->last_server_hash[14]));
        int rand_len = get_server_rand_len(local, data_len, server->overhead);
        int len = rand_len + data_len;
        if (len >= 4096) {
            local->recv_buffer_size = 0;
            error = 1;
            break;
        }
        if ((len += 4) > local->recv_buffer_size)
            break;

        char hash[16];
        ss_md5_hmac_with_key(hash, (char*)recv_buffer, len - 2, key, key_len);
        if (memcmp(hash, recv_buffer + len - 2, 2)) {
            local->recv_buffer_size = 0;
            error = 1;
            break;
        }

        int pos;
        if (data_len > 0 && rand_len > 0)
        {
            pos = 2 + get_rand_start_pos(rand_len, &local->random_server);
        }
        else
        {
            pos = 2;
        }
        size_t out_len;
        ss_decrypt_buffer(&local->cipher, local->cipher_server_ctx,
                (char*)recv_buffer + pos, data_len, buffer, &out_len);

        if (local->recv_id == 1) {
            server->tcp_mss = (uint8_t)buffer[0] | ((uint8_t)buffer[1] << 8);
            memmove(buffer, buffer + 2, out_len -= 2);
        }
        memcpy(local->last_server_hash, hash, 16);
        ++local->recv_id;
        buffer += out_len;
        memmove(recv_buffer, recv_buffer + len, local->recv_buffer_size -= len);
    }
    int len;
    if (error == 0) {
        len = (int)(buffer - out_buffer);
        if ((int)*capacity < len) {
            *pplaindata = (char*)realloc(*pplaindata, *capacity = (size_t)(len * 2));
            plaindata = *pplaindata;
        }
        memmove(plaindata, out_buffer, len);
    } else {
        len = -1;
    }
    free(out_buffer);
    free(key);
    return len;
}

int auth_chain_a_client_udp_pre_encrypt(obfs *self, char **pplaindata, int datalength, size_t* capacity) {
    char *plaindata = *pplaindata;
    server_info *server = (server_info*)&self->server;
    auth_chain_local_data *local = (auth_chain_local_data*)self->l_data;
    char out_buffer[datalength + 1024];

    if (local->user_key == NULL) {
        if(self->server.param != NULL && self->server.param[0] != 0) {
            char *param = self->server.param;
            char *delim = strchr(param, ':');
            if(delim != NULL) {
                char uid_str[16] = {};
                strncpy(uid_str, param, delim - param);
                char key_str[128];
                strcpy(key_str, delim + 1);
                long uid_long = strtol(uid_str, NULL, 10);
                memintcopy_lt(local->uid, (uint32_t)uid_long);

                local->user_key_len = (int)strlen(key_str);
                local->user_key = (uint8_t*)malloc((size_t)local->user_key_len);
                memcpy(local->user_key, key_str, local->user_key_len);
            }
        }
        if (local->user_key == NULL) {
            rand_bytes((uint8_t *)local->uid, 4);

            local->user_key_len = (int)self->server.key_len;
            local->user_key = (uint8_t*)malloc((size_t)local->user_key_len);
            memcpy(local->user_key, self->server.key, local->user_key_len);
        }
    }

    char auth_data[3];
    uint8_t hash[16];
    ss_md5_hmac_with_key((char*)hash, auth_data, 3, server->key, server->key_len);
    int rand_len = udp_get_rand_len(&local->random_client, hash);
    uint8_t rnd_data[rand_len];
    rand_bytes(rnd_data, (int)rand_len);
    int outlength = datalength + rand_len + 8;

    char password[256] = {0};
    base64_encode(local->user_key, local->user_key_len, password);
    base64_encode(hash, 16, password + strlen(password));

    {
        enc_init(&local->cipher, password, "rc4");
        enc_ctx_t ctx;
        enc_ctx_init(&local->cipher, &ctx, 1);
        size_t out_len;
        ss_encrypt_buffer(&local->cipher, &ctx,
                plaindata, datalength, out_buffer, &out_len);
        enc_ctx_release(&local->cipher, &ctx);
        enc_release(&local->cipher);
    }
    uint8_t uid[4];
    for (int i = 0; i < 4; ++i) {
        uid[i] = local->uid[i] ^ hash[i];
    }
    memmove(out_buffer + datalength, rnd_data, rand_len);
    memmove(out_buffer + outlength - 8, auth_data, 3);
    memmove(out_buffer + outlength - 5, uid, 4);

    ss_md5_hmac_with_key((char*)hash, out_buffer, outlength - 1, local->user_key, local->user_key_len);
    memmove(out_buffer + outlength - 1, hash, 1);

    if ((int)*capacity < outlength) {
        *pplaindata = (char*)realloc(*pplaindata, *capacity = (size_t)(outlength * 2));
        plaindata = *pplaindata;
    }
    memmove(plaindata, out_buffer, outlength);
    return outlength;
}

int auth_chain_a_client_udp_post_decrypt(obfs *self, char **pplaindata, int datalength, size_t* capacity) {
    if (datalength <= 8)
        return 0;

    char *plaindata = *pplaindata;
    server_info *server = (server_info*)&self->server;
    auth_chain_local_data *local = (auth_chain_local_data*)self->l_data;

    uint8_t hash[16];
    ss_md5_hmac_with_key((char*)hash, plaindata, datalength - 1, local->user_key, local->user_key_len);

    if (*hash != ((uint8_t*)plaindata)[datalength - 1])
        return 0;

    ss_md5_hmac_with_key((char*)hash, plaindata + datalength - 8, 7, server->key, server->key_len);

    int rand_len = udp_get_rand_len(&local->random_server, hash);
    int outlength = datalength - rand_len - 8;

    char password[256] = {0};
    base64_encode(local->user_key, local->user_key_len, password);
    base64_encode(hash, 16, password + strlen(password));

    {
        enc_init(&local->cipher, password, "rc4");
        enc_ctx_t ctx;
        enc_ctx_init(&local->cipher, &ctx, 0);
        size_t out_len;
        ss_decrypt_buffer(&local->cipher, &ctx,
                plaindata, outlength, plaindata, &out_len);
        enc_ctx_release(&local->cipher, &ctx);
        enc_release(&local->cipher);
    }

    return outlength;
}

//auth_chain_b
void auth_chain_b_set_server_info(obfs *self, server_info *server) {
    memmove(&self->server, server, sizeof(server_info));

    auth_chain_local_data *local = (auth_chain_local_data*)self->l_data;
    shift128plus_ctx* random = &(local->random_server);
    shift128plus_init_from_bin(random, server->key, 16);
    local->data_size_list_len = shift128plus_next(random) % 8 + 4;
    local->data_size_list = (int*)malloc(local->data_size_list_len * sizeof(int));

    int i;
    for(i = 0; i < local->data_size_list_len; i++) {
        local->data_size_list[i] = shift128plus_next(random) % 2340 % 2040 % 1440;
    }

    quick_sort(local->data_size_list, local->data_size_list_len);

    local->data_size_list2_len = shift128plus_next(random) % 16 + 8;
    local->data_size_list2 = (int*)malloc(local->data_size_list2_len * sizeof(int)); //Thanks to anonymous

    for(i = 0; i < local->data_size_list2_len; i++) {
        local->data_size_list2[i] = shift128plus_next(random) % 2340 % 2040 % 1440;
    }

    quick_sort(local->data_size_list2, local->data_size_list2_len);

}

unsigned int auth_chain_b_get_rand_len(int datalength, shift128plus_ctx* random, uint8_t* last_hash, int data_size_list[], int data_size_list_len, int data_size_list2[], int data_size_list2_len, int overhead) {
    if (datalength > 1440)
        return 0;
    shift128plus_init_from_bin_datalen(random, last_hash, 16, datalength);
    int pos = array_bin_search(0, data_size_list_len - 1, data_size_list, datalength + overhead);
    int final_pos = pos + shift128plus_next(random) % data_size_list_len;
    if(final_pos < data_size_list_len) {
        return data_size_list[final_pos] - datalength - overhead;
    }

    pos = array_bin_search(0, data_size_list2_len - 1, data_size_list2, datalength + overhead);
    final_pos = pos + shift128plus_next(random) % data_size_list2_len;
    if(final_pos < data_size_list2_len) {
        return data_size_list2[final_pos] - datalength - overhead;
    }
    if(final_pos < pos + data_size_list2_len - 1) {
        return 0;
    }

    if (datalength > 1300)
        return shift128plus_next(random) % 31;
    if (datalength > 900)
        return shift128plus_next(random) % 127;
    if (datalength > 400)
        return shift128plus_next(random) % 521;
    return shift128plus_next(random) % 1021;
}

obfs * auth_chain_b_new_obfs() {
    obfs * self = new_obfs();
    self->l_data = malloc(sizeof(auth_chain_local_data));
    auth_chain_local_data_init((auth_chain_local_data*)self->l_data);
    ((auth_chain_local_data*)self->l_data)->salt = "auth_chain_b";
    ((auth_chain_local_data*)self->l_data)->rnd = auth_chain_b_get_rand_len;
    return self;
}
