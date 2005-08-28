#include <complearn/complearn.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <assert.h>

static struct TreeAdaptor *loadUBTRA(struct UnrootedBinary *ub);
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
  struct DoubleA *da = doubleaNew();
  do {
    qbase_t cur = randomKernelNode(ub);
    doubleaAddQBIfNew(da, cur);
  } while (doubleaSize(da) < howMany);
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
  struct DoubleA *result = unrootedbinaryNodes(ub, NULL);
  if (doubleaSize(result) != ub->nodecount) {
    printf("Error, inconsistent node list with size %d but nodecount %d\n",
      doubleaSize(result), ub->nodecount);
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
  doubleaFree(result);
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
    i1 = doubleaGetValueAt(swappers, 0).i;
    i2 = doubleaGetValueAt(swappers, 1).i;
    assert(i1 != i2);
    assert(adjaNeighborCount(ub->aa, i1) == 3);
    assert(adjaNeighborCount(ub->aa, i2) == 3);
    doubleaFree(swappers);
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

int unrootedbinaryLastMutationCount(const struct UnrootedBinary *ub)
{
  return ub->mc;
}

void unrootedbinaryDoComplexMutation(struct UnrootedBinary *ub)
{
  mutateComplex(ub);
}

struct UnrootedBinary *unrootedbinaryNew(int howManyLeaves)
{
  int i;
  struct DoubleA *leaves;
  struct UnrootedBinary *ub = clCalloc(sizeof(struct UnrootedBinary), 1);
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
  assert(doubleaSize(leaves) == howManyLeaves);
  ub->labelperm = labelpermNew(leaves);
  assert(ub->labelperm);

  doubleaFree(leaves);

  verifyTree(ub);
  return ub;
}

struct UnrootedBinary *unrootedbinaryClone(const struct UnrootedBinary *ub)
{
  struct UnrootedBinary *cp;
  cp = clCalloc(sizeof(*cp), 1);
  cp->nodecount = ub->nodecount;
  cp->startNode = ub->startNode;
  cp->mc = ub->mc;
  cp->aa = adjaClone(ub->aa);
  cp->labelperm = labelpermClone(ub->labelperm);
  return cp;
}


int unrootedbinaryIsQuartetableNode(const struct UnrootedBinary *ub, qbase_t which)
{
  return adjaNeighborCount(ub->aa, which) < 3;
}

int isFlippableNode(struct UnrootedBinary *ub, qbase_t which)
{
  return adjaNeighborCount(ub->aa, which) == 3;
}

qbase_t unrootedbinaryStartingNode(const struct UnrootedBinary *ub)
{
  return 0;
}

struct DoubleA *unrootedbinaryNodes(const struct UnrootedBinary *ub, struct CLNodeSet *flips)
{
  union PCTypes p = zeropct;
  struct DoubleA *result = doubleaNew();
  struct DoubleA *border = doubleaNew();
  struct CLNodeSet *done = clnodesetNew(ub->nodecount);
  doubleaPush(border, p);
  walkTree(getAdjAdaptorForUB((struct UnrootedBinary *) ub), result, border, done, 0, flips);
  doubleaFree(border);
  clnodesetFree(done);
  return result;
}

struct DoubleA *unrootedbinaryPerimPairs(const struct UnrootedBinary *ub, struct CLNodeSet *flips)
{
  struct DoubleA *nodes = unrootedbinaryNodes(ub, flips);
  struct DoubleA *pairs = doubleaNew();
  union PCTypes p;
  int i;
  int lastval = -1;
  int firstnode = -1;
  for (i = 0;i < doubleaSize(nodes); i += 1) {
    int curnode = doubleaGetValueAt(nodes, i).i;
    if (unrootedbinaryIsQuartetableNode(ub, curnode)) {
      if (firstnode == -1)
        firstnode = curnode;
      if (lastval != -1) {
        p = zeropct;
        p.ip.x = lastval;
        p.ip.y = curnode;
        doubleaPush(pairs, p);
      }
      lastval = curnode;
    }
  }
  p.ip.x = lastval;
  p.ip.y = firstnode;
  doubleaPush(pairs, p);
  doubleaFree(nodes);
  return pairs;
}

void unrootedbinaryFree(struct UnrootedBinary *ub)
{
  if (ub->labelperm)
    labelpermFree(ub->labelperm);
  ub->labelperm = NULL;
  if (ub->aa)
    adjaFree(ub->aa);
  ub->aa = NULL;
  clFreeandclear(ub);
}

static struct DoubleA *getLabellableNodes(const struct UnrootedBinary *ub)
{
  int i;
  struct DoubleA *result = doubleaNew();
  for (i = 0; i < ub->nodecount; i += 1) {
    if (unrootedbinaryIsQuartetableNode(ub, i) == 1) {
      union PCTypes p = zeropct;
      p.i = i;
      doubleaPush(result, p);
    }
  }
  return result;
}

struct DoubleA *unrootedbinaryLeafLabels(const struct UnrootedBinary *ub)
{
  struct DoubleA *result = doubleaNew();
  int i;
  for (i = 0; i < labelpermSize(ub->labelperm); i += 1) {
    union PCTypes p = zeropct;
    p.i = labelpermNodeIDForColIndex(ub->labelperm, i);
    doubleaPush(result, p);
  }
  return result;
}

struct LabelPerm *unrootedbinaryLabelPerm(struct UnrootedBinary *ub)
{
  return ub->labelperm;
}

struct TreeAdaptor *ub_treeclone(struct TreeAdaptor *ta)
{
  struct UnrootedBinary *ub = (struct UnrootedBinary *) ta->ptr;
  struct TreeAdaptor *result = clMalloc(sizeof(*result) * 1);
  *result = *ta;
  result->ptr = unrootedbinaryClone(ub);
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
  unrootedbinaryFree(ub);
  ub = NULL;
  memset(ta, 0, sizeof(*ta));
  clFreeandclear(ta);
}

struct LabelPerm *ub_treegetlabelperm(struct TreeAdaptor *ta)
{
  struct UnrootedBinary *ub = (struct UnrootedBinary *) ta->ptr;
  return unrootedbinaryLabelPerm(ub);
}

struct AdjAdaptor *ub_treegetadja(struct TreeAdaptor *ta)
{
  struct UnrootedBinary *ub = (struct UnrootedBinary *) ta->ptr;
  return getAdjAdaptorForUB(ub);
}

int ub_treeisquartetable(struct TreeAdaptor *ta, int which)
{
  struct UnrootedBinary *ub = (struct UnrootedBinary *) ta->ptr;
  return unrootedbinaryIsQuartetableNode(ub, which);
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
  return unrootedbinaryPerimPairs(ub, flips);
}

int ub_treemutecount(struct TreeAdaptor *ta)
{
  struct UnrootedBinary *ub = (struct UnrootedBinary *) ta->ptr;
  return unrootedbinaryLastMutationCount(ub);
}

struct TreeAdaptor *treeaLoadUnrooted(int howBig)
{
  return loadUBTRA(unrootedbinaryNew(howBig));
}

static struct TreeAdaptor *loadUBTRA(struct UnrootedBinary *ub)
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

  struct TreeAdaptor *result = clMalloc(sizeof(*result) * 1);
  *result = c;
  result->ptr = ub;
  return result;
}

