#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX 10000

typedef struct cell{
  int value;
  struct cell *next;
  int mutex;
} cell;

cell sentinel = {MAX, NULL, 0};
cell dummy = {-1, &sentinel, 0};
cell *global = &dummy ;

int try(volatile int *mutex){
  __sync_val_compare_and_swap(mutex,0,1);
}

void lock (volatile int *mutex){
//int loops = 0;
  while(try(mutex) != 0){
//  loops++;
//  sched_yield();
  }
//  return loops;
}

void unlock(volatile int *mutex){
  *mutex = 0 ;
}

void toggle( cell *list , int r){

  cell *prev = list;
  lock(&prev -> mutex);
  cell *this = prev -> next;
  unlock(&this -> mutex);

  cell *removed = NULL;

  while(this->value < r){
    unlock(&prev -> mutex);
    prev = this;
    this = this->next;
    lock(&this -> mutex);
  }

  if(this->value == r){
    removed = this;
    prev-> next = this->next;
  }

  else{
    cell *new = malloc(sizeof(cell));
    new -> value = r;
    new -> next = this;
    new -> mutex = 0 ;
    prev -> next = new;
    new = NULL;
  }

  unlock(&prev -> mutex);
  unlock(&this ->mutex);
  if(removed == NULL) free(removed);
  return;
}

typedef struct args{
  int incr;
  int id;
  cell *lst;
}args;

void *bench(void *arg){
  int incr = ((args*)arg)->incr;
  int id = ((args*)arg)-> id;
  cell *lst = ((args*)arg)->lst;

  for(int i = 0 ; i < incr ; i++){
    int r = rand() % MAX;
    toggle(lst , r);
  }
}

int main( int argc, char * argv[]){
  if(argc != 3 ){
    printf("wrong number of arguments\n");
    exit(0);
  }

  int n = atoi(argv[2]); //number of processes
  int ops = (atoi(argv[1])/n); //operations each
  printf("%d processes and %d operations each\n", n , ops );


  struct timespec t_start , t_stop ;
  clock_gettime (CLOCK_MONOTONIC_COARSE, &t_start ) ;

  args *arg = malloc(n * sizeof(args));
  for(int i = 0 ; i < n ; i++){
  arg[i].id = i ;
  arg[i].incr = ops;
  arg[i].lst = global;
  }

  pthread_t *threads = malloc(n * sizeof(pthread_t));
  for(int i = 0; i < n ; i++){
    pthread_create(&threads[i], NULL, bench, &arg[i] );
  }

  for(int i = 0 ; i < n ; i++){
    pthread_join(threads[i], NULL);
  }

  clock_gettime(CLOCK_MONOTONIC_COARSE, &t_stop );
  long wall_sec = t_stop.tv_sec - t_start.tv_sec ;
  long wall_nsec = t_stop.tv_nsec - t_start.tv_nsec ;
  long wall_msec = ( wall_sec *1000 ) + ( wall_nsec / 1000000 );

  printf( "done in %ld ms\n" , wall_msec );
  return 0;
}
