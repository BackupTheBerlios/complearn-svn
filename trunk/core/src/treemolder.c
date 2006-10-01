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

struct TreeMolder {
  int nodecount;
  double score, minscore, maxscore;
  gsl_matrix *dm;
  struct TreeAdaptor *ta;
  struct CLNodeSet *flips;
};

void clTreemolderFree(struct TreeMolder *tm)
{
  clNodesetFree(tm->flips);
  clTreeaFree(tm->ta);
  clGslmatrixFree(tm->dm);
  clFreeandclear(tm);
}

static void calcRangesTM(struct TreeMolder *tm)
{
  int i;
  gsl_vector_view currow;
  tm->minscore = 0;
  tm->maxscore = 0;
  for (i = 0; i < tm->dm->size1; i += 1) {
    currow = gsl_matrix_row(tm->dm, i);
    tm->minscore += gsl_vector_min((gsl_vector *) &currow);
    tm->maxscore += gsl_vector_max((gsl_vector *) &currow);
  }
}

struct TreeMolder *clTreemolderNew(gsl_matrix *gm, struct TreeAdaptor *ta)
{
  struct TreeMolder *tm = clCalloc(sizeof(*tm), 1);
  struct AdjAdaptor *aa;
  assert(gm->size1 > 0 && gm->size1 == gm->size2 != NULL);
  aa = clTreeaAdjAdaptor(ta);
  assert(aa != NULL);
  tm->nodecount = clAdjaSize(aa);
  tm->ta = clTreeaClone(ta);
  tm->dm = clGslmatrixClone(gm);
  tm->flips = clNodesetNew(tm->nodecount);
  tm->score = -1;
  calcRangesTM(tm);
  return tm;
}

static double scorePerimeter(const gsl_matrix *dm, struct TreeAdaptor *ts, struct CLNodeSet *flips)
{
  double acc = 0.0;
  struct DRA *pairs = clTreeaPerimPairs(ts, flips);
  int i;
  struct LabelPerm *lph = clTreeaLabelPerm(ts);
  for (i = 0; i < clDraSize(pairs); i += 1) {
    union PCTypes p = clDraGetValueAt(pairs, i);
    double x, y;
    x = clLabelpermColIndexForNodeID(lph, p.ip.x);
    y = clLabelpermColIndexForNodeID(lph, p.ip.y);
    acc += gsl_matrix_get(dm, x, y);
  }
  clDraFree(pairs);
  clLabelpermFree(lph);
  return acc;
}

double clTreemolderScoreScaled(struct TreeMolder *tm)
{
  double rawscore = clTreemolderScore(tm);
  return 1.0 - ((rawscore - tm->minscore) / (tm->maxscore-tm->minscore));
}

double clTreemolderScore(struct TreeMolder *tm)
{
  if (tm->score == -1)
    tm->score = scorePerimeter(tm->dm, tm->ta, tm->flips);
  return tm->score;
}

struct CLNodeSet *clTreemolderFlips(struct TreeMolder *tm)
{
  return clNodesetClone(tm->flips);
}

static void mutateFlipArray(struct TreeMolder *tm, struct CLNodeSet *dst)
{
  int whichNode, oldStatus;
  do {
    do {
      whichNode = rand() % tm->nodecount;
    } while (!clTreeaIsFlippable(tm->ta, whichNode));
    oldStatus = clNodesetHasNode(dst, whichNode);
    //printf("About to switch node %d flip from %d to %d.\n", whichNode, oldStatus, !oldStatus);
    clNodesetSetNodeStatus(dst, whichNode, !oldStatus);
  } while ((rand() % 2) == 0);
}

void clTreemolderScramble(struct TreeMolder *tm)
{
  int i;
  for (i = 0; i < tm->nodecount; i += 1)
    mutateFlipArray(tm, tm->flips);
  tm->score = -1;
}

int clTreemolderImprove(struct TreeMolder *tm)
{
  struct CLNodeSet *cand = clNodesetClone(tm->flips);
  double candscore;
  mutateFlipArray(tm, cand);
  candscore = scorePerimeter(tm->dm, tm->ta, cand);
  if (candscore < clTreemolderScore(tm)) {
//    printf("In treemolder %p, raw score impr from %f to %f\n", tm, tm->score, candscore );
    tm->score = candscore;
    clNodesetFree(tm->flips);
    tm->flips = cand;
    return 1;
  }
  else {
    clNodesetFree(cand);
    return 0;
  }
}

struct TreeAdaptor *clTreemolderTreeAdaptor(const struct TreeMolder *tmo)
{
  return clTreeaClone(tmo->ta);
}

int clTreemolderNodeCount(const struct TreeMolder *tmo)
{
  return tmo->nodecount;
}

