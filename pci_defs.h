/*
 * KEDJI Komlan Akpédjé <eric.kedji@gmail.com>
 * http://erickedji.wordpress.com/
 * 
 * Definitions for the PseudoC interpreter
 */

#ifndef _PCI_DEFS_H_

#define _PCI_DEFS_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#include "shared.h"

#define END_OF_IC_OP 100    /* intermediate code file reaches EOF   */
#define UNDEFINED_OP 101    /* unrecognised operator                */

#define EVER    ;;
#define MAX_CODE            99999   /* size of the byte-code store (char)   */
#define MAX_EXPR_STACK      99999   /* size of expression stack (int)       */
#define MAX_CALL_STACK      99999   /* size of call stack (int)             */
#define FIRST_BC_INST_LOC   1       /* location of the first byte-code, > 0 */


typedef struct {
    char name[10];
    char  code;
    UT_hash_handle hh;      /* makes this structure hashable */
} operator_t;

typedef struct {
    char  code;
    int  num;
    char str[MAX_STR];
} ic_inst_t;                /* intermediate code instruction */

typedef struct {
    char code;
    int  loc;
    int  size;
} bc_inst_t;                /* byte-code instruction */

typedef struct _mlt{
    int loc;
    struct _mlt *next;
} mem_loc_t;                /* list of memory locations (for backpatching) */

typedef struct {
    char name[10];
    int  loc;
    /* memory locations that should be backpatched with this label */
    mem_loc_t *back_patch_locs;
    UT_hash_handle hh;      /* makes this structure hashable */
} label_t;                  /* a label, used for jumps */

typedef struct {
    char name[MAX_STR];
    int  size;
    int  loc;
    char *str;
    mem_loc_t *back_patch_locs;
    UT_hash_handle hh;      /* makes this structure hashable */
} datum_t;

extern operator_t  *operators;
extern label_t     *labels;
extern datum_t     *vars;
extern int  ic_line;
extern char code[MAX_CODE];
extern FILE *ic_file;
extern const char *progname; /* name of the interpreter executable */
extern const char *opnames[];

extern int generate_bytecode(void);
extern void eprint(char *format, ...); /* print an error */
extern void run(int instp);
extern ic_inst_t *next_ic_inst(void); /* get the next intermediate code instruction */



#endif /* _PCI_DEFS_H_ */

