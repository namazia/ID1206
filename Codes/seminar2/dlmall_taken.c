#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/mman.h>
#include <errno.h>


#define TRUE 1
#define FALSE 0
#define HEAD ( sizeof ( struct taken ) )
#define MIN(size) (((size) > (16))? (size) : (16))
#define LIMIT(size) (MIN( 0 ) + HEAD + size )
#define MAGIC(memory) ( ( struct taken*)memory - 1 )
#define HIDE(block) ( void *) ( ( struct taken* ) block + 1 )
#define ALIGN 8
#define ARENA (64*1024)

int space = 24 ;
struct head *arena = NULL;
struct head *flist ;

struct head {
  uint16_t bfree ; // 2 bytes , the status of block before
  uint16_t bsize ; // 2 bytes , the size of block before
  uint16_t free ; // 2 bytes , the status of the block
  uint16_t size ; // 2 bytes , the size (max 2^16 i.e. 64 Ki byte)
  struct head * next ; // 8 bytes pointer
  struct head * prev ; // 8 bytes pointer
} ;

struct taken {
  uint16_t bsize;
  uint16_t bfree;
  uint16_t size;
  uint16_t free;
};

void printSpace(){
  printf("space for HEADERS %d\n", space );
}

struct head *after ( struct head *block ) {
  return ( struct head* ) ( (char *)block + HEAD + block-> size ) ;
}

struct head *before ( struct head *block ) {
  return ( struct head* ) ( (char *)block - HEAD - block-> bsize ) ;
}

struct head *split ( struct head *block , int size ) {

  int rsize = block->size - (size + HEAD);
  block->size = rsize;
 space += HEAD;

  struct head *splt = after(block);
  splt->bsize = rsize;
  splt->bfree = block->free;
  splt->size = size;
  splt->free = FALSE;

  struct head *aft = after(splt);
  aft->bsize = size;
  aft->bfree = FALSE;

  return splt;
}

struct head *new ( ) {

  if ( arena != NULL) {
      printf ( "one arena already allocated \n" ) ;
      return NULL;
    }

    struct head *new = mmap(NULL, ARENA,
      PROT_READ | PROT_WRITE,
      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0 ) ;

       printf("The location of the arena: %p\n\n", new);

      if ( new == MAP_FAILED) {
        printf ( "mmap failed : error %d\n" , errno ) ;
        return NULL;
      }
/* make room for head and dummy */
    uint size = ARENA - 2*HEAD;

    new->bfree = FALSE;
    new->bsize =  0;
    new->free= TRUE;
    new->size= size;

    struct head *sentinel = after(new) ;
/* only touch the status fields */
    sentinel ->bfree = TRUE;
    sentinel ->bsize = size;
    sentinel ->free = FALSE;
    sentinel ->size = 0 ;

/* this is the only arena we have */
    arena = ( struct head *) new ;

  return new ;
}



void detach ( struct head * block ) {
  if(block->next == NULL || block->prev == NULL){
    printf("asked block is not in the list\n");
    exit(1);
  }

  block->prev->next = block->next;
  block->next->prev = block->prev;
  if(flist == block){
    flist = block->next ;
  }

}


void insert ( struct head *block ) {

    block->next = flist;

    block->prev = flist->prev;

    flist->prev->next = block;
  //  printf("safety\n");
    flist->prev = block;

    flist = block;
}

int adjust (size_t request){
  return (request < 8) ? (int) MIN(request) : (int) MIN(request) + (MIN(0) - request % MIN(0));
}

struct head *find(int size){
  struct head *first = flist;
    if(first -> size >= size){

        if(first -> size > LIMIT(size)){
      //    printf("so far so good\n" );
          return split(first,size);
        }

      detach(first);
      first -> free = FALSE;
      struct head *afterr = after(first);
      afterr->bfree = FALSE;
      return first;
  }

  else{
  //  printf("safety for find\n");
    struct head *nextt = first -> next ;
    while (nextt != flist) {
        if(nextt -> size >= size){
            if(nextt-> size > LIMIT(size)){
              return split(nextt,size);
            }
          struct head *afterr = after(nextt);
          nextt -> free = FALSE;
          afterr -> bfree = FALSE;
          detach(nextt);
          return nextt;
        }
    nextt = nextt-> next;
    }
  return NULL;
  }
}

struct head *merge( struct head *block){
    struct head *aft = after(block);
    struct head *bfr = before(block);
    int size = 0;
    if(block -> bfree){
      space -= HEAD;
      detach(bfr);
      size = (block -> bsize) + HEAD + block -> size ;
      block = bfr;
      block -> size = size;
    }

    if(aft -> free){
      space += HEAD;
      detach(aft);
      size = block -> size + HEAD + aft -> size ;
      block -> size = size ;
      block -> free = TRUE;

    }

    return block;
}

void * dalloc ( size_t request ) {
  if (arena == NULL){
    flist= new();
    flist -> next = flist;
    flist -> prev = flist ;
    printf("made an arena with size %d for user\n" , flist -> size);
  }
  if ( request <= 0 ){
//      printf("invalid size \n");
  return NULL;
  }

  int size = adjust ( request ) ;
//  printf("adjusting went fine and size is %d\n" ,size );

  struct head *taken = find ( size ) ;
//  printf("in the first run should taken should be null\n");
  if ( taken == NULL){
//  printf("this should happen\n" );
  return NULL;
 }
  else
  return HIDE(taken);

}

void dfree ( void *memory ) {
  if (memory != NULL) {
      struct head *block = (struct head*) MAGIC(memory);
      struct head *aft = after(block);
      block->free = TRUE;
      aft->bfree = TRUE;
    //  printf("all good in dfree before insert\n" );
      insert(merge(block));
    //  insert(block);
    }
return ;
}

void printFlist(){
  int i = 0;
  struct head *first = flist;
  printf("First block ===> position %d size %d location %p\n", i, first->size, first);
  first = first-> next;
  while(first != flist){
//    printf("does this happen?\n" );
  i++;
//  sleep(1);
  printf("   Block:position %d size %d location %p\n", i, first->size,first);
  first = first->next;
  }


}
