#include <complearn/complearn.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <assert.h>

#include <gsl/gsl_randist.h>

static struct TreeAdaptor *loadRBTRA(struct RootedBinary *rb);
static struct DRA *getLabellableNodes(const struct RootedBinary *rb);

struct RootedBinary {
  int nodecount;
  qbase_t root;
  int mc;
  struct LabelPerm *labelperm;
  struct AdjAdaptor *aa;
};

struct AdjAdaptor *clRootedbinaryAdjAdaptor(struct RootedBinary *rb)
{
  return rb->aa;
}

static qbase_t randomTreeNode(struct RootedBinary *rb)
{
  return rand() % rb->nodecount;
}

static qbase_t randomKernelNode(struct RootedBinary *rb)
{
  qbase_t result;
  int nsize;
  do {
    result = randomTreeNode(rb);
    nsize = clAdjaNeighborCount(rb->aa, result);
  } while (nsize == 1);
  return result;
}

static int randomNeighbor(struct RootedBinary *rb, qbase_t who)
{
  for (;;) {
    qbase_t result = randomTreeNode(rb);
    if (clAdjaGetConState(rb->aa, who, result))
      return result;
  }
}

static int verifyTree(struct RootedBinary *rb)
{
  int i, j;
  int nc;
  struct DRA *result = clRootedbinaryNodes(rb);
  if (clDraSize(result) != rb->nodecount) {
    printf("Error, inconsistent node list with size %d but nodecount %d\n",
      clDraSize(result), rb->nodecount);
    clAdjaPrint(rb->aa);
    for (i = 0; i < rb->nodecount; ++i) {
      for (j = 0; j < i; ++j)
        printf(" ");
      for (j = i+1; j < rb->nodecount; ++j)
        printf("%c", clAdjaGetConState(rb->aa, i, j) + '0');
      printf("\n");
    }
    return 0;
  }
  for (i = 0; i < rb->nodecount; ++i) {
    nc = clAdjaNeighborCount(rb->aa, i);
    if ((nc != 1 && nc != 3 && i != rb->root) || (nc != 2 && i == rb->root)) {
      int nbp[20], retval;
      int nlenhere = 20;
      printf("Showing bad neighbors...   ************\n");
      retval = clAdjaNeighbors(rb->aa, i, nbp, &nlenhere);
      assert(retval == CL_OK);
      printf("Bad tree with %d neighbors on node %d\n", nc, i);

      return 0;
    }
  }
  clDraFree(result);
  return 1;
}

static void mutateSubtreeTransfer(struct RootedBinary *rb)
{
  qbase_t k1, k2, i1, m1, m2, m3;
#define MAXPATHNODES 128
  static int pbuf[MAXPATHNODES];
  int pathlen, retval;
  int nbufms[MAXNEIGHBORS], nsizems;
tryagain:
  do {
    do {
    k1 = randomTreeNode(rb);
    k2 = randomKernelNode(rb);
    } while (k1 == rb->root || k1 == k2);
    pathlen = MAXPATHNODES;
    retval = clPathFinder(rb->aa, k1, k2, pbuf, &pathlen);
    assert(retval == CL_OK);
    assert(pbuf[0] == k1);
    assert(clAdjaGetConState(rb->aa, k1, pbuf[1]));
  } while (pathlen <= 2);
  i1 = pbuf[1];
  assert(i1 < MAXPATHNODES);
  assert(clAdjaGetConState(rb->aa, k1, i1));
  nsizems = MAXNEIGHBORS;

  retval = clAdjaNeighbors(rb->aa, i1, nbufms, &nsizems);
  assert(retval == CL_OK);
  if (nsizems < 3)
    goto tryagain;
  clAdjaSetConState(rb->aa, k1, i1, 0);
  retval = clAdjaNeighbors(rb->aa, i1, nbufms, &nsizems);
  assert(retval == CL_OK);
  m1 = nbufms[0];
  m2 = nbufms[1];
  if (nsizems < 2) {
    clogError( "Warning, got node %d with too many (%d) neighbors, %d...\n",
        i1, nsizems, m1);
    assert(nsizems == 2);
  }
  do {
    m3 = randomNeighbor(rb, k2);
  } while (m3 == pbuf[pathlen-2]);

  clAdjaSetConState(rb->aa, m1, i1, 0);
  clAdjaSetConState(rb->aa, m2, i1, 0);
  clAdjaSetConState(rb->aa, m3, k2, 0);

  clAdjaSetConState(rb->aa, m1, m2, 1);
  clAdjaSetConState(rb->aa, k2, i1, 1);
  clAdjaSetConState(rb->aa, m3, i1, 1);
  clAdjaSetConState(rb->aa, k1, i1, 1);

}

