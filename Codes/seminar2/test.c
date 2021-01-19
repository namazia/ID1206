#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/mman.h>
#include <errno.h>
#include "dlmall.h"

#define LOOPS 10
#define BLOCKS 100
#define MAX_SIZE 500

int main(){

//  printf("this is the start\n");
  void *memory;
  for(int i = 0; i < LOOPS; i++){
    for(int j = 0; j < BLOCKS; j++){
  //    printf("should be still fine\n");
  //    size_t size = rand()%MAX_SIZE;
//      printf("random size is %ld \n",size );
      memory = dalloc(rand()%MAX_SIZE);
//      printf("before this may be where the problem happens\n");
      if(j%3 == 0){
	       void *memory2 = dalloc(rand()%MAX_SIZE);
	       void *memory3 = dalloc(rand()%MAX_SIZE);
       dfree(memory);
       }
    //   printf("iteration of j %d\n", j );
    }
      printf("End of iteration i %d \n", i);
     printFlist();
  }

 printSpace();
return 0;
}
