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

//producer

int main(){
  /*create the named pipe*/
  int mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
  mkfifo("sesame",mode);

  int flag = O_WRONLY;
  int pipe = open("sesame", flag);
  for (int i = 0 ; i < ITERATIONS ; i++){
    for(int j = 0 ; j < BURST ; j++){
      write(pipe, "0123456789", 10);
    }
    printf("operation burst %d is done \n" , i);
  }
  printf("producer done\n");
  return 0;
}
