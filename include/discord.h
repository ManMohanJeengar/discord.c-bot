#ifndef DISCORD_H
#define DISCORD_H

#include <openssl/ssl.h>
#include <stddef.h>


typedef struct {
  int limit;
  int remaining;
  float reset_after;
} RateLimit;


typedef struct {
  int status;
  char *body;
  size_t body_len;
  RateLimit ratelimit;
} HttpResponse;


typedef struct {
  int sockfd;
  SSL_CTX *ctx;
  SSL *ssl;
  int connected;
} Botconn;


int tls_init(Botconn *conn);
void tls_cleanup(Botconn *conn);
int tls_connect(Botconn *conn, const char *host, const char *port);
void tls_disconnect(Botconn *conn);
int tls_write(Botconn *conn, const char *buf, int len);
int tls_read(Botconn *conn, char *buf, int len);

int http_get(Botconn *conn, const char *path, const char *token, HttpResponse *resp);
int http_post(Botconn *conn, const char *path, const char *token, const char *body, HttpResponse *resp);
void http_response_free(HttpResponse *resp);

#endif
