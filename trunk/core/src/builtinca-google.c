#include <complearn/complearn.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "newcomp.h"
#include "ncabz2.h"

static char *shortName = "google";
struct GoogleCompressionInstance {
  void *baseClass;
  char *gkey;    /*!< GoogleKey which must be obtained by Google */
  char *daystr;
  double m;
};

static const char *fshortNameCB(void)
{
  return shortName;
}

static const char *flongNameCB(void)
{
  return "Google web search engine";
}

static int fallocSizeCB(void)
{
  return sizeof(struct GoogleCompressionInstance);
}

static double fcompressCB(struct CompressionBase *cb, struct DataBlock *src)
{
  struct GoogleCompressionInstance *gci = (struct GoogleCompressionInstance *) cb;
  double pagecount, compsize;
  char *cur;
  char *str = clCalloc(1,clDatablockSize(src)+1);
  struct StringStack *terms;
  const double NOTFOUNDWEIGHT = 0.5;

  terms = clStringstackNew();

  memset(str, 0, clDatablockSize(src)+1);
  memcpy(str,clDatablockData(src), clDatablockSize(src));
//  printf("Str is <%s>\n", str);
  for (cur = strtok(str, "\r\n"); cur ; cur = strtok(NULL, "\r\n"))
    clStringstackPush(terms, cur);
  pagecount = clFetchSampleSimple(terms, gci->gkey, NULL);
  if (pagecount < 1) /* probably 0 */
    pagecount = NOTFOUNDWEIGHT;   /* a small amount given just for asking */
  compsize = -log(pagecount/gci->m)/log(2.0);

  clStringstackFree(terms);
  clFreeandclear(str);
  return (double) compsize;
}

static void ffreeCB(struct CompressionBase *cb)
{
  struct GoogleCompressionInstance *gci = (struct GoogleCompressionInstance *) cb;
  clFreeandclear(gci->gkey);
}

static int fspecificInitCB(struct CompressionBase *cb)
{
  return 0;
}

static int fisAutoEnabledCB(void)
{
  return 0;
}

#if HAVE_LIBCSOAP_SOAP_CLIENT_H
#include <libcsoap/soap-client.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

static int fprepareToCompressCB(struct CompressionBase *cb)
{
  const char *gkp = clEnvmapValueForKey(clGetParametersCB(cb), "GoogleKey");
  struct GoogleCompressionInstance *gci = (struct GoogleCompressionInstance *) cb;
  char *args[1] = { NULL };
  const char *propname = "GoogleKey";
  struct EnvMap *em = clGetParametersCB(cb);
  herror_t err;
  err = soap_client_init_args(0, args);
  if (gkp == NULL) {
    char buf[256];
    sprintf(buf, "Cannot use google adaptor without %s property set",propname);
    clSetLastErrorCB(cb, buf);
    return 1;
  }
  clEnvmapSetKeyPrivate(em, propname);
  gci->gkey = clStrdup(gkp);
  gci->m = clCalculateM(NULL, gci->gkey);
  //clogLog("M=%f\n", ci->m);
  return 0;
}
static int fisCompileProblemCB(void)
{
  if (cldbIsGDBMProblem()) {
    clSetLastStaticErrorCB(shortName, cldbReasonWhy());
    return 1;
  }
  return 0;
}
#else
static int fisCompileProblemCB(void)
{
  clSetLastStaticErrorCB(shortName, "No CSOAP available; need <libcsoap/soap-client.h>");
  return 1;
}
static int fprepareToCompressCB(struct CompressionBase *cb)
{
  exit(1);
}
#endif

static const double refFactor = 2.0;

/*
 *  Get a Google API Key here:
 *    http://www.google.com/apis/
 *  If a GoogleKey is not set in the CompLearn configuration file, a error
 *  message will be printed to stdout.  For details on how to create a
 *  configuration file, see
 *    http://www.complearn.org/config.html
 *
 */
static const char *refWords[] = {
  "ten",
//  "important",
  "words",
//  "just",
  "typical",
  "among",
  "everyday",
//  "internet",
//  "people",
//  "city",
  "wolf",
  NULL
};

double clCalculateM(const char *daystr, const char *gkey)
{
  return refFactor * clCalculateMbase(daystr, gkey);
}

double clCalculateMbase(const char *daystr, const char *gkey)
{
  double acc = 0.0;
  int i;
  for (i = 0; refWords[i]; i += 1) {
    struct StringStack *terms = clStringstackNewSingle(refWords[i]);
    double val;
    val = clFetchSampleSimple(terms, gkey, daystr);
    acc += val;
    clStringstackFree(terms);
  }
  return acc;
}

static struct CompressionBaseAdaptor cba = {
  VIRTFUNCEXPORT(specificInitCB),
  VIRTFUNCEXPORT(prepareToCompressCB),
  VIRTFUNCEXPORT(isCompileProblemCB),
  VIRTFUNCEXPORT(compressCB),
  VIRTFUNCEXPORT(shortNameCB),
  VIRTFUNCEXPORT(longNameCB),
  VIRTFUNCEXPORT(isAutoEnabledCB),
  VIRTFUNCEXPORT(freeCB),
  VIRTFUNCEXPORT(allocSizeCB)
};

void initGoogle(void)
{
  clRegisterCB(&cba);
}

