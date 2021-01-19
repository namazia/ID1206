#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <linux/futex.h>
#include <sys/syscall.h>

volatile int global = 0;
volatile int counter = 0 ;

int futex_wait(volatile int *futex){
 return syscall(SYS_futex, futex, FUTEX_WAIT, 1, NULL, NULL, 0);
}

void futex_wake(volatile int *futex){
   syscall(SYS_futex, futex, FUTEX_WAKE, 1, NULL, NULL, 0);
}

int try(volatile int *mutex){
  __sync_val_compare_and_swap(mutex,0,1);
}

int lock (volatile int *mutex){
int loops = 0;
  while(try(mutex) != 0){
  loops++;
  futex_wait(mutex);
  }
  return loops;
}

void unlock(volatile int *mutex){
  *mutex = 0 ;
  futex_wake(mutex);
}

typedef struct args {
  int id ;
  int inc ;
  volatile int *mutex;
} args;

void *increment(void *arg){
  int id = ((args*)arg)-> id;
  int inc = ((args*)arg)-> inc;
  int spins = 0;
  volatile int *mutex = ((args*)arg)->mutex;

  printf("thread %d start\n",id );
  for(int i = 0 ; i < inc ; i++){
   spins += lock(mutex);
    counter++;
    unlock(mutex);
  }
  printf("%d spins for thread %d\n",spins, id );
}

int main(int argc , char *argv[]){

  if(argc != 2 ){
    printf("peterson <inc>\n");
    exit(0);
  }
int inc = atoi(argv[1]);

  pthread_t thread_one,thread_two;
  args  one,two;

  one.inc = inc;
  two.inc = inc;

  one.id = 0;
  two.id = 1;

  one.mutex = &global;
  two.mutex = &global;

  pthread_create(&thread_one, NULL, increment, &one);
  pthread_create(&thread_two, NULL, increment, &two);
  pthread_join(thread_one, NULL);
  pthread_join(thread_two, NULL);

  printf("result is %d\n", counter);
  return 0;
}
