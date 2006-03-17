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
#ifndef __ADJLIST_H
#define __ADJLIST_H

/*! \file adjlist.h */

/** \brief AdjList represents a symmetric, undirected adclJacency matrix
 *
 * An AdjList is a simple, fast data structure that allows for efficient
 * adclJacency information tracking.  It represents a diagonally-symmetric
 * binary matrix where 0 means the nodes are disconnected, and 1 means they
 * are connected.  Note that any operation to adjust the connectivity at
 * coordinate i,j will necessarily affect the connectivity read back both
 * at this location (i,j) as well as the transposed location, (j,i).
 * This structure does not change size after it is created.
 *
 *  \sa adjlist.h
 * \struct AdjList
 */
struct AdjList;
struct AdjAdaptor;
struct DRA;

/** \brief Create a new AdjList with a given size
 *
 * This function creates a new AdjList with the given dimension on a side.
 * This memory should be freed using clAdjlistFree when it is no longer needed.
 * An AdjList of size n supports node-labels in the range 0 to n-1, inclusive.
 *
 * \param howbig the number of nodes to support
 * \return a pointer to a newly allocated AdjList of the given size
 */
struct AdjList *clAdjlistNew(int howbig);

/** \brief Clones an AdjList to make an independent copy
 * This function simply clones an AdjList and returns a new copy.
 *
 * \param inp a pointer to the AdjList to be cloned
 * \return a pointer to a new copy of the input AdjList
 */
struct AdjList *clAdjlistClone(const struct AdjList *inp);

/** \brief Deallocates memory used for an AdjList
 *
 * This function frees memory used to store an AdjList.
 * Do not use this pointer after the AdjList has been freed.
 *
 * \param adj a pointer to the AdjList that must be deallocated
 *
 */
void clAdjlistFree(struct AdjList *adj);

/** \brief Queries the connected status between two numbered nodes
 *
 * This function can be used to quickly determine if node i and node
 * j are connected.  It will return 1 if they are connected, 0 if they
 * are not.
 *
 * \param adj a pointer to the AdjList to be queried
 * \param i One of the two node labels under consideration
 * \param j The other node label under consideration
 * \return 0 or 1, indicating if i and j are disconnected or connected,
 * respectively
 *
 */
int clAdjlistGetConState(const struct AdjList *adj, int i, int j);

/** \brief Adjusts the connected status between two numbered nodes
 *
 * This function connects or disconnects two nodes, depending on if
 * the third parameter is 1 or 0, respectively.
 *
 * \param adj a pointer to the AdjList to be adjusted
 * \param i the first node label
 * \param j the second node label
 * \param conStatus an integer indicating whether to 0=disconnect or 1=connect
 */
void clAdjlistSetConState(struct AdjList *adj, int i, int j, int conState);

/** \brief Retrieves a list of neighbors for a given node
 *
 * This function stores, into a given buffer, the integer labels for each
 * neighbor of the given node.
 *
 * \param adj a pointer to the AdjList to be queried
 * \param from the label of the node whose neighbors must be found
 * \param nbuf pointer to a buffer to contain the neighbors
 * \param nsize pointer to in/out parameter that starts with size of buffer
 *              and ends with number of neighbors iff CL_OK is returned
 *              If anything other than CL_OK is returned, the value pointed
 *              to by nsize will not be affected.
 * \return CL_OK if the buffer was big enough, or CL_ERRFULL if it wasn't.
 */
int clAdjlistNeighbors(const struct AdjList *adj, int from, int *nbuf, int *nsize);

/** \brief Counts the number of neighbors a given node contains
 *
 * This function counts the number of neighbors a given node has
 * within an AdjList.  It will return a number between 0 and the
 * full size of one side of the AdjList.
 *
 * \param adj a pointer to the AdjList to be investigated
 * \param from the integer label for the node to be investigated
 * \return an integer representing the total count of neighbors for node from
 */
int clAdjlistNeighborCount(const struct AdjList *adj, int from);

/** \brief Returns the size of a given AdjList)
 *
 *  This function returns the size of a dimension of an AdjList.  This
 *  number is the same value passed to clAdjlistNew() when a newly initialized
 *  AdjList object is created.
 *
 *  \param adj a pointer to the AdjList to be investigated
 *  \return an integer representing the size of an AdjList
 */
int clAdjlistSize(const struct AdjList *adj);

/** \brief Prints a visual representation of an AdjList to stdout
 *
 * This function prints an AdjList where 1's represent a state of connection
 * between nodes, and 0's represent a state of disconnection.
 * This function will print a number of lines equal to the number of nodes.
 *
 * \param adj a pointer to the AdjList to be printed
 */
void clAdjlistPrint(const struct AdjList *which);

/** \brief Allocates a new AdjList and wraps it within an AdjAdaptor
 *
 * This function allocates a new AdjList structure of the given size.  It
 * wraps this struct in an AdjAdaptor (adclJacency adaptor) structure and returns
 * a pointer to this newly allocated AdjAdaptor.  The caller owns this AdjAdaptor and
 * should free it using adjafree(aa).
 *
 * \param howBig maximum number of nodes that this AdjAdaptor can support
 * \returns pointer to the newly allocated AdjAdaptor struct
 */
struct AdjAdaptor *clAdjaLoadAdjList(int howBig);

#endif
