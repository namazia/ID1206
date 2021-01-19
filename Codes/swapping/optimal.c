#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#define HIGH 20
#define FREQ 80
#define PAGES 100
#define SAMPLES 20

void init ( int *sequence , int refs , int pages ) {

  int high = (int)(pages * ((float) HIGH/100));
  for(int i = 0 ; i < refs ; i++){

      if( (rand() % 100) < FREQ){
          sequence[i] = rand() % high;
      }

      else{
        sequence[i] = high + rand()%(pages-high);
      }
  }

}

typedef struct pte{
  int present ;
} pte;

void clear_page_table ( pte *table , int pages){
  for(int i = 0 ; i < pages ; i++){
    table[i].present = 0 ;
  }
}

int simulate ( int *sequence , pte *table , int refs, int frames, int pages){

  int hits = 0;
  int allocated = 0;

  int i;

  for(i = 0 ; i < refs ; i++){
    int next = sequence[i];
    pte *entry = &table[next];

    if(entry -> present == 1){
      hits ++;
    }
    else{
      if(allocated < frames){
        allocated++;
        entry->present = 1;
      }
      else{
        pte *evict;
        /* i n i t i a l v a l u e s */
        int sofar = 0 ;
        int candidate = pages ;

        for ( int c = 0 ; c < pages ; c++) {
          if ( table[c].present == 1 ) {
              /* t h e page i s a l l o c a t e d */
              int dist = 0 ;
              while ( sequence [i + dist] != c && i+dist < refs ) {
                dist ++;
              }
              if ( dist > sofar) {
                candidate = c ;
                sofar = dist ;
              }
          }
        }
      evict = &table [candidate] ;
      evict -> present = 0 ;
      entry -> present = 1 ;
        }
      }
    }
  return hits;
}


int main ( int argc , char * argv [ ] ) {

  /* could be command line arguments */
  int refs = 100000;
  //int pages = 100;

printf ( "# This is a benchmark of random replacement \n" ) ;
printf ( "# %d page references \n" , refs ) ;
printf ( "# %d pages \n" , PAGES ) ;
printf ( "#\n#\n#frames\tratio \n" ) ;

int frames;
int incr = PAGES/SAMPLES;

pte table [PAGES]; //If we know we will always use a page table of size 100, will be allocated on the stack
//pte *table = (pte *) malloc(pages * sizeof(pte)); // allocate on the heap in the run time

int *sequence = ( int *) malloc ( refs * sizeof(int)) ;

init ( sequence , refs , PAGES ) ;

for(frames = incr ; frames <= PAGES ; frames += incr){

  /* clear page table entries*/
  clear_page_table(table , PAGES);

  int hits = simulate (sequence , table , refs, frames , PAGES);

  float ratio = (float) hits/refs;

  printf("%d\t%.2f\n" , frames , ratio );
}


/* a small experiment to show that it works
  for ( int i = 0 ; i < refs ; i++) {
  printf ( " , %d" , sequence [ i ] ) ;
}*/
printf ( "\n" ) ;
return 0 ;
}
