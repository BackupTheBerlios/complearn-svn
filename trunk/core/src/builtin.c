#include <stdio.h>
#include <complearn/complearn.h>

struct CompAdaptor *builtin_BZIP(void);
struct CompAdaptor *builtin_GOOG(void);
struct CompAdaptor *builtin_SC(const char *url, const char *urn, const char *method);
struct CompAdaptor *builtin_ZLIB(void);
struct CompAdaptor *builtin_blocksort(void);
struct CompAdaptor *builtin_RealComp(const char *cmd);
struct CompAdaptor *builtin_VirtComp(const char *cmd);


struct CompAdaptor *compaLoadGoogle(void)
{
#if HAVE_LIBCSOAP_SOAP_CLIENT_H
  return builtin_GOOG();
#else
  return NULL;
#endif
}

struct CompAdaptor *compaLoadSOAP(const char *url, const char *urn)
{
#if HAVE_LIBCSOAP_SOAP_CLIENT_H
  return builtin_SC(url, urn, "compfunc");
#else
  return NULL;
#endif
}

struct CompAdaptor *compaLoadBzip2(void)
{
  return builtin_BZIP();
}

struct CompAdaptor *compaLoadBlockSort(void)
{
  return builtin_blocksort();
}

#if HAVE_ZLIB_H
#include "builtinta-ungz.c"
#endif

struct CompAdaptor *compaLoadZlib(void)
{
  return builtin_ZLIB();
}

struct CompAdaptor *compaLoadReal(const char *cmd)
{
  struct CompAdaptor *result = builtin_RealComp(cmd);
  return result;
}

struct CompAdaptor *compaLoadBuiltin(const char *name)
{
  struct CompAdaptor *result = NULL;
  if (strcmp(name, "zlib") == 0)
    result = compaLoadZlib();
  if (strncmp(name, "bz",2) == 0)
    result = compaLoadBzip2();
#if HAVE_LIBCSOAP_SOAP_CLIENT_H
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
    stringstackPush(ss,compaShortName(ca));
    compaFree(ca);
  }
}

struct StringStack *compaListBuiltin(void)
{
  struct StringStack *ss;
  ss = stringstackNew();
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
  stringstackPrint(sup);
  stringstackFree(sup);
}
