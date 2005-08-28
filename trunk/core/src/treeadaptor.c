#include <complearn/complearn.h>

void treeaMutate(struct TreeAdaptor *tra)
{
  assert(tra);
  tra->treemutate(tra);
}

struct TreeAdaptor *treeaNew(int isRooted, int howbig)
{
  if (isRooted)
    return treeaLoadRootedBinary(howbig);
  else
    return treeaLoadUnrooted(howbig);
}

void treeaFree(struct TreeAdaptor *tra)
{
  assert(tra);
  assert(tra->treefree);
  tra->treefree(tra);
}

struct TreeAdaptor *treeaClone(struct TreeAdaptor *tra)
{
  assert(tra);
  return tra->treeclone(tra);
}

struct LabelPerm *treeaLabelPerm(struct TreeAdaptor *tra)
{
  assert(tra);
  return tra->treegetlabelperm(tra);
}

struct AdjAdaptor *treeaAdjAdaptor(struct TreeAdaptor *tra)
{
  assert(tra);
  return tra->treegetadja(tra);
}

int treeaIsQuartettable(struct TreeAdaptor *tra, int which)
{
  assert(tra);
  return tra->treeisquartetable(tra, which);
}

int treeaIsFlippable(struct TreeAdaptor *tra, int which)
{
  assert(tra);
  return tra->treeisflippable(tra, which);
}
int treeaIsRoot(struct TreeAdaptor *tra, int which)
{
  assert(tra);
  return tra->treeisroot(tra, which);
}


int treeaMutationCount(struct TreeAdaptor *tra)
{
  assert(tra);
  return tra->treemutecount(tra);
}

struct DoubleA *treeaPerimPairs(struct TreeAdaptor *tra, struct CLNodeSet *flips)
{
  assert(tra);
  return tra->treeperimpairs(tra, flips);
}

int treeaNodeCount(struct TreeAdaptor *tra)
{
  struct AdjAdaptor *ad = treeaAdjAdaptor(tra);
  return adjaSize(ad);
}

static double nchoosefourScale(int n, int inp)
{
  double bign = n;
  bign = (bign * (bign - 1) * (bign - 2) * (bign - 3)) / 24.0;
  return ((double) inp) / bign;
}

double treeaDifferenceScore(struct TreeAdaptor *tra1, struct TreeAdaptor *tra2)
{
  struct AdjAdaptor *ad1, *ad2;
  struct LabelPerm *lab1, *lab2;
  int diff;
  lab1 = treeaLabelPerm(tra1);
  lab2 = treeaLabelPerm(tra2);
  ad1 = treeaAdjAdaptor(tra1);
  ad2 = treeaAdjAdaptor(tra2);
  diff = countTrinaryDifferences(ad1, lab1, ad2, lab2);
  return nchoosefourScale(labelpermSize(lab1), diff);
}

struct DoubleA *treeaNodes(struct TreeAdaptor *ta)
{
  struct DoubleA *n = doubleaNew();
  union PCTypes p = zeropct;
  int i;
  for (i = 0; i < treeaNodeCount(ta) ;  i += 1) {
    p.i = i;
    doubleaPush(n,p);
  }
  return n;
}
