/*
 * KEDJI Komlan Akpédjé <eric.kedji@gmail.com>
 * http://erickedji.wordpress.com/
 * Table de symbole du compilateur Pseudo-C
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#include "pcc_defs.h"
/* some parts of pc_symtable.h should be visible only if 
 * inlcuded from here. I know, it is ugly.
 */
#define DEFINE_DATA_AND_SYMBOL_TYPES_NAMES
#include "pc_symtable.h"

/* allows check_duplicate to signal a duplicated function
 * prototype to declare_function so that the function prototype
 * is not saved twice. FALSE is 0, TRUE is 1. DUPLICATED_PROTO
 * must be considered true by other functions (as it is harmless)
 */
#define DUPLICATED_PROTO 2


void print_fn_signature(symbol *s) {
    assert (s != NULL);
    symbol *pl =  s->u.func.plist;

    fprintf(stderr, " Function `%s':\t", s->name);

    if ( !pl ) {
        fprintf(stderr, "[Procedure]");
    } else {
        fprintf(stderr, "%s", data_type_names[pl->u.param.data_type]);
        pl = pl->u.param.next;
    }

    while ( pl ) {
        fprintf(stderr, ", %s", data_type_names[pl->u.param.data_type]);
        pl = pl->u.param.next;
    }

    fprintf(stderr, " -> %s", data_type_names[s->u.func.return_type]);
    fputc('\n', stderr);
}


/* add a variable declaration or a function prototype
 * declaration to the symbol table
 */
symbol*    declare_symbol(
        symbol_type_t       symbol_type,
        char                *name,
        YYLTYPE             loc,
        visibility_level_t  level,
        data_type_t         data_type,
        int                 size) {

    symbol *s;
    MALLOC(s, symbol, 1);
    s->type = symbol_type;
    s->loc  = loc;
    strcpy(s->name, name);

    if ( symbol_type == FUNCTION ) {
        s->u.func.arity = size;
        s->u.func.return_type = data_type;
        s->u.func.defined = FALSE;
    } else { /* var*/
        s->u.var.size = size;
        s->u.var.data_type = data_type;
    }
    
    if ( check_duplicate(level, s) ) {
#ifndef NDEBUG
        fprintf(stderr, " Adding symbol `%s' with data type \t%s\n",
                s->name,
                data_type_names[(s->type == FUNCTION) ?
                    s->u.func.return_type : s->u.var.data_type]);
#endif
        add_symbol(level, s);
        return s;
    } else {
        nberrs++;
        return NULL;
    }

}

/* save a prototype */
void declare_function(
        char                *name,
        YYLTYPE             loc,
        data_type_t         return_type,
        symbol*             param_list) {

    symbol *s;
    MALLOC(s, symbol, 1);
    s->type = FUNCTION;
    s->loc  = loc;
    strcpy(s->name, name);

    //s->u.func.arity = size;
    s->u.func.return_type = return_type;
    s->u.func.defined = FALSE;
    s->u.func.plist = param_list;

    /* do not add it if retval is FALSE or DUPLICATED_PROTO */
    int retval = check_duplicate(GLOBAL_LEVEL, s);
    if ( retval == TRUE ) {
        add_symbol(GLOBAL_LEVEL, s);
    } else if (retval == FALSE) {
        nberrs++;
    } // else: retval = DUPLICATED_PROTO


}

/* add a function definition to the symbol table */
/* TODO: refactor (duplicate of declare_function) */
/* TODO: obey static to declare a function to the file level */
symbol *
define_function(
        char                *name,
        YYLTYPE             loc,
        data_type_t         return_type,
        symbol*             param_list) {
    
    symbol *s;
    MALLOC(s, symbol, 1);
    s->type = FUNCTION;
    s->loc  = loc;
    strcpy(s->name, name);

    //s->u.func.arity = size;
    s->u.func.return_type = return_type;
    s->u.func.defined = TRUE;
    s->u.func.plist = param_list;

    print_fn_signature(s);


    if ( check_duplicate(GLOBAL_LEVEL, s) ) {
        /* if a prototype exists, replace it by the definition */
        symbol *sym;
        if ( (sym = find_symbol(GLOBAL_LEVEL, s->name)) ) {
            delete_symbol(GLOBAL_LEVEL, sym);
        }
        add_symbol(GLOBAL_LEVEL, s);
        // add all params as vars to the current function symtable
        for ( sym = param_list; sym; sym = sym->u.param.next )
            if ( !declare_symbol(VARIABLE,
                        sym->name, sym->loc, FUNCTION_LEVEL,
                        sym->u.param.data_type, sym->u.param.size) )
                return NULL;
        return s;
    } else {
        nberrs++;
        return NULL;
    }
}

