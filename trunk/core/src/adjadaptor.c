#include <complearn/complearn.h>

void adjaPrint(struct AdjAdaptor *aa)
{
  aa->vptr->adjaprint(aa);
}
int adjaSize(struct AdjAdaptor *aa)
{
  return aa->vptr->adjasize(aa);
}
void adjaFree(struct AdjAdaptor *aa)
{
  aa->vptr->adjafree(aa);
}
struct AdjAdaptor *adjaClone(struct AdjAdaptor *aa)
{
  return aa->vptr->adjaclone(aa);
}
int adjaGetConState(struct AdjAdaptor *aa, int i, int j)
{
  return aa->vptr->adjagetconstate(aa, i, j);
}
void adjaSetConState(struct AdjAdaptor *aa, int i, int j, int which)
{
  aa->vptr->adjasetconstate(aa, i, j, which);
}
int adjaNeighborCount(struct AdjAdaptor *aa, int i)
{
  return aa->vptr->adjagetneighborcount(aa, i);
}
int adjaNeighbors(struct AdjAdaptor *aa, int i, int *nbuf, int *nsize)
{
  return aa->vptr->adjagetneighbors(aa, i, nbuf, nsize);
}
struct DoubleA *adjaSPMMap(struct AdjAdaptor *aa)
{
  return aa->vptr->adjaspmmap(aa);
}
