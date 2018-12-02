#include <stdio.h>
#include "mythread.h"

int chan;

void foo(int c) {
    twait(&chan);
    while (c < 5) {
        printf("foo : %d\n", c);
        c += 1;
        yield();
    }
}

void bar(int c) {
    twait(&chan);
    while (c < 20) {
        printf("bar : %d\n", c);
        yield();
        c += 2;
    }
}

void baz(int c) {
    while (c < 15) {
        printf("baz : %d\n", c);
        yield();
        c += 3;
        printf("baz : %d\n", c);
        yield();
        c += 3;
    }
    notify_all(&chan);
}

int main() {
    new_thread(foo, 0);
    new_thread(bar, 0);
    new_thread(baz, 0);
    start_threads();
    return 0;
}
