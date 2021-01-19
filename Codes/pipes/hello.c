#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>


#define TEST "Hello, hello"
#define MAX 512
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

int main(void){
  int sock;
  char buffer[MAX];

  assert((sock = socket(AF_INET,SOCK_DGRAM, 0)) != -1); //return a file descriptor for the new socket

  struct sockaddr_in name ;
  name.sin_family = AF_INET;
  name.sin_port = 0;
  name.sin_addr.s_addr = htonl(INADDR_ANY);
  assert(bind(sock, (struct sockaddr *)&name, sizeof(name)) != -1);

  struct sockaddr_in server ;
  server.sin_family=AF_INET;
  server.sin_port= htons(SERVER_PORT);
  server.sin_addr.s_addr = inet_addr(SERVER_IP);

  int size = sizeof(struct sockaddr_in);
  int n = sizeof(TEST);

  assert(sendto(sock, TEST, n, 0, (struct sockaddr *) &server, size ) != -1 );
  n = recvfrom(sock, buffer, MAX-1 , 0 , (struct sockaddr *)&server, &size);

  if(n == -1)
    perror("server");

    buffer[0];
    printf("client received %s\n", buffer);
    exit(0);
  }
