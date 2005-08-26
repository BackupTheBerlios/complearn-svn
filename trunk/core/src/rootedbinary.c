#include <complearn/complearn.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <assert.h>

#if GSL_RDY
#include <gsl/gsl_randist.h>
#endif

static struct DoubleA *getLabellableNodes(const struct RootedBinary *rb);

struct RootedBinary {
  int nodecount;
  qbase_t root;
  int mc;
  struct LabelPerm *labelperm;
  struct AdjAdaptor *aa;
};

struct AdjAdaptor *getAdjAdaptorForRB(struct RootedBinary *rb)
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
    nsize = adjaGetNeighborCount(rb->aa, result);
  } while (nsize == 1);
  return result;
}

static int randomNeighbor(struct RootedBinary *rb, qbase_t who)
{
  for (;;) {
    qbase_t result = randomTreeNode(rb);
    if (adjaGetConState(rb->aa, who, result))
      return result;
  }
}

static int verifyTree(struct RootedBinary *rb)
{
  int i, j;
  int nc;
  struct DoubleA *result = getTreeNodesRB(rb);
  if (getSize(result) != rb->nodecount) {
    printf("Error, inconsistent node list with size %d but nodecount %d\n",
      getSize(result), rb->nodecount);
    adjaPrint(rb->aa);
    for (i = 0; i < rb->nodecount; ++i) {
      for (j = 0; j < i; ++j)
        printf(" ");
      for (j = i+1; j < rb->nodecount; ++j)
        printf("%c", adjaGetConState(rb->aa, i, j) + '0');
      printf("\n");
    }
    return 0;
  }
  for (i = 0; i < rb->nodecount; ++i) {
    nc = adjaGetNeighborCount(rb->aa, i);
    if ((nc != 1 && nc != 3 && i != rb->root) || (nc != 2 && i == rb->root)) {
      int nbp[20], retval;
      int nlenhere = 20;
      printf("Showing bad neighbors...   ************\n");
      retval = adjaGetNeighbors(rb->aa, i, nbp, &nlenhere);
      assert(retval == CL_OK);
      printf("Bad tree with %d neighbors on node %d\n", nc, i);

      return 0;
    }
  }
  freeDoubleDoubler(result);
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
    retval = pathFinder(rb->aa, k1, k2, pbuf, &pathlen);
    assert(retval == CL_OK);
    assert(pbuf[0] == k1);
    assert(adjaGetConState(rb->aa, k1, pbuf[1]));
  } while (pathlen <= 2);
  i1 = pbuf[1];
  assert(i1 < MAXPATHNODES);
  assert(adjaGetConState(rb->aa, k1, i1));
  nsizems = MAXNEIGHBORS;

  retval = adjaGetNeighbors(rb->aa, i1, nbufms, &nsizems);
  assert(retval == CL_OK);
  if (nsizems < 3)
    goto tryagain;
  adjaSetConState(rb->aa, k1, i1, 0);
  retval = adjaGetNeighbors(rb->aa, i1, nbufms, &nsizems);
  assert(retval == CL_OK);
  m1 = nbufms[0];
  m2 = nbufms[1];
  if (nsizems < 2) {
    fprintf(stderr, "Warning, got node %d with too many (%d) neighbors, %d...\n",
        i1, nsizems, m1);
    assert(nsizems == 2);
  }
  do {
    m3 = randomNeighbor(rb, k2);
  } while (m3 == pbuf[pathlen-2]);

  adjaSetConState(rb->aa, m1, i1, 0);
  adjaSetConState(rb->aa, m2, i1, 0);
  adjaSetConState(rb->aa, m3, k2, 0);

  adjaSetConState(rb->aa, m1, m2, 1);
  adjaSetConState(rb->aa, k2, i1, 1);
  adjaSetConState(rb->aa, m3, i1, 1);
  adjaSetConState(rb->aa, k1, i1, 1);

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
    retval = pathFinder(rb->aa, i1, i2, pathbuf, &pathlen);
    assert(retval == CL_OK);
  } while (pathlen <= 2);
  n1 = pathbuf[1];
  if (n1 == rb->root)
    goto tryagain;
  assert(adjaGetConState(rb->aa, n1, i1));
  n2 = pathbuf[pathlen-2];
  if (n2 == rb->root)
    goto tryagain;
  assert(adjaGetConState(rb->aa, n2, i2));
  flipCrosswise(rb->aa, i1, n1, i2, n2);
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
    case 0: mutateSpecies(rb->aa, rb->labelperm); break;
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
    return howManyMutationsTwoMinusExp();
  else
    return howManyMutationsWeirdLogFormula();
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

