/*
 * KEDJI Komlan Akpédjé <eric.kedji@gmail.com>
 * http://erickedji.wordpress.com/
 *
 * General utilities for the interpreter.
 */

#include "pci_defs.h"
#define DEFINE_OPNAMES
#include "ic_generator.h"

/* print an error message */
void eprint(char *format, ...) {
    va_list argp;

    va_start(argp, format);
    fprintf(stderr, " %s: ", progname);
    vfprintf(stderr, format, argp);
    va_end(argp);
}

