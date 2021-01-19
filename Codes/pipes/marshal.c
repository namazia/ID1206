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
      double buffer;
      read(descr[0], &buffer, sizeof(double));
      printf("received %f\n", buffer );
      sleep(1);
    }
    printf("consumer done\n");
    return 0 ;
  }
  /*producer*/
  printf("I'm the mother with pid %d\n", getpid());
  for (int i = 0 ; i < ITERATIONS ; i++){
  double pi = 3.14*i;
  write(descr[1], &pi, sizeof(double));
  }

  printf("producer done\n");
  wait(NULL);
  printf("all done\n" );
}
