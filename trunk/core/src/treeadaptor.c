#include <complearn/complearn.h>

void treemutateTRA(struct TreeAdaptor *tra)
{
  assert(tra);
  tra->treemutate(tra);
}

struct TreeAdaptor *newTreeTRA(int isRooted, int howbig)
{
  if (isRooted)
    return loadNewRootedTRA(howbig);
  else
    return loadNewUnrootedTRA(howbig);
}

void treefreeTRA(struct TreeAdaptor *tra)
{
  assert(tra);
  assert(tra->treefree);
  tra->treefree(tra);
}

struct TreeAdaptor *treecloneTRA(struct TreeAdaptor *tra)
{
  assert(tra);
  return tra->treeclone(tra);
}

struct LabelPerm *treegetlabelpermTRA(struct TreeAdaptor *tra)
{
  assert(tra);
  return tra->treegetlabelperm(tra);
}

struct AdjAdaptor *treegetadjaTRA(struct TreeAdaptor *tra)
{
  assert(tra);
  return tra->treegetadja(tra);
}

int treeIsQuartettable(struct TreeAdaptor *tra, int which)
{
  assert(tra);
  return tra->treeisquartetable(tra, which);
}

int treeIsFlippable(struct TreeAdaptor *tra, int which)
{
  assert(tra);
  return tra->treeisflippable(tra, which);
}
int treeIsRoot(struct TreeAdaptor *tra, int which)
{
  assert(tra);
  return tra->treeisroot(tra, which);
}


int treemutecountTRA(struct TreeAdaptor *tra)
{
  assert(tra);
  return tra->treemutecount(tra);
}

struct DoubleA *treeperimpairsTRA(struct TreeAdaptor *tra, struct CLNodeSet *flips)
{
  assert(tra);
  return tra->treeperimpairs(tra, flips);
}

int treeGetNodeCountTRA(struct TreeAdaptor *tra)
{
  struct AdjAdaptor *ad = treegetadjaTRA(tra);
  return adjaSize(ad);
}

static double nchoosefourScale(int n, int inp)
{
  double bign = n;
  bign = (bign * (bign - 1) * (bign - 2) * (bign - 3)) / 24.0;
  return ((double) inp) / bign;
}

double getTreeDifferenceScore(struct TreeAdaptor *tra1, struct TreeAdaptor *tra2)
{
  struct AdjAdaptor *ad1, *ad2;
  struct LabelPerm *lab1, *lab2;
  int diff;
  lab1 = treegetlabelpermTRA(tra1);
  lab2 = treegetlabelpermTRA(tra2);
  ad1 = treegetadjaTRA(tra1);
  ad2 = treegetadjaTRA(tra2);
  diff = countTrinaryDifferences(ad1, lab1, ad2, lab2);
  return nchoosefourScale(labelpermSize(lab1), diff);
}

