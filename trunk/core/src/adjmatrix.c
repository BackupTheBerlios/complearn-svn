#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <malloc.h>
#include <complearn/complearn.h>
#include <complearn/adjadaptor.h>

struct AdjMatrix {
  int size;
  unsigned char *adj;
};

struct AdjMatrix *adjmatrixNew(int howbig)
{
  struct AdjMatrix *adj = clCalloc(sizeof(struct AdjMatrix), 1);
  adj->size = howbig;
  adj->adj = clCalloc(sizeof(adj->adj[0]), howbig*howbig);
  return adj;
}

void adjmatrixPrint(const struct AdjMatrix *which)
{
  int i, j;
  for (i = 0; i < which->size; ++i) {
    for (j = 0; j < i; ++j)
      printf(" ");
    for (j = i+1; j < which->size; ++j)
      printf("%c", adjmatrixGetConState(which, i, j) + '0');
    printf("\n");
  }
}

struct AdjMatrix *adjmatrixClone(const struct AdjMatrix *inp)
{
  struct AdjMatrix *adj = clCalloc(sizeof(struct AdjMatrix), 1);
  adj->size = inp->size;
  adj->adj = clCalloc(sizeof(adj->adj[0]), inp->size*inp->size);
  memcpy(adj->adj, inp->adj, sizeof(adj->adj[0]) * inp->size * inp->size);
  return adj;
}


void adjmatrixFree(struct AdjMatrix *adj)
{
  clFreeandclear(adj->adj);
  clFreeandclear(adj);
}

int adjmatrixSize(const struct AdjMatrix *adj)
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

int adjmatrixGetConState(const struct AdjMatrix *adj, int i, int j)
{
  if (i > j)
    return adjmatrixGetConState(adj, j, i);
  checkBounds(adj, i, j);
  return adj->adj[i*adj->size+j];
}

void adjmatrixSetConState(struct AdjMatrix *adj, int i, int j, int conState)
{
  if (i > j)
    adjmatrixSetConState(adj, j, i, conState);
  else {
    checkBounds(adj, i, j);
    assert(conState == 0 || conState == 1);
    adj->adj[i*adj->size+j] = conState;
    //printf("Set Con state to %d for %d -- %d\n", conState, i, j);
    //adjmatrixPrint(adj);
  }
}

int adjmatrixNeighbors(const struct AdjMatrix *adj, int from, int *nbuf, int *nsize)
{
  int i, j=0;
  for (i = 0; i < adj->size; ++i)
    if (adjmatrixGetConState(adj, from, i)) {
      if (j == *nsize)
        return CL_ERRFULL;
      nbuf[j++] = i;
    }
  *nsize = j;
  return CL_OK;
}

int adjmatrixNeighborCount(const struct AdjMatrix *adj, int from)
{
  int nc = 0;
  int i;
  for (i = 0; i < adj->size; ++i)
    if (adjmatrixGetConState(adj, from, i))
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
  adjmatrixPrint(am);
}

static struct AdjAdaptor *ajam_clone(struct AdjAdaptor *aa)
{
  struct AdjAdaptor *ab = (struct AdjAdaptor *) clCalloc(sizeof(*aa), 1);
  *ab = *aa;
  ab->ptr = adjmatrixClone(aa->ptr);
  return ab;
}

static int ajam_getconstate(struct AdjAdaptor *aa, int i, int j)
{
  int retval;
  struct AdjMatrix *am = (struct AdjMatrix *) aa->ptr;
  retval = adjmatrixGetConState(am, i, j);
//  printf("G?(%d,%d) => %d\n", i, j, retval);
  return retval;
}

static void ajam_setconstate(struct AdjAdaptor *aa, int i, int j, int which)
{
  struct AdjMatrix *am = (struct AdjMatrix *) aa->ptr;
//  printf("S=(%d,%d) => %d\n", i, j, which);
  adjmatrixSetConState(am, i, j, which);
}

static int ajam_getneighborcount(struct AdjAdaptor *aa, int i)
{
  struct AdjMatrix *am = (struct AdjMatrix *) aa->ptr;
  return adjmatrixNeighborCount(am, i);
}

static int ajam_getneighbors(struct AdjAdaptor *aa, int i, int *nbuf, int *nsize)
{
  struct AdjMatrix *am = (struct AdjMatrix *) aa->ptr;
  return adjmatrixNeighbors(am, i, nbuf, nsize);
}

struct AdjAdaptor *adjaLoadAdjMatrix(int howBig)
{
  struct AdjAdaptor *aj;
  aj = clCalloc(sizeof(struct AdjAdaptor), 1);
  aj->ptr = adjmatrixNew(howBig);
  aj->adjafree = ajam_free;
  aj->adjasize = ajam_size;
  aj->adjaprint = ajam_print;
  aj->adjaclone = ajam_clone;
  aj->adjagetconstate = ajam_getconstate;
  aj->adjasetconstate = ajam_setconstate;
  aj->adjagetneighbors = ajam_getneighbors;
  aj->adjagetneighborcount = ajam_getneighborcount;
  /* No spmmap */
  return aj;
}

