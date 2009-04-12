/*
 * KEDJI Komlan Akpédjé <eric_kedji@yahoo.fr>
 *
 * Test case for array manipulation
 */

int baz[5];

#include "test.h"

void foo(int bar[5]) {
    int i;
    i = 0;
    while ( i < 5 ) {
        ws("bar[");
        wi(1, i);
        ws("] =\t");
        wi(1, bar[i]);
        i = i + 1;
    }
}

int main(void) {
    int tab[5];
    int i;

    i = 0;
    while( i < 5 ) {
        tab[i] = 42;
        baz[i] = 75;
        i = i + 1;
    }

    i = 0;
    while(i < 5) {
        wi(1, tab[i]);
        i = i + 1;
    }

    foo(tab);
    foo(baz);

    return 0;
}

