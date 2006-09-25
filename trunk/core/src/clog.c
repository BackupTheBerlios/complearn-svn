/*
* Copyright (c) 2006 Rudi Cilibrasi, Rulers of the RHouse
* All rights reserved.     cilibrar@cilibrar.com
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the RHouse nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE RULERS AND CONTRIBUTORS "AS IS" AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE RULERS AND CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <complearn/complearn.h>
#include <stdarg.h>
#include <stdio.h>

struct Clog { /* NL */
  FILE *fp;
};

static t_emitclFunc emitFunc;

t_emitclFunc clLogSetEmitFunction(t_emitclFunc newclFunc)
{
  t_emitclFunc oldFunc;
  oldFunc = emitFunc;
  emitFunc = newclFunc;
  return oldFunc;
}

void clLogGenericPrintFILE( FILE *outfp, const char *filename, int lineno, const char *msg, const char *fmt, ...)
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

