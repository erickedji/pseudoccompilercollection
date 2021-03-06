/*
 * KEDJI Komlan Akpédjé <eric.kedji@gmail.com>
 * http://erickedji.wordpress.com/
 *
 * Implementation of PseudoC system calls in C.
 * Used to implement a nice testing framework
 * where the same code is compiled with gcc and
 * pseudocc. The outputs of the two programs are
 * then diff-ed.
 * 
 * Sunday 13 April 2008
 * Now listening to: DIAMS, Me revoila.
 *
 * TODO: How to test array-argument passing?
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/* write string est utilisé tel quel par les deux langages */
void write_string(const char *str)  { printf("%s", str);      }

void _libpc_read_int(int *n)        { scanf("%d", n);   }

void _libpc_write_int(int nb_args, ...) {
    va_list argp;

    va_start(argp, nb_args);
    while ( nb_args --> 0 ) {
        printf("%d ", va_arg(argp, int));
    }
    va_end(argp);
}


