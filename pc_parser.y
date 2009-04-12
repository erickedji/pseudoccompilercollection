/*
 * KEDJI Komlan Akpédjé <eric.kedji@gmail.com>
 * http://erickedji.wordpress.com/
 *
 * Grammaire du Pseudo-C
 */

%{
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "pc_symtable.h"
/* some definitions in ic_generator.h should be visible only here */
#define COMPILER_CODE
#include "ic_generator.h"
#include "pcc_defs.h"
#include "pc_parser.tab.h" /* ça s'appele manger sa propre queue :-) */

/* tersity revisited :-) */
#define DECSYM declare_symbol
#define DECFN declare_function
#define DEFFN define_function
#define AP add_param
#define FUNC_OR_GLOB ((IN_FUNCTION) ? FUNCTION_LEVEL : GLOBAL_LEVEL)
#define FUNC_OR_FILE ((IN_FUNCTION) ? FUNCTION_LEVEL : FILE_LEVEL)
#define LVAR_OR_DATA ((IN_FUNCTION) ? OPLVAR : OPDATA)

#define PUSHV_L_OR_G    ((l == FUNCTION_LEVEL) ? OPPUSHLV : OPPUSHGV)
#define PUSHA_L_OR_G    ((l == FUNCTION_LEVEL) ? OPPUSHLA : OPPUSHGA)
#define PUSHAE_L_OR_G    ((l == FUNCTION_LEVEL) ? OPPUSHLAE : OPPUSHGAE)
#define ASSGNV_L_OR_G   ((l == FUNCTION_LEVEL) ? OPASSGNLV : OPASSGNGV)
#define ASSGNAE_L_OR_G   ((l == FUNCTION_LEVEL) ? OPASSGNLAE : OPASSGNGAE)
#define READ_L_OR_G     ((l == FUNCTION_LEVEL) ? OPREADL : OPREADG)

#define EMIT_PARAMS(plist)  while (plist) {             \
    emit(OPPARAM, plist->name, plist->u.param.size);    \
    plist = plist->u.param.next;                        \
}

int  yylex(void);
void yyerror(char const *msg);
void init(void);

symbol_table_t symbol_tables[3];    /* global, file, & function level */
char current_fn_name[MAX_STR];      /* nom de la fonction courante    */
char current_file_name[MAX_STR];    /* nom du fichier courant         */
int  current_arglist_length;        /* longeur de la liste d'arguments
                                       de l'appel de fonction en préparation */
FILE *out;                          /* fichier de sortie              */

%}

%error-verbose
%locations

%union {
    int     num;
    char    str[MAX_STR];
    symbol *symp;
}

%token EQ NE LE GE OR AND
%token READINT WRITEINT WRITESTR
%token WHILE IF ELSE RETURN EXIT INT VOID STATIC
%token <str> IDF STR
%token <num> NUM

%type <num> expr arg_list arg_seq
%type <symp> param type type_list param_list

%left ','
%nonassoc '='
%left OR
%left AND
%left EQ NE
%left '>' '<' LE GE
%left '+' '-'
%left '/' '*' '%'
%left '!'
/* le moins unaire est plus prioritaire
    que tous les opérateurs binaires */
%left UNARY_MINUS

/* Précédences fictives pour éviter
    les shift/reduce warnings */
%nonassoc IF_SANS_ELSE
%nonassoc ELSE

%%

program:     
decl_list func_list   { emit(OPHALT); }
    ;
decl_list:
/* empty */ 
| decl_list decl
    ;
decl:
  INT IDF ';'
{ if ( DECSYM(VARIABLE, $2, @2,FUNC_OR_GLOB, INT_TYPE, 1) )
    emit(LVAR_OR_DATA ,$2, 1);}
| STATIC INT IDF ';'                
{ if ( DECSYM(VARIABLE, $3, @3, FUNC_OR_FILE, INT_TYPE, 1) )
    emit(LVAR_OR_DATA, $3, 1);}
| INT IDF '[' NUM ']' ';'           
{ if ( DECSYM(VARIABLE, $2, @2, FUNC_OR_GLOB, ARRAY_TYPE, $4) )
    emit(LVAR_OR_DATA, $2, $4);}
| STATIC INT IDF '[' NUM ']' ';'
{ if ( DECSYM(VARIABLE, $3, @3, FUNC_OR_FILE, ARRAY_TYPE, $5) )
    emit(LVAR_OR_DATA, $3, $5);}
