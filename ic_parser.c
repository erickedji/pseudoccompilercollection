/*
 * KEDJI Komlan Akpédjé <eric.kedji@gmail.com>
 * http://erickedji.wordpress.com/
 *
 * Decode an intermediate code instruction.
 */

#include "pci_defs.h"
#include "ic_generator.h"

operator_t  *operators;
label_t     *labels;
datum_t     *vars;

/* get the next instruction from the intermediate code file */
ic_inst_t *
next_ic_inst(void) {
    ic_inst_t *inst;
    int c;
    char opname[10];
    operator_t *op;
    
    MALLOC(inst, ic_inst_t, 1);
    c = fgetc(ic_file);

    switch ( c ) {
        case EOF:
            inst->code = END_OF_IC_OP;
            break;
        case 'L':   /* a label definition */
            inst->code = OPLABEL;
            *inst->str = 'L';
            fscanf(ic_file, "%5[0-9]", inst->str + 1);
            break;
        case 'F':   /* a function definition */
            inst->code = OPFUNC;
            (void) fgetc(ic_file);               /* remove '_' in F_foo */
            fscanf(ic_file, "%[a-z]", inst->str);
            break;
        case '\t':  /* a normal instruction */
            fscanf(ic_file, "%[A-Z]", opname);
            HASH_FIND_STR( operators, opname, op);
            inst->code = (op) ? op->code : UNDEFINED_OP;
            (void) fgetc(ic_file);                   /* remove '\t'      */
            switch ( inst->code ) {
                case UNDEFINED_OP:
                    printf("Undefined: '%s'\n", opname);
                case NO_ARG_OPS:
                    break;
                case VAR_NAME_ARG_OPS:
                case LABEL_NAME_ARG_OPS:
                    fscanf(ic_file, "%s", inst->str);
                    break;
                case OPWRITEINT:
                case OPPUSHC:
                    //TODO: how is 02 converted? (octal?)
                    fscanf(ic_file, "%d", &inst->num);
                    break;
                case OPWRITESTR:
                    (void) fgetc(ic_file);           /* remove '"'       */
                    fscanf(ic_file, "%[^\"]", inst->str);
                    (void) fgetc(ic_file);           /* remove '"'       */
                    break;
                case OPDATA:
                case OPLVAR:
                case OPPARAM:
                    fscanf(ic_file, "%[a-z_]", inst->str);     /* get var name     */
                    (void) fgetc(ic_file);           /* remove ','       */
                    fscanf(ic_file, "%d", &inst->num);    /* get var size     */
                    break;
                default:
                    printf(" What are we doing here? nasty bug !!!\n");
                    printf("\topcode = %d, opname = %s\n",
                            inst->code, opnames[(int)inst->code]);
            }
            break;
        default:
            inst->code = UNDEFINED_OP;
    }

    while ( (c=fgetc(ic_file)) != '\n' && c!= EOF)
        ;                   /* skip to end of line */
    if ( (c=fgetc(ic_file)) != EOF ) /* are we at end of file ? */
        ungetc(c, ic_file);
    else
        inst->code = END_OF_IC_OP;

    ic_line ++;
    return inst;
}

