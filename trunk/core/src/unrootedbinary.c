#include <complearn/complearn.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <assert.h>

static struct DoubleA *getLabellableNodes(const struct UnrootedBinary *ub);

struct UnrootedBinary {
  int nodecount;
  qbase_t startNode;
  int mc;
  struct LabelPerm *labelperm;
  struct AdjAdaptor *aa;
  struct CLNodeSet *flips;
};

struct AdjAdaptor *getAdjAdaptorForUB(struct UnrootedBinary *ub)
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
    nsize = adjaNeighborCount(ub->aa, result);
  } while (nsize != 3 );
  return result;
}

static struct DoubleA *randomKernelNodes(struct UnrootedBinary *ub, int howMany)
{
  struct DoubleA *da = newDoubleDoubler();
  do {
    qbase_t cur = randomKernelNode(ub);
    addIfNewqb(da, cur);
  } while (getSize(da) < howMany);
  return da;
}

static int randomNeighbor(struct UnrootedBinary *ub, qbase_t who)
{
  for (;;) {
    qbase_t result = randomTreeNode(ub);
    if (adjaGetConState(ub->aa, who, result))
      return result;
  }
}

static int verifyTree(struct UnrootedBinary *ub)
{
  int i, j;
  int nc;
  struct DoubleA *result = getTreeNodes(ub, NULL);
  if (getSize(result) != ub->nodecount) {
    printf("Error, inconsistent node list with size %d but nodecount %d\n",
      getSize(result), ub->nodecount);
    adjaPrint(ub->aa);
    for (i = 0; i < ub->nodecount; ++i) {
      for (j = 0; j < i; ++j)
        printf(" ");
      for (j = i+1; j < ub->nodecount; ++j)
        printf("%c", adjaGetConState(ub->aa, i, j) + '0');
      printf("\n");
    }
    return 0;
  }
  for (i = 0; i < ub->nodecount; ++i) {
    nc = adjaNeighborCount(ub->aa, i);
    if (nc != 1 && nc != 3) {
      int nbp[20], retval;
      int nlenhere = 20;
      printf("Showing bad neighbors...   ************\n");
      retval = adjaNeighbors(ub->aa, i, nbp, &nlenhere);
      assert(retval == CL_OK);
      printf("Bad tree with %d neighbors on node %d\n", nc, i);

      return 0;
    }
  }
  freeDoubleDoubler(result);
  return 1;
}

static void mutateSubtreeTransfer(struct UnrootedBinary *ub)
{
  qbase_t k1, k2, i1, m1, m2, m3;
#define MAXPATHNODES 128
  static int pbuf[MAXPATHNODES];
  int pathlen, retval;
  int nbufms[MAXNEIGHBORS], nsizems;
  do {
    do {
    k1 = randomTreeNode(ub);
    k2 = randomKernelNode(ub);
    } while (k1 == k2);
    pathlen = MAXPATHNODES;
    retval = pathFinder(ub->aa, k1, k2, pbuf, &pathlen);
    assert(retval == CL_OK);
    assert(pbuf[0] == k1);
    assert(adjaGetConState(ub->aa, k1, pbuf[1]));
  } while (pathlen <= 2);
  i1 = pbuf[1];
  assert(i1 < MAXPATHNODES);
  assert(adjaGetConState(ub->aa, k1, i1));
  nsizems = MAXNEIGHBORS;
  retval = adjaNeighbors(ub->aa, i1, nbufms, &nsizems);
  assert(nsizems == 3);
  adjaSetConState(ub->aa, k1, i1, 0);
  assert(!adjaGetConState(ub->aa, k1, i1));

  nsizems = MAXNEIGHBORS;
  retval = adjaNeighbors(ub->aa, i1, nbufms, &nsizems);
  assert(retval == CL_OK);
  m1 = nbufms[0];
  m2 = nbufms[1];
  if (nsizems != 2) {
    fprintf(stderr, "Warning, got node %d with only %d neighbors, %d...\n",
        i1, nsizems, m1);
    assert(nsizems == 2);
  }
  do {
    m3 = randomNeighbor(ub, k2);
  } while (m3 == pbuf[pathlen-2]);

  adjaSetConState(ub->aa, m1, i1, 0);
  adjaSetConState(ub->aa, m2, i1, 0);
  adjaSetConState(ub->aa, m3, k2, 0);

  adjaSetConState(ub->aa, m1, m2, 1);
  adjaSetConState(ub->aa, k2, i1, 1);
  adjaSetConState(ub->aa, m3, i1, 1);
  adjaSetConState(ub->aa, k1, i1, 1);

}

