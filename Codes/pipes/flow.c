#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <assert.h>

#define ITERATIONS 10
#define BURST 1000

int main(){
  int descr[2];

  assert(0 == pipe(descr));

  if(fork() == 0){
    /*consumer*/
    printf("I'm the child with pid %d\n", getpid());
    for (int i = 0 ; i < ITERATIONS ; i++){
      for(int j = 0 ; j < BURST ; j++){
        int buffer[10];
        read(descr[0], &buffer, 10);
      }
      sleep(1);
    }
    printf("consumer done\n");
    return 0 ;
  }
  /*producer*/
  printf("I'm the mother with pid %d\n", getpid());
  for (int i = 0 ; i < ITERATIONS ; i++){
    for(int j = 0 ; j < BURST ; j++){
      write(descr[1], "0123456789", 10);
    }
    printf("operation burst %d is done \n" , i);
  }
  printf("producer done\n");
  wait(NULL);
  printf("all done\n" );
}
