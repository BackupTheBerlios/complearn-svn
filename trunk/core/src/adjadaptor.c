#include <complearn.h>

void adjaPrint(struct AdjA *aa)
{
  aa->adjaprint(aa);
}
int adjaSize(struct AdjA *aa)
{
  return aa->adjasize(aa);
}
void adjaFree(struct AdjA *aa)
{
  aa->adjafree(aa);
}
struct AdjA *adjaClone(struct AdjA *aa)
{
  return aa->adjaclone(aa);
}
int adjaGetConState(struct AdjA *aa, int i, int j)
{
  return aa->adjagetconstate(aa, i, j);
}
void adjaSetConState(struct AdjA *aa, int i, int j, int which)
{
  aa->adjasetconstate(aa, i, j, which);
}
int adjaGetNeighborCount(struct AdjA *aa, int i)
{
  return aa->adjagetneighborcount(aa, i);
}
int adjaGetNeighbors(struct AdjA *aa, int i, int *nbuf, int *nsize)
{
  return aa->adjagetneighbors(aa, i, nbuf, nsize);
}
struct DoubleA *adjaSPMMap(struct AdjA *aa)
{
  return aa->adjaspmmap(aa);
}