static void mutateSubtreeInterchange(struct RootedBinary *rb)
{
  qbase_t i1, i2;
  qbase_t n1, n2;
  int pathbuf[MAXPATHNODES];
  int pathlen;
  int retval;
tryagain:
  do {
    do {
      i1 = randomKernelNode(rb);
      i2 = randomKernelNode(rb);
    } while (i1 == i2 || i1 == rb->root || i2 == rb->root);
    pathlen = MAXPATHNODES;
    retval = clPathFinder(rb->aa, i1, i2, pathbuf, &pathlen);
    assert(retval == CL_OK);
  } while (pathlen <= 2);
  n1 = pathbuf[1];
  if (n1 == rb->root)
    goto tryagain;
  assert(clAdjaGetConState(rb->aa, n1, i1));
  n2 = pathbuf[pathlen-2];
  if (n2 == rb->root)
    goto tryagain;
  assert(clAdjaGetConState(rb->aa, n2, i2));
  clFlipCrosswise(rb->aa, i1, n1, i2, n2);
}

static void mutateSimple(struct RootedBinary *rb)
{
  int c;
#if LOGICWALL
  int wasGood;

  wasGood = verifyTree(rb);
  assert(wasGood);
#endif
  do {
    c = rand() % 3;
  } while ((c == 1 && rb->nodecount <= 10) || (c == 2 && rb->nodecount < 8));
//  printf("About to do mutation %d\n", c);
//  printf("Doing MUTATION %d (%p)\n", c, rb);
  switch (c) {
    case 0: clMutateSpecies(rb->aa, rb->labelperm); break;
    case 1: mutateSubtreeInterchange(rb); break;
    case 2: mutateSubtreeTransfer(rb); break;
    default: assert(0 && "bad tree op"); break;
  }
#if LOGICWALL
  wasGood = verifyTree(rb);
  if (wasGood == 0) {
    printf("out mutsim tree problem with c %d\n", c);
    exit(1);
  }
//  printf("Did finish to do mutation %d\n", c);
  assert(wasGood);
//  printf("DONE MUTATION %d (%p)\n", c, rb);
#endif
}

static int howManyMutations(void)
{
  if (0 == 0)
    return clHowManyMutationsTwoMinusExp();
  else
    return clHowManyMutationsWeirdLogFormula();
}

static void mutateComplex(struct RootedBinary *rb)
{
  int mutnum;
  /* TODO: add test for same treeness */
  rb->mc = 0;
  mutnum = howManyMutations();
  while (mutnum > 0) {
    mutnum -= 1;
    mutateSimple(rb);
    rb->mc += 1;
  }
}

int clRootedbinaryLastMutationCount(const struct RootedBinary *rb)
{
  return rb->mc;
}

void clRootedbinaryComplexMutation(struct RootedBinary *rb)
{
  mutateComplex(rb);
}

void clRootedbinaryLabelPermSetter(struct RootedBinary *rb, int j, int i)
{
  clLabelpermSetColumnIndexToNodeNumber(rb->labelperm, j, i);
}

struct RootedBinary *clRootedbinaryNew(int howManyLeaves, struct AdjAdaptor *uaa, struct LabelPerm *ulabelperm)
{
  int i;
  struct DRA *leaves;
  struct RootedBinary *rb = clCalloc(sizeof(struct RootedBinary), 1);

  assert(howManyLeaves > 3);

  rb->nodecount = 2*howManyLeaves-3;

