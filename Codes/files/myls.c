#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

int main(int argc, char const *argv[]) {
  if (argc < 2){
    perror("usage :/ myls \n");
    return -1;
  }

  char *path = argv[1];

  DIR *dirp = opendir(path);

  struct dirent *entry;

  struct stat file_st;

  while((entry = readdir(dirp)) != NULL){
    switch ( entry->d_type ) {
      case DT_BLK : // Th is i s a b l o c k d e v i c e .
      printf ( "b : " ) ;
      break ;

      case DT_CHR : // Th is i s a c h a r a c t e r d e v i c e .
      printf ( " c : " ) ;
      break ;

      case DT_DIR : // Th is i s a d i r e c t o r y .
      printf ( "d : " ) ;
      break ;

      case DT_FIFO : // Th is i s a named p i p e .
      printf ( "p : " ) ;
      break ;

      case DT_LNK : // Th is i s a s ym b ol i c l i n k .
      printf ( " l : " ) ;
      break ;

      case DT_REG : // Th is i s a r e g u l a r f i l e .
      printf ( " f : " ) ;
      break ;

      case DT_SOCK : // Th is i s a UNIX domain s o c k e t .
      printf ( " s : " ) ;
      break ;

      case DT_UNKNOWN : // The f i l e t y p e i s unknown .
      printf ( "u : " ) ;
      break ;
    }
    fstatat(dirfd(dirp), entry->d_name, &file_st, 0);
    printf( "\tdev : 0x%lx " , file_st .st_dev ) ;
    printf( "\tinode : %lu " , file_st .st_ino ) ;
    printf ( "\tinode : %lu " , entry->d_ino ) ;
    printf("\tsize : %lu\n", file_st.st_size );
    printf ( "\tname : %s\n" , entry->d_name ) ;
}

  return 0;
}
