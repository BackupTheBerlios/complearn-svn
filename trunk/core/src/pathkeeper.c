#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <malloc.h>
#include <complearn/complearn.h>
#include <complearn/adjadaptor.h>

struct PathKeeper {
  struct AdjAdaptor outer;
  struct AdjAdaptor *basis;
  struct DoubleA *spmmap;
};

int pk_getneighborcount(struct AdjAdaptor *ad, int i)
{
  struct PathKeeper *pk = (struct PathKeeper *) ad->ptr;
  return adjaNeighborCount(pk->basis, i);
}

int pk_getneighbors(struct AdjAdaptor *ad, int i, int *nbuf, int *nsize)
{
  struct PathKeeper *pk = (struct PathKeeper *) ad->ptr;
  return adjaNeighbors(pk->basis, i, nbuf, nsize);
}

void pk_print(struct AdjAdaptor *ad)
{
  struct PathKeeper *pk = (struct PathKeeper *) ad->ptr;
 // printf("PathKeeper around:\n");
  adjaPrint(pk->basis);
}

int pk_getconstate(struct AdjAdaptor *ad, int i, int j)
{
  struct PathKeeper *pk = (struct PathKeeper *) ad->ptr;
  return adjaGetConState(pk->basis, i, j);
}

static void pk_freespmifpresent(struct AdjAdaptor *ad)
{
  struct PathKeeper *pk = (struct PathKeeper *) ad->ptr;
  if (pk->spmmap) {
    freeSPMMap(pk->spmmap);
    pk->spmmap = NULL;
  }
}

void pk_setconstate(struct AdjAdaptor *ad, int i, int j, int which)
{
  struct PathKeeper *pk = (struct PathKeeper *) ad->ptr;
  adjaSetConState(pk->basis, i, j, which);
  pk_freespmifpresent(ad);
}

struct AdjAdaptor *pk_clone(struct AdjAdaptor *ad)
{
  struct PathKeeper *pk = (struct PathKeeper *) ad->ptr;
  struct PathKeeper *pkc = gcalloc(sizeof(*pkc), 1);
  pkc->basis = adjaClone(pk->basis);
  pkc->outer = pk->outer;
  pkc->outer.ptr = pkc;
  return &pkc->outer;
}

int pk_size(struct AdjAdaptor *ad)
{
  struct PathKeeper *pk = (struct PathKeeper *) ad->ptr;
  return adjaSize(pk->basis);
}

struct DoubleA *pk_spmmap(struct AdjAdaptor *ad)
{
  struct PathKeeper *pk = (struct PathKeeper *) ad->ptr;
  if (pk->spmmap == NULL) {
    pk->spmmap = makeSPMMap(ad);
  }
  return pk->spmmap;
}

void pk_free(struct AdjAdaptor *ad)
{
  struct PathKeeper *pk = (struct PathKeeper *) ad->ptr;
  pk_freespmifpresent(ad);
  adjaFree(pk->basis);
  memset(pk, 0, sizeof(*pk));
  gfreeandclear(pk);
}

struct AdjAdaptor *newPathKeeper(struct AdjAdaptor *basis)
{
  struct PathKeeper *pk = gcalloc(sizeof(*pk), 1);
  pk->basis = basis;
  pk->outer.ptr = pk;
  pk->outer.adjaprint = pk_print;
  pk->outer.adjasize = pk_size;
  pk->outer.adjafree = pk_free;
  pk->outer.adjaclone = pk_clone;
  pk->outer.adjagetconstate = pk_getconstate;
  pk->outer.adjasetconstate = pk_setconstate;
  pk->outer.adjagetneighborcount = pk_getneighborcount;
  pk->outer.adjagetneighbors = pk_getneighbors;
  pk->outer.adjaspmmap = pk_spmmap;

  return &pk->outer;
}

int pathFinder(struct AdjAdaptor *ad, qbase_t from, qbase_t to, int *pathbuf, int *bufsize)
{
  int pathlen = 0;
  struct DoubleA *spmmap = adjaSPMMap(ad);
  const struct DoubleA *spm = getValueAt(spmmap, to).ar;
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
    cur = getValueAt(spm, cur).i;
    if (cur >= adjaSize(ad)) {
      printf("Problem with cur for %d\n", pathbuf[pathlen-1]);
    }
    assert(cur < adjaSize(ad));
  }
  while (cur != to);
//  p = zeropct; p.i = to;
  if (pathlen == *bufsize)
    return CL_ERRFULL;
  pathbuf[pathlen] = to;
  pathlen += 1;
//    pushValue(result, p);
#if LOGICWALL
//  assert(getValueAt(result, 0).i == from);
//  assert(getValueAt(result, getSize(result)-1).i == to);
#endif
  *bufsize = pathlen;
  return CL_OK;
}

void freeSPMMap(struct DoubleA *ub)
{
  freeDeepDoubleDoubler(ub, 1);
}

struct DoubleA *makeSPMMap(struct AdjAdaptor *aa)
{
  int i;
  struct DoubleA *result = doubleaNew();
  for (i = 0; i < adjaSize(aa); ++i) {
    union PCTypes p = zeropct;
    p.ar = makeSPMFor(aa, i);
    pushValue(result, p);
  }
  return result;
}

