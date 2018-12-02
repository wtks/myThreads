#include "context.h"

#define push(s, v) (*--(s)=(v))

struct context *new_context(uint *sp, void (*fun)(int), int arg) {
    push(sp, arg);
    push(sp, 0);
    sp -= sizeof(struct context) / sizeof(uint);
    struct context *ctx = (struct context *)sp;
    ctx->eip = (uint)fun;
    return ctx;
}
