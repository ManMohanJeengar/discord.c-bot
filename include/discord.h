#ifndef DISCORD_H
#define DISCORD_H

#include <openssl/ssl.h>


typedef struct {
  int sockfd;
  SSL_CTX *ctx;
  SSL *ssl;
} Botconn;


int tls_connect(Botconn *conn, const char *host, const char *port);
void tls_disconnect(Botconn *conn);
int tls_write(Botconn *conn, const char *buf, int len);
int tls_read(Botconn *conn, char *buf, int len);

#endif