int getLastMutationCountRB(const struct RootedBinary *rb)
{
  return rb->mc;
}

void doComplexMutationRB(struct RootedBinary *rb)
{
  mutateComplex(rb);
}

struct RootedBinary *newRootedBinary(int howManyLeaves)
{
  int i;
  struct DoubleA *leaves;
  struct RootedBinary *rb = gcalloc(sizeof(struct RootedBinary), 1);
  assert(howManyLeaves > 3);
  rb->nodecount = 2*howManyLeaves-3;
  rb->aa = newPathKeeper(loadAdaptorAL(rb->nodecount));
  for (i = 0; i < howManyLeaves-2; ++i) {
    adjaSetConState(rb->aa, i, i+howManyLeaves-1, 1);
    adjaSetConState(rb->aa, i, i+1, 1);
  }

  leaves = getLabellableNodes(rb);
  assert(getSize(leaves) == howManyLeaves);
  rb->labelperm = newLabelPerm(leaves);
  freeDoubleDoubler(leaves);

  verifyTree(rb);
  return rb;
}

struct RootedBinary *cloneTreeRB(const struct RootedBinary *rb)
{
  struct RootedBinary *cp;
  cp = gcalloc(sizeof(*cp), 1);
  cp->nodecount = rb->nodecount;
  cp->root = rb->root;
  cp->mc = rb->mc;
  cp->nodecount = rb->nodecount;
  cp->aa = adjaClone(rb->aa);
  cp->labelperm = cloneLabelPerm(rb->labelperm);
  return cp;
}


int isQuartetableNodeRB(const struct RootedBinary *rb, qbase_t which)
{
  return adjaGetNeighborCount(rb->aa, which) < 3;
}

int isFlippableNodeRB(struct RootedBinary *rb, qbase_t which)
{
  int nc = adjaGetNeighborCount(rb->aa, which);
// return nc != 1;
  return nc == 2 || nc == 3;
}

qbase_t getStartingNodeRB(const struct RootedBinary *rb)
{
  return 0;
}

struct DoubleA *getTreeNodesRB(const struct RootedBinary *rb)
{
  union PCTypes p = zeropct;
  struct DoubleA *result = newDoubleDoubler();
  struct DoubleA *border = newDoubleDoubler();
  struct CLNodeSet *done = newCLNodeSet(rb->nodecount);
  pushValue(border, p);
  walkTree(getAdjAdaptorForRB((struct RootedBinary *) rb), result, border, done, 0, NULL);
  freeDoubleDoubler(border);
  freeCLNodeSet(done);
  return result;
}

struct DoubleA *getPerimeterPairsRB(const struct RootedBinary *rb, struct CLNodeSet *flips)
{
  struct DoubleA *pairs = newDoubleDoubler();
  union PCTypes p = zeropct;
  int i;
  int lastval = -1;
  int firstnode = -1;
  struct DoubleA *traversalseq = newDoubleDoubler();
  struct DoubleA *border = newDoubleDoubler();
  struct CLNodeSet *done = newCLNodeSet(rb->nodecount);
  p.i = rb->root; /* start at root */
  pushValue(border, p);
  walkTree(getAdjAdaptorForRB((struct RootedBinary *) rb), traversalseq, border, done, 0, flips);

  for (i = 0;i < getSize(traversalseq); i += 1) {
    int curnode = getValueAt(traversalseq, i).i;
    if (curnode == rb->root)
      continue;
    if (isQuartetableNodeRB(rb, curnode)) {
      if (firstnode == -1)
        firstnode = curnode;
      if (lastval != -1) {
        p = zeropct;
        p.ip.x = lastval;
        p.ip.y = curnode;
        pushValue(pairs, p);
      }
      lastval = curnode;
    }
  }
  freeDoubleDoubler(traversalseq);
  return pairs;
}

