#ifndef __CLOG_H
#define __CLOG_H

#include <complearn/quartet.h>

/*! \file clog.h */

/* prints with msg */
void clogGenericPrint(const char *msg, const char *fmt, const char *filename, int lineno, ...);

/* prints and exits */
#define clogError(x, args...) do { clogGenericPrint("CompLearn Error", x, __FILE__, __LINE__, args); exit(1); } while (0)
/* prints */
#define clogWarning(x, args...) do { clogGenericPrint("CompLearn Warning", x, __FILE__, __LINE__, args); } while (0)

#endif
