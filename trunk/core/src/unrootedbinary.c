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
#include <stdlib.h>
#include <assert.h>

static struct TreeAdaptor *loadUBTRA(struct UnrootedBinary *ub);
static struct DRA *getLabellableNodes(const struct UnrootedBinary *ub);

struct UnrootedBinary {
  int nodecount;
  qbase_t startNode;
  int mc;
  struct LabelPerm *labelperm;
  struct AdjAdaptor *aa;
  struct CLNodeSet *flips;
};

void clUnrootedBinaryLabelPermSetter(struct UnrootedBinary *ub, int j, int i)
{
  clLabelpermSetColumnIndexToNodeNumber(ub->labelperm, j, i);
}

struct AdjAdaptor *clGetAdjAdaptorForUB(struct UnrootedBinary *ub)
{
  return ub->aa;
}

static qbase_t randomTreeNode(struct UnrootedBinary *ub)
{
  return rand() % ub->nodecount;
}

static qbase_t randomKernelNode(struct UnrootedBinary *ub)
{
  qbase_t result;
  int nsize;
  do {
    result = randomTreeNode(ub);
    nsize = clAdjaNeighborCount(ub->aa, result);
  } while (nsize != 3 );
  return result;
}

static struct DRA *randomKernelNodes(struct UnrootedBinary *ub, int howMany)
{
  struct DRA *da = clDraNew();
  do {
    qbase_t cur = randomKernelNode(ub);
    clDraAddQBIfNew(da, cur);
  } while (clDraSize(da) < howMany);
  return da;
}

static int randomNeighbor(struct UnrootedBinary *ub, qbase_t who)
{
  for (;;) {
    qbase_t result = randomTreeNode(ub);
    if (clAdjaGetConState(ub->aa, who, result))
      return result;
  }
}

static int verifyTree(struct UnrootedBinary *ub)
{
  int i, j;
  int nc;
  struct DRA *result = clUnrootedBinaryNodes(ub, NULL);
  if (clDraSize(result) != ub->nodecount) {
    printf("Error, inconsistent node list with size %d but nodecount %d\n",
      clDraSize(result), ub->nodecount);
    clAdjaPrint(ub->aa);
    for (i = 0; i < ub->nodecount; ++i) {
      for (j = 0; j < i; ++j)
        printf(" ");
      for (j = i+1; j < ub->nodecount; ++j)
        printf("%c", clAdjaGetConState(ub->aa, i, j) + '0');
      printf("\n");
    }
    return 0;
  }
  for (i = 0; i < ub->nodecount; ++i) {
    nc = clAdjaNeighborCount(ub->aa, i);
    if (nc != 1 && nc != 3) {
      int nbp[20], retval;
      int nlenhere = 20;
      printf("Showing bad neighbors...   ************\n");
      retval = clAdjaNeighbors(ub->aa, i, nbp, &nlenhere);
      assert(retval == CL_OK);
      printf("Bad tree with %d neighbors on node %d\n", nc, i);

      return 0;
    }
  }
  clDraFree(result);
  return 1;
}

