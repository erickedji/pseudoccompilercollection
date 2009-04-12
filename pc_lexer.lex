/*
 * KEDJI Komlan Akpédjé <eric.kedji@gmail.com>
 * http://erickedji.wordpress.com/
 *
 * Scanner du Pseudo-C
 */

%{
#include <stdio.h>
#include <string.h>
#include "pc_symtable.h"
#include "pc_parser.tab.h"
#define YY_NO_INPUT

/* these macros help to write terse line and column tracing code */
#define FL yylloc.first_line
#define FC yylloc.first_column
#define LL yylloc.last_line
#define LC yylloc.last_column

/* keeps trac of the current index in strings */
int i;
%}

%option nounput
%option noyywrap

%x IN_COMMENT
%x IN_STRING

%%

"=="                    { FC = LC + 1; LC += 2; return EQ;  }
">="                    { FC = LC + 1; LC += 2; return GE;  }
"<="                    { FC = LC + 1; LC += 2; return LE;  }
"!="                    { FC = LC + 1; LC += 2; return NE;  }

"||"                    { FC = LC + 1; LC += 2; return OR;  }  
"&&"                    { FC = LC + 1; LC += 2; return AND; }

[()[\]{}=<>+\-*/%!,;]   { FC = LC + 1; LC ++;
                          return (unsigned char) *yytext;   }

"read_int"              { FC = LC + 1; LC += 8; return READINT; }
"write_int"             { FC = LC + 1; LC += 9; return WRITEINT;}
"write_string"          { FC = LC + 1; LC += 12;return WRITESTR;}
	/* shortcuts for the 3 keywords above  (not in the specs) */
"_ri"                   { FC = LC + 1; LC += 3; return READINT; }
"_wi"                   { FC = LC + 1; LC += 3; return WRITEINT;}
"_ws"                   { FC = LC + 1; LC += 3; return WRITESTR;}

"if"                    { FC = LC + 1; LC += 2; return IF;      }
"else"                  { FC = LC + 1; LC += 4; return ELSE;    }
"while"                 { FC = LC + 1; LC += 5; return WHILE;   }
"return"                { FC = LC + 1; LC += 6; return RETURN;  }
"exit"                  { FC = LC + 1; LC += 4; return EXIT;    }
"int"                   { FC = LC + 1; LC += 3; return INT;     }
"void"                  { FC = LC + 1; LC += 4; return VOID;    }
"static"                { FC = LC + 1; LC += 6; return STATIC;  }

[[:alpha:]]+            { FC = LC + 1; LC += strlen(yytext);
                          strcpy(yylval.str, yytext);
                          return IDF;                           }
[[:digit:]]+            { FC = LC + 1; LC += strlen(yytext);     
                          yylval.num = atoi(yytext);   
                          return NUM;                           }

[[:blank:]]+            { /* strip whitespace */
                          FC = LC + 1; LC += strlen(yytext);            }
"\n"                    { FC = LC = 0; FL++; LL++; yylineno++;          }
    /* use start conditions to parse comments */
<INITIAL>"/*"           { FC = LC + 1; LC +=2; BEGIN(IN_COMMENT);       }
<IN_COMMENT>{
"*/"                    { FC = LC + 1; LC +=2; BEGIN(INITIAL);          }
"\n"                    { FC = LC = 1; FL++; LL++; yylineno++;          }
"*"+[^*/\n]+            { /* strip non-closing stars */
                          FC = LC + 1; LC += strlen(yytext);            }
[^*\n]+                 { /* strip the comment */
                          FC = LC + 1; LC += strlen(yytext);            }
}
    /* use start conditions to parse string litterals */
<INITIAL>\"             { FC = LC + 1; LC ++; BEGIN(IN_STRING); i = 0;  }
<IN_STRING>{
\"                      { FC = LC + 1; LC ++; BEGIN(INITIAL);
                          yylval.str[i] = '\0';
                          if ( i >= MAX_STR ) {
                                fprintf(stderr, " pclexer: string too long.\n");
                                return 0;
                          }
                          return STR;         }
\\\"                    { LC += 2; strcpy(yylval.str + i, "\\\""); i+= 2; }
\n                      { fprintf(stderr,
                        " lexer: invalid string (strings should not span lines).\n");
                        return 0;}
[^"\n]+                 { strcpy(yylval.str, yytext); i += strlen(yytext);}
}
.                       { fprintf(stderr, " pclexer: invalid character: '%c'.\n", *yytext);
                          return 0;}
%%
