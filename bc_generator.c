/*
 * KEDJI Komlan Akpédjé <eric.kedji@gmail.com>
 * http://erickedji.wordpress.com/
 *
 * Generate byte-code instructions from ic_inst_t structures
 */

#include "pci_defs.h"
#include "ic_generator.h"

static int  local_data_storep;

static label_t *add_label(char *name, int loc);
static datum_t *add_var(char *name, int size);
static label_t *find_label(char *name);
static datum_t *find_datum(char *name);

/* convert escape sequences in a string */
static char
*unescape(char *str) {
    char *ustr, *ustrp, *strp;
    
    MALLOC(ustr, char, MAX_STR);

    ustrp = ustr;
    strp = str;

    while ( *strp != '\0' ) {
        if ( *strp == '\\' ) {
            strp++;
            switch ( *strp ) {
                case 'n':
                    *ustrp++ = '\n';
                    break;
                case 't':
                    *ustrp++ = '\t';
                    break;
                case '\\':
                case '"':
                    *ustrp++ = *strp;
                    break;
                case '\0':
                    *ustrp++ = '\\';
                    *ustrp = '\0';
                    return ustr;
                default:
                    *ustrp++ = '\\';
                    *ustrp++ = *strp;
            }
            strp ++;
        } else {
            *ustrp++ = *strp++;
        }
    }

    *ustrp = '\0';

    return ustr;

}

/* generate a new string litteral identifier */
static char *
new_str_id(void) {
    static unsigned int i = 0;
    char *id = GC_MALLOC(8);

    assert( id != NULL );
    assert( i != 999 );

    sprintf(id, "_str%03u", i++);
    return id;
}

static label_t *
find_label(char *name) {
    label_t *label;

    HASH_FIND_STR(labels, name, label);

    return label;
}

static datum_t *
find_datum(char *name) {
    datum_t *datum;

    HASH_FIND_STR(vars, name, datum);

    return datum;
}

static label_t *
add_label(char *name, int loc) {
    label_t *label;

#ifndef NDEBUG
    if ( find_label(name) ) {
        eprint(" !!! Duplicated label: %s\n", name);
        return NULL;
    }
#endif

    MALLOC(label, label_t, 1);
    label->back_patch_locs = NULL;
    label->loc = loc;
    strcpy(label->name, name);

    HASH_ADD_STR(labels, name, label);

#ifndef NDEBUG
    eprint(" Added label %s\n", label->name);
#endif

    return label;
}

static void
back_patch_label(label_t *label) {
    mem_loc_t *ml;

    for ( ml = label->back_patch_locs; ml; ml = ml->next ) {
        memcpy(code + ml->loc, &label->loc, sizeof(int));
#ifndef NDEBUG
        eprint(" Back-patching loc %d with label %s[%d]\n",
                ml->loc, label->name, label->loc);
#endif
    }
}

static datum_t *
add_var(char *name, int size) {
    datum_t *datum;

#ifndef NDEBUG
    if ( find_datum(name) ) {
        eprint(" !!! Duplicated datum: %s\n", name);
        return NULL;
    }
#endif
    
    MALLOC(datum, datum_t, 1);
    datum->back_patch_locs = NULL;
    datum->size = size;
    /* important: reste à NULL pour les variables normales,
     * pointe sur la chaine pour les constantes chaine de caractère
     */
    datum->str = NULL; 
    strcpy(datum->name, name);

    HASH_ADD_STR(vars, name, datum);

    return datum;
}

/* make snafuced :-)
 * ever red "How to write unmaintenable programs" ?
 * you should
 */
