#include <stdlib.h>
#include <stdio.h>
#include <ucontext.h>
#include <assert.h>
#include <signal.h>
#include <sys/time.h>
#include "green.h"

#define FALSE 0
#define TRUE 1
#define PERIOD 100
#define STACK_SIZE 4094

static sigset_t block;
static ucontext_t main_cntx = {0};
static green_t main_green = {&main_cntx, NULL, NULL, NULL, NULL, FALSE};

static green_t *running = &main_green;

void time_handler(int);

static void init() __attribute__((constructor));

void init(){
 sigemptyset(&block);
 sigaddset(&block, SIGVTALRM);

 struct sigaction act = {0};
 struct timeval interval;
 struct itimerval period;

 act.sa_handler = time_handler;
 assert(sigaction(SIGVTALRM, &act, NULL) == 0);

 interval.tv_sec = 0;
 interval.tv_usec = PERIOD;
 period.it_interval = interval;
 period.it_value = interval;
 setitimer(ITIMER_VIRTUAL, &period, NULL);

 getcontext(&main_cntx);

}

void time_handler(int sig){
  green_yield();
}

void add_queue(green_t** queue, green_t *thread){
  green_t* current = *queue;
  if(current == NULL){
    *queue = thread;
  }

  else{
    while(current->next != NULL){
      current = current->next;
    }
    current->next = thread;
  }
}

void add_ready_queue(green_t *thread){
  add_queue(&running, thread);
}

green_t* get_from_queue(green_t** queue){
  green_t* thread = *queue;
  if(thread != NULL){
    *queue = thread ->next;
    thread ->next = NULL;
  }

  return thread;
}

void set_next(){
  get_from_queue(&running);

  if(running == NULL)
    printf("No thread ready to run!\n");
}

void green_thread(){
    sigprocmask(SIG_UNBLOCK, &block, NULL);
    green_t *this = running;

    void *result = (*this->fun)(this->arg);

    sigprocmask(SIG_BLOCK, &block, NULL);

    if(this->join != NULL){
      add_ready_queue(this->join);
    }

    free(this->context->uc_stack.ss_sp);
    free(this->context);

    this->zombie = TRUE;

    set_next();

    setcontext(running->context);

}

int green_create(green_t *new, void *(*fun)(void*), void *arg){

  ucontext_t *cntx = (ucontext_t *) malloc(sizeof(ucontext_t));
  getcontext(cntx);

  void *stack = malloc(STACK_SIZE);

  cntx->uc_stack.ss_sp = stack;
  cntx->uc_stack.ss_size = STACK_SIZE;
  makecontext(cntx, green_thread, 0);

  new->context = cntx;
  new->fun = fun;
  new->arg = arg;
  new->next = NULL;
  new->join = NULL;
  new->retval = NULL;
  new->zombie = FALSE;

  sigprocmask(SIG_BLOCK, &block, NULL);
  add_ready_queue(new);
  sigprocmask(SIG_UNBLOCK, &block, NULL);

  return 0;
}

int green_yield(){
  sigprocmask(SIG_BLOCK, &block, NULL);
  green_t *susp = running;

  add_ready_queue(susp);

  set_next();

  swapcontext(susp->context, running->context);
  sigprocmask(SIG_UNBLOCK, &block, NULL);

  return 0;
}

int green_join(green_t *thread){
  if(thread->zombie)
    return 0;

  green_t* susp = running;
  sigprocmask(SIG_BLOCK, &block, NULL);

  if(thread->join == NULL){
    thread->join = susp;
  }
  else{
    green_t* current = thread->join;
    while(current->next != NULL)
      current = current->next;

    current->next = susp;
  }

  set_next();

  swapcontext(susp->context, running->context);
  sigprocmask(SIG_UNBLOCK, &block, NULL);
  return 0;
}

void green_cond_init(green_cond_t* cond){
  cond->queue = NULL;
}

void green_cond_wait(green_cond_t* cond, green_mutex_t* mutex){
  sigprocmask(SIG_BLOCK, &block, NULL);
  add_queue(&(cond->queue), running);

  if(mutex != NULL) {
    add_ready_queue(mutex->susp);
    mutex->susp = NULL;
    mutex->taken = FALSE;
  }

  green_t* susp = running;
  set_next();
  swapcontext(susp->context, running->context);

  if(mutex != NULL) {
   while(mutex->taken) {
     add_queue(&mutex->susp, susp);
     set_next();
     swapcontext(susp->context, running->context);
   }
   mutex->taken = TRUE;
 }
 sigprocmask(SIG_UNBLOCK, &block, NULL);
}

void green_cond_signal(green_cond_t *cond){
   sigprocmask(SIG_BLOCK, &block, NULL);
   green_t * signal = get_from_queue(&cond->queue);
   add_ready_queue(signal);
   sigprocmask(SIG_UNBLOCK, &block, NULL);
}

int green_mutex_init(green_mutex_t *mutex){
  mutex->taken = FALSE;
  mutex->susp = NULL;
  return 0;
}

int green_mutex_lock(green_mutex_t *mutex){
  sigprocmask(SIG_BLOCK, &block, NULL);

  green_t* susp = running;
  while(mutex->taken){
    add_queue(&mutex->susp, susp);
    set_next();
    swapcontext(susp->context, running->context);
  }

  mutex->taken = TRUE;
  sigprocmask(SIG_UNBLOCK, &block, NULL);
  return 0;
}

int green_mutex_unlock(green_mutex_t *mutex){
  sigprocmask(SIG_BLOCK, &block, NULL);
  add_ready_queue(mutex->susp);
  mutex->susp = NULL;
  mutex->taken = FALSE;
  sigprocmask(SIG_UNBLOCK, &block, NULL);
  return 0;
}
