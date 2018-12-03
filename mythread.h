#ifndef MYTHREADS_MYTHREAD_H
#define MYTHREADS_MYTHREAD_H

#include <stdbool.h>
#include "context.h"

enum tstate {
    TH_READY, TH_SLEEP, TH_FINISHED
};

typedef struct mythread {
    enum tstate state;
    void *chan;
    uint *stack;
    struct context *context;
} *mythread_t;

mythread_t new_thread(void (*)(int), int);

void start_threads();

void start_threads_with_preemption(uint);

void th_exit();

void yield();

void twait(void *);

void notify(mythread_t, void *);

void notify_all(void *);

void set_max_stack_size(uint);

#endif //MYTHREADS_MYTHREAD_H
