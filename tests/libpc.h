/*
 * KEDJI Komlan Akpédjé <eric.kedji@gmail.com>
 * http://erickedji.wordpress.com/
 */


#ifndef _LIBPC_H_
#define _LIBPC_H_

#ifdef _C_PROGRAM_
    void write_string(const char *str);
    #define _ws write_string

    void _libpc_read_int(int *n);
    #define _cri _libpc_read_int

    void _libpc_write_int(int nb_args, ...);
    #define _cwi _libpc_write_int

    #define WI(nb_args, ...) _libpc_write_int(nb_args, __VA_ARGS__)
#else
    #define WI(nb_args, ...) write_int(__VA_ARGS__)
#endif /* _C_PROGRAM_ */


#endif /* _LIBPC_H_ */

