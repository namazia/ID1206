#include "green.h"
#include <stdio.h>
#include <time.h>
#include <pthread.h>

int flag = 0;
int numThreads  = 2;
green_cond_t cond;
green_mutex_t mutex;
pthread_cond_t emptyP, fullP;
pthread_mutex_t mutexP;
int counter = 0;

// void *test(void *arg){
//   int id = *(int*)arg;
//   int loop = 10;
//   while(loop > 0){
//     if(flag == id){
//     printf("thread %d: %d\n", id, loop );
//     loop--;
//     flag = (id + 1) % 2;
//         green_yield();
//    green_cond_signal(&cond);
//  }
//  else{
//    green_cond_wait(&cond);
//    }
//   }
// }

void *test(void *arg){
  int id = *(int*)arg;
  int loop = 10000;
  while(loop > 0){
    printf("thread %d: %d\n", id, loop );
    loop--;
}
}

void *testFail(void *arg){
  int id =*(int*)arg;
  for(int i=0; i<100; i++){

    counter++;
    printf("thread %d: %d %d\n", id, i, counter );
  }
}

void *testShared(void *arg){
  int id = *(int*)arg;
  for(int i = 0; i < 100; i++) {
   green_mutex_lock(&mutex);
   int temp = counter;

   int dummy = 0;
   if(temp > 2000) // waste time between read and write
     dummy++;

   temp++;
   counter = temp;
   printf("thread %d: %d %d\n", id, i, counter );
   green_mutex_unlock(&mutex);
 }
}

int buffer;
int productions;
green_cond_t full, empty;
void produce() {
  for(int i = 0; i < productions/(numThreads/2); i++) {
    green_mutex_lock(&mutex);
    while(buffer == 1) // wait for consumer before producing more
      green_cond_wait(&empty, &mutex);
    buffer = 1;
    //printf("Produced!\n");
    green_cond_signal(&full);
    green_mutex_unlock(&mutex);
  }
}

void consume() {
  for(int i = 0; i < productions/(numThreads/2); i++) {
    green_mutex_lock(&mutex);
    while(buffer == 0) // wait for producer before consuming
      green_cond_wait(&full, &mutex);
    buffer = 0;
    //printf("Consumed!\n");
    green_cond_signal(&empty);
    green_mutex_unlock(&mutex);
  }
}

void produceP() { // heahae
  for(int i = 0; i < productions/(numThreads/2); i++) {
    pthread_mutex_lock(&mutexP);
    while(buffer == 1) // wait for consumer before producing more
      pthread_cond_wait(&emptyP, &mutexP);
    buffer = 1;
    //printf("Produced!\n");
    pthread_cond_signal(&fullP);
    pthread_mutex_unlock(&mutexP);
  }
}

void consumeP() { // heahae
  for(int i = 0; i < productions/(numThreads/2); i++) {
    pthread_mutex_lock(&mutexP);
    while(buffer == 0) // wait for producer before consuming
      pthread_cond_wait(&fullP, &mutexP);
    buffer = 0;
    //printf("Consumed!\n");
    pthread_cond_signal(&emptyP);
    pthread_mutex_unlock(&mutexP);
  }
}

void* testConsumerProducer(void* arg) {
  int id = *(int*)arg;
  if(id % 2 == 0) { // producer
    //printf("Producing...\n");
    produce();
  } else { // consumer
    //printf("Consuming...\n");
    consume();
  }
}

void* testConsumerProducerP(void* arg) {
  int id = *(int*)arg;
  if(id % 2 == 0) { // producer
    //printf("Producing...\n");
    produceP();
  } else { // consumer
    //printf("Consuming...\n");
    consumeP();
  }
}


void testGreen(int* args) {
  green_t threads[numThreads];

  for(int i = 0; i < numThreads; i++)
    green_create(&threads[i], testConsumerProducer, &args[i]);
  /*
  green_create(&threads[0], test, &args[0]);
  for(int i = 1; i < numThreads; i++)
    green_create(&threads[i], testJoin, &threads[0]);
  */
  for(int i = 0; i < numThreads; i++)
    green_join(&threads[i]);
}

void testPthread(int* args) {
  pthread_t threads[numThreads];

  for(int i = 0; i < numThreads; i++)
    pthread_create(&threads[i], NULL, testConsumerProducerP, &args[i]);

  for(int i = 0; i < numThreads; i++)
    pthread_join(threads[i], NULL);
}

int main(){

  clock_t c_start, c_stop;
  double processTimeGreen = 0, processTimeP = 0;
//  green_cond_init(&cond);
   green_cond_init(&full);
   green_cond_init(&empty);
   green_mutex_init(&mutex);

  pthread_cond_init(&fullP, NULL);
  pthread_cond_init(&emptyP, NULL);
  pthread_mutex_init(&mutexP, NULL);
 //  green_t g0, g1;
 //
 // int a0 = 0;
 // int a1 = 1;
 // green_create(&g0, test, &a0);
 // green_create(&g1, test, &a1);
 //
 // green_join(&g0);
 // green_join(&g1);
 // printf("%d\n",counter );
printf("#Benchmark, creating and producing/consuming with threads!\n#\n#\n");
printf("#{#productions\ttimeGreen(ms)\ttimePthread(ms)}\n");

int numRuns = 10;
 for(int run = 1; run <= numRuns; run++) {
   //printf("Starting run %d\n", run);
   buffer = 0;
   productions = 1000 * 2 * run; // Must be multiple of 2

   int args[numThreads];
   for(int i = 0; i < numThreads; i++)
     args[i] = i;

   c_start = clock();
   testGreen(args);
   c_stop = clock();
   processTimeGreen = ((double)(c_stop - c_start)) / ((double)CLOCKS_PER_SEC/1000);

   c_start = clock();
testPthread(args);
c_stop = clock();
processTimeP = ((double)(c_stop - c_start)) / ((double)CLOCKS_PER_SEC/1000);

   printf("%d\t%f\t%f\n", productions, processTimeGreen, processTimeP);
//  printf("done\n");

 }
 return 0;
}
