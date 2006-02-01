#include <complearn/complearn.h>
#include <stdarg.h>
#include <stdio.h>

struct Clog { /* NL */
  FILE *fp;
};

static t_emitfunc emitFunc;

t_emitfunc clogSetEmitFunction(t_emitfunc newfunc)
{
  t_emitfunc oldFunc;
  oldFunc = emitFunc;
  emitFunc = newfunc;
  return oldFunc;
}

void clogGenericPrintFILE( FILE *outfp, const char *filename, int lineno, const char *msg, const char *fmt, ...)
{
  static char buf[16384], *ptr;
  va_list args;
  ptr = buf;
  va_start( args, fmt );
  ptr += sprintf( ptr, "%s:%d  %s\n", filename, lineno, msg);
  ptr += vsprintf( ptr, fmt, args );
//  sprintf( ptr, "\n" );
  va_end( args );
  if (emitFunc)
    emitFunc(buf);
  else
    fprintf(outfp, buf);
}

