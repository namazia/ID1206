#define _GNU_SOURCE


#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <ucontext.h>



static void handler ( int sig , siginfo_t *siginfo , void *cntx) {

    ucontext_t *context = (ucontext_t *)cntx;
    unsigned long pc = context -> uc_mcontext.gregs[REG_RIP];

    printf("Illegal instruction at 0x%lx value 0x%x\n", pc , *(int *) pc );
    context -> uc_mcontext.gregs[REG_RIP] = pc + 1;

  }

int main ( ) {

  struct sigaction sa ;
  int pid = getpid( ) ;
  printf ( "ok ,let's go , kill me (%d) if you can! \n " , pid );

  sa.sa_sigaction = handler ;
  sa.sa_flags = SA_SIGINFO ;
  sigemptyset(&sa.sa_mask ) ;

  sigaction( SIGSEGV , &sa , NULL);

  printf ( " lets go \n " ) ;

  asm(".word 0x00000000");

  here:
      printf(" this call is here %p\n",&&here );


  return ( 0 ) ;
}
