/*
 * KEDJI Komlan Akpédjé <eric.kedji@gmail.com>
 * http://erickedji.wordpress.com/
 *
 * Utility to output a readable representation of the
 * tokens of a PseudoC program
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "shared.h"
#include "pc_symtable.h"
#include "pc_parser.tab.h"

typedef struct {
    char name[20];
    int id;
    UT_hash_handle hh;         /* makes this structure hashable */
} tok;

YYSTYPE yylval;
YYLTYPE yylloc;
static tok *tokens = NULL; 
int yylex(void);


void
add_token(int id, char *name) {
    tok *t;
    int l;

    MALLOC(t, tok, 1);
    t->id = id;
    strcpy(t->name, name);

    l = strlen(name);
    while ( l < 19 ) {
        t->name[l++] = ' ';
    }
    t->name[l] = '\0';

    HASH_ADD_INT( tokens, id, t );
}

#define a add_token

int main (int argc, char const* argv[])
{
    int id;
    tok *token;

    a(',',      "COMMA");
    a(';',      "SEMI_COLON");
    a('=',      "ASSIGN");
    a('>',      "GREATER_THAN");
    a('<',      "LOWER_THAN");
    a('+',      "ADD");
    a('-',      "SUBTRACT");
    a('/',      "DIVIDE");
    a('*',      "MULTIPLY");
    a('%',      "MODULO");
    a('!',      "NOT");
    a('(',      "PAREN_OPEN");
    a(')',      "PAREN_CLOSE");
    a('{',      "BRACE_OPEN");
    a('}',      "BRACE_CLOSE");
    a('[',      "BRACKET_OPEN");
    a(']',      "BRACKET_CLOSE");
    a(OR,       "OR");
    a(AND,      "AND");
    a(EQ,       "EQUAL");
    a(NE,       "NOT_EQUAL");
    a(LE,       "LOWER_OR_EQUAL");
    a(GE,       "GREATER_OR_EQUAL");
    a(READINT,  "READ_INT");
    a(WRITEINT, "WRITE_INT");
    a(WRITESTR, "WRITE_STRING");
    a(WHILE,    "WHILE");
    a(IF,       "IF");
    a(ELSE,     "ELSE");
    a(RETURN,   "RETURN");
    a(EXIT,     "EXIT");
    a(INT,      "INT");
    a(VOID,     "VOID");
    a(STATIC,   "STATIC");
    a(IDF,      "IDENTIFIER");
    a(NUM,      "NUMBER");
    a(STR,      "STRING_LITERAL");

    int j = 0;
    while ( (id = yylex()) ) {
        if ( ++j > 3 ) {
            putchar('\n');
            j = 0;
        }

        HASH_FIND_INT( tokens, &id, token );

        if ( token )
            printf("%s ", token->name);
        else
            printf("UNKNOWN (%d) ", id);
    }

    putchar('\n');

    return 0;
}