| VOID IDF '(' VOID ')' ';'         { DECFN($2, @2, VOID_TYPE, NULL);   }
| INT IDF '(' VOID ')' ';'          { DECFN($2, @2, INT_TYPE, NULL);    }
| VOID IDF '(' type_list ')' ';'    { DECFN($2, @2, VOID_TYPE, $4);     }
| INT IDF '(' type_list ')' ';'     { DECFN($2, @2, INT_TYPE, $4);      }
    ;
type_list: 
  type                  { $$ = $1;                          }
| type ',' type_list    { $1->u.param.next = $3; $$ = $1;   }
    ;
type: 
  INT                   { $$ = AP("", @1, INT_TYPE, 1, NULL);   }
| INT '[' NUM ']'       { $$ = AP("", @1, ARRAY_TYPE, $3, NULL);}
    ;
func_list:    
  func
| func func_list
    ;
func:       
func_header '{' func_body '}' { END_FUNC(); }
    ;
func_header:
VOID IDF '(' VOID ')'        
{ DEFFN($2, @2, VOID_TYPE, NULL); START_FUNC($2); emit(OPFUNC, $2); } 
| INT IDF '(' VOID ')'      
{ DEFFN($2, @2, INT_TYPE, NULL); START_FUNC($2); emit(OPFUNC, $2); } 
| INT IDF '(' param_list ')' 
{ DEFFN($2, @2, INT_TYPE, $4); START_FUNC($2); emit(OPFUNC, $2); EMIT_PARAMS($4); } 
| VOID IDF '(' param_list ')'
{ DEFFN($2, @2, VOID_TYPE, $4); START_FUNC($2); emit(OPFUNC, $2); EMIT_PARAMS($4); } 
/* TODO: deal with args */
    ;
param_list:
  param                     { $$ = $1;                          }
| param ',' param_list      { $1->u.param.next = $3; $$ = $1;   }
;
param:
  INT IDF                   { $$ = AP($2, @2, INT_TYPE, 1, NULL);   }
| INT IDF '[' NUM ']'       { $$ = AP($2, @2, ARRAY_TYPE, $4, NULL);}
    ;
func_body:
decl_list inst_list         { emit(OPRET); }
    ;
inst_list:
/* empty */
| inst_list inst
    ;
inst:       
  IDF '=' expr ';'
    { int l = level($1, INT_TYPE, @1);
      if (l != -1) emit(ASSGNV_L_OR_G, $1, l); else nberrs++;  }
| IDF '[' expr ']' '=' expr ';'
    { int l = level($1, ARRAY_TYPE, @1);
      if (l != -1) emit(ASSGNAE_L_OR_G, $1, l); else nberrs++;  }
| EXIT '(' NUM ')' ';'          { emit(OPPUSHC, $3); emit(OPEXIT);          }
| RETURN expr ';'               { emit(OPRETE);         }
| RETURN ';'                    { emit(OPRET);          }
| IF '(' expr ')' after_if_condition inst ELSE
    { char *end_label = new_label();
      char *else_label = pop_label();
      emit(OPJUMP, end_label);
      emit(OPLABEL, else_label);    /* start of else        */
      push_label(end_label);        /* end of the whole if  */
    }
inst
    { emit(OPLABEL, pop_label()); }
| IF '(' expr ')' after_if_condition inst          %prec IF_SANS_ELSE
    { emit(OPLABEL, pop_label()); }
| WHILE
    {
      char *start_label = new_label();
      char *end_label = new_label();
      push_label(end_label);
      push_label(start_label); 
      emit(OPLABEL, start_label);
    }
'(' expr ')'
    { 
      char *start_label = pop_label();
      char *end_label = pop_label();
      emit(OPJFALSE, end_label);
      push_label(end_label);
      push_label(start_label);
    }
inst
    { emit(OPJUMP, pop_label());      /* start label */
      emit(OPLABEL, pop_label());   /* end label */
    }
| WRITESTR '(' STR ')' ';'      { emit(OPWRITESTR, $3); }
| WRITEINT '(' arg_list ')' ';' { emit(OPWRITEINT, $3); }
| READINT IDF ';'
    { int l = level($2, INT_TYPE, @2);
      if (l != -1) emit(READ_L_OR_G, $2, l); else nberrs++;  }
| '{' inst_list '}'             { }
| expr ';'                      { /* BUG: il faut un moyen pour détecter
si ce n'est pas un appel à une procédure avant de s'aventure à faire un
pop emit(OPPOP);         */ }
| ';' /* TODO: empty instructions: why not ? */     { }
    ;
