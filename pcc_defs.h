/*
 * KEDJI Komlan Akpédjé <eric.kedji@gmail.com>
 * http://erickedji.wordpress.com/
 *
 * Définitions du compilateurs PseudoC
 */


#ifndef _PCC_DEFS_H_
#define _PCC_DEFS_H_

#include "shared.h"


/* parser macros */
#define IN_FUNCTION (*current_fn_name != '\0')
/* save infos about the current function for use by error message etc */
#define START_FUNC(name)  do {                                  \
    strcpy(current_fn_name, name);                              \
} while(0);

#define END_FUNC()        do {                                  \
    printf("-> Function %s: symbol table:\n", current_fn_name); \
    print_symbol_table(FUNCTION_LEVEL);                         \
    *current_fn_name = '\0';                                    \
    symbol_tables[FUNCTION_LEVEL] = NULL;                       \
} while(0);

#ifdef NDEBUG
#undef END_FUNC
#define END_FUNC()        do {                                  \
    *current_fn_name = '\0';                                    \
    symbol_tables[FUNCTION_LEVEL] = NULL;                       \
} while(0);
#endif

/* start a new function call */
#define START_FN_CALL() do {                                    \
    current_arglist_length;                                     \
} while (0);

extern int nberrs;
extern char current_fn_name[MAX_STR];   /* nom de la fonction courante    */
extern char current_file_name[MAX_STR]; /* nom du fichier courant         */
extern int current_arglist_length;      /* longeur de la liste d'arguments
                                           de l'appel de fonction en préparation */

#endif /* _PCC_DEFS_H_ */

