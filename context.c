#include "context.h"
#include "mythread.h"

#define push(s, v) (*--(s)=(v))

struct context *new_context(uint *sp, void (*fun)(int), int arg) {
    push(sp, arg);
    push(sp, (uint)th_exit);
    sp -= sizeof(struct context) / sizeof(uint);
    struct context *ctx = (struct context *)sp;
    ctx->eip = (uint)fun;
    return ctx;
}
