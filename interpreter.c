/*
 * KEDJI Komlan Akpédjé <eric.kedji@gmail.com>
 * http://erickedji.wordpress.com/
 *
 * Run the the byte-code generated until program end.
 */

#include "pci_defs.h"
#include "ic_generator.h"

static int  expr_stack[MAX_EXPR_STACK];
static int  call_stack[MAX_CALL_STACK];
//TODO: tell the compiler to put them into registers
static int expr_stackp;    /* top of expression stack      */
static int call_stackp;    /* top of call stack            */
static int framep;         /* current stack frame pointer  */


/* pop from expression stack */
static int
epop() {
    if ( expr_stackp <= 0 ) {
        eprint("Expression stack underflow, exiting.\n");
        exit(-1);
    }
    return expr_stack[--expr_stackp];
}

/* push onto expression stack */
static void 
epush(int n) {
    if ( expr_stackp >= MAX_EXPR_STACK ) {
        eprint("Expression stack overflow, exiting.\n");
        exit(-1);
    }
    expr_stack[expr_stackp++] = n;
}

/* pop from call stack */
static int 
cpop() {
    if ( call_stackp <= 0 ) {
        eprint("Call stack underflow, exiting.\n");
        exit(-1);
    }
    return call_stack[--call_stackp];
}

/* push onto call stack */
static void 
cpush(int n) {
    if ( call_stackp >= MAX_CALL_STACK ) {
        eprint("Call stack overflow, exiting.\n");
        exit(-1);
    }
    call_stack[call_stackp++] = n;
}

#ifndef NDEBUG
/* print a stack dump to stdout */
static void
stack_dump(int *stack, int p) {
    while ( p > 0) {
        p --;
        printf(" [%d]: %d  \t(hex:%x)\n", p, stack[p], stack[p]);
    }
}
#endif



/*
 * Runnable r = new Runnable();
 * r.run(); :-)
 * Yegge: "Execution in the kingdom of nouns"
 * (worth reading, really)
 * Seriously, this function executes the generated byte code
 */
