#ifdef _USE_HTTP_SSL
#include "config_ssl.h"

//初始化时调用，根据证书生成一个SSL上下文
SSL_CTX* SSL_preliminaries(const char *certpath,const char *keypath){

   printf("%s %s\n",certpath,keypath);

   SSL_CTX* ctx;
  /* SSL 库初始化 */
   SSL_library_init();
   /* 载入所有 SSL 错误消息 */
   SSL_load_error_strings();
    /* 载入所有 SSL 算法 */
   OpenSSL_add_all_algorithms();
    /* 以 SSL V2 和 V3 标准兼容方式产生一个 SSL_CTX ，即 SSL Content Text */
   ctx = SSL_CTX_new(SSLv23_server_method());
 /* 也可以用 SSLv2_server_method() 或 SSLv3_server_method() 单独表示 V2 或 V3标准 */
   if (!ctx) {
     ERR_print_errors_fp(stderr);
     exit(2);
   }
    /* 载入用户的数字证书， 此证书用来发送给客户端。 证书里包含有公钥 */
   if (SSL_CTX_use_certificate_file(ctx, certpath, SSL_FILETYPE_PEM) <= 0) {
      ERR_print_errors_fp(stderr);
      exit(3);
   }
   /* 载入用户私钥 */
   if (SSL_CTX_use_PrivateKey_file(ctx, keypath, SSL_FILETYPE_PEM) <= 0) {
       ERR_print_errors_fp(stderr);
       exit(4);
   }
   /* 检查用户私钥是否正确 */
   if (!SSL_CTX_check_private_key(ctx)) {
      fprintf(stderr,"Private key does not match the certificate public key\n");
      exit(5);
   }

   return ctx;
}


//当套接字编程 accept()获取到客户端链接时，通过此函数，创建SSL对象
 SSL* createSSLInstence(SSL_CTX* ctx,int sd){

      /* TCP connection is ready. Do server side SSL. */
      int err;
      SSL*     ssl;
       /* 基于 ctx 产生一个新的 SSL */
      ssl = SSL_new(ctx);   CHK_NULL(ssl);
      SSL_set_fd(ssl, sd);   /* 将连接用户的 socket 加入到 SSL */
       /* 建立 SSL 连接 */
      err = SSL_accept(ssl);
      if(err == -1)
           printf("#######SSL_accept_error#######\n");
      CHK_SSL(err);
      /* Get the cipher - opt */
      //printf ("SSL connection using %s\n", SSL_get_cipher (ssl));
      return ssl;
}


/* Get client's certificate (note: beware of dynamic allocation) - opt */
void getClientCert(SSL* ssl)
{
    X509*    client_cert;
    char*    str;
    client_cert = SSL_get_peer_certificate (ssl);
    if (client_cert != NULL)
    {
        printf ("Client certificate:\n");

        str = X509_NAME_oneline (X509_get_subject_name (client_cert), 0, 0);
        CHK_NULL(str);
        printf ("\t subject: %s\n", str);
        OPENSSL_free (str);

        str = X509_NAME_oneline (X509_get_issuer_name  (client_cert), 0, 0);
        CHK_NULL(str);
        printf ("\t issuer: %s\n", str);
        OPENSSL_free (str);

    /* We could do all sorts of certificate verification stuff here before
       deallocating the certificate. */
        X509_free (client_cert);

    } else{
        printf ("Client does not have certificate.\n");
    }


}


//释放连接
void freeSSL(SSL_CTX* ctx,SSL* ssl)
{
    /* 关闭 SSL 连接 */
    SSL_shutdown(ssl);
    /* 释放 SSL */
    SSL_free (ssl);
    /* 释放 CTX */
    SSL_CTX_free (ctx);
}

#endif

