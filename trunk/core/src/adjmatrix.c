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

struct AdjMatrix *newAdjMatrix(int howbig)
{
  struct AdjMatrix *adj = gcalloc(sizeof(struct AdjMatrix), 1);
  adj->size = howbig;
  adj->adj = gcalloc(sizeof(adj->adj[0]), howbig*howbig);
  return adj;
}

void printAdjMatrix(const struct AdjMatrix *which)
{
  int i, j;
  for (i = 0; i < which->size; ++i) {
    for (j = 0; j < i; ++j)
      printf(" ");
    for (j = i+1; j < which->size; ++j)
      printf("%c", getConState(which, i, j) + '0');
    printf("\n");
  }
}

struct AdjMatrix *cloneAdjMatrix(const struct AdjMatrix *inp)
{
  struct AdjMatrix *adj = gcalloc(sizeof(struct AdjMatrix), 1);
  adj->size = inp->size;
  adj->adj = gcalloc(sizeof(adj->adj[0]), inp->size*inp->size);
  memcpy(adj->adj, inp->adj, sizeof(adj->adj[0]) * inp->size * inp->size);
  return adj;
}


void freeAdjMatrix(struct AdjMatrix *adj)
{
  gfreeandclear(adj->adj);
  gfreeandclear(adj);
}

int getAMSize(const struct AdjMatrix *adj)
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

int getConState(const struct AdjMatrix *adj, int i, int j)
{
  if (i > j)
    return getConState(adj, j, i);
  checkBounds(adj, i, j);
  return adj->adj[i*adj->size+j];
}

void setConState(struct AdjMatrix *adj, int i, int j, int conState)
{
  if (i > j)
    setConState(adj, j, i, conState);
  else {
    checkBounds(adj, i, j);
    assert(conState == 0 || conState == 1);
    adj->adj[i*adj->size+j] = conState;
    //printf("Set Con state to %d for %d -- %d\n", conState, i, j);
    //printAdjMatrix(adj);
  }
}

int getNeighbors(const struct AdjMatrix *adj, int from, int *nbuf, int *nsize)
{
  int i, j=0;
  for (i = 0; i < adj->size; ++i)
    if (getConState(adj, from, i)) {
      if (j == *nsize)
        return CL_ERRFULL;
      nbuf[j++] = i;
    }
  *nsize = j;
  return CL_OK;
}

int getNeighborCount(const struct AdjMatrix *adj, int from)
{
  int nc = 0;
  int i;
  for (i = 0; i < adj->size; ++i)
    if (getConState(adj, from, i))
      nc += 1;
  return nc;
}

static int ajam_size(struct AdjA *a)
{
  struct AdjMatrix *am = (struct AdjMatrix *) a->ptr;
  return am->size;
}

static void ajam_free(struct AdjA *aa)
{
  struct AdjMatrix *am = (struct AdjMatrix *) aa->ptr;
  free(am);
  aa->ptr = NULL;
  free(aa);
}

static void ajam_print(struct AdjA *aa)
{
  struct AdjMatrix *am = (struct AdjMatrix *) aa->ptr;
  printAdjMatrix(am);
}

static struct AdjA *ajam_clone(struct AdjA *aa)
{
  struct AdjA *ab = (struct AdjA *) gcalloc(sizeof(*aa), 1);
  *ab = *aa;
  ab->ptr = cloneAdjMatrix(aa->ptr);
  return ab;
}

static int ajam_getconstate(struct AdjA *aa, int i, int j)
{
  int retval;
  struct AdjMatrix *am = (struct AdjMatrix *) aa->ptr;
  retval = getConState(am, i, j);
//  printf("G?(%d,%d) => %d\n", i, j, retval);
  return retval;
}

static void ajam_setconstate(struct AdjA *aa, int i, int j, int which)
{
  struct AdjMatrix *am = (struct AdjMatrix *) aa->ptr;
//  printf("S=(%d,%d) => %d\n", i, j, which);
  setConState(am, i, j, which);
}

static int ajam_getneighborcount(struct AdjA *aa, int i)
{
  struct AdjMatrix *am = (struct AdjMatrix *) aa->ptr;
  return getNeighborCount(am, i);
}

static int ajam_getneighbors(struct AdjA *aa, int i, int *nbuf, int *nsize)
{
  struct AdjMatrix *am = (struct AdjMatrix *) aa->ptr;
  return getNeighbors(am, i, nbuf, nsize);
}

struct AdjA *loadAdaptorAM(int howBig)
{
  struct AdjA *aj;
  aj = gcalloc(sizeof(struct AdjA), 1);
  aj->ptr = newAdjMatrix(howBig);
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

