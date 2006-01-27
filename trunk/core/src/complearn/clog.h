#ifndef __CLOG_H
#define __CLOG_H

/*! \file clog.h */

/* prints with msg */
void clogGenericPrint( const char *filename, int lineno, const char *msg, const char *fmt, ...);
t_emitfunc clogSetEmitFunction(t_emitfunc newfunc);

/* prints and exits */
#define clogError(x...) do { clogGenericPrint( __FILE__, __LINE__, "CompLearn Error",x); exit(1); } while (0)
/* prints */
#define clogWarning(x...) do { clogGenericPrint(__FILE__, __LINE__, "CompLearn Warning",x); } while (0)

#endif
