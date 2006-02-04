#include <complearn/complearn.h>
#include <stdarg.h>
#include <stdio.h>

struct Clog { /* NL */
  FILE *fp;
};

static t_emitclFunc emitFunc;

t_emitclFunc clogSetEmitFunction(t_emitclFunc newclFunc)
{
  t_emitclFunc oldFunc;
  oldFunc = emitFunc;
  emitFunc = newclFunc;
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
  else {
    if (outfp == stderr || outfp == stdout)
      fprintf(outfp, "\n");
    fprintf(outfp, buf);
    if (outfp == stderr || outfp == stdout)
      fprintf(outfp, "\n");
    fflush(outfp);
  }
}