static void mutateSubtreeInterchange(struct UnrootedBinary *ub)
{
  qbase_t i1, i2;
  qbase_t n1, n2;
  int pathbuf[MAXPATHNODES];
  int pathlen;
  int retval;
  do {
    struct DoubleA *swappers = randomKernelNodes(ub, 2);
    i1 = getValueAt(swappers, 0).i;
    i2 = getValueAt(swappers, 1).i;
    assert(i1 != i2);
    assert(adjaNeighborCount(ub->aa, i1) == 3);
    assert(adjaNeighborCount(ub->aa, i2) == 3);
    freeDoubleDoubler(swappers);
    pathlen = MAXPATHNODES;
    retval = pathFinder(ub->aa, i1, i2, pathbuf, &pathlen);
    assert(retval == CL_OK);
  } while (pathlen <= 3);
  n1 = pathbuf[1];
  assert(adjaGetConState(ub->aa, n1, i1));
  n2 = pathbuf[pathlen-2];
  assert(adjaGetConState(ub->aa, n2, i2));
  flipCrosswise(ub->aa, i1, n1, i2, n2);
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
    case 0: mutateSpecies(ub->aa, ub->labelperm); break;
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
//    return howManyMutationsTwoMinusExp();
//  else
  return howManyMutationsWeirdLogFormula();
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

int getLastMutationCount(const struct UnrootedBinary *ub)
{
  return ub->mc;
}

void doComplexMutation(struct UnrootedBinary *ub)
{
  mutateComplex(ub);
}

struct UnrootedBinary *newUnrootedBinary(int howManyLeaves)
{
  int i;
  struct DoubleA *leaves;
  struct UnrootedBinary *ub = gcalloc(sizeof(struct UnrootedBinary), 1);
  assert(howManyLeaves > 3);
  ub->nodecount = 2*howManyLeaves-2;
  ub->aa = newPathKeeper(adjaLoadAdjList(ub->nodecount));
  for (i = 0; i < howManyLeaves-2; ++i) {
    adjaSetConState(ub->aa, i, i+howManyLeaves, 1);
    adjaSetConState(ub->aa, i, i+1, 1);
  }
  adjaSetConState(ub->aa, 0, howManyLeaves-1, 1);

  leaves = getLabellableNodes(ub);
  assert(leaves);
  assert(getSize(leaves) == howManyLeaves);
  ub->labelperm = newLabelPerm(leaves);
  assert(ub->labelperm);

  freeDoubleDoubler(leaves);

  verifyTree(ub);
  return ub;
}

struct UnrootedBinary *cloneTree(const struct UnrootedBinary *ub)
{
  struct UnrootedBinary *cp;
  cp = gcalloc(sizeof(*cp), 1);
  cp->nodecount = ub->nodecount;
  cp->startNode = ub->startNode;
  cp->mc = ub->mc;
  cp->aa = adjaClone(ub->aa);
  cp->labelperm = cloneLabelPerm(ub->labelperm);
  return cp;
}


int isQuartetableNode(const struct UnrootedBinary *ub, qbase_t which)
{
  return adjaNeighborCount(ub->aa, which) < 3;
}

int isFlippableNode(struct UnrootedBinary *ub, qbase_t which)
{
  return adjaNeighborCount(ub->aa, which) == 3;
}

qbase_t getStartingNode(const struct UnrootedBinary *ub)
{
  return 0;
}

struct DoubleA *getTreeNodes(const struct UnrootedBinary *ub, struct CLNodeSet *flips)
{
  union PCTypes p = zeropct;
  struct DoubleA *result = newDoubleDoubler();
  struct DoubleA *border = newDoubleDoubler();
  struct CLNodeSet *done = clnodesetNew(ub->nodecount);
  pushValue(border, p);
  walkTree(getAdjAdaptorForUB((struct UnrootedBinary *) ub), result, border, done, 0, flips);
  freeDoubleDoubler(border);
  clnodesetFree(done);
  return result;
}

struct DoubleA *getPerimeterPairs(const struct UnrootedBinary *ub, struct CLNodeSet *flips)
{
  struct DoubleA *nodes = getTreeNodes(ub, flips);
  struct DoubleA *pairs = newDoubleDoubler();
  union PCTypes p;
  int i;
  int lastval = -1;
  int firstnode = -1;
  for (i = 0;i < getSize(nodes); i += 1) {
    int curnode = getValueAt(nodes, i).i;
    if (isQuartetableNode(ub, curnode)) {
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
  p.ip.x = lastval;
  p.ip.y = firstnode;
  pushValue(pairs, p);
  freeDoubleDoubler(nodes);
  return pairs;
}

void freeUnrootedBinary(struct UnrootedBinary *ub)
{
  if (ub->labelperm)
    freeLabelPerm(ub->labelperm);
  ub->labelperm = NULL;
  if (ub->aa)
    adjaFree(ub->aa);
  ub->aa = NULL;
  gfreeandclear(ub);
}

static struct DoubleA *getLabellableNodes(const struct UnrootedBinary *ub)
{
  int i;
  struct DoubleA *result = newDoubleDoubler();
  for (i = 0; i < ub->nodecount; i += 1) {
    if (isQuartetableNode(ub, i) == 1) {
      union PCTypes p = zeropct;
      p.i = i;
      pushValue(result, p);
    }
  }
  return result;
}

struct DoubleA *getLeafLabels(const struct UnrootedBinary *ub)
{
  struct DoubleA *result = newDoubleDoubler();
  int i;
  for (i = 0; i < getSizeLP(ub->labelperm); i += 1) {
    union PCTypes p = zeropct;
    p.i = getNodeIDForColumnIndexLP(ub->labelperm, i);
    pushValue(result, p);
  }
  return result;
}

struct LabelPerm *getLabelPerm(struct UnrootedBinary *ub)
{
  return ub->labelperm;
}

struct TreeAdaptor *ub_treeclone(struct TreeAdaptor *ta)
{
  struct UnrootedBinary *ub = (struct UnrootedBinary *) ta->ptr;
  struct TreeAdaptor *result = gmalloc(sizeof(*result) * 1);
  *result = *ta;
  result->ptr = cloneTree(ub);
  return result;
}

void ub_treemutate(struct TreeAdaptor *ta)
{
  struct UnrootedBinary *ub = (struct UnrootedBinary *) ta->ptr;
  mutateComplex(ub);
}

void ub_treefree(struct TreeAdaptor *ta)
{
  struct UnrootedBinary *ub = (struct UnrootedBinary *) ta->ptr;
  freeUnrootedBinary(ub);
  ub = NULL;
  memset(ta, 0, sizeof(*ta));
  gfreeandclear(ta);
}

struct LabelPerm *ub_treegetlabelperm(struct TreeAdaptor *ta)
{
  struct UnrootedBinary *ub = (struct UnrootedBinary *) ta->ptr;
  return getLabelPerm(ub);
}

struct AdjAdaptor *ub_treegetadja(struct TreeAdaptor *ta)
{
  struct UnrootedBinary *ub = (struct UnrootedBinary *) ta->ptr;
  return getAdjAdaptorForUB(ub);
}

int ub_treeisquartetable(struct TreeAdaptor *ta, int which)
{
  struct UnrootedBinary *ub = (struct UnrootedBinary *) ta->ptr;
  return isQuartetableNode(ub, which);
}

static int ub_treeisroot(struct TreeAdaptor *ta, int which)
{
  return 0;
}

int ub_treeisflippable(struct TreeAdaptor *ta, int which)
{
  struct UnrootedBinary *ub = (struct UnrootedBinary *) ta->ptr;
  return isFlippableNode(ub, which);
}

struct DoubleA *ub_treeperimpairs(struct TreeAdaptor *ta, struct CLNodeSet *flips)
{
  struct UnrootedBinary *ub = (struct UnrootedBinary *) ta->ptr;
  return getPerimeterPairs(ub, flips);
}

int ub_treemutecount(struct TreeAdaptor *ta)
{
  struct UnrootedBinary *ub = (struct UnrootedBinary *) ta->ptr;
  return getLastMutationCount(ub);
}

struct TreeAdaptor *loadNewUnrootedTRA(int howBig)
{
  return loadUBTRA(newUnrootedBinary(howBig));
}

struct TreeAdaptor *loadUBTRA(struct UnrootedBinary *ub)
{
  struct TreeAdaptor c = {
    ptr:NULL,
    treemutate:ub_treemutate,
    treefree:ub_treefree,
    treeclone:ub_treeclone,
    treegetlabelperm:ub_treegetlabelperm,
    treegetadja:ub_treegetadja,
    treeisquartetable:ub_treeisquartetable,
    treeisflippable:ub_treeisflippable,
    treeisroot:ub_treeisroot,
    treemutecount:ub_treemutecount,
    treeperimpairs:ub_treeperimpairs
  };

  struct TreeAdaptor *result = gmalloc(sizeof(*result) * 1);
  *result = c;
  result->ptr = ub;
  return result;
}

