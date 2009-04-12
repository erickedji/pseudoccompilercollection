# Makefile for the PCC (PseudoC Compiler Collection)
#
# KEDJI Komlan Akpédjé <eric.kedji@gmail.com>
# http://erickedji.wordpress.com/
#
# TODO: include commands to compile gc

#Commentez la ligne avec -NDEBUG et décommentez la suivante
# pour activer le tracage (utile lors du debuggage)
#CFLAGS= -Wall -Igc/include -Lgc/lib -DNDEBUG
CFLAGS = -Wall -g -Igc/include -Lgc/lib

# sources files that belong to the interpreter
INTERPRETER_SOURCES= pseudoci.c ic_parser.c bc_generator.c interpreter.c pci_defs.c
INTERPRETER_HEADERS= pci_defs.h uthash.h
# sources files that belong to the compiler
COMPILER_SOURCES= pseudocc.c pc_lexer.c pc_parser.tab.c pc_symtable.c ic_generator.c
COMPILER_HEADERS= ic_generator.h pcc_defs.h pc_parser.tab.h pc_symtable.h
EXECUTABLES= pseudocc pseudocl pseudoci opname

FILES= $(INTERPRETER_SOURCES) $(COMPILER_SOURCES) 	\
       $(INTERPRETER_HEADERS) $(COMPILER_HEADERS) 	\
       shared.h pc_parser.y pc_lexer.lex pseudocl.c 	\
       opname.c Makefile rapport.tex generator.scm generator igen \
       $(EXECUTABLES)

all: $(EXECUTABLES) rapport.pdf

rapport.pdf: rapport.tex
	pdflatex rapport.tex 1>/dev/null 2>/dev/null
	pdflatex rapport.tex 1>/dev/null 2>/dev/null
	rm rapport.aux  rapport.log  rapport.out rapport.toc

pseudoci: $(INTERPRETER_SOURCES) $(INTERPRETER_HEADERS) shared.h
	gcc $(CFLAGS) -o pseudoci $(INTERPRETER_SOURCES) -lgc

pseudocc: $(COMPILER_SOURCES) $(COMPILER_HEADERS) shared.h
	gcc $(CFLAGS) -o pseudocc $(COMPILER_SOURCES) -lgc

pseudocl: pc_lexer.c pseudocl.c pc_parser.tab.h shared.h
	gcc $(CFLAGS) -o pseudocl pseudocl.c pc_lexer.c -lgc

pc_parser.tab.c pc_parser.tab.h: pc_parser.y
	bison --defines pc_parser.y

pc_lexer.c: pc_parser.tab.h pc_lexer.lex
	flex -o pc_lexer.c pc_lexer.lex

opname: opname.c ic_generator.h
	gcc $(CFLAGS) -o opname opname.c

clean:
	rm -rf pc_lexer.c pc_parser.tab.c $(EXECUTABLES) rapport.pdf

PseudoCCompilerCollection.zip: $(FILES)
	mkdir PseudoCCompilerCollection
	cp -r $(FILES) gc tests rapport.pdf PseudoCCompilerCollection 
	zip -r PseudoCCompilerCollection PseudoCCompilerCollection
	rm -rf PseudoCCompilerCollection

backup: PseudoCCompilerCollection.zip
	if [ -d /media/disk/works ]; then mv PseudoCCompilerCollection.zip /media/disk/works/; sync; fi

gen:
	generator | pseudocc
	cat PseudoCBinary.pcb

