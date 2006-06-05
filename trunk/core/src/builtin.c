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
#include <string.h>
#include <complearn/complearn.h>

struct CompAdaptor *clBuiltin_BZIP(void);
struct CompAdaptor *clBuiltin_GOOG(void);
struct CompAdaptor *clBuiltin_SC(const char *url, const char *urn, const char *method);
struct CompAdaptor *clBuiltin_ZLIB(void);
struct CompAdaptor *clBuiltin_blocksort(void);
struct CompAdaptor *clBuiltin_RealComp(const char *cmd);
struct CompAdaptor *builtin_VirtComp(const char *cmd);

struct TransformAdaptor *clBuiltin_UNGZ(void);

struct CompAdaptor *clCompaLoadGoogle(void)
{
#if HAVE_LIBCSOAP_SOAP_CLIENT_H
  return clBuiltin_GOOG();
#else
  return NULL;
#endif
}

struct CompAdaptor *clCompaLoadSOAP(const char *url, const char *urn)
{
#if HAVE_LIBCSOAP_SOAP_CLIENT_H
  return clBuiltin_SC(url, urn, "compclFunc");
#else
  return NULL;
#endif
}

struct CompAdaptor *clCompaLoadBzip2(void)
{
  return clBuiltin_BZIP();
}

struct CompAdaptor *clCompaLoadBlockSort(void)
{
  return clBuiltin_blocksort();
}


struct CompAdaptor *clCompaLoadZlib(void)
{
  return clBuiltin_ZLIB();
}

struct CompAdaptor *clCompaLoadReal(const char *cmd)
{
  struct CompAdaptor *result = clBuiltin_RealComp(cmd);
  return result;
}

struct CompAdaptor *clCompaLoadBuiltin(const char *name)
{
  struct CompAdaptor *result = NULL;
  if (strcmp(name, "zlib") == 0)
    result = clCompaLoadZlib();
  if (strncmp(name, "bz",2) == 0)
    result = clCompaLoadBzip2();
#if HAVE_LIBCSOAP_SOAP_CLIENT_H
  if (strcmp(name, "google") == 0)
    result = clCompaLoadGoogle();
#endif
  if (strcmp(name, "blocksort") == 0)
    result = clCompaLoadBlockSort();
  return result;
}

static void addIfPresent(struct StringStack *ss, const char *name)
{
  struct CompAdaptor *ca;
  ca = clCompaLoadBuiltin(name);
  if (ca) {
    clStringstackPush(ss,clCompaShortName(ca));
    clCompaFree(ca);
  }
}

struct StringStack *clCompaListBuiltin(void)
{
  struct StringStack *ss;
  ss = clStringstackNew();
  addIfPresent(ss, "zlib");
  addIfPresent(ss, "bzip");
  addIfPresent(ss, "google");
  addIfPresent(ss, "blocksort");
  return ss;
}

void clCompaPrintBuiltin(void)
{
  struct StringStack *sup;
  printf("Your supported compressors are:\n\n");
  sup = clCompaListBuiltin();
  clStringstackPrint(sup);
  clStringstackFree(sup);
}
