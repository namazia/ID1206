#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/socket.h>
#include <sys/un.h>

#define TEST "This is a TeSt to SEE if iT wORks"
#define CLINET "help"
#define MAX 512
#define SERVER "lower"

int main(void){
  int sock;
  char buffer[MAX];

  assert((sock = socket(AF_UNIX,SOCK_DGRAM, 0)) != -1); //return a file descriptor for the new socket

  struct sockaddr_un name = {AF_UNIX, CLINET};
  assert(bind(sock, (struct sockaddr *)&name, sizeof(name)) != -1);

  struct sockaddr_un server = {AF_UNIX, SERVER};
  int size = sizeof(struct sockaddr_un);

  int n = sizeof(TEST);

  assert(sendto(sock, TEST, n, 0, (struct sockaddr *) &server, size ) != -1 );
  n = recvfrom(sock, buffer, MAX-1 , 0 , (struct sockaddr *)&server, &size);

  if(n == -1)
    perror("server");

    buffer[0];
    printf("client received %s\n", buffer);
    unlink(CLINET);
    exit(0);
  }
