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
#include <string.h>
#include <unistd.h>

const char *getLikelyTmpPrefix()
{
  static const char *tmpFileDir;
  if (tmpFileDir == NULL) {
    tmpFileDir = getenv("TMPDIR");
    if (tmpFileDir == NULL)
      tmpFileDir = "/tmp";
  }
  return tmpFileDir;
}

static FILE *makeTmpCopyStdin(void)
{
  static char tmpfile[128];
  int fd;
  FILE *fp;
  if (tmpfile[0] == 0) {
    struct DataBlock *db = clFilePtrToDataBlockPtr(stdin);
    sprintf(tmpfile, "%s/clstdintmp-XXXXXX", getLikelyTmpPrefix());
    fd = mkstemp(tmpfile);
    close(fd);
    clDatablockWriteToFile(db, tmpfile);
    clDatablockFreePtr(db);
  }
  fp = fopen(tmpfile,"r");
  return fp;
}

FILE *clFopen(const char *fname, char *mode)
{
  if (strcmp(fname,"-") == 0) {
    return (mode[0] == 'r' ? makeTmpCopyStdin() : stdout);
  }
  else
    return fopen(fname, mode);
}

void clFclose(FILE *fp)
{
  if ( (fp == stdin) || (fp == stdout) )
    return;
  else {
    fclose(fp);
    return;
  }
}