static void mutateSubtreeTransfer(struct UnrootedBinary *ub)
{
  qbase_t k1, k2, i1, m1, m2, m3;
#define MAXPATHNODES 16384
  static int pbuf[MAXPATHNODES];
  int pathlen, retval;
  int nbufms[MAXNEIGHBORS], nsizems;
  do {
    do {
    k1 = randomTreeNode(ub);
    k2 = randomKernelNode(ub);
    } while (k1 == k2);
    pathlen = MAXPATHNODES;
    retval = clPathFinder(ub->aa, k1, k2, pbuf, &pathlen);
    assert(retval == CL_OK);
    assert(pbuf[0] == k1);
    assert(clAdjaGetConState(ub->aa, k1, pbuf[1]));
  } while (pathlen <= 2);
  i1 = pbuf[1];
  assert(i1 < MAXPATHNODES);
  assert(clAdjaGetConState(ub->aa, k1, i1));
  nsizems = MAXNEIGHBORS;
  retval = clAdjaNeighbors(ub->aa, i1, nbufms, &nsizems);
  assert(nsizems == 3);
  clAdjaSetConState(ub->aa, k1, i1, 0);
  assert(!clAdjaGetConState(ub->aa, k1, i1));

  nsizems = MAXNEIGHBORS;
  retval = clAdjaNeighbors(ub->aa, i1, nbufms, &nsizems);
  assert(retval == CL_OK);
  m1 = nbufms[0];
  m2 = nbufms[1];
  if (nsizems != 2) {
    clLogError( "Warning, got node %d with only %d neighbors, %d...\n",
        i1, nsizems, m1);
    assert(nsizems == 2);
  }
  do {
    m3 = randomNeighbor(ub, k2);
  } while (m3 == pbuf[pathlen-2]);

  clAdjaSetConState(ub->aa, m1, i1, 0);
  clAdjaSetConState(ub->aa, m2, i1, 0);
  clAdjaSetConState(ub->aa, m3, k2, 0);

  clAdjaSetConState(ub->aa, m1, m2, 1);
  clAdjaSetConState(ub->aa, k2, i1, 1);
  clAdjaSetConState(ub->aa, m3, i1, 1);
  clAdjaSetConState(ub->aa, k1, i1, 1);

}

static void mutateSubtreeInterchange(struct UnrootedBinary *ub)
{
  qbase_t i1, i2;
  qbase_t n1, n2;
static  int pathbuf[MAXPATHNODES];
  int pathlen;
  int retval;
  do {
    struct DRA *swappers = randomKernelNodes(ub, 2);
    i1 = clDraGetValueAt(swappers, 0).i;
    i2 = clDraGetValueAt(swappers, 1).i;
    assert(i1 != i2);
    assert(clAdjaNeighborCount(ub->aa, i1) == 3);
    assert(clAdjaNeighborCount(ub->aa, i2) == 3);
    clDraFree(swappers);
    pathlen = MAXPATHNODES;
    retval = clPathFinder(ub->aa, i1, i2, pathbuf, &pathlen);
    assert(retval == CL_OK);
  } while (pathlen <= 3);
  n1 = pathbuf[1];
  assert(clAdjaGetConState(ub->aa, n1, i1));
  n2 = pathbuf[pathlen-2];
  assert(clAdjaGetConState(ub->aa, n2, i2));
  clFlipCrosswise(ub->aa, i1, n1, i2, n2);
}

static void mutateSimple(struct UnrootedBinary *ub)
{
  int c;
#if LOGICWALL
  int wasGood;

  wasGood = verifyTree(ub);
  assert(wasGood);
#endif
  do {
    c = rand() % 3;
  } while ((c == 1 && ub->nodecount <= 10) || (c == 2 && ub->nodecount <= 8));
  switch (c) {
    case 0: clMutateSpecies(ub->aa, ub->labelperm); break;
    case 1: mutateSubtreeInterchange(ub); break;
    case 2: mutateSubtreeTransfer(ub); break;
    default: assert(0 && "bad tree op"); break;
  }
#if LOGICWALL
  wasGood = verifyTree(ub);
  if (wasGood == 0) {
    printf("out mutsim tree problem with c %d\n", c);
    exit(1);
  }
  assert(wasGood);
#endif
}

static int howManyMutations(void)
{
//  if (0 == 0)
//    return clHowManyMutationsTwoMinusExp();
//  else
  return clHowManyMutationsWeirdLogFormula();
}

static void mutateComplex(struct UnrootedBinary *ub)
{
  int mutnum;
  /* TODO: add test for same treeness */
  ub->mc = 0;
  mutnum = howManyMutations();
  while (mutnum > 0) {
    mutnum -= 1;
    mutateSimple(ub);
    ub->mc += 1;
  }
}

int clUnrootedBinaryLastMutationCount(const struct UnrootedBinary *ub)
{
  return ub->mc;
}

void clUnrootedBinaryDoComplexMutation(struct UnrootedBinary *ub)
{
  mutateComplex(ub);
}

