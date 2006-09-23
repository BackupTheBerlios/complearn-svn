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

#if HAVE_DLFCN_H

#include <dlfcn.h>

static void *dl_musthavesymbol(void *dlhandle, const char *str)
{
	void *result;
	result = dlsym(dlhandle, str);
	if (result == NULL) {
		clogError( "Error, symbol %s not defined for "
                    "dynamic library compressor\n", str);
		exit(1);
	}
	return result;
}

#if 0
void doSymbol(void *dlhandle, const char *modName, const char *symName, void **loc)
{
	static char symbuf[1024];
	sprintf(symbuf, "%s_%s", modName, symName);
	*loc = dlsym(dlhandle, symbuf);
  printf("Loaded %p for symbol %s\n", *loc, symbuf);
}
#endif
/*
void clCompaLoadDynamicLibModuleNamed(const char *moduleName)
{
  void *d = dlopen(0, RTLD_LAZY);
  void *f = &d;
  doSymbol(d, "art", "freeCB", &f);
  printf("F is now %p\n", f);
  exit(0);
}
*/
#if 0
  int (*specificInitCB)(struct CompressionBase *cb);
  void (*freeCB)(struct CompressionBase *cb);
  int (*getAPIVersionCB)(void);
  const char *(*longNameCB)(void);

  int (*isDisabledCB)(void);

    // Should read params and use them.   This will be called only once
    // prior to compression.  This should do any compressor-specific
    // initialization at this point and return 0 to indicate success or
    // nonzero to indicate an error.  Don't forget to clSetLastErrorCB().
  int (*prepareToCompressCB)(struct CompressionBase *cb);

  int (*getWindowSizeCB)(void); // Return 0 for "infinite window"
  int (*doesRoundWholeBytesCB)(void);

  int (*isCompileProblemCB)(void); // Returns != 0 iff compile-time problem
       // is detected that will prevent this compressor from working.
       // If != 0 is to be returned, a call to setLastErrorCB should first
       // occur to explain the reason for the error.
  int (*isRuntimeProblemCB)(void); // Returns != 0 iff runtime problem
  const char *(*shortNameCB)(void);
  int (*allocSizeCB)(void);

  const char *(*toStringCB)(struct CompressionBase *cb);

  const char *(*paramStringCB)(struct CompressionBase *cb);  // Should show params somehow

  /* Returns result in bits */
  double (*compressCB)(struct CompressionBase *cb, struct DataBlock *datum);
  struct DataBlock *(*concatCB)(struct CompressionBase *cb, struct DataBlock *dat1, struct DataBlock *dat2);
  int (*isAutoEnabledCB)(void);
#endif

int clCompaLoadDynamicLib(const char *libraryname)
{
  void *dlhandle;
  t_cldlinitcl nca;
	dlhandle = dlopen(libraryname, RTLD_LAZY);
	if (dlhandle == NULL) {
    perror("dlopen");
		clogError( "Error opening dynamic library %s\n", libraryname);
		exit(1);
	}

  nca = (t_cldlinitcl) dl_musthavesymbol(dlhandle, FUNCNAMENCA);
  nca();
  return 0;
}

#else

int clCompaLoadDynamicLib(const char *libraryname)
{
  fprintf(stderr, "Cannot load %s\n", libraryname);
  clogError("No Dynamic Library Support compiled in, sorry.");
  return NULL;
}

#endif
