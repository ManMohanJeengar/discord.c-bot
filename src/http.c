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
  rl->reser_after = 1.0f;

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
    sscanf
  }
}
