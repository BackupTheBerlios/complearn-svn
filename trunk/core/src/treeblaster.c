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
#include <assert.h>

#define MAXFAILS 100000

struct TreeBlaster {
  gsl_matrix *dm;
  int k;
  int failcount;
  struct TreeAdaptor *ta;
  int lastChanged;
  struct TreeMolder *best;
  double printedScore;
  struct TreeMolder *tm[64];
  struct TreeOrderObserver *tob;
};

static void setBestPtr(struct TreeBlaster *tm)
{
  int i;
  for (i = 0; i < tm->k; i += 1)
    if (i == 0 || clTreemolderScoreScaled(tm->best) < clTreemolderScoreScaled(tm->tm[i]))
      tm->best = tm->tm[i];
  if (clTreemolderScoreScaled(tm->best) > tm->printedScore) {
    tm->printedScore = clTreemolderScoreScaled(tm->best);
    if (tm->tob && tm->tob->treeorderimproved)
      tm->tob->treeorderimproved(tm->tob, tm->best, clTreemolderFlips(tm->best));
  }
}

struct TreeBlaster *clTreebNew(gsl_matrix *gsl, struct TreeAdaptor *ta)
{
  int i, howbig;
  struct TreeBlaster *tm = clCalloc(sizeof(struct TreeBlaster), 1);
  assert(gsl);
  assert(gsl->size1 == gsl->size2);
  howbig = gsl->size1;
  tm->ta = clTreeaClone(ta);
  tm->dm = clGslmatrixClone(gsl);
  tm->printedScore = -1;
  assert(tm->dm);
  if (gsl->size1 < 14)
    tm->k = 5;
  else
    tm->k = 3;
  if (gsl->size1 < 10) /* small trees are very quick yet very uncertain */
    tm->k += 1;
  for (i = 0; i < tm->k; i += 1) {
    tm->tm[i] = clTreemolderNew(tm->dm, tm->ta);
//    printf("Tree holder %d starts with score %f\n", i, clTreemolderScore(tm->tm[i]));
    clTreemolderScramble(tm->tm[i]);
//    printf("Tree holder %d scrmables to score %f\n", i, clTreemolderScore(tm->tm[i]));
  }
  setBestPtr(tm);
  return tm;
}

static int doStep(struct TreeBlaster *tm)
{
  //int whoseTurn = rand() % 2;
  int choseTree;
  int result;
  choseTree = rand() % tm->k;
//  printf("Trying tree %d\n", choseTree);
  result = clTreemolderImprove(tm->tm[choseTree]);
  if (result) {
    tm->failcount = 0;
    setBestPtr(tm);
  } else {
    tm->failcount += 1;
//    printf("no improvement, trying another tree... (%d)\n", clTreehFailCount(tm->th[choseTree]));
  }
  return result;
}

static int checkDone(struct TreeBlaster *tm)
{
  int i;
  if (tm->failcount > MAXFAILS)
    return 1;
  for (i = 1; i < tm->k; ++i) {
    if (clTreemolderScoreScaled(tm->tm[i-1]) != clTreemolderScoreScaled(tm->tm[i]))
      return 0;
  }
  return 1;
}

struct CLNodeSet *clTreebFindTreeOrder(struct TreeBlaster *tm, double *s)
{
  int retval;
  if (tm->tob && tm->tob->treeordersearchstarted)
    tm->tob->treeordersearchstarted(tm->tob);
  while (!checkDone(tm)) {
    retval = doStep(tm);
  }
  if (tm->tob && tm->tob->treeorderdone)
    tm->tob->treeorderdone(tm->tob, tm->best, clTreemolderFlips(tm->best));
  *s = clTreemolderScoreScaled(tm->tm[0]);
  printf("Finished with tree, score is %f\n", *s);
  return clTreemolderFlips(tm->best); /* TODO: does this need clone? */
}

void clTreebFree(struct TreeBlaster *tm)
{
  int i;
  gsl_matrix_free(tm->dm);
  tm->dm = NULL;
  clTreeaFree(tm->ta);
  tm->ta = NULL;
  for (i = 0; i < tm->k; i += 1) {
    clTreemolderFree(tm->tm[i]);
  }
  clFreeandclear(tm);
}

void clTreebSetTreeOrderObserver(struct TreeBlaster *tm, struct TreeOrderObserver *tob)
{
  tm->tob = tob;
}

int clTreebK(struct TreeBlaster *tbl)
{
  assert(tbl);
  return tbl->k;
}

int clGetNodeCountTB(struct TreeBlaster *tbl)
{
  assert(tbl);
  struct TreeMolder *tmo = tbl->best;
  return clTreemolderNodeCount(tmo);
}

int clTreebLabelCount(struct TreeBlaster *tbl)
{
  assert(tbl);
  return tbl->dm->size1;
}

