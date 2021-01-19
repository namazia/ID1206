#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

volatile int counter = 0;
volatile int turn = 0;
volatile int request[2] = {0,0};

void lock (int id){

  request[id] = 1;
  int other = 1 - id ;
  turn = other ;

  while(request[other] == 1 && turn == other) {};//spin
}

void unlock(int id){
  request[id] = 0;
}

typedef struct args {
  int id ;
  int inc ;
} args;

void *increment(void *arg){
  int id = ((args*)arg)-> id;  // &ref -> or *(&ref). 
  int inc = ((args*)arg)-> inc;

  for(int i = 0 ; i < inc ; i++){
    lock(id);
    counter++;
    unlock(id);
  }
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

  pthread_create(&thread_one, NULL, increment, &one);
  pthread_create(&thread_two, NULL, increment, &two);
  pthread_join(thread_one, NULL);
  pthread_join(thread_two, NULL);

  printf("result is %d\n", counter);
}
