#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){

  int pid = fork();

  if( pid == 0){
    printf("check the status \n") ;
    sleep (10) ;
    printf("and again\n");


  //  printf(" I'm the child %d\n", getpid() );
  //  sleep(1);
  return 42;
  }

  else{

  //  printf("My child is %d\n", pid );
    sleep(20);
    int res ;
    wait(&res);
  //  printf("My child has terminated \n");
  printf(" Value was %d\n", WEXITSTATUS( res) );
  printf("and again\n");
  sleep(10);
  }

//  printf("This is the end %d\n",getpid() );

  return 0;
}
