/*
 * KEDJI Komlan Akpédjé <eric.kedji@gmail.com>
 * http://erickedji.wordpress.com/
 *
 * Front-end de l'interpréteur du PseudoC
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "pci_defs.h"
#include "ic_generator.h"

#define OUTPUTFILE "PseudoCBinary.pcb"

char code[MAX_CODE];
FILE *ic_file;
int  ic_line;
const char *progname; /* name of the interpreter executable */


//TODO: add safety checks (field length checks to scanf for op->str


static void
init(void) {
    operator_t *op;
    int i;

    operators = NULL;
    labels = NULL;
    vars = NULL;
    ic_line = 1;

    for (i = FIRST_OP; i <= LAST_OP; i++) {
        MALLOC(op, operator_t, 1);
        strcpy(op->name, opnames[i]);
        op->code = i;
        HASH_ADD_STR(operators, name, op);
    }
}

/* sanity check after bytecode generation:
 * check if all used labels have been defined
 */
static int
check_label_locs(void) {
    label_t *l;
    int retval = 1;
    for ( l = labels; l; l = l->hh.next ) {
        if ( !l->loc ) {
            eprint(
                    " ! Label or function `%s' used but not declared.\n",
                    l->name);
            retval = 0;
        }
    }
    return retval;
}

int main (int argc, char const* argv[])
{
    FILE *bin;
    int last_instp; /* address of top of inst stack after code generation */


    progname = argv[0];

    if ( argc == 1 ) {
        /* no file name specified, read stdin */
        ic_file = stdin;
    } else if (argc == 2 ) {
        ic_file = fopen(argv[1], "r");
    } else {
        printf("Usage:\n\tpseudoci <PseudoCIntermediateCode.pcb>\n");
        exit(1);
    }

    bin = fopen(OUTPUTFILE, "wb");


    assert( bin != NULL );
    assert( ic_file != NULL );

    init();
    /* generate byte-code
     * TODO: if there is a bytecode file, just use it
     * there are issues: compile time, ...
     */

    last_instp = generate_bytecode();

    if ( !check_label_locs() ) {
        eprint(" !!! There were undefined labels/function names.\n"
                " Surely a bug not caugth by the compiler.\n");
        exit(1);
    }

    fwrite(code, 1, last_instp, bin);
    fclose(bin);
    fclose(ic_file); 

    run(FIRST_BC_INST_LOC);
    
    return 0;
}

