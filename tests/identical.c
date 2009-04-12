/*
 * KEDJI Komlan Akpédjé <eric_kedji@yahoo.fr>
 *
 * Test if two files are identical (byte-to-byte comparison).
 *
 * I know, I could just use diff, but i want it to exit early
 * (right after a difference has been detected), and run fast
 * as hell.
 * ``The good thing about reinventing the wheel is that you
 * get a round one''.
 * -- Douglass Crockford.
 */

#include <stdio.h>
#include <stdlib.h>

FILE *open_file(const char *progname, const char *filename) {
    FILE *f = fopen(filename, "r");

    if ( !f ) {
        fprintf(stderr, " %s: %s: no such file.\n",
                progname, filename);
    }

    return f;
}

int main (int argc, char const* argv[])
{
    FILE *f1, *f2;
    int c1, c2;
    if ( argc != 3 ) {
        fprintf(stderr, " Tells if two files are identical.\n"
                " Exits with status 0 if they are identical, 1 otherwise.\n"
                " (Shell convention for exit status)\n"
                "\n\tUsage: %s file1 file2\n\n", argv[0]);
        exit(-1);
    }

    f1 = open_file(argv[0], argv[1]);
    f2 = open_file(argv[0], argv[2]);
    if ( !f1 || !f2 ) {
        exit(-1);
    }
    
    while ( 1 ) {
        c1 = fgetc(f1);
        c2 = fgetc(f2);

        if ( c1 != c2 )
            exit(1);

        if ( c1 == EOF ) // assert(c2 == EOF)
            exit(0);
    }

    // unreachable
    return -1;
}

