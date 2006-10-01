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

void clTreeaclLabelpermSetColumnIndexToNodeNumber(struct TreeAdaptor *ta,
    int colInd, int nodeNum)
{
  ta->treelpsetat(ta, colInd, nodeNum);
}

void clTreeaMutate(struct TreeAdaptor *tra)
{
  clAssert(tra != NULL);
  tra->treemutate(tra);
}

struct TreeAdaptor *clTreeaNew(int isRooted, int howbig)
{
  if (isRooted)
    return clTreeaLoadRootedBinary(howbig);
  else
    return clTreeaLoadUnrooted(howbig);
}

void clTreeaFree(struct TreeAdaptor *tra)
{
  clAssert(tra != NULL);
  clAssert(tra->treefree != NULL);
  tra->treefree(tra);
}

struct TreeAdaptor *clTreeaClone(struct TreeAdaptor *tra)
{
  clAssert(tra != NULL);
  return tra->treeclone(tra);
}

struct LabelPerm *clTreeaLabelPerm(struct TreeAdaptor *tra)
{
  clAssert(tra && "NULL TreeAdaptor in clTreeaLabelPerm" != NULL);
  return tra->treegetlabelperm(tra);
}

struct AdjAdaptor *clTreeaAdjAdaptor(struct TreeAdaptor *tra)
{
  clAssert(tra != NULL);
  return tra->treegetadja(tra);
}

int clTreeaIsQuartettable(struct TreeAdaptor *tra, int which)
{
  clAssert(tra != NULL);
  return tra->treeisquartetable(tra, which);
}

int clTreeaIsFlippable(struct TreeAdaptor *tra, int which)
{
  clAssert(tra != NULL);
  return tra->treeisflippable(tra, which);
}
int clTreeaIsRoot(struct TreeAdaptor *tra, int which)
{
  clAssert(tra != NULL);
  return tra->treeisroot(tra, which);
}


int clTreeaMutationCount(struct TreeAdaptor *tra)
{
  clAssert(tra != NULL);
  return tra->treemutecount(tra);
}

struct DRA *clTreeaPerimPairs(struct TreeAdaptor *tra, struct CLNodeSet *flips)
{
  clAssert(tra != NULL);
  return tra->treeperimpairs(tra, flips);
}

int clTreeaNodeCount(struct TreeAdaptor *tra)
{
  struct AdjAdaptor *ad = clTreeaAdjAdaptor(tra);
  return clAdjaSize(ad);
}

static double nchoosefourScale(int n, int inp)
{
  double bign = n;
  bign = (bign * (bign - 1) * (bign - 2) * (bign - 3)) / 24.0;
  return ((double) inp) / bign;
}

double clTreeaDifferenceScore(struct TreeAdaptor *tra1, struct TreeAdaptor *tra2)
{
  struct AdjAdaptor *ad1, *ad2;
  struct LabelPerm *lab1, *lab2;
  int diff;
  int lps;
  lab1 = clTreeaLabelPerm(tra1);
  lab2 = clTreeaLabelPerm(tra2);
  lps = clLabelpermSize(lab1);
  ad1 = clTreeaAdjAdaptor(tra1);
  ad2 = clTreeaAdjAdaptor(tra2);
  diff = clCountTrinaryDifferences(ad1, lab1, ad2, lab2);
  clLabelpermFree(lab1);
  clLabelpermFree(lab2);
  return nchoosefourScale(lps, diff);
}

struct DRA *clTreeaNodes(struct TreeAdaptor *ta)
{
  struct DRA *n = clDraNew();
  union PCTypes p = zeropct;
  int i;
  for (i = 0; i < clTreeaNodeCount(ta) ;  i += 1) {
    p.i = i;
    clDraPush(n,p);
  }
  return n;
}
