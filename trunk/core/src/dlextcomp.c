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
#include <stdlib.h>
#include <complearn/complearn.h>

#include <dirent.h>

#if HAVE_DLFCN_H

#include <dlfcn.h>

void clScanDirForModules(const char *dirname, struct GeneralConfig *cur)
{
  DIR *d;
  struct dirent *dp;
  d = opendir(dirname);
  if (d == NULL)
    return;
  while ((dp = readdir(d)) != NULL) {
    char *name = dp->d_name;
    char *goodname;
    int retval;
    if (name[0] == '.')
      continue;
    goodname = clJoinAsPath(dirname, name);
    if (cur && cur->fVerbose) {
      printf("Loading %s...", goodname);
      fflush(stdout);
    }
    retval = clCompaLoadDynamicLib(goodname);
    if (cur && cur->fVerbose) {
      if (retval)
        printf("failed\n");
      else
        printf("succeeded\n");
    }
    free(goodname);
  }
  closedir(d);
}

static void *dl_musthavesymbol(void *dlhandle, const char *str)
{
  void *result;
  result = dlsym(dlhandle, str);
  if (result == NULL) {
    clLogError( "Error, symbol %s not defined for "
                    "dynamic library compressor\n", str);
    exit(1);
  }
  return result;
}

int clCompaLoadDynamicLib(const char *libraryname)
{
  void *dlhandle;
  t_cldlinitcl nca;
  dlhandle = dlopen(libraryname, RTLD_LAZY | RTLD_LOCAL);
  if (dlhandle == NULL) {
    perror("dlopen");
    clLogError( "Error opening dynamic library %s\n", libraryname);
    exit(1);
  }

  nca = (t_cldlinitcl) dl_musthavesymbol(dlhandle, INITFUNCQUOTED);
  nca();
  return 0;
}

#else

int clCompaLoadDynamicLib(const char *libraryname)
{
  fprintf(stderr, "Cannot load %s\n", libraryname);
  clLogError("No Dynamic Library Support compiled in, sorry.");
  return NULL;
}

void clScanDirForModules(const char *dirname)
{
  clCompaLoadDynamicLib(dirname);
}

#endif
