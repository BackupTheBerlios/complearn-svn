#ifndef __ADJADAPTOR_H
#define __ADJADAPTOR_H

#include <complearn/cltypes.h>
/*! \file adjadaptor.h */

/** \brief Adjacency matrix interface to CompLearn.
 *
 * \struct AdjA
 *
 * An AdjA is the interface for adjacency module implementations such as
 * AdjMatrix and AdjList.  It represents a diagonally-symmetric binary matrix
 * where 0 means the nodes are disconnected, and 1 means they are connected.
 * Note that any operation to adjust the connectivity at coordinate i,j will
 * necessarily affect the connectivity read back both at this location (i,j) as
 * well as the transposed location, (j,i).
 *
 *  AdjA contains pointers to 9 functions which are defined in an
 *  external adjacency module.
 *
 * \sa adjadaptor.h
 */
struct AdjA {
  void *ptr;
  t_adjaprint adjaprint;  /*!< Pointer to function which prints a binary matrix
                               representing the connectivity of all nodes */
  t_adjasize adjasize;    /*!< Pointer to function which returns the size of
                               one side of the square matrix */
  t_adjafree adjafree;    /*!< Pointer to function which frees adjacency module
                               instance from memory */
  t_adjaclone adjaclone;  /*!< Pointer to function which returns a pointer to
                               a duplicate instance of this adjacency module */
  t_adjagetconstate adjagetconstate; /*!< Pointer to function which returns the
                                          connectivity state of 2 given nodes */
  t_adjasetconstate adjasetconstate; /*!< Pointer to function which sets the
                                          connectivity state of 2 given nodes */
  t_adjagetneighborcount adjagetneighborcount; /*!< Pointer to function which
                                                  returns the number of
                                                  neighbors a given node
                                                  contains */
  t_adjagetneighbors adjagetneighbors; /*!< Pointer to function which retrieves
                                         a list of neighbors for a given node
                                         */
  t_adjaspmmap adjaspmmap;
};

/** \brief Prints a binary matrix representing the connectivity of all nodes
 *  This function prints to stdout a square matrix of 0s and 1s, where 0
 *  indicates a non-connected state between two nodes and 1 represents a
 *  connected state.
 *
 *  \param aa a pointer to the AdjA to be printed
 */
void adjaPrint(struct AdjA *aa);

/** \brief Returns the size of one side of the square matrix
 *  This function returns the size of a dimension the binary matrix.  This
 *  number is the same value passed when AdjA instance was initialized
 *
 *  \param aa a pointer to the AdjA
 */
int adjaSize(struct AdjA *aa);

/** \brief Deallocates memory used for an AdjA
 *
 *  This function frees memory used to store an AdjA.
 *  Do not use this pointer after the AdjA has been freed.
 *
 *  \param aa a pointer to the AdjA that must be deallocated
 *
 */
void adjaFree(struct AdjA *aa);

/** \brief Clones an AdjA to make an independent copy
 * This function simply clones an AdjA and returns a pointer to a new copy.
 *
 * \param aa a pointer to the AdjA to be cloned
 * \return a pointer to a new copy of the input AdjA
 */
struct AdjA *adjaClone(struct AdjA *aa);

/** \brief Queries the connected status between two numbered nodes
 *
 * This function can be used to quickly determine if node i and node
 * j are connected.  It will return 1 if they are connected, 0 if they
 * are not.
 *
 * \param adj a pointer to the AdjA to be queried
 * \param i One of the two node labels under consideration
 * \param j The other node label under consideration
 * \return 0 or 1, indicating if i and j are disconnected or connected,
 * respectively
 */
int adjaGetConState(struct AdjA *aa, int i, int j);

/** \brief Adjusts the connected status between two numbered nodes
 *
 * This function connects or disconnects two nodes, depending on if
 * the third parameter is 1 or 0, respectively.
 *
 * \param aa a pointer to the AdjA to be adjusted
 * \param i the first node label
 * \param j the second node label
 * \param conStatus an integer indicating whether to 0=disconnect or 1=connect
 */
void adjaSetConState(struct AdjA *aa, int i, int j, int which);

/** \brief Counts the number of neighbors a given node contains
 *
 * This function counts the number of neighbors a given node has
 * within an AdjA.  It will return a number between 0 and the
 * full size of one side of the AdjA.
 *
 * \param aa a pointer to the AdjA to be investigated
 * \param from the integer label for the node to be investigated
 * \return an integer representing the total count of neighbors for node from
 */
int adjaGetNeighborCount(struct AdjA *aa, int i);

/** \brief Retrieves a list of neighbors for a given node
 *
 * This function stores, into a given buffer, the integer labels for each
 * neighbor of the given node.
 *
 * \param aa pointer to the AdjA to be queried
 * \param from the label of the node whose neighbors must be found
 * \param nbuf pointer to a buffer to contain the neighbors
 * \param nsize pointer to in/out parameter that starts with size of buffer
 *              and ends with number of neighbors iff CL_OK is returned
 *              If anything other than CL_OK is returned, the value pointed
 *              to by nsize will not be affected.
 * \return CL_OK if the buffer was big enough, or CL_ERRFULL if it wasn't.
 */
int adjaGetNeighbors(struct AdjA *aa, int i, int *nbuf, int *nsize);

/** \brief Returns the SPM Map for a given AdjA with a PathKeeper on top
 *  \param aa pointer to the AdjA
 */
struct DoubleA *adjaSPMMap(struct AdjA *aa);


#define SPMMAPFOR(adjaptr) (adjaptr->adjaspmmap(adjaptr))

#endif