void
run(int instp) {
    char op;
    int loc;
    int n, a, b, tmp;

    expr_stackp = call_stackp = framep = 0;

    for ( EVER ) {
        op = code[instp++];
#ifndef NDEBUG
        printf(" -> call stack dump:\n");
        stack_dump(call_stack, call_stackp);
        printf(" -> expr stack dump:\n");
        stack_dump(expr_stack, expr_stackp);
        printf(" next instruction: %d (%s)\tat %d\t(hex:%x)\n",
                op, opnames[(int) op], instp - 1, instp - 1);
        printf(" press enter to continue");
        (void) getchar();
#endif
        switch ( op ) {
            case OPADD:
                a = epop(); b = epop(); epush(b + a);   break;
            case OPSUB:
                a = epop(); b = epop(); epush(b - a);   break;
            case OPMULT:
                a = epop(); b = epop(); epush(b * a);   break;
            case OPDIV:
                a = epop(); b = epop(); epush(b / a);   break;
            case OPMOD:
                a = epop(); b = epop(); epush(b % a);   break;
            case OPEQ:
                a = epop(); b = epop(); epush(b == a);  break;
            case OPNE:
                a = epop(); b = epop(); epush(b != a);  break;
            case OPLT:
                a = epop(); b = epop(); epush(b < a);   break;
            case OPLE:
                a = epop(); b = epop(); epush(b <= a);  break;
            case OPGT:
                a = epop(); b = epop(); epush(b > a);   break;
            case OPGE:
                a = epop(); b = epop(); epush(b >= a);  break;
            case OPNOT:
                tmp = epop(); epush( !tmp );    break;
            case OPNEG:
                tmp = epop(); epush( -tmp );    break;
            case OPPOP:
                (void) epop();                  break;
            case OPEXIT:
                exit(epop());                   break;
            case OPHALT:
                return;

            case OPLVAR:
                n = *((int *) (code + instp)); /* size */
                assert ( n % sizeof(int) == 0);
                call_stackp += n / sizeof(int);
                instp += sizeof(int);
                break;

            case OPPARAM:
                /* n is the total size of all the parameters in bytes
                 * move the parameters from the expr stack to the call
                 * stack
                 */
                assert( call_stackp == framep );
                n = *((int *) (code + instp));
                //printf("params size = %d, expr_stackp=%d\n", n, expr_stackp);
                assert( n % sizeof(int) == 0);
                memcpy(call_stack + framep,
                        expr_stack + expr_stackp - (n / sizeof(int)),
                        n);
                call_stackp += n / sizeof(int);
                while( n ) {
                    epop();
                    n -= sizeof(int);
                }
                instp += sizeof(int);
                break;
            //TODO: why have two return ops? cause i am a poor designer
            case OPRETE: 
            case OPRET:
                call_stackp = framep;
                instp = cpop();
                framep = cpop();
                break;

            case OPCALL:
                loc = *((int *) (code + instp));
                cpush(framep);
                cpush(instp + sizeof(int));
                instp = loc;
                framep = call_stackp;
                break;

            case OPJFALSE:
                tmp = epop();
                loc = *((int *) (code + instp));
                if ( !tmp ) instp = loc; else instp += sizeof(int);
                break;

            case OPJTRUE:
                tmp = epop();
                loc = *((int *) (code + instp));
                if ( tmp ) instp = loc; else instp += sizeof(int);
                break;
                
            case OPJUMP:
                instp = *((int *) (code + instp));  break;
                

            case OPPUSHC:
                tmp = *((int *) (code + instp));
                epush(tmp);
                instp += sizeof(int);
                break;

            case OPPUSHGV:
                loc = *((int *) (code + instp));
                epush(*((int *) (code + loc)));
                instp += sizeof(int);
                break;

            case OPPUSHLV:
                loc = *((int *) (code + instp));
                assert( loc % sizeof(int) == 0 );
                epush(call_stack[framep + (loc/sizeof(int))]);
                instp += sizeof(int); 
                break;

            case OPPUSHGA:
                n = epop(); /* size of array */
                loc = *((int *) (code + instp));
                for ( tmp = 0; tmp < n; tmp ++ ) {
                    epush(*((int *) (code + loc + tmp*sizeof(int) )));
                }
                instp += sizeof(int); 
                break;

            case OPPUSHGAE:
                n = epop(); /* array index */
                loc = *((int *) (code + instp));
                epush(*((int *) (code + loc + n*sizeof(int))));
                instp += sizeof(int); 
                break;

            case OPPUSHLA:
                n = epop(); /* size of array */
                loc = *((int *) (code + instp));
                for ( tmp = 0; tmp < n; tmp ++ ) {
                    epush(call_stack[framep + (loc/sizeof(int)) + tmp]);
                }
                instp += sizeof(int); 
                break;

            case OPPUSHLAE:
                n = epop(); /* array index */
                loc = *((int *) (code + instp));
                assert( loc % sizeof(int) == 0 );
                epush(call_stack[framep + (loc/sizeof(int)) + n]);
                instp += sizeof(int);
                break;

            case OPREADG:
                loc = *((int *) (code + instp));
                scanf("%d", (int *) (code + loc));
                instp += sizeof(int);
                break;

            case OPREADL:
                loc = *((int *) (code + instp));
                assert( loc % sizeof(int) == 0 );
                scanf("%d", call_stack + framep + (loc/sizeof(int)));
                instp += sizeof(int);
                break;

            case OPASSGNGV:
                loc = *((int *) (code + instp));
                tmp = epop();
                memcpy(code + loc, &tmp, sizeof(int));
                instp += sizeof(int);
                break;
                
            case OPASSGNLV:
                loc = *((int *) (code + instp));
                assert( loc % sizeof(int) == 0 );
                tmp = epop();
                memcpy(call_stack + framep + (loc/sizeof(int)),
                        &tmp, sizeof(int));
                instp += sizeof(int);
                break;
                
            case OPASSGNGAE:
                loc = *((int *) (code + instp));
                tmp = epop();
                n = epop(); /* array index */
                memcpy(code + loc + n*sizeof(int),
                        &tmp, sizeof(int));
                instp += sizeof(int);
                break;

            case OPASSGNLAE:
                loc = *((int *) (code + instp));
                assert( loc % sizeof(int) == 0 );
                tmp = epop();
                n = epop(); /* array index */
                memcpy(call_stack + framep + (loc/sizeof(int)) + n,
                        &tmp, sizeof(int));
                instp += sizeof(int);
                break;

            case OPWRITESTR:
                loc = *((int *) (code + instp));
                printf("%s", code + loc);
                instp += sizeof(int);
                break;
            case OPWRITEINT:
                n = *((int *) (code + instp)); /* number of ints to print */
                for ( tmp = n; tmp > 0; tmp -- ) {
                    printf("%d ", expr_stack[expr_stackp - tmp]);
                }
                while ( n --> 0 ) {
                    (void) epop();
                }
                instp += sizeof(int);
                break;

            default:
                eprint("Unimplemented byte-code:%d\n", op);
        }
    }
}
