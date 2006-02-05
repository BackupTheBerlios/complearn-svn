#include <complearn/complearn.h>

void clTreeaclLabelpermSetColumnIndexToNodeNumber(struct TreeAdaptor *ta,
    int colInd, int nodeNum)
{
  ta->treelpsetat(ta, colInd, nodeNum);
}

void clTreeaMutate(struct TreeAdaptor *tra)
{
  assert(tra);
  tra->treemutate(tra);
}

struct TreeAdaptor *clTreeaNew(int isRooted, int howbig)
{
  if (isRooted)
    return clTreeaLoadRootedBinary(howbig);
  else
    return clTreeaLoadUnrooted(howbig);
}

void clTreeaFree(struct TreeAdaptor *tra)
{
  assert(tra);
  assert(tra->treefree);
  tra->treefree(tra);
}

struct TreeAdaptor *clTreeaClone(struct TreeAdaptor *tra)
{
  assert(tra);
  return tra->treeclone(tra);
}

struct LabelPerm *clTreeaLabelPerm(struct TreeAdaptor *tra)
{
  assert(tra && "NULL TreeAdaptor in clTreeaLabelPerm");
  return tra->treegetlabelperm(tra);
}

struct AdjAdaptor *clTreeaAdjAdaptor(struct TreeAdaptor *tra)
{
  assert(tra);
  return tra->treegetadja(tra);
}

int clTreeaIsQuartettable(struct TreeAdaptor *tra, int which)
{
  assert(tra);
  return tra->treeisquartetable(tra, which);
}

int clTreeaIsFlippable(struct TreeAdaptor *tra, int which)
{
  assert(tra);
  return tra->treeisflippable(tra, which);
}
int clTreeaIsRoot(struct TreeAdaptor *tra, int which)
{
  assert(tra);
  return tra->treeisroot(tra, which);
}


int clTreeaMutationCount(struct TreeAdaptor *tra)
{
  assert(tra);
  return tra->treemutecount(tra);
}

struct DRA *clTreeaPerimPairs(struct TreeAdaptor *tra, struct CLNodeSet *flips)
{
  assert(tra);
  return tra->treeperimpairs(tra, flips);
}

int clTreeaNodeCount(struct TreeAdaptor *tra)
{
  struct AdjAdaptor *ad = clTreeaAdjAdaptor(tra);
  return clAdjaSize(ad);
}

static double nchoosefourScale(int n, int inp)
{
  double bign = n;
  bign = (bign * (bign - 1) * (bign - 2) * (bign - 3)) / 24.0;
  return ((double) inp) / bign;
}

double clTreeaDifferenceScore(struct TreeAdaptor *tra1, struct TreeAdaptor *tra2)
{
  struct AdjAdaptor *ad1, *ad2;
  struct LabelPerm *lab1, *lab2;
  int diff;
  int lps;
  lab1 = clTreeaLabelPerm(tra1);
  lab2 = clTreeaLabelPerm(tra2);
  lps = clLabelpermSize(lab1);
  ad1 = clTreeaAdjAdaptor(tra1);
  ad2 = clTreeaAdjAdaptor(tra2);
  diff = clCountTrinaryDifferences(ad1, lab1, ad2, lab2);
  clLabelpermFree(lab1);
  clLabelpermFree(lab2);
  return nchoosefourScale(lps, diff);
}

struct DRA *clTreeaNodes(struct TreeAdaptor *ta)
{
  struct DRA *n = clDraNew();
  union PCTypes p = zeropct;
  int i;
  for (i = 0; i < clTreeaNodeCount(ta) ;  i += 1) {
    p.i = i;
    clDraPush(n,p);
  }
  return n;
}
