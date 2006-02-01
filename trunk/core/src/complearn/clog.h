#ifndef __CLOG_H
#define __CLOG_H

/*! \file clog.h */

/** \brief Unique and authentic type of wooden shoe
 * \struct Clog
 *
 * This structure maintain CompLearn log information.  It may direct
 * output to a file, to stderr, or through a user-defined error
 * function depending on how it is used.
 */
struct Clog;

#include <stdio.h>

/* prints with msg */
void clogGenericPrintFILE( FILE *outfp, const char *filename, int lineno, const char *msg, const char *fmt, ...);
void clogGenericPrint( const char *filename, int lineno, const char *msg, const char *fmt, ...);
t_emitfunc clogSetEmitFunction(t_emitfunc newfunc);
//void clogGenericLog( const char *filename, int lineno, const char *msg, const char *fmt, ...);

/* prints and exits */
#define clogGenericPrint(x...) do { clogGenericPrintFILE( stderr, __FILE__, __LINE__, "CompLearn Error",x); exit(1); } while (0)
#define clogError(x...) do { clogGenericPrint( __FILE__, __LINE__, "CompLearn Error",x); exit(1); } while (0)
/* prints */
#define clogWarning(x...) do { clogGenericPrint(__FILE__, __LINE__, "CompLearn Warning",x); } while (0)
#define clogLog(x...) do { clogGenericPrintFILE(stdout, __FILE__, __LINE__, "complearnlog:",x); } while (0)

#endif
