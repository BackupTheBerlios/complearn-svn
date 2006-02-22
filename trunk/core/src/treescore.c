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
#include <assert.h>
#include <complearn/complearn.h>

#define MAXPATHPI 16384
#define MAXPATHTS 16384

struct TreeScore {
  gsl_matrix *dm;
  double penalty;
};

struct TreeScore *clCloneTreeScore(struct TreeScore *ts)
{
  struct TreeScore *newts = clCalloc(sizeof(struct TreeScore), 1);
  newts->penalty = ts->penalty;
  newts->dm = clGslmatrixClone(ts->dm);
  return newts;
}

struct TreeScore *clInitTreeScore(gsl_matrix *dm)
{
  struct TreeScore *ts = clCalloc(sizeof(struct TreeScore), 1);
  ts->dm = clGslmatrixClone(dm);
  return ts;
}

static int pathsIntersect(int *path1, int len1, int *path2, int len2)
{
  static int pathWatcher[MAXPATHPI];
  int i;
  int retval = 0;
  for (i = 0; i < len1; ++i)
    pathWatcher[ path1[i] ] = 0;
  for (i = 0; i < len2; ++i)
    pathWatcher[ path2[i] ] = 1;
  for (i = 0; i < len1; ++i)
    if (pathWatcher[ path1[i] ] == 1) {
      retval = 1;
      break;
    }
  return retval;
}

int clIsConsistent(struct AdjAdaptor *ad, struct Quartet q)
{
  static int nbuf1[MAXPATHTS], nbuf2[MAXPATHTS];
  int p1length = MAXPATHTS, p2length = MAXPATHTS;
  int pathretval;
  pathretval = clPathFinder(ad, q.q[0], q.q[1], nbuf1, &p1length);
  assert(pathretval == CL_OK);
  pathretval = clPathFinder(ad, q.q[2], q.q[3], nbuf2, &p2length);
  assert(pathretval == CL_OK);
  return !pathsIntersect(nbuf1, p1length, nbuf2, p2length);
}

double clScoreTree(struct TreeScore *ts, struct TreeAdaptor *ta)
{
  int i, j, k, m, p, x;
  double sum = 0;
  double maxtot = 0, mintot = 0;
  struct LabelPerm *lp = clTreeaLabelPerm(ta);
  assert(lp);
  assert(ts->dm->size1 == ts->dm->size2);
  assert(ts->dm->size1 == clLabelpermSize(lp));
  ALLQUARTETS(ts->dm->size1, i, j, k, m) {
    double mincur=0, maxcur=0;
    for (p = 0; p < 3; p += 1) {
      struct Quartet q = clPermuteLabelsDirect(i, j, k, m, p);
      double c1a = gsl_matrix_get(ts->dm,q.q[0],q.q[1]);
      double c1b = gsl_matrix_get(ts->dm,q.q[1],q.q[0]);
      double c2a = gsl_matrix_get(ts->dm,q.q[2],q.q[3]);
      double c2b = gsl_matrix_get(ts->dm,q.q[3],q.q[2]);
      double c1 = c1a < c1b ? c1a : c1b;
      double c2 = c2a < c2b ? c2a : c2b;
      double curcost = c1 + c2;
      for (x = 0; x < 4; x += 1)
        q.q[x] = clLabelpermNodeIDForColIndex(lp, q.q[x]);
      if (p == 0 || (curcost < mincur))
        mincur = curcost;
      if (p == 0 || (curcost > maxcur))
        maxcur = curcost;
      if (clIsConsistent(clTreeaAdjAdaptor(ta), q))
        sum += curcost;
    }
    maxtot += maxcur;
    mintot += mincur;
  }
  clLabelpermFree(lp);
  return 1.0 - ((sum - mintot) / (maxtot - mintot));
}

void clFreeTreeScore(struct TreeScore *ts)
{
  if (ts == NULL)
    clogError("tried to free NULL pointer in clFreeTreeScore");
  clGslmatrixFree(ts->dm);
  clFreeandclear(ts);
}

