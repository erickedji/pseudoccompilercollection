/*
 * KEDJI Komlan Akpédjé <eric.kedji@gmail.com>
 * http://erickedji.wordpress.com/
 *
 * Utilitaire pour obtenir le nom d'un opérateur du
 * langage intermédiare du PseudoC à partir de son code
 */
#include <stdlib.h>
#include <stdio.h>

#define DEFINE_OPNAMES
#include "ic_generator.h"

int main (int argc, char const* argv[])
{
    int code;
    if ( argc != 2 ) {
        printf(" Utility to get an operator name from its code\n"
                "  -> code in decimal: no prefix (ex: `45')\n"
                "  -> code in hexadecimal: prefix `x' (ex: `x1c')\n\n"
                "\tUsage:\t %s <dec_code> | %s x<hex_code>\n",
                argv[0], argv[0]);
        exit(1);
    }

    if ( *argv[1] == 'x' ) {
        sscanf(argv[1] + 1, "%x", &code);
    } else {
        sscanf(argv[1], "%d", &code);
    }

    if ( code >= FIRST_OP && code <= LAST_OP ) {
        printf(" dec=%d\t hex=%x\t %s\n", code, code, opnames[code]);
    } else {
        printf(" Unknown opcode: dec=%d\t hex=%x\n", code, code);
    }
    return 0;
}

