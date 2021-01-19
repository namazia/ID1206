#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX 10000

typedef struct cell{
  int value;
  struct cell *next;
} cell;

cell sentinel = {MAX, NULL};
cell dummy = {-1, &sentinel};
cell *global = &dummy ;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void toggle( cell *list , int r){

  cell *prev = NULL;
  cell *this = list;
  cell *removed = NULL;

  pthread_mutex_lock(&mutex);

  while(this->value < r){
    prev = this;
    this = this->next;
  }

  if(this->value == r){
    removed = this;
    prev-> next = this->next;
  }

  else{
    cell *new = malloc(sizeof(cell));
    new -> value = r;
    new -> next = this;
    prev -> next = new;
  }

  pthread_mutex_unlock(&mutex);
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

  pthread_mutex_init(&mutex, NULL);

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
  long wall_sec = t_stop.tv_sec - t_start . tv_sec ;
  long wall_nsec = t_stop.tv_nsec - t_start.tv_nsec ;
  long wall_msec = ( wall_sec *1000 ) + ( wall_nsec / 1000000 );

  printf( "done in %ld ms\n" , wall_msec );
  return 0;
}
