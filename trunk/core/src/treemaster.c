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
#include <string.h>
#include <assert.h>

struct TreeMasterConfig {
  int fSelfAgreementTermination;
  int fIsRooted;
  int maxFailCount;
};

struct TreeMaster {
  gsl_matrix *dm;
  int k;
  int nodecount;
  int lastChanged;
  int fAbortNow;
  void *udata;
  struct TreeHolder *best;
  double printedScore;
  struct TreeHolder *th[16];
  struct TreeObserver *tob;
  struct TreeMasterConfig activeConfig;
  struct CLDateTime *startTime, *endTime;
};

int clTreemasterK(struct TreeMaster *tm)
{
  if (tm == NULL) {
    clogError("NULL ptr in clTreemasterK()\n");
  }
  assert(tm);
  return tm->k;
}

static void callImprovedFunctionMaybe(struct TreeMaster *tm)
{
  if (tm == NULL) {
    clogError("NULL ptr in callImprovedFunctionMaybe()\n");
  }
  if (tm->tob && tm->tob->treeimproved) {
    struct TreeHolder *th = clTreehClone(tm->best);
    tm->tob->treeimproved(tm->tob, th);
    clTreehFree(th);
  }
}

static void setBestPtr(struct TreeMaster *tm)
{
  int i;
  if (tm == NULL) {
    clogError("NULL ptr in setBestPtr()\n");
  }
  for (i = 0; i < tm->k; i += 1)
    if (i == 0 || clTreehScore(tm->best) < clTreehScore(tm->th[i])) {
      if (tm->best) {
        clTreehFree(tm->best);
        tm->best = NULL;
        }
      tm->best = clTreehClone(tm->th[i]);
    }
  if (clTreehScore(tm->best) > tm->printedScore) {
    tm->printedScore = clTreehScore(tm->best);
    callImprovedFunctionMaybe(tm);
  }
}

static struct TreeMasterConfig getTreeMasterDefaultConfig(void)
{
  struct TreeMasterConfig tmc_default = { 1, 0, 100000};
  return tmc_default;
}

static void tm_setIntValueMaybe(struct EnvMap *srcenv, const char *keyname, int *placeToSet) {
  char *val;
  if (srcenv == NULL || keyname == NULL) {
    clogError("NULL ptr in tm_setIntValueMaybe()\n");
  }
  val = clEnvmapValueForKey(srcenv,keyname);
  if (val)
    *placeToSet = atoi(val);
}

static void validateMatrixForTree(gsl_matrix *gsl)
{
  int i, j;
  if (gsl == NULL) {
    clogError("NULL ptr in validateMatrixForTree()\n");
  }
  if (gsl->size1 != gsl->size2) {
    clogError( "Matrix must be square, but this one is %d by %d\n", gsl->size1, gsl->size2);
    exit(1);
  }
  for (i = 0; i < gsl->size1; i += 1) {
    for (j = 0; j < gsl->size2; j += 1) {
      if (gsl_matrix_get(gsl, i, j) != gsl_matrix_get(gsl, 0, 0))
        return;
    }
  }
  clogError( "Sorry, this is a constant matrix so no information is available.\n");
  exit(1);
}

struct TreeMaster *clTreemasterNewEx(gsl_matrix *gsl, int isRooted, struct EnvMap *em)
{
  struct TreeMaster *result;
  struct TreeMasterConfig tmc = getTreeMasterDefaultConfig();
  if (em == NULL || gsl == NULL) {
    clogError("NULL ptr in clTreemasterNewEx()\n");
  }
  assert(em);

  validateMatrixForTree(gsl);
  tmc.fIsRooted = isRooted;

  tm_setIntValueMaybe(em, "isRooted", &tmc.fIsRooted);
  tm_setIntValueMaybe(em, "selfAgreementTermination", &tmc.fSelfAgreementTermination);
  if (!tmc.fSelfAgreementTermination)
    tm_setIntValueMaybe(em, "maxFailCount", &tmc.maxFailCount);
  else
    tmc.maxFailCount = -1;

  result = clTreemasterNew(gsl, tmc.fIsRooted);
  result->activeConfig = tmc;
  if (!result->activeConfig.fSelfAgreementTermination)
    result->k = 1;
  return result;
}

struct TreeMaster *clTreemasterNewEz(gsl_matrix *gsl)
{
  if (gsl == NULL) {
    clogError("NULL ptr in clTreemasterNewEz()\n");
  }
  return clTreemasterNew(gsl, 0);
}

