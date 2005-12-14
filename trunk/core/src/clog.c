#include <complearn/complearn.h>
#include <stdarg.h>
#include <stdio.h>

struct Clog { /* TODO: more */
  FILE *fp;
};

void clogGenericPrint( const char *filename, int lineno, const char *msg, const char *fmt, ...)
{
  va_list args;
  va_start( args, msg );
  fprintf( stderr, "%s:%d  %s\n", filename, lineno, msg);
  vfprintf( stderr, fmt, args );
  fprintf( stderr, "\n" );
  va_end( args );
}

