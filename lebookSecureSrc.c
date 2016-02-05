

#include <stdlib.h>
#include <string.h>
#include "lebookaes.h"
#include "lebookmd5.h"
#include "Base64.h"
#include "lebookSecureSrc.h"

void print_hex(BYTE str[], int len)
{
    printf("\n");
    
    int idx;
    
    for(idx = 0; idx < len; idx++)
        printf("%02x", str[idx]);
    
    printf("\n");
}

void lebook_encrypt(const BYTE *key, char *str_1, char *str_2, BYTE *str_out ,size_t *len_out)
{
    size_t len_total = strlen(str_1) + strlen(str_2);
    *len_out = (len_total/16 + (len_total%16 ? 1:0))*16;
    
    BYTE *text = malloc(*len_out + 5);
    memset(text, 0, *len_out + 5);
    memcpy(text, str_1, strlen(str_1));
    strncat((char *)text, str_2, strlen(str_2));
    strncat((char *)text, "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^", (*len_out - len_total));
    
    BYTE iv[16] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};
    
    WORD key_schedule[60];
    
    lebook_aes_key_setup(key, key_schedule, 256);
    lebook_aes_encrypt_cbc(text, *len_out, str_out, key_schedule, 256, iv);
    
    free(text);
    text = NULL;
}

void lebook_decrypt(const BYTE *key, BYTE *str, size_t len_total, BYTE *str_out)
{
    BYTE iv[16] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};
    
    WORD key_schedule[60];
    
    lebook_aes_key_setup(key, key_schedule, 256);
    
    lebook_aes_decrypt_cbc(str, len_total, str_out, key_schedule, 256, iv);
}

void lebook_get_16(char *str_1, char *str_2, BYTE *buf)
{
    size_t len_total = strlen(str_1)*2 + strlen(str_2);
    BYTE *text = malloc(len_total + 5);
    memset(text, 0, len_total + 5);
    memcpy(text, str_1, strlen(str_1));
    strncat((char *)text, str_2, strlen(str_2));
    strncat((char *)text, str_1, strlen(str_1));
    
    MD5_CTX ctx;
    lebook_md5_init(&ctx);
    lebook_md5_update(&ctx, text, strlen((char *)text));
    lebook_md5_final(&ctx, buf);
    
//    printf("%u,text:%s\n",strlen(str_2), text);
    
    free(text);
    text = NULL;
}

char * lebook_book_infos_encrypt(char *book, char *user, char *str_src, size_t *out_len)
{
    if (book == 0 || str_src == 0){
        *out_len = 0;
        return (char *)"";
    }
    
    if (user == 0) user = "1101";
    
    if (strlen(book) == 0 || strlen(str_src) == 0 || strlen(user) == 0) {
        *out_len = 0;
        return (char *)"";
    }
    
    char salts[3][9] = {
        {0x6e,0x54,0xbd,0x57,0x1d,0xd2,0x72,0x52,0x00},
        {0xbf,0xa6,0x9c,0xe0,0x81,0x7f,0x6b,0x18,0x00},
        {0xb5,0x03,0x22,0x57,0xf1,0xd2,0x72,0x52,0x00},
    };
    
    BYTE key[32] = {0x00};
    
    lebook_get_16(salts[0], book, key);
    lebook_get_16(user, salts[1],(key+16));
    
//    print_hex(key,32);

    BYTE *encd_str = malloc(strlen(str_src) + 64);
    memset(encd_str, 0, strlen(str_src) + 64);
    
    lebook_encrypt(key, str_src, salts[2], encd_str, out_len);
    
//    printf("encd_str:");
//    print_hex(encd_str,*out_len);
    
    return (char *)encd_str;
}

char * lebook_book_infos_decrypt(char *book, char *user, char *str_src, const size_t in_len)
{
    if (book == 0 || str_src == 0){
        return (char *)"";
    }
    
    if (user == 0) user = "1101";
    
    if (strlen(book) == 0 || in_len == 0 || strlen(user) == 0) {
        return (char *)"";
    }
    
    char salts[3][9] = {
        {0x6e,0x54,0xbd,0x57,0x1d,0xd2,0x72,0x52,0x00},
        {0xbf,0xa6,0x9c,0xe0,0x81,0x7f,0x6b,0x18,0x00},
        {0xb5,0x03,0x22,0x57,0xf1,0xd2,0x72,0x52,0x00},
    };
    
    BYTE key[32] = {0x00};
    
    lebook_get_16(salts[0], book, key);
    lebook_get_16(user, salts[1], (key+16));
    
//    print_hex(key,32);
    
    BYTE *str_out = malloc(in_len + 32);
    memset(str_out, 0, in_len + 32);
    
//    printf("str_src:");
//    print_hex(str_src, in_len);
    
    lebook_decrypt(key, str_src, in_len, str_out);
    
    // 祛渍
    size_t idx = in_len - 8;
    while (idx > 0) {
        if (!memcmp(salts[2], str_out+idx, 8)) {
            memset(str_out+idx, 0, in_len - idx);
            break;
        }
        idx -= 1;
    }
    
    
    
    return (char *)str_out;
}



/**************************************************** V2(集成base64) ****************************************************/

char *lebook_book_string_encrypt(char *book, char *user, char *str_src)
{
    size_t len_cipher;
    char *cipher = lebook_book_infos_encrypt(book, user, str_src, &len_cipher);
    
    int len_out = (int)(len_cipher/3 + 2)*4;
    char *str_out = malloc(len_out + 5);
    memset(str_out, 0, len_out + 5);
    
    struct base64_context context;
    base64_init(&context, 0);
    base64_encode(&context, cipher, (int)len_cipher, str_out, &len_out);
    
    if (cipher != NULL) {
        free(cipher);
        cipher = NULL;
    }
    
    return str_out;
}

char *lebook_book_string_decrypt(char *book, char *user, char *str_src)
{
    int len_cipher = (int)(strlen(str_src)/4 + 2)*3;
    char *cipher = malloc(len_cipher + 5);
    memset(cipher, 0, len_cipher + 5);
    
    struct base64_context context;
    base64_init(&context, 0);
    base64_decode(&context, str_src, (int)strlen(str_src), cipher, &len_cipher);
    char *string = lebook_book_infos_decrypt(book, user, cipher, len_cipher);
    
    if (cipher != NULL) {
        free(cipher);
        cipher = NULL;
    }
    
    return string;
}