struct TreeMaster *clTreemasterNew(gsl_matrix *gsl, int isRooted)
{
  int i, howbig;
  struct TreeMasterConfig tmc = getTreeMasterDefaultConfig();
  struct TreeAdaptor *tra;
  struct TreeMaster *tm = clCalloc(sizeof(struct TreeMaster), 1);
  struct AdjAdaptor *aa;
  //clogWarning("Staring the cltmnew...\n");
  if (gsl == NULL) {
    clogError("NULL ptr in clTreemasterNew()\n");
  }
//  clogWarning("Staring the B cltmnew...\n");
//  clogWarning("About to look at gsl=%p\n", gsl);
//  clogWarning("Staring the C cltmnew...\n");
//  clogWarning("And size %d\n", gsl->size1);
  if (gsl->size1 < 4) {
    clogError("Dist matrix needs at least 4 entries but has just %d.\n", gsl->size1);
  }
  assert(gsl);
  assert(gsl->size1 == gsl->size2);
  assert(gsl->size1 > 3);
  howbig = gsl->size1;
  tm->dm = clGslmatrixClone(gsl);
  tm->printedScore = -1;
  tm->fAbortNow = 0;
  assert(tm->dm);
  if (gsl->size1 < 16)
    tm->k = 3;
  else
    tm->k = 2;
  if (gsl->size1 < 18) /* to push accuracy up a bit */
    tm->k += 1;
  if (gsl->size1 < 10) /* small trees are very quick yet very uncertain */
    tm->k += 1;
  if (gsl->size1 <= 5)
    tm->k += 1;
  tra = clTreeaNew(isRooted, howbig);
  aa = clTreeaAdjAdaptor(tra);
  tm->nodecount = clAdjaSize(aa);
  for (i = 0; i < tm->k; i += 1) {
    tm->th[i] = clTreehNew(tm->dm, tra);
    clTreehSetTreeIndex(tm->th[i], i);
    clTreehScramble(tm->th[i]);
  }
  tm->activeConfig = tmc;
  clTreeaFree(tra);
  setBestPtr(tm);
  return tm;
}

int clTreemasterTreeCount(struct TreeMaster *tm)
{
  if (tm == NULL) {
    clogError("NULL ptr in clTreemasterTreeCount()\n");
  }
  if (tm->activeConfig.fSelfAgreementTermination) {
    int i;
    int sum = 0;
    for (i = 0; i < tm->k; i += 1)
      sum += clTreehTreeCount(tm->th[i]);
    return sum;
  } else {
    return clTreehTreeCount(tm->best);
  }
}

static int doStep(struct TreeMaster *tm)
{
  //int whoseTurn = rand() % 2;
  int choseTree;
  int result;
  if (tm == NULL) {
    clogError("NULL ptr in doStep()\n");
  }
  if (!tm->activeConfig.fSelfAgreementTermination) {
    result = clTreehImprove(tm->best);
    if (result)
      callImprovedFunctionMaybe(tm);
    return result;
  }
  choseTree = rand() % tm->k;
  result = clTreehImprove(tm->th[choseTree]);
  if (result) {
//    printf("Tree %d improved to %f after %d tries (%d mutation stepsize)\n", choseTree, clTreehScore(tm->th[choseTree]), clTreehTreeCount(tm->th[choseTree]), clTreeaMutationCount(clTreehTreeAdaptor(tm->th[choseTree])));
    tm->lastChanged = choseTree;
    setBestPtr(tm);
  } else {
//    printf("no improvement, trying another tree... (%d)\n", clTreehFailCount(tm->th[choseTree]));
    if (!tm->fAbortNow && tm->tob && tm->tob->treerejected)
      tm->tob->treerejected(tm->tob);
  }
  return result;
}

static int clIsIdenticalTreeTRA(struct TreeAdaptor *ta1, struct TreeAdaptor *ta2)
{
  struct LabelPerm *lab1, *lab2;
  int result;
  if (ta1 == NULL || ta2 == NULL) {
    clogError("NULL ptr in clIsIdenticalTreeTRA()\n");
  }
  lab1 = clTreeaLabelPerm(ta1);
  lab2 = clTreeaLabelPerm(ta2);
  result = clIsIdenticalTree(clTreeaAdjAdaptor(ta1), lab1,
                         clTreeaAdjAdaptor(ta2), lab2);
  clLabelpermFree(lab1);
  clLabelpermFree(lab2);
  return result;
}

static int checkDone(struct TreeMaster *tm)
{
  int i;
  if (tm == NULL) {
    clogError("NULL ptr in checkDone()\n");
  }
  if (tm->activeConfig.fSelfAgreementTermination) {
    for (i = 1; i < tm->k; ++i) {
      if (clTreehScore(tm->th[i-1]) != clTreehScore(tm->th[i]))
        return 0;
      if (!clIsIdenticalTreeTRA(clTreehTreeAdaptor(tm->th[i-1]), clTreehTreeAdaptor(tm->th[i])))
        return 0;
    }
    return 1;
  } else {
//    printf("failcount: %d  maxcount: %d\n", clTreehFailCount(tm->best), tm->activeConfig.maxFailCount);
    return !(clTreehFailCount(tm->best) < tm->activeConfig.maxFailCount);
  }
}

