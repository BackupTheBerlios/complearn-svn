#include <complearn/complearn.h>

void treealabelpermSetColumnIndexToNodeNumber(struct TreeAdaptor *ta,
    int colInd, int nodeNum)
{
  if (ta == NULL) {
    clogError("NULL ptr in treealabelpermSetColumnIndexToNodeNumber()\n");
  }
  ta->treelpsetat(ta, colInd, nodeNum);
}

void treeaMutate(struct TreeAdaptor *tra)
{
  if (tra == NULL) {
    clogError("NULL ptr in treeaMutate()\n");
  }
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
  if (tra == NULL) {
    clogError("NULL ptr in treeaFree()\n");
  }
  assert(tra->treefree);
  tra->treefree(tra);
}

struct TreeAdaptor *treeaClone(struct TreeAdaptor *tra)
{
  if (tra == NULL) {
    clogError("NULL ptr in treeaClone()\n");
  }
  return tra->treeclone(tra);
}

struct LabelPerm *treeaLabelPerm(struct TreeAdaptor *tra)
{
  if (tra == NULL) {
    clogError("NULL ptr in treeaLabelPerm()\n");
  }
  return tra->treegetlabelperm(tra);
}

struct AdjAdaptor *treeaAdjAdaptor(struct TreeAdaptor *tra)
{
  if (tra == NULL) {
    clogError("NULL ptr in treeaAdjAdaptor()\n");
  }
  return tra->treegetadja(tra);
}

int treeaIsQuartettable(struct TreeAdaptor *tra, int which)
{
  if (tra == NULL) {
    clogError("NULL ptr in treeaIsQuartettable()\n");
  }
  return tra->treeisquartetable(tra, which);
}

int treeaIsFlippable(struct TreeAdaptor *tra, int which)
{
  if (tra == NULL) {
    clogError("NULL ptr in treeaIsFlippable()\n");
  }
  return tra->treeisflippable(tra, which);
}
int treeaIsRoot(struct TreeAdaptor *tra, int which)
{
  if (tra == NULL) {
    clogError("NULL ptr in treeaIsRoot()\n");
  }
  return tra->treeisroot(tra, which);
}


int treeaMutationCount(struct TreeAdaptor *tra)
{
  if (tra == NULL) {
    clogError("NULL ptr in treeaMutationCount()\n");
  }
  return tra->treemutecount(tra);
}

struct DoubleA *treeaPerimPairs(struct TreeAdaptor *tra, struct CLNodeSet *flips)
{
  if (tra == NULL) {
    clogError("NULL ptr in treeaPerimPairs()\n");
  }
  return tra->treeperimpairs(tra, flips);
}

int treeaNodeCount(struct TreeAdaptor *tra)
{
  struct AdjAdaptor *ad;
  if (tra == NULL) {
    clogError("NULL ptr in treeaNodeCount()\n");
  }
  ad = treeaAdjAdaptor(tra);
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
  int lps;
  if (tra1 == NULL || tra2 == NULL) {
    clogError("NULL ptr in treeaDifferenceScore()\n");
  }
  lab1 = treeaLabelPerm(tra1);
  lab2 = treeaLabelPerm(tra2);
  lps = labelpermSize(lab1);
  ad1 = treeaAdjAdaptor(tra1);
  ad2 = treeaAdjAdaptor(tra2);
  diff = countTrinaryDifferences(ad1, lab1, ad2, lab2);
  labelpermFree(lab1);
  labelpermFree(lab2);
  return nchoosefourScale(lps, diff);
}

struct DoubleA *treeaNodes(struct TreeAdaptor *ta)
{
  struct DoubleA *n = doubleaNew();
  union PCTypes p = zeropct;
  int i;
  if (ta == NULL) {
    clogError("NULL ptr in treeaNodes()\n");
  }
  for (i = 0; i < treeaNodeCount(ta) ;  i += 1) {
    p.i = i;
    doubleaPush(n,p);
  }
  return n;
}