/* juste après un 'if (cond)': générer et empiler
 * le label pour cond == FALSE
 */
after_if_condition:
/* empty */ { char *false_label = new_label();
              emit(OPJFALSE, false_label);
              push_label(false_label);          }
    ;
expr:       
  expr '+' expr     { emit(OPADD);  }
| expr '-' expr     { emit(OPSUB);  }
| expr '*' expr     { emit(OPMULT); }
| expr '/' expr     { emit(OPDIV);  }
| expr '%' expr     { emit(OPMOD);  }
| expr '>' expr     { emit(OPGT);   }
| expr '<' expr     { emit(OPLT);   }
| expr EQ expr      { emit(OPEQ);   }
| expr NE expr      { emit(OPNE);   }
| expr GE expr      { emit(OPGE);   }
| expr LE expr      { emit(OPLE);   }
| expr OR
/* short-circuit for OR and AND requires a bit of magic:
    the second term should be evaluated only if the first
    one is not sufficient to derive the value of the whole
    expression.
    If you have written some assembly, this should be familiar :-)
    TODO: properly document this
    */
    { char *label_true = new_label();
      char *label_end = new_label();

      push_label(label_end);
      push_label(label_true);
      emit(OPJTRUE, label_true);
    }
expr
    { char *label_true = pop_label();
      char *label_end = pop_label();
      emit(OPJTRUE, label_true);
      emit(OPPUSHC, FALSE);
      emit(OPJUMP, label_end);
      emit(OPLABEL, label_true);
      emit(OPPUSHC, TRUE);
      emit(OPLABEL, label_end);
    }
| expr AND 
    { char *label_false = new_label();
      char *label_end = new_label();

      push_label(label_end);
      push_label(label_false);
      emit(OPJFALSE, label_false);
    }
expr
    { char *label_false = pop_label();
      char *label_end = pop_label();
      emit(OPJFALSE, label_false);
      emit(OPPUSHC, TRUE);
      emit(OPJUMP, label_end);
      emit(OPLABEL, label_false);
      emit(OPPUSHC, FALSE);
      emit(OPLABEL, label_end);
    }
| '-' expr   %prec UNARY_MINUS  { emit(OPNEG);  }
| '!' expr                      { emit(OPNOT);  }
| '(' expr ')'          { }
| IDF
{   int l;
    // is it a scalar ?
    if ( (l = level($1, INT_TYPE, @1)) != -1) {
        emit(PUSHV_L_OR_G, $1, l);
    } else if ( (l = level($1, ARRAY_TYPE, @1)) != -1 ) { // is it an array ?
        // TODO:raise an error if not in fn_call
        // TODO:an error is emitted by the first
        // call to level: find a way to suppress it.
        symbol *s = find_symbol(l, $1);
        assert( s != NULL );
        // put array size on stack
        emit(OPPUSHC, s->u.var.size);
        emit(PUSHA_L_OR_G, $1, l);
    }
}
| NUM                   { emit(OPPUSHC, $1); }
| IDF '(' arg_seq ')'
{ /*the args are on the expression stack */
    symbol *s = find_symbol(FILE_LEVEL, $1);
    if ( !s ) s = find_symbol(GLOBAL_LEVEL, $1);

    if ( !s ) {
        fprintf(stderr, " Undeclared function `%s' (line %d).\n", $1,
        @1.first_line);
        nberrs++;
    } else if ( s->type != FUNCTION ) {
        fprintf(stderr, " `%s' is not a function (line %d).\n", $1,
        @1.first_line);
        nberrs++;
    } else {
        // check arguments count and types.
        emit(OPCALL, $1);
    }
}
| IDF '[' expr ']'
    { int l = level($1, ARRAY_TYPE, @1);
      if (l != -1) emit(PUSHAE_L_OR_G, $1, l); else nberrs++; }
;
arg_seq:
/* empty */             { $$ = 0;       }
| arg_list              { $$ = $1;      }
    ;
arg_list:
expr                    { $$ = 1;       }
| expr ',' arg_list     { $$ = $3 + 1;  }
    ;
%%


void yyerror (char const *msg)
{
    fprintf(stderr, " %s\n", msg);
    fprintf(stderr, " Line %d (col %d) to line %d (col %d)\n",
            yylloc.first_line, yylloc.first_column, 
            yylloc.last_line, yylloc.last_column);
    nberrs++;
}

