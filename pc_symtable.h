/*
 * KEDJI Komlan Akpédjé <eric.kedji@gmail.com>
 * http://erickedji.wordpress.com/
 *
 * Définitions de la table de symbole du compilateur PseudoC
 */

#ifndef _SYMBOL_TABLE_H_
#define _SYMBOL_TABLE_H_

#include "shared.h"

/* must be defined before including pc_parser.tab.h because
 * the union used by bison contains a symbol*
 */
typedef struct _symbol symbol;

#include "pcc_defs.h"
#include "pc_parser.tab.h"

typedef enum { FUNCTION, VARIABLE, PARAMETER } symbol_type_t;
typedef enum { VOID_TYPE, INT_TYPE, ARRAY_TYPE } data_type_t;
typedef enum { FUNCTION_LEVEL, FILE_LEVEL, GLOBAL_LEVEL} visibility_level_t;

#ifdef DEFINE_DATA_AND_SYMBOL_TYPES_NAMES
/* must be in sync with data_type_t */
char *data_type_names[3] = {
    "VOID","INT","ARRAY"
};
/* must be in sync with symbol_type_t */
char *symbol_type_names[3] = {
    "FUNCTION","VARIABLE","PARAMETER"
};
#endif


typedef struct {
    int         arity;
    data_type_t return_type;
    int         defined;
    symbol      *plist;
} function_sym_t;

typedef struct {
    data_type_t data_type;
    int         size;
} variable_sym_t;

typedef struct {
    data_type_t data_type;
    int         size;       /* number of elements for arrays    */
    symbol      *next;      /* pointer to next param in function*/
} parameter_sym_t;


struct _symbol{
    char            name[MAX_STR];
    symbol_type_t   type;
    YYLTYPE         loc;
    union {
        variable_sym_t var;
        function_sym_t func;
        parameter_sym_t param;
    } u;
    UT_hash_handle hh;         /* makes this structure hashable, see uthash.h */
};

typedef symbol *symbol_table_t;

symbol*    declare_symbol(
    symbol_type_t       stype,
    char                *name,
    YYLTYPE             loc,
    visibility_level_t  vl,
    data_type_t         data_type,
    int                 size);

void declare_function(
        char                *name,
        YYLTYPE             loc,
        data_type_t         return_type,
        symbol*             param_list);

symbol * define_function(
    char                *name,
    YYLTYPE             loc,
    data_type_t         return_type,
    symbol*             param_list);

symbol * add_param(
    char                *name,
    YYLTYPE             loc,
    data_type_t         dtype,
    int                 size,
    symbol              *param_list);

void    add_symbol(visibility_level_t vl, symbol *sym);
int     check_duplicate(visibility_level_t vl, symbol *sym);
symbol* find_symbol(visibility_level_t vl, char *name);
void    delete_symbol(visibility_level_t vl, symbol *sym);
void    print_symbol_table(visibility_level_t vl); 
void    sort_symbol_table(visibility_level_t vl);
int     level(char *name, data_type_t type, YYLTYPE loc);
//void    free_symbol_table(visibility_level_t vl);


extern symbol_table_t symbol_tables[3];

#endif /* _SYMBOL_TABLE_H_ */

