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
#include <complearn/treeholder.h>
#include <assert.h>
#include <string.h>

#include <gsl/gsl_linalg.h>

struct TreeHolder {
  gsl_matrix *dm;
  struct TreeAdaptor *best;
  struct TreeScore *ts;
  double bestscore;
  int totalCount;
  int failedCount;
  int treeindex;
};

struct TreeHolder *clTreehClone(const struct TreeHolder *th)
{
  assert(th);
  assert(th->best && "th->best is NULL");
  struct TreeHolder *result;
  result =  clCalloc(sizeof(*th), 1);
  result->best = clTreeaClone(th->best);
  result->dm = clGslmatrixClone(th->dm);
  result->ts = clCloneTreeScore(th->ts);
  result->bestscore = th->bestscore;
  result->totalCount = th->totalCount;
  result->failedCount = th->failedCount;
  result->treeindex = th->treeindex;
  return result;
}

static double calculateScore(struct TreeHolder *th, struct TreeAdaptor *ta)
{
  double result;
  result = clScoreTree(th->ts, ta);
  th->totalCount += 1;
  return result;
}

struct TreeHolder *clTreehNew(const gsl_matrix *distmat, struct TreeAdaptor *tra)
{
  struct TreeHolder *th = clCalloc(sizeof(*th), 1);
  struct LabelPerm *lp;
  assert(tra);
  assert(distmat->size1 >= 4);
  assert(distmat->size1 == distmat->size2);
  th->best = clTreeaClone(tra);
  lp = clTreeaLabelPerm(th->best);
  assert(clLabelpermSize(lp) == distmat->size1);
  assert(th->best);
  assert(th->best->ptr);
  th->dm = clGslmatrixClone(distmat);
  th->ts = clInitTreeScore(th->dm);
  th->bestscore = calculateScore(th, th->best);
  th->treeindex = -1;
  clLabelpermFree(lp);
  return th;
}

void clTreehScramble(struct TreeHolder *th)
{
  int i, mutnum = 10;
  assert(th->best);
  for (i = 0; i < mutnum; i += 1)
    clTreeaMutate(th->best);
  th->bestscore = calculateScore(th, th->best);
}

double clTreehScore(const struct TreeHolder *th)
{
  return th->bestscore;
}

void clTreehSetTreeIndex(struct TreeHolder *th, int treeind)
{
  th->treeindex = treeind;
}

int clTreehGetTreeIndex(struct TreeHolder *th)
{
  return th->treeindex;
}

struct TreeAdaptor *clTreehTreeAdaptor(const struct TreeHolder *th)
{
  return clTreeaClone(th->best);
}

/* returns 1 if it did improve, 0 otherwise */
int clTreehImprove(struct TreeHolder *th)
{
  int itWorked;
  double candscore;
  struct TreeAdaptor *cand = clTreeaClone(th->best);
  clTreeaMutate(cand);
  candscore = calculateScore(th, cand);
//  if (candscore == 1 || candscore > th->bestscore) {
  if (candscore > th->bestscore) {
    th->failedCount = 0;
    itWorked = 1;
    clTreeaFree(th->best);
    th->best = cand;
    th->bestscore = candscore;
  } else {
    th->failedCount += 1;
//    printf("failedcount: %d\n",th->failedCount);
    itWorked = 0;
    clTreeaFree(cand);
  }
  return itWorked;
}

int clTreehTreeCount(const struct TreeHolder *th)
{
  return th->totalCount;
}

int clTreehFailCount(const struct TreeHolder *th)
{
  return th->failedCount;
}

gsl_matrix *clTreehDistMatrix(const struct TreeHolder *th)
{
  return th->dm;
}

void clTreehFree(struct TreeHolder *th)
{
  if (th->best) {
    clTreeaFree(th->best);
    th->best = NULL;
  }
  gsl_matrix_free(th->dm);
  th->dm = NULL;
  clFreeTreeScore(th->ts);
  th->ts = NULL;
  clFreeandclear(th);
}

