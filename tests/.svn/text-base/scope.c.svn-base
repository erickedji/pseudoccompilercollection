/*
 * KEDJI Komlan Akpédjé <eric_kedji@yahoo.fr>
 *
 * Test case for declaration and use levels
 * (function, file, global)
 */


static int foo;
int foobar[5];

#include "test.h"

int baz(int n){
    int k;
    k = 5;
    wi(1, n+2-k);
    return n/2; 
}

int bar(int n){
    int k; 
    k = n*45; 
    wi(1, k); 
    return k / n;
}

int main(void) {
    int k;
    int bazbaz[1];

    k = 85;
    foobar[3] = 7854;
    bazbaz[10-10] = 5;

    wi(1, k);
    wi(1, foobar[3]);
    wi(1, baz(k));
    wi(1, bar(k));
    wi(1, bazbaz[3/2 - 1]);
    ws("\nBye\n");

    return 0;
}

