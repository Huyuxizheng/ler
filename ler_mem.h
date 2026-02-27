#ifndef LER_MEM_H
#define LER_MEM_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define ler_malloc(size)    malloc(size)
#define ler_free(ptr)       free(ptr)
#define ler_strdup(str)     strdup(str)

#define ler_log( ...)       printf(__VA_ARGS__)
#define ler_err( ...)       printf("error: "__VA_ARGS__)

#endif
