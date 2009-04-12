/*
 * KEDJI Komlan Akpédjé <eric.kedji@gmail.com>
 * http://erickedji.wordpress.com/
 *
 * Définitions partagées par l'ensemble des codes du
 * PseudoC Compiler Collection
 */


#ifndef _SHARED_H_
#define _SHARED_H_

#include "gc.h"
#define MALLOC(var, type, size) do{                     \
    var = (type *) GC_MALLOC(size * sizeof(type));      \
    if ( var == NULL ) {                                \
        fprintf(stderr, "Error allocating memory.\n");  \
        exit(-1);                                       \
    }                                                   \
} while( 0 )

#include "uthash.h"
/* undefine the defaults */
#undef uthash_bkt_malloc
#undef uthash_bkt_free
#undef uthash_tbl_malloc
#undef uthash_tbl_free
/* re-define, specifying alternate functions */
#define uthash_bkt_malloc(sz) GC_MALLOC(sz)  /* for UT_hash_bucket */
#define uthash_bkt_free(ptr) GC_FREE(ptr)
#define uthash_tbl_malloc(sz) GC_MALLOC(sz)  /* for UT_hash_table  */
#define uthash_tbl_free(ptr) GC_FREE(ptr)

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/* maximum length of identifiers and strings in pseudoC */
#define MAX_STR 256

#endif /* _SHARED_H_ */