  if (uaa) {
    rb->aa = clNewPathKeeper(uaa);
  }
    else {
      rb->aa = clNewPathKeeper(clAdjaLoadAdjList(rb->nodecount));
    for (i = 0; i < howManyLeaves-2; ++i) {
      clAdjaSetConState(rb->aa, i, i+howManyLeaves-1, 1);
      clAdjaSetConState(rb->aa, i, i+1, 1);
    }
  }

  leaves = getLabellableNodes(rb);
  assert(clDraSize(leaves) == howManyLeaves);

  if (! (rb->labelperm = ulabelperm))
    rb->labelperm = clLabelpermNew(leaves);

  clDraFree(leaves);

  verifyTree(rb);
  return rb;
}

struct RootedBinary *clRootedbinaryClone(const struct RootedBinary *rb)
{
  struct RootedBinary *cp;
  cp = clCalloc(sizeof(*cp), 1);
  cp->nodecount = rb->nodecount;
  cp->root = rb->root;
  cp->mc = rb->mc;
  cp->nodecount = rb->nodecount;
  cp->aa = clAdjaClone(rb->aa);
  cp->labelperm = clLabelpermClone(rb->labelperm);
  return cp;
}


int clRootedbinaryIsQuartetableNode(const struct RootedBinary *rb, qbase_t which)
{
  return clAdjaNeighborCount(rb->aa, which) < 3;
}

int clRootedbinaryIsFlippableNode(struct RootedBinary *rb, qbase_t which)
{
  int nc = clAdjaNeighborCount(rb->aa, which);
// return nc != 1;
  return nc == 2 || nc == 3;
}

qbase_t clRootedbinaryStartingNode(const struct RootedBinary *rb)
{
  return 0;
}

struct DRA *clRootedbinaryNodes(const struct RootedBinary *rb)
{
  union PCTypes p = zeropct;
  struct DRA *result = clDraNew();
  struct DRA *border = clDraNew();
  struct CLNodeSet *done = clnodesetNew(rb->nodecount);
  clDraPush(border, p);
  clWalkTree(clRootedbinaryAdjAdaptor((struct RootedBinary *) rb), result, border, done, 0, NULL);
  clDraFree(border);
  clnodesetFree(done);
  return result;
}

struct DRA *clRootedbinaryPerimeterPairs(const struct RootedBinary *rb, struct CLNodeSet *flips)
{
  struct DRA *pairs = clDraNew();
  union PCTypes p = zeropct;
  int i;
  int lastval = -1;
  int firstnode = -1;
  struct DRA *traversalseq = clDraNew();
  struct DRA *border = clDraNew();
  struct CLNodeSet *done = clnodesetNew(rb->nodecount);
  p.i = rb->root; /* start at root */
  clDraPush(border, p);
  clWalkTree(clRootedbinaryAdjAdaptor((struct RootedBinary *) rb), traversalseq, border, done, 0, flips);

