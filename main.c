#include <stdio.h>
#include <string.h>
#include "discord.h"

int main(void){

  Botconn conn = {0};

  if(tls_init(&conn)!=0){
    return 1;
  }

  if(tls_connect(&conn, "discord.com", "443")!=0){
    fprintf(stderr,"connection failed\n");
    tls_cleanup(&conn);
    return 1;
  }


  const char *req =
    "GET / HTTP/1.1\r\n"
    "Host: discord.com\r\n"
    "Connection: close\r\n"
    "\r\n";
  

  tls_write(&conn, req, strlen(req));

  char buf[4096];
  int n;

  while((n = tls_read(&conn, buf, sizeof(buf)-1))>0){
    buf[n]='\0';
    printf("%s",buf);
  }

  tls_disconnect(&conn);
  tls_cleanup(&conn);
  return 0;

}

