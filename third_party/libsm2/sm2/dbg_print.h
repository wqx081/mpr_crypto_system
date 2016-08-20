#ifndef _DBG_PRINT_H
#define _DBG_PRINT_H
#include <stdio.h>
#include <string.h>
#include "mpz_t.h"
#include "mpzlib.h"
//#define _MPZ_DBG

#ifdef _MPZ_DBG
#define DBG_DEF(str)       debug_print_default(str)
#define DBG_INF(str, inf)  debug_print(str, inf)
#else
#define DBG_DEF(str)
#define DBG_INF(str, inf)
#endif

#ifdef __cplusplus
extern "C" {
#endif

void debug_print_default(const char *);
void debug_print(const char *, void *);
#ifdef __cplusplus
}
#endif

#endif