#ifndef MYTHREADS_CONTEXT_H
#define MYTHREADS_CONTEXT_H

typedef unsigned int uint;

struct context {
    uint edi;
    uint esi;
    uint ebx;
    uint ebp;
    uint eip;
};

struct context *new_context(uint *, void (*)(int), int);

#define STACK_SIZE 4096

#endif //MYTHREADS_CONTEXT_H
