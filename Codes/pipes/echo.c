#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>


#define PORT 8080
#define MAX 512

int main(void){
  int sock;
  char buffer[MAX];

  struct sockaddr_in client;
  int size = sizeof(struct sockaddr_in);

  assert((sock = socket(AF_INET,SOCK_DGRAM, 0)) != -1);

  struct sockaddr_in name ;
  name.sin_family = AF_INET;
  name.sin_port =htons(PORT);
  name.sin_addr.s_addr = htonl(INADDR_ANY);

  assert(bind(sock, (struct sockaddr *)&name, sizeof(name)) != -1);

  while(1){
    int n ;
    n = recvfrom(sock, buffer, MAX-1 , 0 , (struct sockaddr *)&client, &size);
    if(n != -1)
      perror("server");

    buffer[0];
    printf("server received %s\n", buffer);
    printf( "Server : from destination %s %d\n" , inet_ntoa( client.sin_addr ) , ntohs( client.sin_port));

    assert(sendto(sock, buffer, n, 0 , (struct sockaddr*) &client, size) == n);
  }
//  unlink(SERVER);
}