struct UnrootedBinary *clUnrootedBinaryNew(int howManyLeaves)
{
  int i;
  struct DRA *leaves;
  struct UnrootedBinary *ub = clCalloc(sizeof(struct UnrootedBinary), 1);
  assert(howManyLeaves > 3);
  ub->nodecount = 2*howManyLeaves-2;
  ub->aa = clNewPathKeeper(clAdjaLoadAdjList(ub->nodecount));
  for (i = 0; i < howManyLeaves-2; ++i) {
    clAdjaSetConState(ub->aa, i, i+howManyLeaves, 1);
    clAdjaSetConState(ub->aa, i, i+1, 1);
  }
  clAdjaSetConState(ub->aa, 0, howManyLeaves-1, 1);

  leaves = getLabellableNodes(ub);
  assert(leaves);
  assert(clDraSize(leaves) == howManyLeaves);
  ub->labelperm = clLabelpermNew(leaves);
  assert(ub->labelperm);

  clDraFree(leaves);

  verifyTree(ub);
  return ub;
}

struct UnrootedBinary *clUnrootedBinaryClone(const struct UnrootedBinary *ub)
{
  struct UnrootedBinary *cp;
  cp = clCalloc(sizeof(*cp), 1);
  cp->nodecount = ub->nodecount;
  cp->startNode = ub->startNode;
  cp->mc = ub->mc;
  cp->aa = clAdjaClone(ub->aa);
  cp->labelperm = clLabelpermClone(ub->labelperm);
  return cp;
}


int clUnrootedBinaryIsQuartetableNode(const struct UnrootedBinary *ub, qbase_t which)
{
  return clAdjaNeighborCount(ub->aa, which) < 3;
}

int clIsFlippableNode(struct UnrootedBinary *ub, qbase_t which)
{
  return clAdjaNeighborCount(ub->aa, which) == 3;
}

qbase_t clUnrootedBinaryStartingNode(const struct UnrootedBinary *ub)
{
  return 0;
}

struct DRA *clUnrootedBinaryNodes(const struct UnrootedBinary *ub, struct CLNodeSet *flips)
{
  union PCTypes p = zeropct;
  struct DRA *result = clDraNew();
  struct DRA *border = clDraNew();
  struct CLNodeSet *done = clNodesetNew(ub->nodecount);
  clDraPush(border, p);
  clWalkTree(clGetAdjAdaptorForUB((struct UnrootedBinary *) ub), result, border, done, 0, flips);
  clDraFree(border);
  clNodesetFree(done);
  return result;
}

struct DRA *clUnrootedBinaryPerimPairs(const struct UnrootedBinary *ub, struct CLNodeSet *flips)
{
  struct DRA *nodes = clUnrootedBinaryNodes(ub, flips);
  struct DRA *pairs = clDraNew();
  union PCTypes p;
  int i;
  int lastval = -1;
  int firstnode = -1;
  for (i = 0;i < clDraSize(nodes); i += 1) {
    int curnode = clDraGetValueAt(nodes, i).i;
    if (clUnrootedBinaryIsQuartetableNode(ub, curnode)) {
      if (firstnode == -1)
        firstnode = curnode;
      if (lastval != -1) {
        p = zeropct;
        p.ip.x = lastval;
        p.ip.y = curnode;
        clDraPush(pairs, p);
      }
      lastval = curnode;
    }
  }
  p.ip.x = lastval;
  p.ip.y = firstnode;
  clDraPush(pairs, p);
  clDraFree(nodes);
  return pairs;
}

void clUnrootedBinaryFree(struct UnrootedBinary *ub)
{
  if (ub->labelperm)
    clLabelpermFree(ub->labelperm);
  ub->labelperm = NULL;
  if (ub->aa)
    clAdjaFree(ub->aa);
  ub->aa = NULL;
  clFreeandclear(ub);
}

static struct DRA *getLabellableNodes(const struct UnrootedBinary *ub)
{
  int i;
  struct DRA *result = clDraNew();
  for (i = 0; i < ub->nodecount; i += 1) {
    if (clUnrootedBinaryIsQuartetableNode(ub, i) == 1) {
      union PCTypes p = zeropct;
      p.i = i;
      clDraPush(result, p);
    }
  }
  return result;
}

