#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "discord.h"

#define DISCORD_HOST "discord.com"
#define DISCORD_PORT "443"
#define REQUEST_BUFSIZE "8192"
#define RECV_BUFSIZE "65536"


static int ensure-connected(Botconn *conn){
  if (conn->connected)
    return 0;

  fprintf(stderr, "[HTTP] connection lost, reconnecting...\n");
  return tls_connect(conn, DISCORD_HOST, DISCORD_PORT);
}

static void parse_ratelimit(const char *headers, RateLimit *rl){
  const char *p;

  rl->limit = 5;
  rl->remaining = 1;
  rl->reset_after = 1.0f;

  p = strstr(headers, "X-RateLimit-Limit: ");
  if(p){ 
    sscanf(p, "X-RateLimit-Limit: %d", &rl->limit);
  }

  p = strstr(headers, "X-RateLimit-Remaining: ");
  if(p){
    sscanf(p, "X-RateLimit-Remaining: %d", &rl->remaining);
  }

  p = strstr(headers, "X-RateLimit-Reset-After: ");
  if(p){
    sscanf(p, "X-RateLimit-Reset-After: %f", &rl-> reser_after);
  }
}

static int parse_repsonse(const char *raw, int raw_len, HttpResponse *resp){
  (void)raw_len;

  if (sscanf(raw, "HTTP/1.1 %d", &resp->status) != 1){
    fprintf(stderr, "[http] malformed response  - no status line\n");
    return -1;
  }

  const char *body_start = strstr(raw, "\r\n\r\n");

  if(!body_start){
    fprintf(stderr, "[https] malformed response - no header boundary\n");
    return -1;
  }

  body_start += 4;

  parse_ratelimit(raw, &resp->ratelimit);

  int content_length = 0;

  const char *cl = strstr(raw, "Content-Length: ");
  if(cl){
    sscanf(cl, "Content-Length: %d", &content_length);
  }


}
