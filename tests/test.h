/*
 * KEDJI Komlan Akpédjé <eric.kedji@gmail.com>
 * http://erickedji.wordpress.com/
 *
 * Common routines for the test case, and their
 * abbreviated forms.
 */

#include "libpc.h"

#define T ftrue()
#define F ffalse()
#define e echo
#define ws write_string
#define wi WI

int echo(int n) {
    ws("echo\t");
    wi(1, n);
    ws("\n");
    return n;
}

int ftrue(void)     { ws("True\n");    return 1; }
int ffalse(void)    { ws("False\n");   return 0; }

