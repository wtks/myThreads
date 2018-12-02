#ifndef MYTHREADS_MYTHREAD_H
#define MYTHREADS_MYTHREAD_H

#include <stdbool.h>
#include "context.h"

typedef struct mythread {
    // whether finished
    bool finished;
    // stack
    uint *stack;
    // context
    struct context *context;
} *mythread_t;

mythread_t new_thread(void (*fun)(int), int arg);

void start_threads();

void th_exit();

void yield();

void set_max_stack_size(uint size);

#endif //MYTHREADS_MYTHREAD_H
