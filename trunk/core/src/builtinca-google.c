#include <complearn/complearn.h>

#if CSOAP_RDY
#include <libcsoap/soap-client.h>
#include <zlib.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>

static const double refFactor = 2.0;


static void goog_clsetenv(struct CompAdaptor *ca);
static double goog_compfunc(struct CompAdaptor *ca, struct DataBlock src);
static void goog_freecompfunc(struct CompAdaptor *ca);
static char *goog_shortname(void);
static char *goog_longname(void);
static int goog_apiver(void);

/* bzip2 compression interface */

/** \brief The Google compression adaptor instance
 *  \struct CompInstance
 *
 *  This structure holds all the persistent configuration information
 *  necessary for the Google compressor to work.
 */
struct googCompInstance {
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
  struct googCompInstance *gci;
  ca = gcalloc(sizeof(*ca), 1);
  *ca = c;
  ca->cptr = gcalloc(sizeof(struct googCompInstance), 1);
  gci = (struct googCompInstance *) ca->cptr;

  goog_clsetenv(ca);

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
    struct StringStack *terms = newSingleSS(refWords[i]);
    acc += fetchSampleSimple(terms, gkey, daystr);
    freeSS(terms);
  }
  return acc;
}

static void goog_clsetenv(struct CompAdaptor *ca)
{
	struct googCompInstance *ci = (struct googCompInstance *) ca->cptr;
  struct CLDateTime *dt;
  char *args[1] = { NULL };
  char *userKey;
  struct EnvMap *em = loadDefaultEnvironment()->em;
  herror_t err;
  err = soap_client_init_args(0, args);
  dt = cldtNow();
  ci->daystr = gstrdup(cldt_daystring(dt));
  userKey = readValForEM(em, "GoogleKey");
  setKeyPrivateEM(em, "GoogleKey");
  if (userKey == NULL) {
    fprintf(stderr, "Error, cannot use google adaptor without GoogleKey property set\n");
    exit(1);
  }
  ci->gkey = gstrdup(userKey);
  ci->m = calculateM(ci->daystr, ci->gkey);
  cldtfree(dt);
}

static double goog_compfunc(struct CompAdaptor *ca, struct DataBlock src)
{
	struct googCompInstance *sci = (struct googCompInstance *) ca->cptr;
  double pagecount, compsize;
  char *cur;
  char *str = gcalloc(1,src.size+1);
  struct StringStack *terms;
  const double NOTFOUNDWEIGHT = 0.5;



  terms = newStringStack();

  memset(str, 0, src.size+1);
  memcpy(str,src.ptr, src.size);
//  printf("Str is <%s>\n", str);
  for (cur = strtok(str, "\r\n"); cur ; cur = strtok(NULL, "\r\n"))
    pushSS(terms, cur);
  pagecount = fetchSampleSimple(terms, sci->gkey, NULL);
  if (pagecount < 1) /* probably 0 */
    pagecount = NOTFOUNDWEIGHT;   /* a small amount given just for asking */
  compsize = -log(pagecount/sci->m)/log(2.0);
  
  freeSS(terms);
  gfreeandclear(str);
	return (double) compsize;
}

static void goog_freecompfunc(struct CompAdaptor *ca)
{
	struct googCompInstance *sci = (struct googCompInstance *) ca->cptr;
  gfreeandclear(sci->gkey);
  gfreeandclear(sci->daystr);

  gfreeandclear(sci);

	gfreeandclear(ca->cptr);
	gfreeandclear(ca);
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
