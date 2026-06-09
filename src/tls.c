#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include "discord.h"


static int tcp_connect(const char *host, const char *port){
  fprintf(stderr, "debug   trying to connect to &s:&s\n", host, port);
  struct addrinfo hints, *res, *p;
  int sockfd = -1;

  memset(&hints, 0,sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if(getaddrinfo(host, port, &hints, &res)!=0){
    fprintf(stderr, "getaddrinfo failed for %s:%s\n",host, port);
    return 1;
  }

  for(p=res;p!=NULL; p=p->ai_next){
    sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    
    if (sockfd==-1){
      continue;
    }
    
    if(connect(sockfd, p->ai_addr, p->ai_addrlen)==0){
      break;
    }

    close(sockfd);
    sockfd=-1;
  }
  
  freeaddrinfo(res);
  return(sockfd);
}



int tls_connect(Botconn *conn, const char *host, const char *port){
  conn->sockfd = tcp_connect(host, port);
  if(conn->sockfd == -1 ){
    fprintf(stderr,"TCP connection failed for \n");
    return -1;

  }
  printf("[+] TCP connected fd = %d\n",conn->sockfd);


  conn->ctx = SSL_CTX_new(TLS_client_method());
  
  if(!conn->ctx){
    fprintf(stderr,"SSL_CTX_new failed\n");
    ERR_print_errors_fp(stderr);
    close(conn->sockfd);
    return -1;

  }

  SSL_CTX_set_verify(conn->ctx, SSL_VERIFY_PEER, NULL);

  if(!SSL_CTX_set_default_verify_paths(conn->ctx)){
    fprintf(stderr, "failed to load CA certs\n");
    SSL_CTX_free(conn->ctx);
    return -1;
  }

  conn->ssl = SSL_new(conn->ctx);

  if(!conn->ssl){
    fprintf(stderr,"SSL_new failed\n");
    SSL_CTX_free(conn->ctx);
    close(conn->sockfd);
    return -1;
  }
  
  SSL_set_fd(conn->ssl, conn->sockfd);

  SSL_set_tlsext_host_name(conn->ssl, host);

  if(SSL_connect(conn->ssl)!=1){
    fprintf(stderr, "SSL_connect failed\n");
    ERR_print_errors_fp(stderr);
    SSL_free(conn->ssl);
    SSL_CTX_free(conn->ctx);
    close(conn->sockfd);
    return -1;
  }

  if(SSL_get_verify_result(conn->ssl)!= X509_V_OK){
    fprintf(stderr, "Certificate verification failed\n");
    SSL_free(conn->ssl);
    SSL_CTX_free(conn->ctx);
    close(conn->sockfd);
    return -1;
  }

  printf("[+] TSL Handshake done cipher=%s\n",SSL_get_cipher(conn->ssl));
  return 0;

}

int tls_write(Botconn *conn, const char *buf, int len){
  int total = 0;
  
  while(total<len){
    int n = SSL_write(conn->ssl, buf+total, len-total);

    if(n<=0){
      ERR_print_errors_fp(stderr);
      return -1;
    }
    total += n;
  }
  return total;
}

int tls_read(Botconn *conn, char *buf, int len){
  int n = SSL_read(conn->ssl, buf, len);
  if(n<=0){
    int err = SSL_get_error(conn->ssl, n);
    if(err != SSL_ERROR_ZERO_RETURN){
      ERR_print_errors_fp(stderr);
    }
    return -1;
  }
  return n;
} 

void tls_disconnect(Botconn *conn){
  if(conn->ssl){
    SSL_shutdown(conn->ssl);
    SSL_free(conn->ssl);
    conn->ssl = NULL;
  }

  if(conn->ctx){
    SSL_CTX_free(conn->ctx);
    conn->ctx = NULL;
  }

  if (conn->sockfd != -1){
    close(conn->sockfd);
    conn->sockfd = -1;
  }

  printf("[+] Disconnected\n");
}

