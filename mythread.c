#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <sys/time.h>
#include "mythread.h"
#include "context.h"
#include "swtch.h"

typedef struct th_list_node {
    mythread_t thread;
    struct th_list_node *next;
} th_list_node_t;

static th_list_node_t *th_list_first = NULL;
static th_list_node_t *th_list_current = NULL;

static void add_thread_to_list(mythread_t t) {
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

static void remove_thread_from_list(mythread_t t) {
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
struct itimerval timerval;
struct sigaction preempt;

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

static void th_destory(mythread_t t) {
    free(t->stack);
    free(t);
}

static void scheduler() {
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

static void sig_handler(int signum) {
    if (signum == SIGALRM) {
        yield();
    }
}

void start_threads_with_preemption() {
    if (th_list_first == NULL || th_list_current != NULL) {
        return;
    }
    th_list_current = th_list_first;

    // 割り込みハンドラ初期化
    memset(&preempt, 0, sizeof(preempt));
    preempt.sa_handler = sig_handler;
    preempt.sa_flags = SA_RESTART | SA_NODEFER;
    sigemptyset(&preempt.sa_mask);
    if (sigaction(SIGALRM, &preempt, NULL) < 0) {
        perror("sigaction error");
        exit(1);
    }

    // 割り込みタイマ設定
    timerval.it_value.tv_sec = 0;
    timerval.it_value.tv_usec = 10000;
    timerval.it_interval = timerval.it_value;
    if (setitimer(ITIMER_REAL, &timerval, NULL) != 0) {
        perror("setitimer error");
        exit(1);
    }

    scheduler();

    // 割り込みタイマ削除
    timerval.it_value.tv_usec = 0;
    timerval.it_interval = timerval.it_value;
    if (setitimer(ITIMER_REAL, &timerval, NULL) != 0) {
        perror("setitimer error");
        exit(1);
    }

    // 割り込みハンドラ削除
    preempt.sa_handler = SIG_DFL;
    if (sigaction(SIGALRM, &preempt, NULL) < 0) {
        perror("sigaction error");
        exit(1);
    }
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
