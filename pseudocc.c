/*
 *
 * KEDJI Komlan Akpédjé <eric.kedji@gmail.com>
 * http://erickedji.wordpress.com/
 *
 * The PseudoC compiler.
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "pcc_defs.h"
#include "pc_symtable.h"
#include "pc_parser.tab.h"
#include "ic_generator.h"

#define OUTPUTFILE "PseudoCIntermediateCode.s"
/* where to write the intermediate code */
FILE *out;
int nberrs;

extern int yyparse(void);
extern FILE *yyin;

static FILE *
open(const char *name) {
    FILE *f = fopen(name, "r");

    if ( !f ) {
        fprintf(stderr, " %s: No such file.\n", name);
        exit(1);
    }

    return f;
}

static void
init(void) {
    int i;

    for ( i = 0; i < 3; i++ )
        symbol_tables[i] = NULL;

    yylloc.first_line = yylloc.last_line = 1;
    yylloc.first_column = yylloc.last_column = 1;

    *current_fn_name = '\0';
    *current_file_name = '\0';

    out = fopen(OUTPUTFILE, "w");
    assert( out != NULL );

    nberrs = 0;
}

int main (int argc, char const* argv[])
{
    int retval, n;

    init();
    if ( argc == 1 ) {      /* no argument, read stdin */
        yyin = stdin;
        strcpy(current_file_name, "<stdin>");
    } else {
        yyin = open(argv[1]);
        strcpy(current_file_name, argv[1]);
    }

    
    emit(OPCALL, "main");   /* at the beginning was main */
    emit(OPEXIT);           /* done */

    n = 1;
    while ( 1 ) {
        retval = yyparse ();
        printf("\n -> Parsed %s with exit status %d\n",
                current_file_name, retval);
        printf(" -> File %s: symbol table:\n", current_file_name);
        print_symbol_table(FILE_LEVEL);
        symbol_tables[FILE_LEVEL] = NULL;

        if ( retval ) exit(-1);

        if ( ++n >= argc ) break;
        strcpy(current_file_name, argv[n]);
        yyin = open(argv[n]);
    }

    printf(" -> Global symbol table:\n");
    print_symbol_table(GLOBAL_LEVEL);

    printf(" -> Total number of errors: %d\n", nberrs);

    return ( nberrs > 0 );
}

