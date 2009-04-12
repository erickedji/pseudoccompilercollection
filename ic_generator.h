/*
 * KEDJI Komlan Akpédjé <eric.kedji@gmail.com>
 * http://erickedji.wordpress.com/
 */

#ifndef _CODE_GENERATOR_H_
#define _CODE_GENERATOR_H_

#include <stdarg.h>
#include "pc_symtable.h"
#include "pc_parser.tab.h"


#define NO_ARG_OPS          OPADD   ... OPHALT
#define VAR_NAME_ARG_OPS    OPPUSHLV ... OPREADL
#define LABEL_NAME_ARG_OPS  OPCALL  ... OPJUMP
#define FIRST_OP            OPADD
#define LAST_OP             OPPARAM
typedef enum {
    /* no-argument operators OPADD ... OPHALT                  (0 - 17) */
    OPADD, OPSUB, OPMULT, OPDIV, OPMOD, OPEQ, OPNE,
    OPLT, OPGT, OPLE, OPGE, OPNOT, OPNEG,
    OPEXIT, OPRETE, OPRET, OPPOP, OPHALT,
    /* one (var name) argument operators OPPUSHLV ... OPREADL (18 - 27) */
    OPPUSHLV, OPPUSHGV, OPPUSHLAE, OPPUSHGAE, OPPUSHGA, OPPUSHLA,
    OPASSGNLV, OPASSGNGV, OPASSGNLAE, OPASSGNGAE, OPREADG, OPREADL, 
    /* one (label name) arg OPCALL ... OPJUMP                 (28 - 31) */
    OPCALL, OPJFALSE, OPJTRUE, OPJUMP,
    /* special guys                                           (32 - 39) */
    OPFUNC, OPLABEL, OPWRITEINT, OPWRITESTR, OPPUSHC, OPDATA,
    OPLVAR, OPPARAM
} opcodes;

#ifdef DEFINE_OPNAMES
/* operator names, must be in sync with opcodes */
const char *opnames[] = {
    /* no arg */
    "ADD", "SUB", "MULT", "DIV", "MOD", "EQ", "NE",
    "LT", "GT", "LE", "GE", "NOT", "NEG",
    "EXIT", "RETE", "RET", "POP", "HALT",
    /* one (var name) arg */
    "PUSHLV", "PUSHGV", "PUSHLAE", "PUSHGAE", "PUSHGA", "PUSHLA",
    "ASSGNLV", "ASSGNGV", "ASSGNLAE", "ASSGNGAE", "READG", "READL",
    /* one (label name) arg */
    "CALL", "JFALSE", "JTRUE", "JUMP",
    /* special guys */
    "FUNC", "LABEL", "WINT", "WSTR", "PUSHC", "DATA",
    "LVAR", "PARAM"
        
};
#endif /* DEFINE_OPNAMES */

#ifdef COMPILER_CODE
char *new_label(void);
char *pop_label();
void push_label(char *label);
#endif

extern void emit(int inst, ...);

#endif /* _CODE_GENERATOR_H_ */

