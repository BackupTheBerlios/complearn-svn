#include <complearn/googlecache.h>
#include <assert.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <complearn/complearn.h>

static const char *ROOTSYM = "0";

struct GoogleCache {
  struct GDBMHelper *samp;
};

/** \brief All saved data from a single sample of the Google indexing database
 *  \struct GCSample
 *
 *  Every search request to Google returns numerous results, most of which are
 *  thrown away.  The saved information is stored in a GCSample.
 *  It contains the pagecount, the date and time of the request,
 *  a link to the last (most recent before this one) sample for the same
 *  (normalized) search term, and a copy of the original (non-normalized)
 *  query string.
 *
 */
struct GCSample {
  double pagecount;  /*!< Number of pages returned */
  unsigned int when; /*!< The time of this request, in seconds since epoch */
  char daystring[32]; /*!< The "daystring", unique for each day (YYYYDDD) */
  char cknext[32];    /*!< A link to the previous request for the same
                           normalized search term. */
  char qorig[128];    /*!< A copy of the original search request, which is
                        perhaps not normalized. */
};

struct GoogleCache *newGC(void)
{
  struct GoogleCache *gc;
  gc = clCalloc(sizeof(struct GoogleCache), 1);
  gc->samp = cldbopen("gsamp");
  return gc;
}

void deleteSavedGC(void)
{
  cldbunlink("gsamp");
}

void freeGC(struct GoogleCache *gc)
{
  cldbclose(gc->samp);
  gc->samp = NULL;
  clFreeandclear(gc);
}

static const char *makeCacheKey(const char *daystr, struct StringStack *terms)
{
  static char ckbuf[3000];
  sprintf(ckbuf, "ck-%s/%s", daystr, makeQueryString(terms));
  return ckbuf;
}

struct DataBlock makeCacheVal(double pg, struct DataBlock lastdbval, const char *qorig)
{
  static struct GCSample d, lastd;
  struct DataBlock dat;
  struct CLDateTime *dt;
  dt = cldatetimeNow();

  memset(&d, 0, sizeof(d));
  memset(&lastd, 0, sizeof(lastd));
  d.pagecount = pg;
  d.when = cldatetimeToInt(dt);
  strcpy(d.daystring, cldatetimeToDayString(dt));
  memcpy(d.cknext, lastdbval.ptr, lastdbval.size);
  strcpy(d.qorig, qorig);

  dat.ptr = (unsigned char *) &d;
  dat.size = sizeof(d);
  return datablockClone(dat);
}

double convertCacheVal(struct DataBlock d)
{
  struct GCSample *r = (struct GCSample *) d.ptr;
  return r->pagecount;
}
double fetchSampleSimple(struct StringStack *terms, const char *gkey, const char *udaystr)
{
  static struct GoogleCache *gc;
  double result;
  char *daystr;
  struct CLDateTime *dt;
  if (udaystr == NULL) {
    dt = cldatetimeNow();
    daystr = clStrdup(cldatetimeToDayString(dt));
    cldatetimeFree(dt);
  }
  else
    daystr = clStrdup(udaystr);
  if (gc == NULL)
    gc = newGC();
  fetchsample(gc, daystr, terms, &result, gkey);
  clFreeandclear(daystr);
  return result;
}
/** \brief Fetches a sample from the local count database with the help of the
 * caching agent, GoogleCache.
 *
 * The fetchsample function is the primary interface to the GoogleCache
 * database, and is expected to be sufficient for most uses.  It supports a
 * GDBM-based query mechanism that allows efficient retrieval of count
 * information for any given day.  In order to fetch a sample, a user must
 * provide a daystring, or specially constructed character string that
 * encodes day information, as well as a searchquery that is comprised of
 * a StringStack of search terms.  The results are returned via the
 * out parameter val as well as a return status code, nominally CL_OK.
 * Another requirement of using fetchsample is a Google API account key.
 * This key looks like "Q/ZMtPCJYCKQrnxeq/pXJ/UNC4DEG1CZe" and is sent to
 * you after applying with Google online.
 *
 * The search terms supplied in the terms StringStack are expected to be
 * unquoted.  The fetchsample function will add double-quotes around these
 * strings as well as preceding each of them with + and joining them with
 * spaces.  This will form one large string query to send to Google from
 * the passed-in array.
 *
 * \param gc pointer to the GoogleCache object previously allocated
 * \param daystr string previously created with cldatetimeToDayString
 * \param terms pointer to a StringStack of terms that will be
 * \return a value 0 indicating a cache-miss, or 1 indicating a cache-success
 */
int fetchsample(struct GoogleCache *gc, const char *daystr, struct StringStack *terms, double *val, const char *gkey)
{
  struct StringStack *normed;
  const char *ckey;
  struct DataBlock *dbroot;
  struct DataBlock *dblastkey;
  struct DataBlock *db;
  struct DataBlock *dbckey;

  dbroot = stringToDataBlockPtr(ROOTSYM); /* FSA01 */
  normed = stringstackClone(terms);                    /* FSA02 */
  normalizeSearchTerms(normed);

  ckey = makeCacheKey(daystr, normed);

  dbckey = stringToDataBlockPtr(ckey);      /* FSA03 */
  db = cldbfetch(gc->samp, *dbckey);
  if (db) {
    assert(db->size == sizeof(struct GCSample));
    *val = convertCacheVal(*db);
    stringstackFree(normed);                       /* FSF02:1/2 */
    datablockFreePtr(db);
    datablockFreePtr(dbroot);                /* FSF01:1/2 */
    datablockFreePtr(dbckey);                /* FSF03:1/2 */
    return 1;
  } else {
    double pgc;
    struct DataBlock newentry, *lastdbval;
    pgc = getPageCount(terms, gkey);
    if (pgc < 0) {
      printf("Error contacting Google, aborting...\n");
      exit(1);
    }
    *val = pgc;
    dblastkey = stringToDataBlockPtr(makeCacheKey(ROOTSYM, normed));
    lastdbval = cldbfetch(gc->samp, *dblastkey);
    if (lastdbval == NULL)
      lastdbval = dbroot;
    newentry = makeCacheVal(pgc, *lastdbval, makeCacheKey(ROOTSYM, terms));
    cldbstore(gc->samp, *dbckey, newentry);
    cldbstore(gc->samp, *dblastkey, *dbckey);
    stringstackFree(normed);                          /* FSF02:2/2 */
    datablockFreePtr(dblastkey);
    datablockFree(newentry);
    datablockFreePtr(dbroot);                /* FSF01:2/2 */
    datablockFreePtr(dbckey);                /* FSF03:2/2 */
    if (lastdbval != &dbroot) {
      datablockFreePtr(lastdbval);
      lastdbval = NULL;
    }
    return 0;
  }
}
