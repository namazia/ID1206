#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int volatile done ;

void handler ( int sig , siginfo_t *siginfo , void *context) {
    printf ( " signal %d was caught\n" , sig ) ;
    printf("your UID is %d\n",siginfo -> si_uid );
    printf("your PID is %d\n", siginfo -> si_pid);

    done = 1;
  }
int main ( ) {

  struct sigaction sa ;
  int pid = getpid( ) ;
  printf ( "ok ,let's go , kill me (%d) if you can! \n " , pid );

  sa.sa_sigaction = handler ;
  sa.sa_flags = SA_SIGINFO ;
  sigemptyset(&sa.sa_mask ) ;

  if ( sigaction( SIGINT , &sa , NULL) != 0 ) {
      return (1) ;
    }

  while ( !done ) {
    }


  printf ( " told you ! \n " ) ;
  return ( 0 ) ;
}
