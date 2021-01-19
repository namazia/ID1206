#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#define BLOCKS 1024*1024
#define SIZE 512
#define STEP 64
#define COUNT 1000
#define LOOP 64
#define RES 10

int compare(const void *a, const void *b){
  return (int)(*(long *)a - *(long *)b);
}

long n_sec(struct timespec *start, struct timespec *stop){
  long diff_sec = stop -> tv_sec - start -> tv_sec;
  long diff_nsec = stop -> tv_nsec - start -> tv_nsec;
  long wall_nsec = (diff_sec * 1000000000) + diff_nsec;
  return wall_nsec;
}

int main(int argc, char const *argv[]) {

  if(argc < 2 ){
    printf("wrong argumetns\n");
    return -1;
  }

  char *name = argv[1];

  //int flag = O_RDWR | O_CREAT; //O_DSYNC will force the OS to actually push the data to the drive
  //int mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

  //int fd = open(name, flag, mode);

  //assert(fd != -1);

  int *heap = malloc(sizeof(int) * BLOCKS * SIZE);

  //test
  for(int b =0; b < BLOCKS; b++) {
    heap[b*SIZE] = b;
  }

  int entries = SIZE / STEP;
  long **table = malloc(sizeof(long) * entries);

  for(int i = 0; i < entries; i++){
    table[i] = malloc(sizeof(long) * COUNT);
  }

  for(int c = 0; c < COUNT; c++){
    int b = rand() % BLOCKS;
  //  lseek(fd, SIZE*b, SEEK_SET);

    for(int e = 0; e < entries; e++){
      struct timespec t_start , t_stop;
      int buffer;
      clock_gettime(CLOCK_MONOTONIC, &t_start);
      for(int l = 0; l < LOOP ; l++){
        buffer = buffer + heap[b*SIZE + e + 1];
      }
      clock_gettime(CLOCK_MONOTONIC, &t_stop);
  //    lseek(fd, STEP - sizeof(int), SEEK_CUR);
      table[e][c] = ((n_sec(&t_start, &t_stop)-RES) / LOOP);
    }
  }
//  close(fd);
  printf("#N\tMin\tQ1\tMed\tQ3\tD9\tMax\n");
  for(int e = 0; e < entries; e++){
    qsort(table[e], COUNT, sizeof(long), compare);
    long min = table[e][0];
    long q1 = table[e][COUNT/4];
    long med = table[e][COUNT/2];
    long q3 = table[e][3*(COUNT/4)];
    long d9 = table[e][9*(COUNT/10)];
    long max = table[e][COUNT-1];
    printf("%d\t%ld\t%ld\t%ld\t%ld\t%ld\t%ld\n", e*STEP, min, q1, med, q3, d9, max);
  }
  printf("done\n");
  return 0;
}
