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
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include <complearn/complearn.h>

#define COMPLEARNDIR ".complearn"
#define CONFIGNAME "config.yml"

struct StringStack *clGetDefaultFileList(void)
{
  struct StringStack *ss;
  char *homedir = clCalloc(1, 2048);
  char *homeenv;
  ss = clStringstackNew();
  clStringstackPush(ss, SYSTEMDATAFILE);
  homeenv = clGetHomeDir();
  if (homeenv) {
    sprintf(homedir, "%s/%s/%s", homeenv, COMPLEARNDIR, CONFIGNAME);
    clStringstackPush(ss, homedir);
  }
  free(homedir);
  return ss;
}

int clDoesFileExist(const char *fname)
{
  FILE *fp;
  fp = clFopen(fname, "rb");
  if (fp)
    clFclose(fp);
  return fp ? 1 : 0;
}

int clReadDefaultConfig(struct EnvMap *dest)
{
  struct StringStack *d = clGetDefaultFileList();
  assert(dest);
  assert(d);
  assert(clStringstackSize(d) > 0);
  assert(clStringstackSize(d) < 10);

  while (!clStringstackIsEmpty(d)) {
    char *str = clShiftSS(d);
    if (clDoesFileExist(str))
      clReadSpecificFile(dest, str);
    clFreeandclear(str);
  }
  clStringstackFree(d);
  return CL_OK;
}

/* returns 1 iff there is a valid string in dst, otherwise 0 */
static int filterVal(const char *src, char *dst)
{
  int len, rs = 0, ei;
  int lc;
  len = strlen(src);
  if (len < 1)
    return 0;
  while (isspace(src[rs]))
    rs += 1;
  ei = len-1;
  while (isspace(src[ei]))
    ei -= 1;
  lc = src[ei];
  if ((lc == '"' || lc == '\'') && lc == src[rs]) {
    ei -= 1;
    rs += 1;
  }
  if (ei < rs)
    return 0;
  memcpy(dst, src+rs, ei-rs+1);
  dst[ei-rs+1] = '\0';
  return 1;
}

#define DELIMS "=:\r\n"

void clHandleLine(struct EnvMap *dest, const char *uline)
{
  char *key = NULL, *val = NULL;
  char *kbuf, *vbuf;
  char *line = clStrdup(uline);
  key = strtok(line, DELIMS);
  if (key == NULL)
    return;
  kbuf = key;
  val = strtok(NULL, DELIMS);
  if (val == NULL)
    return;
  vbuf = val;
  if (filterVal(key, kbuf)) {
    if (filterVal(val, vbuf)) {
      clEnvmapSetKeyVal(dest, kbuf, vbuf);
    }
  }
  clFreeandclear(line);
}

/* Returns CL_OK or CL_ERRBADFILE if it cannot read the given file. */
int clReadSpecificFile(struct EnvMap *dest, const char *fname)
{
#define MAXLINESIZE 1024
  char linebuf[MAXLINESIZE];
  FILE *fp;
  fp = clFopen(fname, "rb");
  assert(fp);
  while (fgets(linebuf, MAXLINESIZE, fp)) {
    clHandleLine(dest, linebuf);
  }
  clFclose(fp);
  return CL_OK;
}
