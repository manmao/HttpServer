#ifdef _USE_HTTP_SSL
#include "config_ssl.h"

SSL_CTX* SSL_preliminaries(const char *certpath,const char *keypath){

   printf("%s %s\n",certpath,keypath);

   SSL_CTX* ctx;
   /* SSL 库初始化 */
   SSL_library_init();
   SSL_load_error_strings();
    /* 载入所有 SSL 算法 */
   OpenSSL_add_all_algorithms();
   ctx = SSL_CTX_new(SSLv23_server_method());

   if (!ctx) {
     ERR_print_errors_fp(stderr);
     exit(2);
   }

   if (SSL_CTX_use_certificate_file(ctx, certpath, SSL_FILETYPE_PEM) <= 0) {
      ERR_print_errors_fp(stderr);
      exit(3);
   }

   if (SSL_CTX_use_PrivateKey_file(ctx, keypath, SSL_FILETYPE_PEM) <= 0) {
       ERR_print_errors_fp(stderr);
       exit(4);
   }

   if (!SSL_CTX_check_private_key(ctx)) {
      fprintf(stderr,"Private key does not match the certificate public key\n");
      exit(5);
   }

   return ctx;
}


 SSL* createSSLInstence(SSL_CTX* ctx,int sd){

      /* TCP connection is ready. Do server side SSL. */
      int err;
      SSL*     ssl;
      ssl = SSL_new(ctx);   CHK_NULL(ssl);
      SSL_set_fd(ssl, sd); //fd必须是非阻塞的，否则会报错
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


void freeSSL(SSL_CTX* ctx,SSL* ssl)
{
    SSL_free (ssl);
    SSL_CTX_free (ctx);
}

#endif

