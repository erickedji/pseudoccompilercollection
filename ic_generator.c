/*
 * KEDJI Komlan Akpédjé <eric.kedji@gmail.com>
 * http://erickedji.wordpress.com/
 *
 * Générateur de code intermédiaire pour le PseudoC
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define DEFINE_OPNAMES
#include "pcc_defs.h"
#define COMPILER_CODE
#include "ic_generator.h"

/* maximum number of temporary labels */
#define MAX_LABELS 256

extern FILE *out;
typedef char lbl[7];
lbl label_stack[MAX_LABELS];
int label_stack_p = 0;

/* générer un nouveau label, L00000 -> L99999 */
char *
new_label(void) {
    static unsigned int i = 0;
    char *label = GC_MALLOC(7);

    assert( label != NULL );
    assert( i != 99999 );

    sprintf(label, "L%05u", i++);
    return label;
}

char *
pop_label() {
    if ( label_stack_p < 0 ) {
        fprintf(stderr, "Label stack underflow, exiting.\n");
        exit(-1);
    }
    return label_stack[label_stack_p--];
}

void
push_label(char *label) {
    if ( label_stack_p >= MAX_LABELS - 1 ) {
        fprintf(stderr, "Label stack overflow, exiting.\n");
        exit(-1);
    }
    strcpy(label_stack[++label_stack_p],label);
}

/* make a variable name unique accross all the program */
static char *
canonize(char *name, int level) {
    char *s;

    s = GC_MALLOC(MAX_STR);
    assert ( s != NULL );

    if ( level == FUNCTION_LEVEL ) {
        sprintf(s, "_%s_%s", current_fn_name, name);
    } else {
        strcpy(s, name);
    }
    return s;
}

void
emit(int inst, ...) {
    va_list argp;
    int num;
    char str[MAX_STR];

    va_start(argp, inst);
    switch( inst ) {
        case NO_ARG_OPS:
            /* la tabulation à la fin est importante:
             * toutes les instruction du CI sont supposés
             * suivis de \t par le parser du code intermédiaire
             */
            fprintf(out, "\t%s\t", opnames[inst]);
            break;
        case VAR_NAME_ARG_OPS :
            strcpy(str, va_arg(argp, char *));  /* var name */
            num = va_arg(argp, int);              /* level    */
            fprintf(out, "\t%s\t%s", opnames[inst], canonize(str, num));
            break;
        case LABEL_NAME_ARG_OPS:
            strcpy(str, va_arg(argp, char *));  /* label name    */
            fprintf(out, "\t%s\t%s", opnames[inst], str);
            break;
        case OPLABEL:
            strcpy(str, va_arg(argp, char *));  /* label name   */
            fprintf(out, "%s:", str);
            break;
        case OPPUSHC:               /* push a constant */
        case OPWRITEINT:            /* write a list of ints to stdout */
            num = va_arg(argp, int);              /* nb args */
            fprintf(out, "\t%s\t%d", opnames[inst], num);
            break;
        case OPWRITESTR:            /* write a string to stdout */
            strcpy(str, va_arg(argp, char *));  /* the string */
            fprintf(out, "\t%s\t\"%s\"", opnames[inst], str);
            break;
        case OPDATA:                /* reserve space for global var */
        case OPLVAR:                /* reserve space for local var */
        case OPPARAM:               /* declare parameter */
            strcpy(str, va_arg(argp, char *));  /* var name     */
            num = va_arg(argp, int);            /* size     */
            fprintf(out, "\t%s\t%s,%d",
                    opnames[inst],
                    canonize(str, (inst == OPDATA) ?
                        GLOBAL_LEVEL : FUNCTION_LEVEL),
                    num * sizeof(int));
            break;
            
        case OPFUNC:
            strcpy(str, va_arg(argp, char *));  /* func name    */
            //s = va_arg(argp, symbol *);       /* arg_list     */
            fprintf(out, "F_%s:", str);
            break;
        default:
            fprintf(stderr, "Unimplemented operation:%d\n", inst);
    }
    
    fputc('\n', out);
    va_end(argp);
}

