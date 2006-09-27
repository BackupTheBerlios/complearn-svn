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
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <complearn/complearn.h>
#include <complearn/adjadaptor.h>

struct PathKeeper {
  struct AdjAdaptor outer;
  struct AdjAdaptor *basis;
  struct DRA *spmmap;
};

static void pk_free(struct AdjAdaptor *aa);
static int pk_size(struct AdjAdaptor *aa);
static void pk_print(struct AdjAdaptor *aa);
static struct AdjAdaptor *pk_clone(struct AdjAdaptor *aa);
static int pk_getconstate(struct AdjAdaptor *aa, int i, int j);
static void pk_setconstate(struct AdjAdaptor *aa, int i, int j, int which);
static int pk_getneighborcount(struct AdjAdaptor *aa, int i);
static int pk_getneighbors(struct AdjAdaptor *aa, int i, int *nbuf, int *nsize);
static struct DRA *pk_spmmap(struct AdjAdaptor *ad);

static struct AdjImplementation pkimpl = {
  adjafree : pk_free,
  adjasize : pk_size,
  adjaprint : pk_print,
  adjaclone : pk_clone,
  adjagetconstate : pk_getconstate,
  adjasetconstate : pk_setconstate,
  adjagetneighbors : pk_getneighbors,
  adjagetneighborcount : pk_getneighborcount,
  adjaspmmap : pk_spmmap,
};

int pk_getneighborcount(struct AdjAdaptor *ad, int i)
{
  struct PathKeeper *pk = (struct PathKeeper *) ad->ptr;
  return clAdjaNeighborCount(pk->basis, i);
}

int pk_getneighbors(struct AdjAdaptor *ad, int i, int *nbuf, int *nsize)
{
  struct PathKeeper *pk = (struct PathKeeper *) ad->ptr;
  return clAdjaNeighbors(pk->basis, i, nbuf, nsize);
}

void pk_print(struct AdjAdaptor *ad)
{
  struct PathKeeper *pk = (struct PathKeeper *) ad->ptr;
 // printf("PathKeeper around:\n");
  clAdjaPrint(pk->basis);
}

int pk_getconstate(struct AdjAdaptor *ad, int i, int j)
{
  struct PathKeeper *pk = (struct PathKeeper *) ad->ptr;
  return clAdjaGetConState(pk->basis, i, j);
}

static void pk_freespmifpresent(struct AdjAdaptor *ad)
{
  struct PathKeeper *pk = (struct PathKeeper *) ad->ptr;
  if (pk->spmmap) {
    clFreeSPMMap(pk->spmmap);
    pk->spmmap = NULL;
  }
}

void pk_setconstate(struct AdjAdaptor *ad, int i, int j, int which)
{
  struct PathKeeper *pk = (struct PathKeeper *) ad->ptr;
  clAdjaSetConState(pk->basis, i, j, which);
  pk_freespmifpresent(ad);
}

struct AdjAdaptor *pk_clone(struct AdjAdaptor *ad)
{
  struct PathKeeper *pk = (struct PathKeeper *) ad->ptr;
  struct PathKeeper *pkc = clCalloc(sizeof(*pkc), 1);
  pkc->basis = clAdjaClone(pk->basis);
  pkc->outer = pk->outer;
  pkc->outer.ptr = pkc;
  return &pkc->outer;
}

int pk_size(struct AdjAdaptor *ad)
{
  struct PathKeeper *pk = (struct PathKeeper *) ad->ptr;
  return clAdjaSize(pk->basis);
}

static struct DRA *pk_spmmap(struct AdjAdaptor *ad)
{
  struct PathKeeper *pk = (struct PathKeeper *) ad->ptr;
  if (pk->spmmap == NULL) {
    pk->spmmap = clMakeSPMMap(ad);
  }
  return pk->spmmap;
}

void pk_free(struct AdjAdaptor *ad)
{
  struct PathKeeper *pk = (struct PathKeeper *) ad->ptr;
  pk_freespmifpresent(ad);
  clAdjaFree(pk->basis);
  memset(pk, 0, sizeof(*pk));
  clFreeandclear(pk);
}

struct AdjAdaptor *clNewPathKeeper(struct AdjAdaptor *basis)
{
  struct PathKeeper *pk = clCalloc(sizeof(*pk), 1);
  pk->basis = basis;
  pk->outer.ptr = pk;
  pk->outer.vptr = &pkimpl;
  return &pk->outer;
}

