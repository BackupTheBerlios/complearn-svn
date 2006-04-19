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
#include <complearn/complearn.h>

#if HAVE_LIBCSOAP_SOAP_CLIENT_H
#include <libcsoap/soap-client.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

static const double refFactor = 2.0;


static void goog_clsetenv(struct CompAdaptor *ca);
static double goog_compclFunc(struct CompAdaptor *ca, struct DataBlock *src);
static void goog_freecompclFunc(struct CompAdaptor *ca);
static char *goog_shortname(void);
static char *goog_longname(void);
static int goog_apiver(void);

/** \brief The Google compression adaptor instance
 *
 *  This structure holds all the persistent configuration information
 *  necessary for the Google compressor to work.
 *
 *  \struct GoogleCompInstance
 */
struct GoogleCompInstance {
  char *gkey;    /*!< GoogleKey which must be obtained by Google */
  char *daystr;
  double m;
};

/** \brief Initializes a GOOGLE CompAdaptor instance
 *
 *  clBuiltin_GOOG() allocates memory to a GOOGLE CompAdaptor instance. The
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
struct CompAdaptor *clBuiltin_GOOG(void)
{
	struct CompAdaptor c =
	{
    cptr: NULL,
//    se :  goog_clsetenv,
    cf:   goog_compclFunc,
    fcf:  goog_freecompclFunc,
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

  clCompaInitParameters(ca);

  return ca;
}

static const char *refWords[] = {
  "ten",
  "important",
  "words",
  "just",
  "typical",
  "among",
  "everyday",
  "internet",
  "people",
  "city",
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

static void goog_clsetenv(struct CompAdaptor *ca)
{
	struct GoogleCompInstance *ci = (struct GoogleCompInstance *) ca->cptr;
  char *args[1] = { NULL };
  char *userKey;
  struct EnvMap *em = clLoadDefaultEnvironment()->em;
  herror_t err;
  err = soap_client_init_args(0, args);
  userKey = clEnvmapValueForKey(em, "GoogleKey");
  clEnvmapSetKeyPrivate(em, "GoogleKey");
  if (userKey == NULL) {
    clogError("Cannot use google adaptor without %s property set","GoogleKey");
  }
  ci->gkey = clStrdup(userKey);
  ci->m = clCalculateM(NULL, ci->gkey);
  //clogLog("M=%f\n", ci->m);
}

static double goog_compclFunc(struct CompAdaptor *ca, struct DataBlock *src)
{
	struct GoogleCompInstance *sci = (struct GoogleCompInstance *) ca->cptr;
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
  pagecount = clFetchSampleSimple(terms, sci->gkey, NULL);
  if (pagecount < 1) /* probably 0 */
    pagecount = NOTFOUNDWEIGHT;   /* a small amount given just for asking */
  compsize = -log(pagecount/sci->m)/log(2.0);

  clStringstackFree(terms);
  clFreeandclear(str);
	return (double) compsize;
}

static void goog_freecompclFunc(struct CompAdaptor *ca)
{
	struct GoogleCompInstance *sci = (struct GoogleCompInstance *) ca->cptr;
  clFreeandclear(sci->gkey);
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
struct CompAdaptor *clBuiltin_GOOG(void)
{
  return NULL;
}
#endif
