#ifndef MYTHREADS_MYTHREAD_H
#define MYTHREADS_MYTHREAD_H

#include "context.h"

typedef struct mythread {
    // thread entry point
    void (*fun)(int);

    // thread arg
    int arg;
    // stack
    uint *stack;
    // context
    struct context *context;
} *mythread_t;

mythread_t new_thread(void (*fun)(int), int arg);

void start_threads();

void yield();

void set_max_stack_size(uint size);

#endif //MYTHREADS_MYTHREAD_H
