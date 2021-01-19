 #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/mman.h>
#include <errno.h>


#define TRUE 1
#define FALSE 0
#define HEAD sizeof(struct taken)

#define MIN(size) (((size) > (16)) ? (size):(16))
#define LIMIT(size) (MIN(0) + HEAD + size)
#define MAGIC(memory) ((struct taken*)memory - 1)
#define HIDE(block) (void*)((struct taken*)block + 1)

#define ALIGN 8
#define ARENA (64*1024)

#define PRINT_DALLOC 0

struct head {
  uint16_t bsize;
  uint16_t bfree;
  uint16_t size;
  uint16_t free;
  struct head *prev;
  struct head *next;
};

struct taken {
  uint16_t bsize;
  uint16_t bfree;
  uint16_t size;
  uint16_t free;  
};

struct head *arena = NULL;

struct head *flist;



struct head *after(struct head *block){
  return (struct head*)((char*)block + HEAD + block->size);
}

struct head *before(struct head *block){
  return (struct head*)((char*)block - block->bsize - HEAD);
}


void print_flist(){
  int i = 0;
  struct head *n = flist;
  printf("Block at position %d has size %d and it located at: %p\n", i, n->size,n);
  n = n->next;
  while(n != flist){
    i++;
    sleep(1);
    printf("Block at position %d has size %d and is located at: %p\n", i, n->size,n);
    n = n->next;
  }
}

struct head *new(){
  if(arena != NULL){
    printf("one arena already allocated \n");
    return NULL;
  }
  
  struct head *new = mmap(NULL, ARENA, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1 ,0);

  printf("The location of the arena: %p\n\n", new);

  if(new == MAP_FAILED){
    printf("mmap failed: error %d\n", errno);
    return NULL;
  }

  uint size = ARENA - 2*HEAD;

  new->bfree = FALSE;
  new->bsize = 0;
  new->free = TRUE;
  new->size = size;

  struct head *sentinel = after(new);

  sentinel->bfree = TRUE;
  sentinel->bsize = size;
  sentinel->free = FALSE;
  sentinel->size = 0;

  arena = (struct head*)new;

  return new;
}


void detach(struct head *block){
  if(block->next == NULL || block->prev == NULL){
    printf("block is not tied in the list\n");
    exit(1);
  }
  struct head *next_blk = block->next;
  struct head *prev_blk = block->prev;
  prev_blk->next = next_blk;
  next_blk->prev = prev_blk;
  if(flist == block){
    flist = next_blk;
  }
}

void insert(struct head *block){
  struct head *prev_blk = flist->prev;
  block->next = flist;
  block->prev = flist->prev;
  prev_blk->next = block;
  flist->prev = block;
  flist = block;
}


struct head *split(struct head *block, int size){
  int rsize = block->size - HEAD - size;
  block->size = rsize;

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

int adjust(size_t request){
  return (request < 8) ? (int) MIN(request) : (int) MIN(request) + (MIN(0) - request % MIN(0));
}

struct head *find(int size){
  struct head *first = flist;
  if(first->size >= size){
    if(first->size > LIMIT(size)){
      return split(first, size);
    }
    detach(first);
    first->free = FALSE;
    struct head *aft = after(first);
    aft->bfree = FALSE;
    return first;
  }
  else {
    struct head *n = first->next;
    while(n != flist){
      if(n->size >= size){
	if(n->size > LIMIT(size)){
	  return split(n,size);
	}
	struct head *aft = after(n);
	aft->bfree = FALSE;
	n->free = FALSE;
	detach(n);
	return n;
      }
      n = n->next;
    }
    return NULL;
  }
}

struct head *merge(struct head *block){
  struct head *aft = after(block);
  struct head *bfr = before(block);
  int new_size, x = 0;
  if(bfr != NULL && block->bfree){
    detach(bfr);
    new_size = HEAD + block->size + block->bsize;
    block = bfr;
    block->size = new_size;
    x = 1;
  }
  if(aft != NULL && aft->free){
    detach(aft);
    new_size = HEAD + block->size + aft->size;
    block->size = new_size;
    block->free = TRUE;
    x = 1;
  }
  if(x)
    printf("Block %p has been merged\n", block);
  return block;
}


void *dalloc(size_t request){
  if(PRINT_DALLOC)
    printf("Number of bytes requested: %d\n", request);
  if(arena == NULL){
    flist = new();
    flist->next = flist;
    flist->prev = flist;
  }
  if(PRINT_DALLOC)
    printf("Number of bytes in the main block before allocation: %d\n",flist->size);  
  if(request <= 0){
    return NULL;
  }
  int size = adjust(request);
  struct head *taken = find(size);
  if(taken == NULL){
    return NULL;
  }
  if(PRINT_DALLOC){
    printf("Number of bytes in the main block after allocation: %d\n\n",flist->size);
    printf("Decided allocation size:  %d\n",taken->size);
    printf("The location of the given block: %p\n", taken);
  }
  return HIDE(taken);
}

void dfree(void *memory){
  if(memory != NULL){
    struct head *block =(struct head*) MAGIC(memory);

    struct head *aft = after(block);
    block->free = TRUE;
    aft->bfree = TRUE;
    insert(merge(block));
  }
  return;
  
}
