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
  int id;
  int present;
  struct pte *next;
  struct pte *prev;
} pte;

void clear_page_table ( pte *table , int pages){
  for(int i = 0 ; i < pages ; i++){
    table[i].present = 0 ;
  }
}

int simulate ( int *sequence , pte *table , int refs, int frames, int pages){

  int hits = 0;
  int allocated = 0;

  pte *first = NULL;
  pte *last = NULL;

  for(int i = 0 ; i < refs ; i++){
    int next = sequence[i];
    pte *entry = &table[next];

    if(entry -> present == 1){
      hits ++;

      if(entry-> prev == NULL){
        first = entry->next;
        first->prev = NULL;
        entry->prev = last;
        last->next = entry;
        last = entry;
        last->next = NULL;
      }
      else if (entry-> next == NULL){}
      else{
        entry->prev->next = entry->next;
        entry->next->prev = entry->prev;
        entry->prev = last;
        last->next = entry;
        last = entry;
        last->next = NULL;
      }

    }
    else{
      if(allocated < frames){
        allocated++;

        entry->present = 1;
        entry->prev = last;
        entry->next = NULL;

        if(first == NULL){
          first = entry;
          last = entry ;
        }

        else {
          last->next = entry;
          last = entry;
        }

      }
      else{
        pte *evict;

        evict = first;
        first = evict -> next;
        first -> prev = NULL;

        evict-> present = 0;

        entry-> present = 1 ;
        entry->prev = last;
        entry->next = NULL;

        last->next = entry;
        last = entry;


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
