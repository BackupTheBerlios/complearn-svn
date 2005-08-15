#ifndef __INCRDM_H
#define __INCRDM_H

#include <gsl/gsl_matrix.h>

/*! \file incrdm.h */

struct IncrementalDistMatrix;

struct IncrementalDistMatrix *newIDM(struct CompAdaptor *ca);

/** \brief Adds a DataBlock to the input IncrementalDistMatrix
 *
 *  Takes a DataBlock and calculates the NCDs between it and the DataBlocks
 *  previously added to the IncrementalDistMatrix.  An updated distance matrix
 *  can then be retrieved by calling getDistMatrixIDM().
 *  \param idm pointer to IncrementalDistMatrix
 *  \param db pointer to DataBlock to be added
 */
void addDataBlock(struct IncrementalDistMatrix *idm, struct DataBlock *db);

/** \brief Frees IncrementDistMatrix from memory
 *  \param idm pointer to IncrementalDistMatrix
 */
void freeIncrementalDistMatrix(struct IncrementalDistMatrix *idm);

gsl_matrix *getDistMatrixIDM(struct IncrementalDistMatrix *idm);
int getSizeIDM(struct IncrementalDistMatrix *idm);

#endif
