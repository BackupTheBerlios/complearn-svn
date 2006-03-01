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

struct AdjMatrix {
  int size;
  qbase_t *adj;
};

static void ajam_free(struct AdjAdaptor *aa);
static int ajam_size(struct AdjAdaptor *aa);
static void ajam_print(struct AdjAdaptor *aa);
static struct AdjAdaptor *ajam_clone(struct AdjAdaptor *aa);
static int ajam_getconstate(struct AdjAdaptor *aa, int i, int j);
static void ajam_setconstate(struct AdjAdaptor *aa, int i, int j, int which);
static int ajam_getneighborcount(struct AdjAdaptor *aa, int i);
static int ajam_getneighbors(struct AdjAdaptor *aa, int i, int *nbuf, int *nsize);

static struct AdjImplementation adimpl = {
  adjafree : ajam_free,
  adjasize : ajam_size,
  adjaprint : ajam_print,
  adclJaclone : ajam_clone,
  adjagetconstate : ajam_getconstate,
  adjasetconstate : ajam_setconstate,
  adjagetneighbors : ajam_getneighbors,
  adjagetneighborcount : ajam_getneighborcount
};

struct AdjMatrix *clAdjmatrixNew(int howbig)
{
  struct AdjMatrix *adj = clCalloc(sizeof(struct AdjMatrix), 1);
  adj->size = howbig;
  adj->adj = clCalloc(sizeof(adj->adj[0]), howbig*howbig);
  return adj;
}

void clAdjmatrixPrint(const struct AdjMatrix *which)
{
  int i, j;
  for (i = 0; i < which->size; ++i) {
    for (j = 0; j < i; ++j)
      printf(" ");
    for (j = i+1; j < which->size; ++j)
      printf("%c", clAdjmatrixGetConState(which, i, j) + '0');
    printf("\n");
  }
}

struct AdjMatrix *clAdjmatrixClone(const struct AdjMatrix *inp)
{
  struct AdjMatrix *adj = clCalloc(sizeof(struct AdjMatrix), 1);
  adj->size = inp->size;
  adj->adj = clCalloc(sizeof(adj->adj[0]), inp->size*inp->size);
  memcpy(adj->adj, inp->adj, sizeof(adj->adj[0]) * inp->size * inp->size);
  return adj;
}


void clAdjmatrixFree(struct AdjMatrix *adj)
{
  clFreeandclear(adj->adj);
  clFreeandclear(adj);
}

int clAdjmatrixSize(const struct AdjMatrix *adj)
{
  return adj->size;
}

static void checkBounds(const struct AdjMatrix *adj, int i, int j)
{
  assert(adj);
  assert(adj->adj);
  assert(i >= 0);
  assert(j >= 0);
  assert(i < adj->size);
  assert(j < adj->size);
}

int clAdjmatrixGetConState(const struct AdjMatrix *adj, int i, int j)
{
  if (i > j)
    return clAdjmatrixGetConState(adj, j, i);
  checkBounds(adj, i, j);
  return adj->adj[i*adj->size+j];
}

void clAdjmatrixSetConState(struct AdjMatrix *adj, int i, int j, int conState)
{
  if (i > j)
    clAdjmatrixSetConState(adj, j, i, conState);
  else {
    checkBounds(adj, i, j);
    assert(conState == 0 || conState == 1);
    adj->adj[i*adj->size+j] = conState;
    //printf("Set Con state to %d for %d -- %d\n", conState, i, j);
    //clAdjmatrixPrint(adj);
  }
}

int clAdjmatrixNeighbors(const struct AdjMatrix *adj, int from, int *nbuf, int *nsize)
{
  int i, j=0;
  for (i = 0; i < adj->size; ++i)
    if (clAdjmatrixGetConState(adj, from, i)) {
      if (j == *nsize)
        return CL_ERRFULL;
      nbuf[j++] = i;
    }
  *nsize = j;
  return CL_OK;
}

int clAdjmatrixNeighborCount(const struct AdjMatrix *adj, int from)
{
  int nc = 0;
  int i;
  for (i = 0; i < adj->size; ++i)
    if (clAdjmatrixGetConState(adj, from, i))
      nc += 1;
  return nc;
}

static int ajam_size(struct AdjAdaptor *a)
{
  struct AdjMatrix *am = (struct AdjMatrix *) a->ptr;
  return am->size;
}

static void ajam_free(struct AdjAdaptor *aa)
{
  struct AdjMatrix *am = (struct AdjMatrix *) aa->ptr;
  free(am);
  aa->ptr = NULL;
  free(aa);
}

static void ajam_print(struct AdjAdaptor *aa)
{
  struct AdjMatrix *am = (struct AdjMatrix *) aa->ptr;
  clAdjmatrixPrint(am);
}

static struct AdjAdaptor *ajam_clone(struct AdjAdaptor *aa)
{
  struct AdjAdaptor *ab = (struct AdjAdaptor *) clCalloc(sizeof(*aa), 1);
  *ab = *aa;
  ab->ptr = clAdjmatrixClone(aa->ptr);
  return ab;
}

static int ajam_getconstate(struct AdjAdaptor *aa, int i, int j)
{
  int retval;
  struct AdjMatrix *am = (struct AdjMatrix *) aa->ptr;
  retval = clAdjmatrixGetConState(am, i, j);
//  printf("G?(%d,%d) => %d\n", i, j, retval);
  return retval;
}

static void ajam_setconstate(struct AdjAdaptor *aa, int i, int j, int which)
{
  struct AdjMatrix *am = (struct AdjMatrix *) aa->ptr;
//  printf("S=(%d,%d) => %d\n", i, j, which);
  clAdjmatrixSetConState(am, i, j, which);
}

static int ajam_getneighborcount(struct AdjAdaptor *aa, int i)
{
  struct AdjMatrix *am = (struct AdjMatrix *) aa->ptr;
  return clAdjmatrixNeighborCount(am, i);
}

static int ajam_getneighbors(struct AdjAdaptor *aa, int i, int *nbuf, int *nsize)
{
  struct AdjMatrix *am = (struct AdjMatrix *) aa->ptr;
  return clAdjmatrixNeighbors(am, i, nbuf, nsize);
}

struct AdjAdaptor *clAdjaLoadAdjMatrix(int howBig)
{
  struct AdjAdaptor *aj;
  aj = clCalloc(sizeof(struct AdjAdaptor), 1);
  aj->ptr = clAdjmatrixNew(howBig);
  aj->vptr = &adimpl;

  return aj;
}