void freeRootedBinaryRB(struct RootedBinary *rb)
{
  freeLabelPerm(rb->labelperm);
  adjaFree(rb->aa);
  rb->aa = NULL;
  gfreeandclear(rb);
}

static struct DoubleA *getLabellableNodes(const struct RootedBinary *rb)
{
  int i;
  struct DoubleA *result = newDoubleDoubler();
  for (i = 0; i < rb->nodecount; i += 1) {
    if (isQuartetableNodeRB(rb, i) == 1) {
      union PCTypes p = zeropct;
      p.i = i;
      pushValue(result, p);
    }
  }
  return result;
}

struct DoubleA *getLeafLabelsRB(const struct RootedBinary *rb)
{
  struct DoubleA *result = newDoubleDoubler();
  int i;
  for (i = 0; i < getSizeLP(rb->labelperm); i += 1) {
    union PCTypes p = zeropct;
    p.i = getNodeIDForColumnIndexLP(rb->labelperm, i);
    pushValue(result, p);
  }
  return result;
}

struct LabelPerm *getLabelPermRB(struct RootedBinary *rb)
{
  return rb->labelperm;
}

static struct TreeAdaptor *rb_treeclone(struct TreeAdaptor *ta)
{
  struct RootedBinary *rb = (struct RootedBinary *) ta->ptr;
  struct TreeAdaptor *result = gmalloc(sizeof(*result) * 1);
  *result = *ta;
  result->ptr = cloneTreeRB(rb);
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
  freeRootedBinaryRB(rb);
  rb = NULL;
  memset(ta, 0, sizeof(*ta));
  gfreeandclear(ta);
}

static struct LabelPerm *rb_treegetlabelperm(struct TreeAdaptor *ta)
{
  struct RootedBinary *rb = (struct RootedBinary *) ta->ptr;
  return getLabelPermRB(rb);
}

static struct AdjAdaptor *rb_treegetadja(struct TreeAdaptor *ta)
{
  struct RootedBinary *rb = (struct RootedBinary *) ta->ptr;
  return getAdjAdaptorForRB(rb);
}

static int rb_treeisquartetable(struct TreeAdaptor *ta, int which)
{
  struct RootedBinary *rb = (struct RootedBinary *) ta->ptr;
  return isQuartetableNodeRB(rb, which);
}

static int rb_treeisroot(struct TreeAdaptor *ta, int which)
{
  struct RootedBinary *rb = (struct RootedBinary *) ta->ptr;
  return rb->root == which;
}

int rb_treeisflippable(struct TreeAdaptor *ta, int which)
{
  struct RootedBinary *rb = (struct RootedBinary *) ta->ptr;
  return isFlippableNodeRB(rb, which);
}

struct DoubleA *rb_treeperimpairs(struct TreeAdaptor *ta, struct CLNodeSet *flips)
{
  struct RootedBinary *rb = (struct RootedBinary *) ta->ptr;
  return getPerimeterPairsRB(rb, flips);
}

int rb_treemutecount(struct TreeAdaptor *ta)
{
  struct RootedBinary *rb = (struct RootedBinary *) ta->ptr;
  return getLastMutationCountRB(rb);
}

struct TreeAdaptor *loadNewRootedTRA(int howBig)
{
  return loadRBTRA(newRootedBinary(howBig));
}

struct TreeAdaptor *loadRBTRA(struct RootedBinary *rb)
{
  struct TreeAdaptor c = {
    ptr:NULL,
    treemutate:rb_treemutate,
    treefree:rb_treefree,
    treeclone:rb_treeclone,
    treegetlabelperm:rb_treegetlabelperm,
    treegetadja:rb_treegetadja,
    treeisquartetable:rb_treeisquartetable,
    treeisflippable:rb_treeisflippable,
    treeisroot:rb_treeisroot,
    treemutecount:rb_treemutecount,
    treeperimpairs:rb_treeperimpairs
  };

  struct TreeAdaptor *result = gmalloc(sizeof(*result) * 1);
  *result = c;
  result->ptr = rb;
  return result;
}

