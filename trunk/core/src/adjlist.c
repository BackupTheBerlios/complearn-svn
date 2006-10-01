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

struct AdjList {
  int size;
  int rowsize;
  qbase_t *adj;
};

static void ajal_free(struct AdjAdaptor *aa);
static int ajal_size(struct AdjAdaptor *aa);
static void ajal_print(struct AdjAdaptor *aa);
static struct AdjAdaptor *ajal_clone(struct AdjAdaptor *aa);
static int ajal_getconstate(struct AdjAdaptor *aa, int i, int j);
static void ajal_setconstate(struct AdjAdaptor *aa, int i, int j, int which);
static int ajal_getneighborcount(struct AdjAdaptor *aa, int i);
static int ajal_getneighbors(struct AdjAdaptor *aa, int i, int *nbuf, int *nsize);

static struct AdjImplementation adimpl = {
  adjafree : ajal_free,
  adjasize : ajal_size,
  adjaprint : ajal_print,
  adjaclone : ajal_clone,
  adjagetconstate : ajal_getconstate,
  adjasetconstate : ajal_setconstate,
  adjagetneighbors : ajal_getneighbors,
  adjagetneighborcount : ajal_getneighborcount,
  adjaspmmap : NULL
};

struct AdjList *clAdjlistNew(int howbig)
{
  int i;
#define MAXDEGREE 10
  struct AdjList *adj = clMalloc(sizeof(struct AdjList) +
                               (sizeof(adj->adj[0]) * howbig * MAXDEGREE*2));
  adj->size = howbig;
  adj->rowsize = MAXDEGREE*2;
  adj->adj = (qbase_t *) ( adj + 1 );
  for (i = 0; i < adj->size; i += 1)
    adj->adj[i*adj->rowsize] = 0;
  return adj;
}

void clAdjlistPrint(const struct AdjList *which)
{
  int i, j;
  for (i = 0; i < which->size; ++i) {
    for (j = 0; j < i; ++j)
      printf(" ");
    for (j = i+1; j < which->size; ++j)
      printf("%c", clAdjlistGetConState(which, i, j) + (int) '0');
    printf("\n");
  }
}
struct AdjList *clAdjlistClone(const struct AdjList *inp)
{
  struct AdjList *adj = clMalloc(sizeof(struct AdjList) + inp->size * inp->rowsize * sizeof(adj->adj[0]));
  adj->size = inp->size;
  adj->rowsize = inp->rowsize;
  adj->adj = (qbase_t *) (adj + 1);
  memcpy(adj->adj, inp->adj, sizeof(adj->adj[0]) * inp->size * inp->rowsize);
  return adj;
}


void clAdjlistFree(struct AdjList *adj)
{
  if (adj->adj != NULL && adj->adj != (qbase_t *) (adj + 1))
    clFreeandclear(adj->adj);
  clFreeandclear(adj);
}

int clAdjlistSize(const struct AdjList *adj)
{
  return adj->size;
}

static void checkBounds(const struct AdjList *adj, int i, int j)
{
  assert(adj != NULL);
  assert(adj->adj != NULL);
  assert(i >= 0);
  assert(j >= 0);
  assert(i < adj->size);
  assert(j < adj->size);
}

int clAdjlistGetConState(const struct AdjList *adj, int i, int j)
{
  int k, nc;
  /*
  if (i > j)
    return clAdjlistGetConState(adj, j, i);
    */
  checkBounds(adj, i, j);
  assert(i >= 0);
  assert(i < adj->size);
  nc = adj->adj[i*adj->rowsize];
  for (k = 0; k < nc; k += 1)
    if (adj->adj[i*adj->rowsize + 1 + k] == j)
      return 1;
/*
  nc = adj->adj[j*adj->rowsize];
  for (k = 0; k < nc; k += 1)
    if (adj->adj[j*adj->rowsize + 1 + k] == i)
      return 1;
*/
  return 0;
}

static int findNeighborIndex(struct AdjList *adj, int i, int j)
{
  int ni, nc;
  nc = adj->adj[i*adj->rowsize];
  for (ni = 0; ni < nc; ni += 1)
    if (adj->adj[ni + i*adj->rowsize + 1] == j)
      return ni;
  assert(0 && "adjlist index error" != NULL);
  return -1;
}

static void removeNeighbor(struct AdjList *adj, int i, int j)
{
  int nc;
  int indj, indi;
  int k;
  /*
  if (i > j) {
    removeNeighbor(adj, j, i);
    return;
  }
  assert(i <= j);
  */
  nc = adj->adj[i*adj->rowsize];
  indj = findNeighborIndex(adj, i, j);
  assert(indj >= 0);
  assert(adj->adj[i*adj->rowsize + indj + 1] == j);
  assert(clAdjlistGetConState(adj, i, j != NULL) == 1);
  for (k = indj; k < nc - 1; k += 1)
    adj->adj[i*adj->rowsize+k+1] = adj->adj[i*adj->rowsize+k+2];
  adj->adj[i*adj->rowsize+k+1] = 0;
  adj->adj[i*adj->rowsize] -= 1;

  nc = adj->adj[j*adj->rowsize];
  indi = findNeighborIndex(adj, j, i);
  for (k = indi; k < nc - 1; k += 1)
    adj->adj[j*adj->rowsize+k+1] = adj->adj[j*adj->rowsize+k+2];
  adj->adj[j*adj->rowsize+k+1] = 0;
  adj->adj[j*adj->rowsize] -= 1;
/*  assert(clAdjlistGetConState(adj, i, j != NULL) == 0); */
}

