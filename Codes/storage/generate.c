#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

#define BLOCKS 1024*1024
#define SIZE 512

 int main(int argc, char const *argv[]) {

  if(argc < 2 ){
    printf("wrong argumetns\n");
    return -1;
  }

  char *name = argv[1];

  int flag = O_RDWR | O_CREAT; //O_DSYNC will force the OS to actually push the data to the drive
  int mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

  int fd = open(name, flag, mode);

  assert(fd != -1);

  int buffer;

  for(int b = 0; b < BLOCKS; b++){
    lseek(fd, SIZE*b, SEEK_SET);
    buffer = b;
    write(fd, &buffer, sizeof(int));
  }
  close(fd);
  printf("done\n");
  return 0;
}
