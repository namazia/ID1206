#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

//char global [] = "This is global string";
//const int read_only = 123456;
void zot(unsigned long *stop){

  unsigned long r = 0x3;

  unsigned long *i;
  for (i = &r ; i <= stop ; i++){
    printf("%p  0x%lx\n",i , *i );
  }
}

void foo(unsigned long *stop){  //just creatinf a stack frame
  unsigned long q = 0x2;
  zot(stop);
}


int main(){

   unsigned long p = 0x1;
   int pid = getpid();

   foo(&p);

   back :
   printf("p: %p\n", &p);
   printf("back: %p\n",&&back );

   
  // foo :

//   printf(" This is process ID %d\n", pid );
//   printf("p : %p\n",&p );
//   printf(" This is global string %p\n", &global );
//   printf(" The code (foo) %p\n", &&foo );
//   printf(" The read only %p\n", &read_only);

   printf ( "\n\n /proc/%d/maps \n\n" , pid ) ;
   char command [50] ;
   sprintf (command , " cat /proc/%d/maps" , pid ) ; //puts the string to the array
   system (command ) ;


   return 0;

}
