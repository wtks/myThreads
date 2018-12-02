#include <stdlib.h>
#include <stdbool.h>
#include "mythread.h"
#include "context.h"
#include "swtch.h"

typedef struct th_list_node {
    mythread_t thread;
    struct th_list_node *next;
} th_list_node_t;

th_list_node_t *th_list_first = NULL;
th_list_node_t *th_list_current = NULL;

void add_thread_to_list(mythread_t th) {
    th_list_node_t *node = malloc(sizeof(th_list_node_t));
    node->thread = th;
    node->next = NULL;

    if (th_list_first == NULL) {
        th_list_first = node;
    } else {
        th_list_node_t *current = th_list_first;
        while (current->next != NULL) current = current->next;
        current->next = node;
    }
}

uint stack_size = STACK_SIZE;
bool running = false;

struct context *sch_context;

mythread_t new_thread(void (*fun)(int), int arg) {
    mythread_t t = malloc(sizeof(mythread_t));
    t->fun = fun;
    t->arg = arg;
    t->stack = valloc(stack_size);
    t->context = new_context(t->stack - STACK_DEPTH, fun, arg);
    add_thread_to_list(t);
    return t;
}

void scheduler() {
    while (true) {
        if (th_list_current == NULL) break;
        swtch(&sch_context, th_list_current->thread->context);
        th_list_current = th_list_current->next;
        if (th_list_current == NULL) {
            th_list_current = th_list_first;
        }
    }
}

void yield() {
    swtch(&(th_list_current->thread->context), sch_context);
}

void start_threads() {
    if (running) {
        return;
    }
    if (th_list_first == NULL) {
        return;
    }
    th_list_current = th_list_first;
    scheduler();
}

void set_max_stack_size(uint size) {
    stack_size = size;
}
