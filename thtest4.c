#include <stdio.h>
#include "mythread.h"

void foo(int c) {
    while (c < 700000000) {
        if (c % 10000000 == 0) printf("foo : %d\n", c);
        c += 1;
    }
}

void bar(int c) {
    while (c < 10) {
        printf("bar : %d\n", c);
        c += 1;
        yield();
    }
}

void baz(int c) {
    while (c < 1000000000) {
        if (c % 100000000 == 0) printf("baz : %d\n", c);
        c += 1;
    }
}

int main() {
    new_thread(foo, 0);
    new_thread(bar, 0);
    new_thread(baz, 0);
    start_threads_with_preemption(10);
    return 0;
}