symbol *
add_param(
    char                *name,
    YYLTYPE             loc,
    data_type_t         data_type,
    int                 size,
    symbol              *param_list) {
    
    symbol *s;
    MALLOC(s, symbol, 1);
    s->type = PARAMETER;
    s->loc  = loc;
    strcpy(s->name, name);
 
    s->u.param.size = size;
    s->u.param.data_type = data_type;
    s->u.param.next = param_list;

    return s;
}

void
add_symbol(visibility_level_t vl, symbol *new_s) {
    HASH_ADD_STR( symbol_tables[vl], name, new_s );
}

int
check_duplicate(visibility_level_t vl, symbol *sym) {
    symbol *old_sym = NULL;

    HASH_FIND_STR( symbol_tables[vl], sym->name, old_sym );

    if (old_sym != NULL) {
        /* on peut déclarer une même fonction plusieurs fois */
        if ( sym->type == FUNCTION && old_sym->type == FUNCTION ) {
            symbol *plist1, *plist2;
            if ( sym->u.func.defined && old_sym->u.func.defined ) {
                fprintf(stderr, " Redefined function: `%s': line %d, line %d\n",
                    sym->name, old_sym->loc.first_line, sym->loc.first_line);
                return FALSE; // exit here to avoid over-nesting
            }
            
            plist1 = old_sym->u.func.plist;
            plist2 = sym->u.func.plist;
            while ( plist1 && plist2 && plist1->type == plist2->type ) {
                plist1 = plist1->u.param.next;
                plist2 = plist2->u.param.next;
            }
            
            if ( plist1 || plist2 ) {
                fprintf(stderr, " Function prototype mismatch: `%s': line %d, line %d\n",
                    sym->name, old_sym->loc.first_line, sym->loc.first_line);
                fprintf(stderr, "  First:");
                print_fn_signature(old_sym);
                fprintf(stderr, "  Second:");
                print_fn_signature(sym);
            } else {
                // duplicate prototype: harmless
                return DUPLICATED_PROTO; 
            }
        } else {
            fprintf(stderr, " Duplicated identifier: `%s': line %d, line %d\n",
                    sym->name, old_sym->loc.first_line, sym->loc.first_line);
        }
        return FALSE; // either valid or not, duplicate -> FALSE
    }

    if ( vl == FILE_LEVEL )
        return check_duplicate(GLOBAL_LEVEL, sym);
    
    return 1;
}

symbol*
find_symbol(visibility_level_t vl, char *name) {
    symbol *s;

    HASH_FIND_STR( symbol_tables[vl], name, s );
    return s;
}

void
delete_symbol(visibility_level_t vl, symbol *s) {
    HASH_DEL( symbol_tables[vl], s );
}

void
print_symbol_table(visibility_level_t vl) {
    symbol *s;

    for( s = symbol_tables[vl]; s != NULL; s = s->hh.next ) {
        printf("\tsymbol name: %s  \t[%s]\n", s->name, symbol_type_names[s->type]);
    }
}

int
name_sort(symbol *a, symbol *b) {
    return strcmp(a->name, b->name);
}

void
sort_symbol_table(visibility_level_t vl) {
    HASH_SORT( symbol_tables[vl], name_sort );
}

/* return the level a *variable* has been declared at
 * or -1 if undeclared
 */
int     
level(char *name, data_type_t data_type, YYLTYPE loc) {
    symbol *s;
    int i;

    for ( i = FUNCTION_LEVEL; i <= GLOBAL_LEVEL; i++ ) {
        s = find_symbol(i, name);
        if ( s ) {
            /* function used as var */
            if ( s->type == FUNCTION ) {
                fprintf(stderr,
                        " Function `%s' used as a variable:"
                        " decl: line %d(this file?), use: line %d\n",
                        s->name, s->loc.first_line, loc.first_line);
                return -1;
            }

            if ( s->u.var.data_type != data_type ) {
                fprintf(stderr,
                        " Type mismatch: variable `%s':"
                        " decl[%s]: line %d(this file?), use[%s]: line %d\n",
                        s->name, data_type_names[s->u.var.data_type],
                        s->loc.first_line, data_type_names[data_type],
                        loc.first_line);
                return -1;
            }

            return i;
        }
    }
    fprintf(stderr, " Undeclared variable: `%s': line %d\n",
            name, loc.first_line);
    return -1;
}

/* we use a garbage collector, no need to free things */
#if 0
void free_symbol_table(visibility_level_t vl) {
    symbol *s, *old_s;

    for( s = symbol_tables[vl]; s != NULL; ) {
        old_s = s;
        s = s->hh.next;
        delete_symbol(vl, old_s);
        free(old_s);
    }
}
#endif