int clPathFinder(struct AdjAdaptor *ad, qbase_t from, qbase_t to, int *pathbuf, int *bufsize)
{
  int pathlen = 0;
  struct DRA *spmmap = clAdjaSPMMap(ad);
  const struct DRA *spm = clDraGetValueAt(spmmap, to).ar;
  qbase_t cur;
  cur = from;
  //printf("Got path request from %d to %d with bufsize %d:  ", from, to, *bufsize);
  if (from == to) {
    //printf("These nodes are equal.\n");
    if (*bufsize < 1)
      return CL_ERRFULL;
    pathbuf[0] = from;
    *bufsize = 1;
    return CL_OK;
  }
  else {
    //printf("These nodes are unequal.\n");
  }
  assert(ad);
  do {
    if (pathlen == *bufsize)
      return CL_ERRFULL;
    pathbuf[pathlen] = cur;
    pathlen += 1;
    cur = clDraGetValueAt(spm, cur).i;
    if (cur >= clAdjaSize(ad)) {
      printf("Problem with cur for %d\n", pathbuf[pathlen-1]);
    }
    assert(cur < clAdjaSize(ad));
  }
  while (cur != to);
//  p = zeropct; p.i = to;
  if (pathlen == *bufsize)
    return CL_ERRFULL;
  pathbuf[pathlen] = to;
  pathlen += 1;
//    clDraPush(result, p);
#if LOGICWALL
//  assert(clDraGetValueAt(result, 0).i == from);
//  assert(clDraGetValueAt(result, clDraSize(result)-1).i == to);
#endif
  *bufsize = pathlen;
  return CL_OK;
}

void clFreeSPMMap(struct DRA *ub)
{
  clDraDeepFree(ub, 1);
}

struct DRA *clMakeSPMMap(struct AdjAdaptor *aa)
{
  int i;
  struct DRA *result = clDraNew();
  for (i = 0; i < clAdjaSize(aa); ++i) {
    union PCTypes p = zeropct;
    p.ar = clMakeSPMFor(aa, i);
    clDraPush(result, p);
  }
  return result;
}

#define PATH_DASH (-1)
#define LENGTH_INIT (9999999)
struct DRA *clMakeSPMFor(struct AdjAdaptor *aa, qbase_t root)
{
  int *path, *length;
  struct DRA *todo, *result;
  int i, cur;
  union PCTypes p;
  int retval;
  int nbuf[MAXNEIGHBORS];
  int nsize = MAXNEIGHBORS;
  /* assert(root >= 0); */
//  printf("Making SPM for tree %p and node %d\n", ub, root);
  assert(root < clAdjaSize(aa));
  path = clCalloc(clAdjaSize(aa), sizeof(*path));
  length = clCalloc(clAdjaSize(aa), sizeof(*length));
  result = clDraNew();
  todo = clDraNew();
  assert(clDraSize(todo) < 100);
  for (i = 0; i < clAdjaSize(aa); i += 1) {
    path[i] = PATH_DASH;
    length[i] = LENGTH_INIT;
  }
  length[root] = 0;
  path[root] = root;
  retval = clAdjaNeighbors(aa, root, nbuf, &nsize);
  assert(retval == CL_OK);
  for (i = 0; i < nsize; i += 1) {
    int neighbor = nbuf[i];
    assert(neighbor >= 0);
    assert(neighbor < clAdjaSize(aa));
    assert(clAdjaGetConState(aa, root, neighbor) == 1);
    p = zeropct;
    p.i = neighbor;
//    printf("Pushing value %d on todo at %p\n", p.i, todo);
    clDraPush(todo, p);
    path[neighbor] = root;
    length[neighbor] = 1;
  }
  while (clDraSize(todo)) {
    nsize = MAXNEIGHBORS;
    cur = clDraShift(todo).i;
    retval = clAdjaNeighbors(aa, cur, nbuf, &nsize);
    assert(retval == CL_OK);
    for (i = 0; i < nsize; i += 1) {
      int neighbor = nbuf[i];
      if (length[neighbor] > length[cur] + 1) {
        path[neighbor] = cur;
        length[neighbor] = length[cur] + 1;
        p = zeropct;
        p.i = neighbor;
        clDraPush(todo, p);
      }
    }
  }
  for (i = 0; i < clAdjaSize(aa); i += 1) {
    p = zeropct;
    p.i = path[i];
    clDraPush(result, p);
  }
  free(path);
  free(length);
  clDraFree(todo);

