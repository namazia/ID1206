#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

#define FREQ_MAX 32

unsigned long freq[FREQ_MAX];

void add_to_freq(unsigned long size){
  if(size != 0){
    int j = 0;
    int n = 2;
    while(size / n != 0 & j < FREQ_MAX){
      n = n*2;
      j++;
    }
    freq[j]++;
  }
}

void count(char *path){
  unsigned long total = 0;

  DIR *dirp = opendir(path);

  char subdir [1024];

  struct dirent *entry;

  struct stat file_st;

  if (dirp == NULL){
    printf("not able to open path %s\n", path);
    exit -1;
  }

  while((entry = readdir(dirp)) != NULL){

    switch (entry -> d_type){
      case DT_DIR:
        if (strcmp(entry->d_name , ".") == 0 | (strcmp(entry->d_name , "..") == 0))
          break;

      sprintf(subdir, "%s/%s", path, entry ->d_name );
      count(subdir);
      break;

      case DT_REG:
      if(fstatat(dirfd(dirp), entry->d_name, &file_st, 0) == 0){
        add_to_freq(file_st.st_size);
      }
     total++;
      break;

      default:
      break;
    }
  }

  closedir(dirp);
//  return total;
}

int main(int argc, char const *argv[]) {
    if(argc < 2){
      perror("usage: total <dir> \n");
      return -1;
    }
    char *path = argv[1];
    count(path);
    printf("The Directory %s: number of files smaller than 2^k: \n", path);
    printf("#k\tnumber\n");

    for(int i = 0; i < FREQ_MAX; i++){
      printf("%d\t%lu\n", (i+1), freq[i] );
    }
  }