struct CLDateTime *clTreemasterEndTime(struct TreeMaster *tm)
{
  if (tm == NULL) {
    clogError("NULL ptr in clTreemasterEndTime()\n");
  }
  return tm->endTime;
}

struct CLDateTime *clTreemasterStartTime(struct TreeMaster *tm)
{
  if (tm == NULL) {
    clogError("NULL ptr in clTreemasterStartTime()\n");
  }
  return tm->startTime;
}

struct TreeHolder *clTreemasterFindTree(struct TreeMaster *tm)
{
  int retval;
  if (tm == NULL) {
    clogError("NULL ptr in clTreemasterFindTree()\n");
  }
  tm->startTime = clDatetimeNow();
  if (tm->tob && tm->tob->treesearchstarted)
    tm->tob->treesearchstarted(tm->tob);
  for (;;) {
    retval = doStep(tm);
    if (tm->fAbortNow)
      break;
    if (retval || !tm->activeConfig.fSelfAgreementTermination) {
      if (checkDone(tm))
        break;
    }
  }
  tm->endTime = clDatetimeNow();
#if 1
  if (!tm->fAbortNow && tm->tob && tm->tob->treedone) {
    struct TreeHolder *th = clTreehClone(tm->th[0]);
    tm->tob->treedone(tm->tob, th);
    clTreehFree(th);
  }
#endif
  tm->fAbortNow = 0;
  return tm->th[0];
}

void clTreemasterFree(struct TreeMaster *tm)
{
  int i;
  if (tm == NULL) {
    clogError("NULL ptr in clTreemasterFree()\n");
  }
//  printf("Freeing treemaster at %p\n", tm);
  for (i = 0; i < tm->k; i += 1) {
    clTreehFree(tm->th[i]);
    tm->th[i]=NULL;
  }
  if (tm->best) {
    clTreehFree(tm->best);
    tm->best=NULL;
  }
  if (tm->endTime) {
    clDatetimeFree(tm->endTime);
    tm->endTime = NULL;
  }
  if (tm->startTime) {
    clDatetimeFree(tm->startTime);
    tm->startTime = NULL;
  }
  gsl_matrix_free(tm->dm);
  tm->dm = NULL;
  if (tm->tob) {
    clFree(tm->tob);
    tm->tob = NULL;
  }
  clFreeandclear(tm);
}

struct TreeObserver *clTreemasterGetTreeObserver(struct TreeMaster *tm)
{
  if (tm == NULL) {
    clogError("NULL ptr in clTreemasterGetTreeObserver()\n");
  }
  return tm->tob;
}

void clTreemasterSetTreeObserver(struct TreeMaster *tm, struct TreeObserver *tob)
{
  if (tm == NULL || tob == NULL) {
    clogError("NULL ptr in clTreemasterSetTreeObserver()\n");
  }
  if (tm->tob) {
    clFreeandclear(tm->tob);
  }
  tm->tob = clCalloc(sizeof(*tob), 1);
  *(tm->tob) = *tob;
}

void clTreemasterSetUserData(struct TreeMaster *tm, void *val)
{
  if (tm == NULL) {
    clogError("NULL ptr in clTreemasterSetUserData()\n");
  }
  tm->udata = val;
}

void *clTreemasterGetUserData(struct TreeMaster *tm)
{
  if (tm == NULL) {
    clogError("NULL ptr in clTreemasterGetUserData()\n");
  }
  return tm->udata;
}

void clTreemasterAbortSearch(struct TreeMaster *tm)
{
  if (tm == NULL) {
    clogError("NULL ptr in clTreemasterAbortSearch()\n");
  }
  tm->fAbortNow = 1;
}

struct TreeHolder *clTreemasterStarterTree(struct TreeMaster *tm)
{
  if (tm == NULL || tm->th[0] == NULL) {
    clogError("NULL ptr in clTreemasterStarterTree()\n");
  }
  assert(tm->th[0]);
  return clTreehClone(tm->th[0]);
}

struct TreeHolder *clTreemasterTreeAtIndex(struct TreeMaster *tm, int i)
{
  if (tm == NULL) {
    clogError("NULL ptr in clTreemasterTreeAtIndex()\n");
  }
  assert(i >= 0);
  assert(i < tm->k);
  return clTreehClone(tm->th[i]);
}

int clTreemasterLabelCount(struct TreeMaster *tm)
{
  if (tm == NULL) {
    clogError("NULL ptr in clTreemasterLabelCount()\n");
  }
  return tm->dm->size1;
}

