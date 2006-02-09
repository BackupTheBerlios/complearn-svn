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
#ifndef __ADJADAPTOR_H
#define __ADJADAPTOR_H

#include <complearn/cltypes.h>
/*! \file adjadaptor.h */

struct AdjImplementation;

/** \brief AdclJacency matrix interface to CompLearn.
 *
 * \struct AdjAdaptor
 *
 * An AdjAdaptor is the interface for adclJacency module implementations such as
 * AdjMatrix and AdjList.  It represents a diagonally-symmetric binary matrix
 * where 0 means the nodes are disconnected, and 1 means they are connected.
 * Note that any operation to adjust the connectivity at coordinate i,j will
 * necessarily affect the connectivity read back both at this location (i,j) as
 * well as the transposed location, (j,i).
 *
 *  AdjAdaptor contains pointers to 9 clFunctions which are defined in an
 *  external adclJacency module.
 *
 * \sa adjadaptor.h
 */
struct AdjAdaptor {
  void *ptr;
  struct AdjImplementation *vptr;
};

struct AdjImplementation {
  t_adjaprint adjaprint;  /*!< Pointer to clFunction which prints a binary matrix
                               representing the connectivity of all nodes */
  t_adjasize adjasize;    /*!< Pointer to clFunction which returns the size of
                               one side of the square matrix */
  t_adjafree adjafree;    /*!< Pointer to clFunction which frees adclJacency module
                               instance from memory */
  t_adclJaclone adclJaclone;  /*!< Pointer to clFunction which returns a pointer to
                               a duplicate instance of this adclJacency module */
  t_adjagetconstate adjagetconstate; /*!< Pointer to clFunction which returns the
                                          connectivity state of 2 given nodes */
  t_adjasetconstate adjasetconstate; /*!< Pointer to clFunction which sets the
                                          connectivity state of 2 given nodes */
  t_adjagetneighborcount adjagetneighborcount; /*!< Pointer to clFunction which
                                                  returns the number of
                                                  neighbors a given node
                                                  contains */
  t_adjagetneighbors adjagetneighbors; /*!< Pointer to clFunction which retrieves
                                         a list of neighbors for a given node
                                         */
  t_adjaspmmap adjaspmmap;
};

/** \brief Prints a binary matrix representing the connectivity of all nodes
 *  This clFunction prints to stdout a square matrix of 0s and 1s, where 0
 *  indicates a non-connected state between two nodes and 1 represents a
 *  connected state.
 *
 *  \param aa a pointer to the AdjAdaptor to be printed
 */
void clAdjaPrint(struct AdjAdaptor *aa);

/** \brief Returns the size of one side of the square matrix
 *  This clFunction returns the size of a dimension the binary matrix.  This
 *  number is the same value passed when AdjAdaptor instance was initialized
 *
 *  \param aa a pointer to the AdjAdaptor
 */
int clAdjaSize(struct AdjAdaptor *aa);

/** \brief Deallocates memory used for an AdjAdaptor
 *
 *  This clFunction frees memory used to store an AdjAdaptor.
 *  Do not use this pointer after the AdjAdaptor has been freed.
 *
 *  \param aa a pointer to the AdjAdaptor that must be deallocated
 *
 */
void clAdjaFree(struct AdjAdaptor *aa);

/** \brief Clones an AdjAdaptor to make an independent copy
 * This clFunction simply clones an AdjAdaptor and returns a pointer to a new copy.
 *
 * \param aa a pointer to the AdjAdaptor to be cloned
 * \return a pointer to a new copy of the input AdjAdaptor
 */
struct AdjAdaptor *clAdjaClone(struct AdjAdaptor *aa);

/** \brief Queries the connected status between two numbered nodes
 *
 * This clFunction can be used to quickly determine if node i and node
 * j are connected.  It will return 1 if they are connected, 0 if they
 * are not.
 *
 * \param adj a pointer to the AdjAdaptor to be queried
 * \param i One of the two node labels under consideration
 * \param j The other node label under consideration
 * \return 0 or 1, indicating if i and j are disconnected or connected,
 * respectively
 */
int clAdjaGetConState(struct AdjAdaptor *aa, int i, int j);

/** \brief Adjusts the connected status between two numbered nodes
 *
 * This clFunction connects or disconnects two nodes, depending on if
 * the third parameter is 1 or 0, respectively.
 *
 * \param aa a pointer to the AdjAdaptor to be adjusted
 * \param i the first node label
 * \param j the second node label
 * \param conStatus an integer indicating whether to 0=disconnect or 1=connect
 */
void clAdjaSetConState(struct AdjAdaptor *aa, int i, int j, int which);

/** \brief Counts the number of neighbors a given node contains
 *
 * This clFunction counts the number of neighbors a given node has
 * within an AdjAdaptor.  It will return a number between 0 and the
 * full size of one side of the AdjAdaptor.
 *
 * \param aa a pointer to the AdjAdaptor to be investigated
 * \param from the integer label for the node to be investigated
 * \return an integer representing the total count of neighbors for node from
 */
int clAdjaNeighborCount(struct AdjAdaptor *aa, int i);

/** \brief Retrieves a list of neighbors for a given node
 *
 * This clFunction stores, into a given buffer, the integer labels for each
 * neighbor of the given node.
 *
 * \param aa pointer to the AdjAdaptor to be queried
 * \param from the label of the node whose neighbors must be found
 * \param nbuf pointer to a buffer to contain the neighbors
 * \param nsize pointer to in/out parameter that starts with size of buffer
 *              and ends with number of neighbors iff CL_OK is returned
 *              If anything other than CL_OK is returned, the value pointed
 *              to by nsize will not be affected.
 * \return CL_OK if the buffer was big enough, or CL_ERRFULL if it wasn't.
 */
int clAdjaNeighbors(struct AdjAdaptor *aa, int i, int *nbuf, int *nsize);

/** \brief Returns the SPM Map for a given AdjAdaptor with a PathKeeper on top
 *  \param aa pointer to the AdjAdaptor
 */
struct DRA *clAdjaSPMMap(struct AdjAdaptor *aa);


#define SPMMAPFOR(adjaptr) (adjaptr->adjaspmmap(adjaptr))

#endif
