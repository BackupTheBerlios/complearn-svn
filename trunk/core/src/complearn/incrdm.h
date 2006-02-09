/*
* Copyright (c) 2006 Rudi Cilibrasi, Rulers of the RHouse
* All rights reserved.     cilibrar@cilibrar.com
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the RHouse nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE RULERS AND CONTRIBUTORS "AS IS" AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE RULERS AND CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef __INCRDM_H
#define __INCRDM_H

#include <complearn/complearn.h>
#include <complearn/cltypes.h>

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
 * This clFunction is the basic constructor for an IncrementalDistMatrix.
 * The user must supply a CompAdaptor to use for compression.
 *
 * \param ca pointer to the CompAdaptor to be used in this distance matrix
 * \return pointer to an IncrementalDistMatrix with no objects yet added
 */
struct IncrementalDistMatrix *clIncrdmNew(struct CompAdaptor *ca);

/** \brief Adds a DataBlock to an IncrementalDistMatrix
 *
 *  Takes a DataBlock and calculates the NCDs between it and the DataBlocks
 *  previously added to the IncrementalDistMatrix.  An updated distance matrix
 *  can then be retrieved by calling clIncrdmDistMatrix().
 *  \param idm pointer to IncrementalDistMatrix
 *  \param db pointer to DataBlock to be added
 */
void clIncrdmAddDataBlock(struct IncrementalDistMatrix *idm, struct DataBlock *db);

/** \brief Frees IncrementDistMatrix from memory
 *  \param idm pointer to IncrementalDistMatrix
 */
void clIncrdmFree(struct IncrementalDistMatrix *idm);

/** \brief Retrieves current snapshop the IncrementalDistMatrix is holding
 *  \param idm pointer to IncrementalDistMatrix to inspect
 *  \return pointer to gsl_matrix
 */
gsl_matrix *clIncrdmDistMatrix(struct IncrementalDistMatrix *idm);

/** \brief Retrieve size of IncrementalDistMatrix
 *  \param idm pointer to IncrementalDistMatrix
 *  \return int size of IncrementalDistMatrix
 */
int clIncrdmSize(struct IncrementalDistMatrix *idm);

#endif
