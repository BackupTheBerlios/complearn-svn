#include <complearn.h>

void adjaPrint(struct AdjAdaptor *aa)
{
  aa->adjaprint(aa);
}
int adjaSize(struct AdjAdaptor *aa)
{
  return aa->adjasize(aa);
}
void adjaFree(struct AdjAdaptor *aa)
{
  aa->adjafree(aa);
}
struct AdjAdaptor *adjaClone(struct AdjAdaptor *aa)
{
  return aa->adjaclone(aa);
}
int adjaGetConState(struct AdjAdaptor *aa, int i, int j)
{
  return aa->adjagetconstate(aa, i, j);
}
void adjaSetConState(struct AdjAdaptor *aa, int i, int j, int which)
{
  aa->adjasetconstate(aa, i, j, which);
}
int adjaNeighborCount(struct AdjAdaptor *aa, int i)
{
  return aa->adjagetneighborcount(aa, i);
}
int adjaNeighbors(struct AdjAdaptor *aa, int i, int *nbuf, int *nsize)
{
  return aa->adjagetneighbors(aa, i, nbuf, nsize);
}
struct DoubleA *adjaSPMMap(struct AdjAdaptor *aa)
{
  return aa->adjaspmmap(aa);
}
