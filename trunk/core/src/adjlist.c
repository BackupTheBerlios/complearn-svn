#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <malloc.h>
#include <complearn/complearn.h>

struct AdjList {
  int size;
  int rowsize;
  unsigned char *adj;
};

struct AdjList *adjlistNew(int howbig)
{
  int i;
#define MAXDEGREE 10
  struct AdjList *adj = gmalloc(sizeof(struct AdjList) +
                               (sizeof(adj->adj[0]) * howbig * MAXDEGREE*2));
  adj->size = howbig;
  adj->rowsize = MAXDEGREE*2;
  adj->adj = (unsigned char *) ( adj + 1 );
  for (i = 0; i < adj->size; i += 1)
    adj->adj[i*adj->rowsize] = 0;
  return adj;
}

void adjlistPrint(const struct AdjList *which)
{
  int i, j;
  for (i = 0; i < which->size; ++i) {
    for (j = 0; j < i; ++j)
      printf(" ");
    for (j = i+1; j < which->size; ++j)
      printf("%c", adjlistGetConState(which, i, j) + '0');
    printf("\n");
  }
}
struct AdjList *adjlistClone(const struct AdjList *inp)
{
  struct AdjList *adj = gmalloc(sizeof(struct AdjList) + inp->size * inp->rowsize * sizeof(adj->adj[0]));
  adj->size = inp->size;
  adj->rowsize = inp->rowsize;
  adj->adj = (unsigned char *) (adj + 1);
  memcpy(adj->adj, inp->adj, sizeof(adj->adj[0]) * inp->size * inp->rowsize);
  return adj;
}


void adjlistFree(struct AdjList *adj)
{
  if (adj->adj && adj->adj != (unsigned char *) (adj + 1))
    gfreeandclear(adj->adj);
  gfreeandclear(adj);
}

int adjlistSize(const struct AdjList *adj)
{
  return adj->size;
}

static void checkBounds(const struct AdjList *adj, int i, int j)
{
  assert(adj);
  assert(adj->adj);
  assert(i >= 0);
  assert(j >= 0);
  assert(i < adj->size);
  assert(j < adj->size);
}

int adjlistGetConState(const struct AdjList *adj, int i, int j)
{
  int k, nc;
  /*
  if (i > j)
    return adjlistGetConState(adj, j, i);
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
//  assert(i <= j);
  nc = adj->adj[i*adj->rowsize];
  for (ni = 0; ni < nc; ni += 1)
    if (adj->adj[ni + i*adj->rowsize + 1] == j)
      return ni;
  assert(0 && "adjlist index error");
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
  assert(adjlistGetConState(adj, i, j) == 1);
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
/*  assert(adjlistGetConState(adj, i, j) == 0); */
}

static void addNeighbor(struct AdjList *adj, int i, int j)
{
  int nc;
/*  assert(i <= j); */
  nc = adj->adj[i*adj->rowsize];
/*
  assert(nc < adj->size);
  assert(adjlistGetConState(adj, i, j) == 0);
  assert(nc < adj->rowsize - 1);
*/
  adj->adj[i*adj->rowsize + nc + 1] = j;
  adj->adj[i*adj->rowsize] += 1;
  nc = adj->adj[j*adj->rowsize];
  adj->adj[j*adj->rowsize + nc + 1] = i;
  adj->adj[j*adj->rowsize] += 1;
/*
  assert(adjlistGetConState(adj, i, j) == 1);
*/
}

void adjlistSetConState(struct AdjList *adj, int i, int j, int conState)
{
  int oldConState;
//  printf("Setting connection state for adjlist %p and node %d->%d to %d\n", adj, i, j, conState);
  assert(conState == 0 || conState == 1);
  /*
  if (i > j)
    adjlistSetConState(adj, j, i, conState);
  else {
  */
  checkBounds(adj, i, j);
  oldConState = adjlistGetConState(adj, i, j);
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

int adjlistNeighbors(const struct AdjList *adj, int from, int *nbuf, int *nsize)
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

int adjlistNeighborCount(const struct AdjList *adj, int from)
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
  adjlistFree(al);
  aa->ptr = NULL;
  free(aa);
}

static void ajal_print(struct AdjAdaptor *aa)
{
  struct AdjList *al = (struct AdjList *) aa->ptr;
  adjlistPrint(al);
}

static struct AdjAdaptor *ajal_clone(struct AdjAdaptor *aa)
{
  struct AdjAdaptor *ab = (struct AdjAdaptor *) gmalloc(sizeof(*aa) * 1);
  *ab = *aa;
  ab->ptr = adjlistClone(aa->ptr);
  return ab;
}

static int ajal_getconstate(struct AdjAdaptor *aa, int i, int j)
{
  int retval;
  struct AdjList *al = (struct AdjList *) aa->ptr;
  retval = adjlistGetConState(al, i, j);
//  printf("LG?(%d,%d) => %d\n", i, j, retval);
  return retval;
}

static void ajal_setconstate(struct AdjAdaptor *aa, int i, int j, int which)
{
  struct AdjList *al = (struct AdjList *) aa->ptr;
//assert(i != j);
//  printf("LS=(%d,%d) => %d\n", i, j, which);
  adjlistSetConState(al, i, j, which);
}

static int ajal_getneighborcount(struct AdjAdaptor *aa, int i)
{
  struct AdjList *al = (struct AdjList *) aa->ptr;
  return adjlistNeighborCount(al, i);
}

static int ajal_getneighbors(struct AdjAdaptor *aa, int i, int *nbuf, int *nsize)
{
  struct AdjList *al = (struct AdjList *) aa->ptr;
  return adjlistNeighbors(al, i, nbuf, nsize);
}

struct AdjAdaptor *adjaLoadAdjList(int howBig)
{
  struct AdjAdaptor *aj;
  aj = gcalloc(sizeof(struct AdjAdaptor), 1);
  aj->ptr = adjlistNew(howBig);
  aj->adjafree = ajal_free;
  aj->adjasize = ajal_size;
  aj->adjaprint = ajal_print;
  aj->adjaclone = ajal_clone;
  aj->adjagetconstate = ajal_getconstate;
  aj->adjasetconstate = ajal_setconstate;
  aj->adjagetneighbors = ajal_getneighbors;
  aj->adjagetneighborcount = ajal_getneighborcount;
  /* no adjaspmmap */
  return aj;
}

