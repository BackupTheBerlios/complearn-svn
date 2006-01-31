#include <complearn/complearn.h>

#if HAVE_LIBCSOAP_SOAP_CLIENT_H
#include <libcsoap/soap-client.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

static const double refFactor = 2.0;


static void goog_clsetenv(struct CompAdaptor *ca);
static double goog_compfunc(struct CompAdaptor *ca, struct DataBlock *src);
static void goog_freecompfunc(struct CompAdaptor *ca);
static char *goog_shortname(void);
static char *goog_longname(void);
static int goog_apiver(void);

/** \brief The Google compression adaptor instance
 *
 *  \struct GoogleCompInstance
 *
 *  This structure holds all the persistent configuration information
 *  necessary for the Google compressor to work.
 */
struct GoogleCompInstance {
  char *gkey;    /*!< GoogleKey which must be obtained by Google */
  char *daystr;
  double m;
};

/** \brief Initializes a GOOGLE CompAdaptor instance
 *
 *  builtin_GOOG() allocates memory to a GOOGLE CompAdaptor instance. The
 *  GOOGLE CompAdaptor is required for NGD calculations.
 *
 *  For the GOOGLE CompAdaptor to work properly, a GoogleKey is required and
 *  must be set in the CompLearn configuration file. You may obtain a key from
 *  Google here http://www.google.com/apis/. If a GoogleKey is not set in the
 *  CompLearn configuration file, a error message will be printed to stdout.
 *  For details on how to create a configuration file, see
 *  http://www.complearn.org/config.html
 *
 *  \return pointer to newly initialized GOOGLE CompAdaptor instance
 */
struct CompAdaptor *builtin_GOOG(void)
{
	struct CompAdaptor c =
	{
    cptr: NULL,
//    se :  goog_clsetenv,
    cf:   goog_compfunc,
    fcf:  goog_freecompfunc,
    sn:   goog_shortname,
    ln:   goog_longname,
    apiv: goog_apiver,
  };
  struct CompAdaptor *ca;
  struct GoogleCompInstance *gci;
  ca = clCalloc(sizeof(*ca), 1);
  *ca = c;
  ca->cptr = clCalloc(sizeof(struct GoogleCompInstance), 1);
  gci = (struct GoogleCompInstance *) ca->cptr;

  goog_clsetenv(ca);

  compaInitParameters(ca);

  return ca;
}

static const char *refWords[] = {
  "ten",
  "most",
  "important",
  "words",
  "are",
  "just",
  "typical",
  "among",
  "everyday",
  "people",
  NULL
};

double calculateM(const char *daystr, const char *gkey)
{
  return refFactor * calculateMbase(daystr, gkey);
}

double calculateMbase(const char *daystr, const char *gkey)
{
  double acc = 0.0;
  int i;
  for (i = 0; refWords[i]; i += 1) {
    struct StringStack *terms = stringstackNewSingle(refWords[i]);
    acc += fetchSampleSimple(terms, gkey, daystr);
    stringstackFree(terms);
  }
  return acc;
}

static void goog_clsetenv(struct CompAdaptor *ca)
{
	struct GoogleCompInstance *ci = (struct GoogleCompInstance *) ca->cptr;
  struct CLDateTime *dt;
  char *args[1] = { NULL };
  char *userKey;
  struct EnvMap *em = loadDefaultEnvironment()->em;
  herror_t err;
  err = soap_client_init_args(0, args);
  dt = cldatetimeNow();
  ci->daystr = clStrdup(cldatetimeToDayString(dt));
  userKey = envmapValueForKey(em, "GoogleKey");
  envmapSetKeyPrivate(em, "GoogleKey");
  if (userKey == NULL) {
    clogError("Cannot use google adaptor without %s property set","GoogleKey");
  }
  ci->gkey = clStrdup(userKey);
  ci->m = calculateM(ci->daystr, ci->gkey);
  cldatetimeFree(dt);
}

static double goog_compfunc(struct CompAdaptor *ca, struct DataBlock *src)
{
	struct GoogleCompInstance *sci = (struct GoogleCompInstance *) ca->cptr;
  double pagecount, compsize;
  char *cur;
  char *str = clCalloc(1,datablockSize(src)+1);
  struct StringStack *terms;
  const double NOTFOUNDWEIGHT = 0.5;

  terms = stringstackNew();

  memset(str, 0, datablockSize(src)+1);
  memcpy(str,datablockData(src), datablockSize(src));
//  printf("Str is <%s>\n", str);
  for (cur = strtok(str, "\r\n"); cur ; cur = strtok(NULL, "\r\n"))
    stringstackPush(terms, cur);
  pagecount = fetchSampleSimple(terms, sci->gkey, NULL);
  if (pagecount < 1) /* probably 0 */
    pagecount = NOTFOUNDWEIGHT;   /* a small amount given just for asking */
  compsize = -log(pagecount/sci->m)/log(2.0);

  stringstackFree(terms);
  clFreeandclear(str);
	return (double) compsize;
}

static void goog_freecompfunc(struct CompAdaptor *ca)
{
	struct GoogleCompInstance *sci = (struct GoogleCompInstance *) ca->cptr;
  clFreeandclear(sci->gkey);
  clFreeandclear(sci->daystr);
  clFreeandclear(sci);
	clFreeandclear(ca);
}

static char *goog_shortname(void)
{
	return "google";
}

static char *goog_longname(void)
{
	return "Google web search engine, newline-delimitted unquoted terms";
}

static int goog_apiver(void)
{
	return APIVER_V1;
}

#else
#include <stdio.h>
struct CompAdaptor *builtin_GOOG(void)
{
  return NULL;
}
#endif
