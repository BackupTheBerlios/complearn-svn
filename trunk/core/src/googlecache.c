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
#include <complearn/googlecache.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <complearn/complearn.h>

struct GoogleCache {
  struct GDBMHelper *samp;
};

/** \brief All saved data from a single sample of the Google indexing database
 *
 *  Every search request to Google returns numerous results, most of which are
 *  thrown away.  The saved information is stored in a GCSample.
 *  It contains the pagecount, the date and time of the request,
 *  a link to the last (most recent before this one) sample for the same
 *  (normalized) search term, and a copy of the original (non-normalized)
 *  query string.
 *
 *  \struct GCSample
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

struct GoogleCache *clNewGC(void)
{
  struct GoogleCache *gc;
  int retryNum = 0;
  int maxTries = 5;
  gc = clCalloc(sizeof(struct GoogleCache), 1);
  for (;;) {
    gc->samp = cldbopen("gsamp");
    if (gc->samp != NULL)
      break;
    if (retryNum > maxTries) {
      clogError( "Error, cannot open GDBM google sample database.  Only one ncd may be running at once.\n");
      exit(1);
    }
    clSleepMillis(rand() % 1000);
  }
  return gc;
}

void clDeleteSavedGC(void)
{
  cldbunlink("gsamp");
}

void clFreeGC(struct GoogleCache *gc)
{
  cldbclose(gc->samp);
  gc->samp = NULL;
  clFreeandclear(gc);
}

static const char *makeCacheKey(const char *daystr, struct StringStack *terms)
{
  static char ckbuf[3000];
  memset(ckbuf, 0, sizeof(ckbuf));
  sprintf(ckbuf, "ck-%s/%s", daystr ? daystr : "last", clMakeQueryString(terms));
  return ckbuf;
}

struct DataBlock *clMakeCacheVal(double pg, struct DataBlock *lastdbval, const char *qorig)
{
  static struct GCSample d, lastd;
  struct CLDateTime *dt;
  dt = cldatetimeNow();

  memset(&d, 0, sizeof(d));
  memset(&lastd, 0, sizeof(lastd));
  d.pagecount = pg;
  d.when = cldatetimeToInt(dt);
  strcpy(d.daystring, cldatetimeToDayString(dt));
  if (lastdbval && clDatablockSize(lastdbval) > 0)
    memcpy(d.cknext, clDatablockData(lastdbval), clDatablockSize(lastdbval));
  strcpy(d.qorig, qorig);
  cldatetimeFree(dt);

  return clDatablockNewFromBlock(&d, sizeof(d));
}

double clConvertCacheVal(struct DataBlock *d)
{
  struct GCSample *r = (struct GCSample *) clDatablockData(d);
  return r->pagecount;
}
double clFetchSampleSimple(struct StringStack *terms, const char *gkey, const char *udaystr)
{
  static struct GoogleCache *gc;
  double result;
  char *daystr;
  struct CLDateTime *dt = NULL;
  int err;
  if (udaystr == NULL) {
    dt = cldatetimeNow();
    daystr = clStrdup(cldatetimeToDayString(dt));
    cldatetimeFree(dt);
    dt = NULL;
  }
  else {
    daystr = clStrdup(udaystr);
  }
  if (gc == NULL)
    gc = clNewGC();
  err = clFetchsample(gc, daystr, terms, &result, gkey);
  clFreeandclear(daystr);
  if (dt)
    cldatetimeFree(dt);
  return result;
}
/** \brief Fetches a sample from the local count database with the help of the
 * caching agent, GoogleCache.
 *
 * The clFetchsample function is the primary interface to the GoogleCache
 * database, and is expected to be sufficient for most uses.  It supports a
 * GDBM-based query mechanism that allows efficient retrieval of count
 * information for any given day.  In order to fetch a sample, a user must
 * provide a daystring, or specially constructed character string that
 * encodes day information, as well as a searchquery that is comprised of
 * a StringStack of search terms.  The results are returned via the
 * out parameter val as well as a return status code, nominally CL_OK.
 * Another requirement of using clFetchsample is a Google API account key.
 * This key looks like "Q/ZMtPCJYCKQrnxeq/pXJ/UNC4DEG1CZe" and is sent to
 * you after applying with Google online.
 *
 * The search terms supplied in the terms StringStack are expected to be
 * unquoted.  The clFetchsample function will add double-quotes around these
 * strings as well as preceding each of them with + and joining them with
 * spaces.  This will form one large string query to send to Google from
 * the passed-in array.
 *
 * \param gc pointer to the GoogleCache object previously allocated
 * \param daystr string previously created with cldatetimeToDayString
 * \param terms pointer to a StringStack of terms that will be
 * \return a value 0 indicating a cache-miss, or 1 indicating a cache-success
 */
int clFetchsample(struct GoogleCache *gc, const char *daystr, struct StringStack *terms, double *val, const char *gkey)
{
  struct StringStack *normed;
  char *daystrcachekey, *lastkeystr;
  struct DataBlock *dblastkey, *lastdbval;
  struct DataBlock *db;
  struct DataBlock *dbdaystrkey;

  normed = clStringstackClone(terms);                    /* FSA02 */
  clNormalizeSearchTerms(normed);
  lastkeystr = (char *) makeCacheKey(NULL, normed);
  dblastkey = clStringToDataBlockPtr(lastkeystr);

  daystrcachekey = clStrdup(makeCacheKey(daystr, normed));

  dbdaystrkey = clStringToDataBlockPtr(daystrcachekey);      /* FSA03 */

  db = cldbfetch(gc->samp, dbdaystrkey);

  lastdbval = cldbfetch(gc->samp, dblastkey);  /* may be NULL */

  if (!db) {
    if (lastdbval) {
      db = cldbfetch(gc->samp, lastdbval);
//      clDatablockFreePtr(dbdaystrkey);                /* FSF03:1/2 */
    }
  }
  if (db) {
    assert(clDatablockSize(db) == sizeof(struct GCSample));
    *val = clConvertCacheVal(db);
    clStringstackFree(normed);                       /* FSF02:1/2 */
    clDatablockFreePtr(db);
    clDatablockFreePtr(dbdaystrkey);                /* FSF03:1/2 */
    clFree(daystrcachekey);
    return 1;
  } else {
    double pgc;
    struct DataBlock *newentry;
    pgc = clGetPageCount(terms, gkey);
    if (pgc < 0) {
      clogError("Error contacting Google, aborting...\n");
    }
    *val = pgc;
    newentry = clMakeCacheVal(pgc, lastdbval, clMakeQueryString(terms));
    cldbstore(gc->samp, dbdaystrkey, newentry);
    cldbstore(gc->samp, dblastkey, dbdaystrkey);
    clStringstackFree(normed);                          /* FSF02:2/2 */
    clDatablockFreePtr(dblastkey);
    clDatablockFreePtr(newentry);
    clDatablockFreePtr(dbdaystrkey);                /* FSF03:2/2 */
    if (lastdbval) {
      clDatablockFreePtr(lastdbval);
      lastdbval = NULL;
    }
    clFree(daystrcachekey);
    return 0;
  }
}