  for (i = 0;i < clDraSize(traversalseq); i += 1) {
    int curnode = clDraGetValueAt(traversalseq, i).i;
    if (curnode == rb->root)
      continue;
    if (clRootedbinaryIsQuartetableNode(rb, curnode)) {
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
  clDraFree(traversalseq);
  return pairs;
}

void clRootedbinaryFreeRB(struct RootedBinary *rb)
{
  clLabelpermFree(rb->labelperm);
  clAdjaFree(rb->aa);
  rb->aa = NULL;
  clFreeandclear(rb);
}

static struct DRA *getLabellableNodes(const struct RootedBinary *rb)
{
  int i;
  struct DRA *result = clDraNew();
  for (i = 0; i < rb->nodecount; i += 1) {
    if (clRootedbinaryIsQuartetableNode(rb, i) == 1) {
      union PCTypes p = zeropct;
      p.i = i;
      clDraPush(result, p);
    }
  }
  return result;
}

struct DRA *clRootedbinaryLeafLabels(const struct RootedBinary *rb)
{
  struct DRA *result = clDraNew();
  int i;
  for (i = 0; i < clLabelpermSize(rb->labelperm); i += 1) {
    union PCTypes p = zeropct;
    p.i = clLabelpermNodeIDForColIndex(rb->labelperm, i);
    clDraPush(result, p);
  }
  return result;
}

struct LabelPerm *clRootedbinaryLabelPerm(struct RootedBinary *rb)
{
  return clLabelpermClone(rb->labelperm);
}

static struct TreeAdaptor *rb_treeclone(struct TreeAdaptor *ta)
{
  struct RootedBinary *rb = (struct RootedBinary *) ta->ptr;
  struct TreeAdaptor *result = clMalloc(sizeof(*result) * 1);
  *result = *ta;
  result->ptr = clRootedbinaryClone(rb);
  return result;
}

static void rb_treemutate(struct TreeAdaptor *ta)
{
  struct RootedBinary *rb = (struct RootedBinary *) ta->ptr;
  mutateComplex(rb);
}

static void rb_treefree(struct TreeAdaptor *ta)
{
  struct RootedBinary *rb = (struct RootedBinary *) ta->ptr;
  clRootedbinaryFreeRB(rb);
  rb = NULL;
  memset(ta, 0, sizeof(*ta));
  clFreeandclear(ta);
}

void clRb_treesetlabelperm(struct TreeAdaptor *ta, int j, int i)
{
  struct RootedBinary *rb = (struct RootedBinary *) ta->ptr;
  clRootedbinaryLabelPermSetter(rb, j, i);
}

static struct LabelPerm *rb_treegetlabelperm(struct TreeAdaptor *ta)
{
  struct RootedBinary *rb = (struct RootedBinary *) ta->ptr;
  return clRootedbinaryLabelPerm(rb);
}

static struct AdjAdaptor *rb_treegetadja(struct TreeAdaptor *ta)
{
  struct RootedBinary *rb = (struct RootedBinary *) ta->ptr;
  return clRootedbinaryAdjAdaptor(rb);
}

static int rb_treeisquartetable(struct TreeAdaptor *ta, int which)
{
  struct RootedBinary *rb = (struct RootedBinary *) ta->ptr;
  return clRootedbinaryIsQuartetableNode(rb, which);
}

static int rb_treeisroot(struct TreeAdaptor *ta, int which)
{
  struct RootedBinary *rb = (struct RootedBinary *) ta->ptr;
  return rb->root == which;
}

int clRb_treeisflippable(struct TreeAdaptor *ta, int which)
{
  struct RootedBinary *rb = (struct RootedBinary *) ta->ptr;
  return clRootedbinaryIsFlippableNode(rb, which);
}

struct DRA *clRb_treeperimpairs(struct TreeAdaptor *ta, struct CLNodeSet *flips)
{
  struct RootedBinary *rb = (struct RootedBinary *) ta->ptr;
  return clRootedbinaryPerimeterPairs(rb, flips);
}

int clRb_treemutecount(struct TreeAdaptor *ta)
{
  struct RootedBinary *rb = (struct RootedBinary *) ta->ptr;
  return clRootedbinaryLastMutationCount(rb);
}

struct TreeAdaptor *clTreeaLoadRootedBinary(int howBig)
{
  return loadRBTRA(clRootedbinaryNew(howBig, NULL, NULL));
}

static struct TreeAdaptor *loadRBTRA(struct RootedBinary *rb)
{
  struct TreeAdaptor c = {
    ptr:NULL,
    treemutate:rb_treemutate,
    treefree:rb_treefree,
    treeclone:rb_treeclone,
    treegetlabelperm:rb_treegetlabelperm,
    treegetadja:rb_treegetadja,
    treeisquartetable:rb_treeisquartetable,
    treeisflippable:clRb_treeisflippable,
    treeisroot:rb_treeisroot,
    treemutecount:clRb_treemutecount,
    treeperimpairs:clRb_treeperimpairs,
    treelpsetat:clRb_treesetlabelperm
  };

  struct TreeAdaptor *result = clMalloc(sizeof(*result) * 1);
  *result = c;
  result->ptr = rb;
  return result;
}

