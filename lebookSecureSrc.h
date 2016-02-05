

#include <stdio.h>


/**************************************************** V2(集成base64) ****************************************************/

/**
 *  加密方法
 *
 *  @param book    固定参数1(iOS 传的图书book_id)
 *  @param user    固定参数2(iOS 传的用户pin/uuid)
 *  @param str_src 需要加密的字节流，以NULL截止
 *
 *  @return 加密后的字节流，以NULL截止 （需调用者释放内存）
 */
char *lebook_book_string_encrypt(char *book, char *user, char *str_src);

/**
 *  解密方法
 *
 *  @param book    须与加密方法同名参数一致
 *  @param user    须与加密方法同名参数一致
 *  @param str_src 需要解密的密文
 *
 *  @return 解密后的字节流，以NULL截止 （需调用者释放内存）
 */
char *lebook_book_string_decrypt(char *book, char *user, char *str_src);

/**************************************************** V2(集成base64) ****************************************************/


/**
 *  加密方法
 *
 *  @param book    固定参数1(iOS 传的图书book_id)
 *  @param user    固定参数2(iOS 传的用户pin/uuid)
 *  @param str_src 需要加密的字节流，以NULL截止
 *
 *  @param out_len 加密后的字节流的长度
 *  @return 加密后的字节流，非NULL截止！（需调用者释放内存）
 */
char * lebook_book_infos_encrypt(char *book, char *user, char *str_src, size_t *out_len);



/**
 *  解密方法
 *
 *  @param book    须与加密方法同名参数一致
 *  @param user    须与加密方法同名参数一致
 *  @param str_src 需要解密的密文，非NULL截止！
 *  @param in_len  需要解密的密文的长度
 *
 *  @return 解密后的字节流，以NULL截止 （需调用者释放内存）
 */
char * lebook_book_infos_decrypt(char *book, char *user, char *str_src, const size_t in_len);


//NOTE:
// 加密后的字节流，非NULL截止！(可base64编码后存储，解密前要base64解码)
// iOS端同时加密了‘booksource’和图书文件的解密‘key’, 先解密验证‘booksource’，验证通过再解密‘key’