#define PATH_DASH (-1)
#define LENGTH_INIT (9999999)
struct DoubleA *makeSPMFor(struct AdjAdaptor *aa, qbase_t root)
{
  int *path, *length;
  struct DoubleA *todo, *result;
  int i, cur;
  union PCTypes p;
  int retval;
  int nbuf[MAXNEIGHBORS];
  int nsize = MAXNEIGHBORS;
  /* assert(root >= 0); */
//  printf("Making SPM for tree %p and node %d\n", ub, root);
  assert(root < adjaSize(aa));
  path = gcalloc(adjaSize(aa), sizeof(*path));
  length = gcalloc(adjaSize(aa), sizeof(*length));
  result = doubleaNew();
  todo = doubleaNew();
  assert(getSize(todo) < 100);
  for (i = 0; i < adjaSize(aa); i += 1) {
    path[i] = PATH_DASH;
    length[i] = LENGTH_INIT;
  }
  length[root] = 0;
  path[root] = root;
  retval = adjaNeighbors(aa, root, nbuf, &nsize);
  assert(retval == CL_OK);
  for (i = 0; i < nsize; i += 1) {
    int neighbor = nbuf[i];
    assert(neighbor >= 0);
    assert(neighbor < adjaSize(aa));
    assert(adjaGetConState(aa, root, neighbor) == 1);
    p = zeropct;
    p.i = neighbor;
//    printf("Pushing value %d on todo at %p\n", p.i, todo);
    pushValue(todo, p);
    path[neighbor] = root;
    length[neighbor] = 1;
  }
  while (getSize(todo)) {
    nsize = MAXNEIGHBORS;
    cur = shiftDoubleDoubler(todo).i;
    retval = adjaNeighbors(aa, cur, nbuf, &nsize);
    assert(retval == CL_OK);
    for (i = 0; i < nsize; i += 1) {
      int neighbor = nbuf[i];
      if (length[neighbor] > length[cur] + 1) {
        path[neighbor] = cur;
        length[neighbor] = length[cur] + 1;
        p = zeropct;
        p.i = neighbor;
        pushValue(todo, p);
      }
    }
  }
  for (i = 0; i < adjaSize(aa); i += 1) {
    p = zeropct;
    p.i = path[i];
    pushValue(result, p);
  }
  free(path);
  free(length);
  freeDoubleDoubler(todo);

  assert(getSize(result) == adjaSize(aa));
  for (i = 0; i < adjaSize(aa); i += 1) {
    cur = getValueAt(result, i).i;
//    printf("Got value %d at position %d\n", cur, i);
    assert(cur >= 0 && cur < adjaSize(aa));
  }

  return result;
}

static int intcomper(const void *ui1, const void *ui2)
{
  int *i1 = (int *) ui1;
  int *i2 = (int *) ui2;
  return (*i1) - (*i2);
}

struct DoubleA *simpleWalkTree(struct TreeAdaptor *ta, struct CLNodeSet *flips)
{
  union PCTypes p = zeropct;
  struct DoubleA *result = doubleaNew();
  struct DoubleA *border = doubleaNew();
  struct CLNodeSet *done = clnodesetNew(treeGetNodeCountTRA(ta));
  pushValue(border, p);
  walkTree(treegetadjaTRA(ta), result, border, done, 0, flips);
  freeDoubleDoubler(border);
  clnodesetFree(done);
  return result;

}

void walkTree(struct AdjAdaptor *aa,
    struct DoubleA *result, struct DoubleA *border, struct CLNodeSet *done,
    int breadthFirst,
    struct CLNodeSet *flipped)
{
  qbase_t cur;
  while (getSize(border) > 0) {
    if (breadthFirst)
      cur = popDoubleDoubler(border).i;
    else
      cur = shiftDoubleDoubler(border).i;
/*    assert(cur >= 0); */
    assert(cur < adjaSize(aa));
    if (!clnodesetIsNodeInSet(done, cur)) {
      union PCTypes p = zeropct;
      int i;
      int retval;
      struct DoubleA *nb = doubleaNew();
      int nbuf[MAXNEIGHBORS];
      int nsize = MAXNEIGHBORS;
      clnodesetAddNode(done, cur);
      p.i = cur;
      pushValue(result, p);
      retval = adjaNeighbors(aa, cur, nbuf, &nsize);
      assert(retval == CL_OK);
      qsort(nbuf, nsize, sizeof(nbuf[0]), intcomper);
      assert(nsize <= 1 || nbuf[0] < nbuf[1]);
      for (i = 0; i < nsize; ++i) {
        union PCTypes p = zeropct;
        p.i = nbuf[i];
        if (!clnodesetIsNodeInSet(done, p.i))
          pushValue(nb, p);
      }
      if (flipped && clnodesetIsNodeInSet(flipped, cur)) {
        if (nsize < 2) {
          //printf("Warning: bogus flip in flip set: %d\n", cur);
        } else {
          swapValues(nb, 0, 1);
        }
      }
      for (i = 0; i < getSize(nb); ++i)
        unshiftValue(border, getValueAt(nb, i));
      freeDoubleDoubler(nb);
    }
  }
}

int isIdenticalTree(struct AdjAdaptor *ad1, struct LabelPerm *lab1, struct AdjAdaptor *ad2, struct LabelPerm *lab2)
{
  return countTrinaryDifferences(ad1, lab1, ad2, lab2) == 0;
}

int countTrinaryDifferences(struct AdjAdaptor *ad1, struct LabelPerm *lab1, struct AdjAdaptor *ad2, struct LabelPerm *lab2)
{
  int i, j, k, m;
  int spec;
  int acc = 0;
  assert(getSizeLP(lab1) == getSizeLP(lab2));
  spec = getSizeLP(lab1);
  ALLQUARTETS(spec, i, j, k, m) {
    qbase_t lab[4];
    int qi1, qi2;
    lab[0] = i; lab[1] = j; lab[2] = k; lab[3] = m;
    qi1 = findConsistentIndex(ad1, lab1, lab);
    qi2 = findConsistentIndex(ad2, lab2, lab);
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

