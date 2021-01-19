#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>



int main(){

  int pid = fork();
  int x = 123;

  if( pid == 0){

  printf("child :  x is %d\n", x);
  x = 42;
  sleep(1);
  printf("child :  x is %d\n", x);
  //  printf(" I'm the child %d\n", getpid() );
  //  sleep(1);

  }

  else{

  printf("mother :  x is %d\n", x);
  x = 13;
  sleep(1);
  printf("mother :  x is %d\n", x);
  wait(NULL);
  //  printf("My child is %d\n", pid );
  //  int res ;
  //  wait(&res);
  //  printf("My child has terminated \n");
  //  printf(" Value was %d\n", WEXITSTATUS( res) );

  }

//  printf("This is the end %d\n",getpid() );

  return 0;
}
