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

/* prints with msg */
void clogGenericPrint( const char *filename, int lineno, const char *msg, const char *fmt, ...);
t_emitfunc clogSetEmitFunction(t_emitfunc newfunc);

/* prints and exits */
#define clogError(x...) do { clogGenericPrint( __FILE__, __LINE__, "CompLearn Error",x); exit(1); } while (0)
/* prints */
#define clogWarning(x...) do { clogGenericPrint(__FILE__, __LINE__, "CompLearn Warning",x); } while (0)

#endif
