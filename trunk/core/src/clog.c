#include <complearn/complearn.h>
#include <stdarg.h>
#include <stdio.h>

struct Clog { /* TODO: more */
  FILE *fp;
};

void clogGenericPrint(const char *msg, const char *fmt, const char *filename, int lineno, ...)
{
  va_list args;
  va_start( args, fmt );
  fprintf( stderr, "%s:%d  %s", filename, lineno, msg);
  vfprintf( stderr, fmt, args );
  fprintf( stderr, "\n" );
  va_end( args );
}

