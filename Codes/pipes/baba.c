#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define ITERATIONS 10
#define BURST 1000

/*consumer*/
int main(){

  int flag = O_RDONLY;
  int pipe = open("sesame",flag);

  for(int i = 0 ; i < ITERATIONS; i++){
    for(int j = 0; j < BURST ; j++){
      int buffer[10];
      read(pipe, &buffer, 10);
    }
    sleep(1);
  }
  printf("consumer done\n" );
  return 0;
}