int
generate_bytecode(void) {
    int instp = FIRST_BC_INST_LOC; /* instruction pointer: next free spot in code[] */
    ic_inst_t *inst;
    label_t *label;
    char *str;
    int num;
    datum_t *datum;
    /* memory location where we should write the total size of the
     * parameters of the current function.
     * = 0 if no param has yet been encountered for the current
     * function
     */
    static int params_size_loc;


    for ( EVER ) {
        inst = next_ic_inst();

        if ( inst->code == END_OF_IC_OP )
            break;

        if ( inst->code == UNDEFINED_OP ) {
            eprint("Undefined operator at line %d\n", ic_line);
            continue;
        }

#ifndef NDEBUG
        printf(" gencode: %s\n", opnames[(int)inst->code]);
#endif
        switch ( inst->code ) {
            case NO_ARG_OPS:
                code[instp++] = inst->code;
                break;

            case OPJTRUE:
            case OPJFALSE:
            case OPJUMP:
            case OPCALL:
                code[instp++] = inst->code;
                label = find_label(inst->str);
                if ( !label ) {  //label undeclared: declare it
                    label = add_label(inst->str, 0);
#ifndef NDEBUG
                    printf("declaring tag %s, opcode=%d\n",
                            inst->str, inst->code);
#endif
                }
                
                if ( label->loc ) {  //label defined, tant mieux.
                    memcpy(code + instp, &label->loc, sizeof(int));
#ifndef NDEBUG
                    printf("using defined tag %s[loc = %d], opcode=%d\n",
                            inst->str, label->loc, inst->code);
#endif
                } else {  ///label undefined: add inst to backpatching list
#ifndef NDEBUG
                    printf("saving location for bp: loc = %d\n",
                            instp);
#endif
                    mem_loc_t *ml;
                    MALLOC(ml, mem_loc_t, 1);
                    ml->loc  = instp;
                    ml->next = label->back_patch_locs;
                    label->back_patch_locs = ml;
                }
                instp += sizeof(int);
                break;

            case OPFUNC:
                local_data_storep = 0;
                params_size_loc = 0;
            case OPLABEL:
                label = find_label(inst->str);
                if ( label ) {
#ifndef NDEBUG
                    printf("defining label %s[loc=%d] and backpatching locs\n",
                            label->name, instp);
#endif
                    label->loc = instp;
                    back_patch_label(label);
                } else {
#ifndef NDEBUG
                    printf("defining label %s[loc=%d] with no previous usage\n",
                            inst->str, instp);
#endif
                    add_label(inst->str, instp);
                }
                break;

            case OPWRITEINT:
            case OPPUSHC:
                code[instp++] = inst->code;
                memcpy(code + instp, &inst->num, sizeof(int));
                instp += sizeof(int);
                break;

            case OPWRITESTR:
                code[instp++] = inst->code;
                str = new_str_id();
                datum = add_var(str, strlen(inst->str) + 1);
                assert( datum != NULL );
                datum->str = inst->str;
                {   mem_loc_t *ml;
                    MALLOC(ml, mem_loc_t, 1);
                    ml->loc  = instp;
                    ml->next = NULL;
                    datum->back_patch_locs = ml;
                }
                instp += sizeof(int);
                break;

            case OPLVAR:
                code[instp++] = inst->code;
                datum = add_var(inst->str, inst->num);
                assert( datum != NULL );
                datum->loc = local_data_storep;
                local_data_storep += inst->num;
                memcpy(code + instp, &inst->num, sizeof(int));
                instp += sizeof(int);
                break;

            case OPPARAM:
                /* write a PARAM operator with a new semantic:
                 * the size argument is the total size in bytes of all
                 * the params of the current function
                 */
                datum = add_var(inst->str, inst->num);
                assert( datum != NULL );
                datum->loc = local_data_storep;
                local_data_storep += inst->num;
                if ( !params_size_loc ) {
                    code[instp++] = inst->code;
                    // write a zero (PARAM 0)
                    memcpy(code + instp, &params_size_loc, sizeof(int)); 
                    params_size_loc = instp;
                    instp += sizeof(int);
                }
                num = *((int *) (code + params_size_loc));
                num += inst->num;
                memcpy(code + params_size_loc, &num, sizeof(int));
                break;

            case OPDATA:
                add_var(inst->str, inst->num);
                break;

            case OPPUSHLV:
            case OPPUSHLA:
            case OPPUSHLAE:
            case OPASSGNLV:
            case OPASSGNLAE:
            case OPREADL:
                code[instp++] = inst->code;
                datum = find_datum(inst->str);
                assert( datum != NULL );
                //datum->loc == offset in stack frame
                memcpy(code + instp, &datum->loc, sizeof(int));
                instp += sizeof(int);
                break;

            case OPPUSHGV:
            case OPPUSHGA:
            case OPPUSHGAE:
            case OPASSGNGV:
            case OPASSGNGAE:
            case OPREADG:
                code[instp++] = inst->code;
                datum = find_datum(inst->str);
                assert( datum != NULL );
                {   mem_loc_t *ml;
                    MALLOC(ml, mem_loc_t, 1);
                    ml->loc  = instp;
                    ml->next = datum->back_patch_locs;
                    datum->back_patch_locs = ml;
                }
                instp += sizeof(int);
                break;

            default:
                eprint(" Unimplemented instruction: %d\n",
                        inst->code);
        }

    }

    // reserve location for global vars (and back_patch locations)
    mem_loc_t *ml;
    for ( datum = vars; datum; datum = datum->hh.next ) {
        ml = datum->back_patch_locs;
        while ( ml ) {
            memcpy(code + ml->loc, &instp, sizeof(int));
            ml = ml->next;
        }
        /* initialize the global variable if it is a string constant */
        if ( datum->str ) {
            strcpy(code + instp, unescape(datum->str));
        }
        instp += datum->size;
    }

    return instp;
}