struct DRA *clUnrootedBinaryLeafLabels(const struct UnrootedBinary *ub)
{
  struct DRA *result = clDraNew();
  int i;
  for (i = 0; i < clLabelpermSize(ub->labelperm); i += 1) {
    union PCTypes p = zeropct;
    p.i = clLabelpermNodeIDForColIndex(ub->labelperm, i);
    clDraPush(result, p);
  }
  return result;
}

struct LabelPerm *clUnrootedBinaryLabelPerm(struct UnrootedBinary *ub)
{
  return clLabelpermClone(ub->labelperm);
}

struct TreeAdaptor *clUb_treeclone(struct TreeAdaptor *ta)
{
  struct UnrootedBinary *ub = (struct UnrootedBinary *) ta->ptr;
  struct TreeAdaptor *result = clMalloc(sizeof(*result) * 1);
  *result = *ta;
  result->ptr = clUnrootedBinaryClone(ub);
  return result;
}

void clUb_treemutate(struct TreeAdaptor *ta)
{
  struct UnrootedBinary *ub = (struct UnrootedBinary *) ta->ptr;
  mutateComplex(ub);
}

void clUb_treefree(struct TreeAdaptor *ta)
{
  struct UnrootedBinary *ub = (struct UnrootedBinary *) ta->ptr;
  clUnrootedBinaryFree(ub);
  ub = NULL;
  memset(ta, 0, sizeof(*ta));
  clFreeandclear(ta);
}

struct LabelPerm *clUb_treegetlabelperm(struct TreeAdaptor *ta)
{
  struct UnrootedBinary *ub = (struct UnrootedBinary *) ta->ptr;
  return clUnrootedBinaryLabelPerm(ub);
}

struct AdjAdaptor *clUb_treegetadja(struct TreeAdaptor *ta)
{
  struct UnrootedBinary *ub = (struct UnrootedBinary *) ta->ptr;
  return clGetAdjAdaptorForUB(ub);
}

int clUb_treeisquartetable(struct TreeAdaptor *ta, int which)
{
  struct UnrootedBinary *ub = (struct UnrootedBinary *) ta->ptr;
  return clUnrootedBinaryIsQuartetableNode(ub, which);
}

static int ub_treeisroot(struct TreeAdaptor *ta, int which)
{
  return 0;
}

int clUb_treeisflippable(struct TreeAdaptor *ta, int which)
{
  struct UnrootedBinary *ub = (struct UnrootedBinary *) ta->ptr;
  return clIsFlippableNode(ub, which);
}

struct DRA *clUb_treeperimpairs(struct TreeAdaptor *ta, struct CLNodeSet *flips)
{
  struct UnrootedBinary *ub = (struct UnrootedBinary *) ta->ptr;
  return clUnrootedBinaryPerimPairs(ub, flips);
}

int clUb_treemutecount(struct TreeAdaptor *ta)
{
  struct UnrootedBinary *ub = (struct UnrootedBinary *) ta->ptr;
  return clUnrootedBinaryLastMutationCount(ub);
}

struct TreeAdaptor *clTreeaLoadUnrooted(int howBig)
{
  return loadUBTRA(clUnrootedBinaryNew(howBig));
}

static void ub_treesetlabelperm(struct TreeAdaptor *ta, int j, int i)
{
  struct UnrootedBinary *ub = (struct UnrootedBinary *) ta->ptr;
  clUnrootedBinaryLabelPermSetter(ub, j, i);
}

static struct TreeAdaptor *loadUBTRA(struct UnrootedBinary *ub)
{
  struct TreeAdaptor c = {
    ptr:NULL,
    treemutate:clUb_treemutate,
    treefree:clUb_treefree,
    treeclone:clUb_treeclone,
    treegetlabelperm:clUb_treegetlabelperm,
    treegetadja:clUb_treegetadja,
    treeisquartetable:clUb_treeisquartetable,
    treeisflippable:clUb_treeisflippable,
    treeisroot:ub_treeisroot,
    treemutecount:clUb_treemutecount,
    treeperimpairs:clUb_treeperimpairs,
    treelpsetat:ub_treesetlabelperm
  };

  struct TreeAdaptor *result = clMalloc(sizeof(*result) * 1);
  *result = c;
  result->ptr = ub;
  return result;
}