static void addNeighbor(struct AdjList *adj, int i, int j)
{
  int nc;
/*  assert(i <= j); */
  nc = adj->adj[i*adj->rowsize];
/*
  assert(nc < adj->size);
  assert(clAdjlistGetConState(adj, i, j != NULL) == 0);
  assert(nc < adj->rowsize - 1);
*/
  adj->adj[i*adj->rowsize + nc + 1] = j;
  adj->adj[i*adj->rowsize] += 1;
  nc = adj->adj[j*adj->rowsize];
  adj->adj[j*adj->rowsize + nc + 1] = i;
  adj->adj[j*adj->rowsize] += 1;
/*
  assert(clAdjlistGetConState(adj, i, j != NULL) == 1);
*/
}

void clAdjlistSetConState(struct AdjList *adj, int i, int j, int conState)
{
  int oldConState;
//  printf("Setting connection state for adjlist %p and node %d->%d to %d\n", adj, i, j, conState);
  assert(conState == 0 || conState == 1);
  /*
  if (i > j)
    clAdjlistSetConState(adj, j, i, conState);
  else {
  */
  checkBounds(adj, i, j);
  oldConState = clAdjlistGetConState(adj, i, j);
  if (oldConState == conState)
    return;
  if (conState == 0)
    removeNeighbor(adj, i, j);
  else
    addNeighbor(adj, i, j);
  /*
  }
  */
}

int clAdjlistNeighbors(const struct AdjList *adj, int from, int *nbuf, int *nsize)
{
  int i, j=0;
  int basenum = from * adj->rowsize + 1;
  int ncount = adj->adj[from*adj->rowsize];
  if (ncount > *nsize)
    return CL_ERRFULL;
  for (i = 0; i < ncount; ++i)
    nbuf[j++] = adj->adj[basenum + i];
  *nsize = j;
  return CL_OK;
}

int clAdjlistNeighborCount(const struct AdjList *adj, int from)
{
  int acc;
  assert(from >= 0 && from < adj->size);
  acc = adj->adj[from*adj->rowsize];
  return acc;
}

static int ajal_size(struct AdjAdaptor *aa)
{
  struct AdjList *al = (struct AdjList *) aa->ptr;
  return al->size;
}

static void ajal_free(struct AdjAdaptor *aa)
{
  struct AdjList *al = (struct AdjList *) aa->ptr;
  clAdjlistFree(al);
  aa->ptr = NULL;
  free(aa);
}

static void ajal_print(struct AdjAdaptor *aa)
{
  struct AdjList *al = (struct AdjList *) aa->ptr;
  clAdjlistPrint(al);
}

static struct AdjAdaptor *ajal_clone(struct AdjAdaptor *aa)
{
  struct AdjAdaptor *ab = (struct AdjAdaptor *) clMalloc(sizeof(*aa) * 1);
  *ab = *aa;
  ab->ptr = clAdjlistClone(aa->ptr);
  return ab;
}

static int ajal_getconstate(struct AdjAdaptor *aa, int i, int j)
{
  int retval;
  struct AdjList *al = (struct AdjList *) aa->ptr;
  retval = clAdjlistGetConState(al, i, j);
//  printf("LG?(%d,%d) => %d\n", i, j, retval);
  return retval;
}

static void ajal_setconstate(struct AdjAdaptor *aa, int i, int j, int which)
{
  struct AdjList *al = (struct AdjList *) aa->ptr;
//assert(i != j);
//  printf("LS=(%d,%d) => %d\n", i, j, which);
  clAdjlistSetConState(al, i, j, which);
}

static int ajal_getneighborcount(struct AdjAdaptor *aa, int i)
{
  struct AdjList *al = (struct AdjList *) aa->ptr;
  return clAdjlistNeighborCount(al, i);
}

static int ajal_getneighbors(struct AdjAdaptor *aa, int i, int *nbuf, int *nsize)
{
  struct AdjList *al = (struct AdjList *) aa->ptr;
  return clAdjlistNeighbors(al, i, nbuf, nsize);
}

struct AdjAdaptor *clAdjaLoadAdjList(int howBig)
{
  struct AdjAdaptor *aj;
  aj = clCalloc(sizeof(struct AdjAdaptor), 1);
  aj->ptr = clAdjlistNew(howBig);
  aj->vptr = &adimpl;
  return aj;
}