  assert(clDraSize(result) == clAdjaSize(aa));
  for (i = 0; i < clAdjaSize(aa); i += 1) {
    cur = clDraGetValueAt(result, i).i;
//    printf("Got value %d at position %d\n", cur, i);
    assert(cur >= 0 && cur < clAdjaSize(aa));
  }

  return result;
}

static int intcomper(const void *ui1, const void *ui2)
{
  int *i1 = (int *) ui1;
  int *i2 = (int *) ui2;
  return (*i1) - (*i2);
}

struct DRA *clSimpleWalkTree(struct TreeAdaptor *ta, struct CLNodeSet *flips)
{
  union PCTypes p = zeropct;
  struct DRA *result = clDraNew();
  struct DRA *border = clDraNew();
  struct CLNodeSet *done = clNodesetNew(clTreeaNodeCount(ta));
  clDraPush(border, p);
  clWalkTree(clTreeaAdjAdaptor(ta), result, border, done, 0, flips);
  clDraFree(border);
  clNodesetFree(done);
  return result;

}

void clWalkTree(struct AdjAdaptor *aa,
    struct DRA *result, struct DRA *border, struct CLNodeSet *done,
    int breadthFirst,
    struct CLNodeSet *flipped)
{
  qbase_t cur;
  while (clDraSize(border) > 0) {
    if (breadthFirst)
      cur = clDraPop(border).i;
    else
      cur = clDraShift(border).i;
/*    assert(cur >= 0); */
    assert(cur < clAdjaSize(aa));
    if (!clNodesetHasNode(done, cur)) {
      union PCTypes p = zeropct;
      int i;
      int retval;
      struct DRA *nb = clDraNew();
      int nbuf[MAXNEIGHBORS];
      int nsize = MAXNEIGHBORS;
      clNodesetAddNode(done, cur);
      p.i = cur;
      clDraPush(result, p);
      retval = clAdjaNeighbors(aa, cur, nbuf, &nsize);
      assert(retval == CL_OK);
      qsort(nbuf, nsize, sizeof(nbuf[0]), intcomper);
      assert(nsize <= 1 || nbuf[0] < nbuf[1]);
      for (i = 0; i < nsize; ++i) {
        union PCTypes p = zeropct;
        p.i = nbuf[i];
        if (!clNodesetHasNode(done, p.i))
          clDraPush(nb, p);
      }
      if (flipped && clNodesetHasNode(flipped, cur)) {
        if (nsize < 2) {
          //printf("Warning: bogus flip in flip set: %d\n", cur);
        } else {
          clDraSwapAt(nb, 0, 1);
        }
      }
      for (i = 0; i < clDraSize(nb); ++i)
        clDraUnshift(border, clDraGetValueAt(nb, i));
      clDraFree(nb);
    }
  }
}

int clIsIdenticalTree(struct AdjAdaptor *ad1, struct LabelPerm *lab1, struct AdjAdaptor *ad2, struct LabelPerm *lab2)
{
  return clCountTrinaryDifferences(ad1, lab1, ad2, lab2) == 0;
}

int clCountTrinaryDifferences(struct AdjAdaptor *ad1, struct LabelPerm *lab1, struct AdjAdaptor *ad2, struct LabelPerm *lab2)
{
  int i, j, k, m;
  int spec;
  int acc = 0;
  assert(clLabelpermSize(lab1) == clLabelpermSize(lab2));
  spec = clLabelpermSize(lab1);
  ALLQUARTETS(spec, i, j, k, m) {
    qbase_t lab[4];
    int qi1, qi2;
    lab[0] = i; lab[1] = j; lab[2] = k; lab[3] = m;
    qi1 = clFindConsistentIndex(ad1, lab1, lab);
    qi2 = clFindConsistentIndex(ad2, lab2, lab);
    if (qi1 != qi2) {
//      printf("Found different quartet %d and %d between %d, %d, %d, %d\n", qi1, qi2, i, j, k, m);
      acc += 1;
    }
    else {
//      printf("Consistent quartet: %d and %d\n", qi1, qi2);
    }
  }
  return acc;
}

