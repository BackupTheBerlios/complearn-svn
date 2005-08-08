#ifndef __ADJADAPTOR_H
#define __ADJADAPTOR_H

#include <complearn/cltypes.h>

/** \brief Adjacency matrix interface
 *
 * \struct AdjA
 *
 */
struct AdjA {
  void *ptr;
  t_adjaprint adjaprint;
  t_adjasize adjasize;
  t_adjafree adjafree;
  t_adjaclone adjaclone;
  t_adjagetconstate adjagetconstate;
  t_adjasetconstate adjasetconstate;
  t_adjagetneighborcount adjagetneighborcount;
  t_adjagetneighbors adjagetneighbors;
  t_adjaspmmap adjaspmmap;
};

void adjaPrint(struct AdjA *aa);
int adjaSize(struct AdjA *aa);
void adjaFree(struct AdjA *aa);
struct AdjA *adjaClone(struct AdjA *aa);
int adjaGetConState(struct AdjA *aa, int i, int j);
void adjaSetConState(struct AdjA *aa, int i, int j, int which);
int adjaGetNeighborCount(struct AdjA *aa, int i);
int adjaGetNeighbors(struct AdjA *aa, int i, int *nbuf, int *nsize);
struct DoubleA *adjaSPMMap(struct AdjA *aa);

/* Returns the SPM Map for a given AdjA with a PathKeeper on top */

#define SPMMAPFOR(adjaptr) (adjaptr->adjaspmmap(adjaptr))

#endif
