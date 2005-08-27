#include <stdio.h>
#include <complearn/complearn.h>

#if 0

#include "builtinca-realcomp.c"
#include "builtinca-virtcomp.c"

#if CSOAP_RDY
#include "builtinca-soap.c"
#include "builtinca-google.c"
#endif

#if BZIP2_RDY
#include "builtinca-bzlib.c"
#include "builtinta-unbzip.c"
#endif

#endif

struct CompAdaptor *builtin_BZIP(void);
struct CompAdaptor *builtin_GOOG(void);
struct CompAdaptor *builtin_SC(const char *url, const char *urn, const char *method);
struct CompAdaptor *builtin_ZLIB(void);
struct CompAdaptor *builtin_blocksort(void);
struct CompAdaptor *builtin_RealComp(const char *cmd);
struct CompAdaptor *builtin_VirtComp(const char *cmd);


struct CompAdaptor *compaLoadGoogle(void)
{
#if CSOAP_RDY
  return builtin_GOOG();
#else
  return NULL;
#endif
}

struct CompAdaptor *compaLoadSOAP(const char *url, const char *urn)
{
#if CSOAP_RDY
  return builtin_SC(url, urn, "compfunc");
#else
  return NULL;
#endif
}

struct CompAdaptor *compaLoadBzip2(void)
{
#if BZIP2_RDY
  return builtin_BZIP();
#else
  return NULL;
#endif
}

struct CompAdaptor *compaLoadBlockSort(void)
{
  return builtin_blocksort();
}


#if ZLIB_RDY
#include "builtinca-zlib.c"
#include "builtinta-unzlib.c"
#include "builtinta-ungz.c"
#endif

struct CompAdaptor *compaLoadZlib(void)
{
#if ZLIB_RDY
  return builtin_ZLIB();
#else
  return NULL;
#endif
}

struct CompAdaptor *compaLoadReal(const char *cmd)
{
  struct CompAdaptor *result = builtin_RealComp(cmd);
  return result;
}

struct CompAdaptor *compaLoadBuiltin(const char *name)
{
  struct CompAdaptor *result = NULL;
#if ZLIB_RDY
  if (strcmp(name, "zlib") == 0)
    result = compaLoadZlib();
#endif
#if BZIP2_RDY
  if (strcmp(name, "bzip") == 0)
    result = compaLoadBzip2();
#endif
#if CSOAP_RDY
  if (strcmp(name, "google") == 0)
    result = compaLoadGoogle();
#endif
  if (strcmp(name, "blocksort") == 0)
    result = compaLoadBlockSort();
  return result;
}

static void addIfPresent(struct StringStack *ss, const char *name)
{
  struct CompAdaptor *ca;
  ca = compaLoadBuiltin(name);
  if (ca) {
    pushSS(ss,compaShortName(ca));
    compaFree(ca);
  }
}

struct StringStack *compaListBuiltin(void)
{
  struct StringStack *ss;
  ss = newStringStack();
  addIfPresent(ss, "zlib");
  addIfPresent(ss, "bzip");
  addIfPresent(ss, "google");
  addIfPresent(ss, "blocksort");
  return ss;
}

void compaPrintBuiltin(void)
{
  struct StringStack *sup;
  printf("Your supported compressors are:\n\n");
  sup = compaListBuiltin();
  printSS(sup);
  freeSS(sup);
}
