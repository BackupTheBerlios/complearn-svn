#ifndef __INCRDM_H
#define __INCRDM_H

#include <gsl/gsl_matrix.h>
#include <complearn/complearn.h>

#define MAXDATABLOCK 256

/*! \file incrdm.h */

/** \brief an incrementally-calculated distance matrix
 *
 * This object allows for a smoothly growing distance matrix gaining just
 * one new object at each step.  This is useful when a set starts out empty
 * and then files are added one at a time but never removed.  This is faster
 * than recomputing the entire n x n distance matrix each time a single new
 * file is added because in the IncrementalDistMatrix only the new row is
 * calculated but the rest of the counts are saved.
 *
 * \sa incrdm.h
 *
 * \struct IncrementalDistMatrix
 */
struct IncrementalDistMatrix {
  int dbcount;
  struct DataBlock *db[MAXDATABLOCK];
  double singlesize[MAXDATABLOCK];
  gsl_matrix *curmat;
  gsl_matrix_view result;
  struct CompAdaptor *ca;
};


/** \brief creates a new IncrementalDistMatrix using a given CompAdaptor
 *
 * This function is the basic constructor for an IncrementalDistMatrix.
 * The user must supply a CompAdaptor to use for compression.
 *
 * \param ca pointer to the CompAdaptor to be used in this distance matrix
 * \return pointer to an IncrementalDistMatrix with no objects yet added
 */
struct IncrementalDistMatrix *incrdmNew(struct CompAdaptor *ca);

/** \brief Adds a DataBlock to an IncrementalDistMatrix
 *
 *  Takes a DataBlock and calculates the NCDs between it and the DataBlocks
 *  previously added to the IncrementalDistMatrix.  An updated distance matrix
 *  can then be retrieved by calling incrdmDistMatrix().
 *  \param idm pointer to IncrementalDistMatrix
 *  \param db pointer to DataBlock to be added
 */
void incrdmAddDataBlock(struct IncrementalDistMatrix *idm, struct DataBlock *db);

/** \brief Frees IncrementDistMatrix from memory
 *  \param idm pointer to IncrementalDistMatrix
 */
void incrdmFree(struct IncrementalDistMatrix *idm);

/** \brief Retrieves current snapshop the IncrementalDistMatrix is holding
 *  \param idm pointer to IncrementalDistMatrix to inspect
 *  \return pointer to gsl_matrix
 */
gsl_matrix *incrdmDistMatrix(struct IncrementalDistMatrix *idm);

/** \brief Retrieve size of IncrementalDistMatrix
 *  \param idm pointer to IncrementalDistMatrix
 *  \return int size of IncrementalDistMatrix
 */
int incrdmSize(struct IncrementalDistMatrix *idm);

#endif
