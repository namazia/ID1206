#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

int volatile count ;

void handler ( int sig ) {
    printf ( " signal %d took care of it\n" , sig ) ;
    exit(1);
    return ;
    }

int not_so_good(){
  int x = 0;
  return 1 % x;
}



int main ( ) {

  struct sigaction sa ;
  printf("ok I will catch my own error\n");

  sa.sa_handler = handler ;
  sa.sa_flags = 0 ;
  sigemptyset(&sa.sa_mask ) ;

  sigaction( SIGFPE , &sa , NULL);


  not_so_good();


  printf ( " probably not gonna show this \n " ) ;
  return ( 0 ) ;
}
