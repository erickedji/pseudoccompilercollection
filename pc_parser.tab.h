/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     EQ = 258,
     NE = 259,
     LE = 260,
     GE = 261,
     OR = 262,
     AND = 263,
     READINT = 264,
     WRITEINT = 265,
     WRITESTR = 266,
     WHILE = 267,
     IF = 268,
     ELSE = 269,
     RETURN = 270,
     EXIT = 271,
     INT = 272,
     VOID = 273,
     STATIC = 274,
     IDF = 275,
     STR = 276,
     NUM = 277,
     UNARY_MINUS = 278,
     IF_SANS_ELSE = 279
   };
#endif
/* Tokens.  */
#define EQ 258
#define NE 259
#define LE 260
#define GE 261
#define OR 262
#define AND 263
#define READINT 264
#define WRITEINT 265
#define WRITESTR 266
#define WHILE 267
#define IF 268
#define ELSE 269
#define RETURN 270
#define EXIT 271
#define INT 272
#define VOID 273
#define STATIC 274
#define IDF 275
#define STR 276
#define NUM 277
#define UNARY_MINUS 278
#define IF_SANS_ELSE 279




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 57 "pc_parser.y"
{
    int     num;
    char    str[MAX_STR];
    symbol *symp;
}
/* Line 1489 of yacc.c.  */
#line 103 "pc_parser.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif

extern YYLTYPE yylloc;
