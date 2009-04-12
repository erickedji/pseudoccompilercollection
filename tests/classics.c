/*
 * Some classics of language flamewars.
 * 
 * KEDJI Komlan Akpédjé <eric_kedji@yahoo.fr> 
 */

#include "test.h"

int fib(int n) {
    if ( n <= 1 )
        return 1;
    else
        return fib(n - 1) + fib(n - 2);
}

int fact (int n) {
    if (n <= 1)
        return 1;
    else
        return n * fact(n - 1);
}

int myremainder(int a, int b) {

    if ( b == 0) {
        ws("myremainder(a, b): error: b is 0\n");
        return 0;
    }

    if ( b < 0 )
        return myremainder(a, -b);

    if ( a < 0 )
        return - myremainder(-a, b);

    if ( a < b ) return a;
    if ( a == b ) return 0;

    return myremainder(a - b, b);
    
}

int gcd(int a, int b) {
    if ( b == 0)
        return a;
    else
        return gcd(b, myremainder(a, b));
}

int prime(int n) {
    /* very dummy test of primality */
    int i;
    i = 2;
    while(i < n) {
        if (n % i == 0)
            return 0;
        i = i + 1;
    }
    return 1;
}

int ack(int x, int y) {
    if ( y == 0 )
        return 0;

    if ( x == 0 )
        return 2 * y;

    if ( y == 1 )
        return 2;

    return ack( x - 1, ack(x, y - 1));
}

int square(int n) { return n * n; }

int even (int n) { return myremainder(n, 2) == 0; }

int fastexpt(int b, int n) {
    if ( n == 0) return 1;

    if (even(n))
        return square(fastexpt(b, n/2));
    else
        return b * fastexpt(b, n - 1);
}

int main (void)
{
    
    ws("\nLanguage flamewar classics.\n");

    ws("\nfibonacci(10)=\t");
    ws("");
    wi(1, fib(10));
    ws("\nfactorial(8)=\t");
    wi(1, fact(10));

    ws("\ngcd(84, 56)=\t");
    wi(1, gcd(84, 56));
    ws("\nprime(761)=\t");
    wi(1, prime(761));
    ws("\nackermann(2, 3)=\t");
    wi(1, ack(2, 3));
    ws("\nfastexpt(2, 10)=\t");
    wi(1, fastexpt(2, 10));
    ws("\nBye\n");
    return 0;
}


