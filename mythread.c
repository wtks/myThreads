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

void add_thread_to_list(mythread_t t) {
    th_list_node_t *node = malloc(sizeof(th_list_node_t));
    node->thread = t;
    node->next = NULL;

    if (th_list_first == NULL) {
        th_list_first = node;
    } else {
        th_list_node_t *current = th_list_first;
        while (current->next != NULL) current = current->next;
        current->next = node;
    }
}

void remove_thread_from_list(mythread_t t) {
    th_list_node_t *before = th_list_first;
    th_list_node_t *current = th_list_first;
    while (current != NULL && current->thread != t) {
        before = current;
        current = current->next;
    }
    if (current == NULL) return;

    if (current == th_list_first) {
        th_list_first = th_list_first->next;
    } else {
        before->next = current->next;
    }
    free(current);
}

uint stack_size = STACK_SIZE;
struct context *sch_context;

mythread_t new_thread(void (*fun)(int), int arg) {
    mythread_t t = malloc(sizeof(mythread_t));
    t->state = TH_READY;
    t->chan = NULL;
    t->stack = valloc(stack_size);
    t->context = new_context(t->stack + (stack_size / sizeof(uint)), fun, arg);
    add_thread_to_list(t);
    return t;
}

void th_exit() {
    th_list_current->thread->state = TH_FINISHED;
    yield();
}

void th_destory(mythread_t t) {
    free(t->stack);
    free(t);
}

void scheduler() {
    while (th_list_current != NULL) {

        if (th_list_current->thread->state == TH_READY) {
            swtch(&sch_context, th_list_current->thread->context);
        }

        th_list_node_t *tmp = th_list_current;
        th_list_current = th_list_current->next;

        if (tmp->thread->state == TH_FINISHED) {
            // 終了処理
            th_destory(tmp->thread);
            remove_thread_from_list(tmp->thread);
        }

        if (th_list_current == NULL) {
            th_list_current = th_list_first;
        }
    }
}

void yield() {
    swtch(&(th_list_current->thread->context), sch_context);
}

void start_threads() {
    if (th_list_first == NULL || th_list_current != NULL) {
        return;
    }
    th_list_current = th_list_first;
    scheduler();
}

void twait(void *a) {
    th_list_current->thread->state = TH_SLEEP;
    th_list_current->thread->chan = a;
    yield();
}

void notify(mythread_t th, void *a) {
    if (th->chan == a) {
        th->chan = NULL;
        th->state = TH_READY;
    }
}

void notify_all(void *a) {
    th_list_node_t *node = th_list_first;
    while (node != NULL) {
        if (node->thread->chan == a) {
            node->thread->chan = NULL;
            node->thread->state = TH_READY;
        }
        node = node->next;
    }
}

void set_max_stack_size(uint size) {
    stack_size = size;
}
