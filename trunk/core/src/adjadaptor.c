#include <complearn/complearn.h>

void clAdjaPrint(struct AdjAdaptor *aa)
{
  aa->vptr->adjaprint(aa);
}
int clAdjaSize(struct AdjAdaptor *aa)
{
  return aa->vptr->adjasize(aa);
}
void clAdjaFree(struct AdjAdaptor *aa)
{
  aa->vptr->adjafree(aa);
}
struct AdjAdaptor *clAdjaClone(struct AdjAdaptor *aa)
{
  return aa->vptr->adclJaclone(aa);
}
int clAdjaGetConState(struct AdjAdaptor *aa, int i, int j)
{
  return aa->vptr->adjagetconstate(aa, i, j);
}
void clAdjaSetConState(struct AdjAdaptor *aa, int i, int j, int which)
{
  aa->vptr->adjasetconstate(aa, i, j, which);
}
int clAdjaNeighborCount(struct AdjAdaptor *aa, int i)
{
  return aa->vptr->adjagetneighborcount(aa, i);
}
int clAdjaNeighbors(struct AdjAdaptor *aa, int i, int *nbuf, int *nsize)
{
  return aa->vptr->adjagetneighbors(aa, i, nbuf, nsize);
}
struct DRA *clAdjaSPMMap(struct AdjAdaptor *aa)
{
  return aa->vptr->adjaspmmap(aa);
}
