#include <stdio.h>
#include <complearn/complearn.h>

struct CompAdaptor *clBuiltin_BZIP(void);
struct CompAdaptor *clBuiltin_GOOG(void);
struct CompAdaptor *clBuiltin_SC(const char *url, const char *urn, const char *method);
struct CompAdaptor *clBuiltin_ZLIB(void);
struct CompAdaptor *clBuiltin_blocksort(void);
struct CompAdaptor *clBuiltin_RealComp(const char *cmd);
struct CompAdaptor *builtin_VirtComp(const char *cmd);


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

#if HAVE_ZLIB_H
#include "builtinta-ungz.c"
#endif

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
