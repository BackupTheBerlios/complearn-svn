#ifndef __ADJMATRIX_H
#define __ADJMATRIX_H

/*! \file adjmatrix.h */

/** \brief AdjMatrix represents a symmetric, undirected adjacency matrix
 *
 * \struct AdjMatrix
 *
 * An AdjMatrix is a simple, fast data structure that allows for efficient
 * adjacency information tracking.  It represents a diagonally-symmetric
 * binary matrix where 0 means the nodes are disconnected, and 1 means they
 * are connected.  Note that any operation to adjust the connectivity at
 * coordinate i,j will necessarily affect the connectivity read back both
 * at this location (i,j) as well as the transposed location, (j,i).
 * This structure does not change size after it is created.
 *
 *  \sa adjmatrix.h
 */
struct AdjMatrix;
struct AdjAdaptor;
struct DoubleA;

/** \brief Create a new AdjMatrix with a given size
 *
 * This function creates a new AdjMatrix with the given dimension on a side.
 * This memory should be freed using freeAdjMatrix when it is no longer needed.
 * An AdjMatrix of size n supports node-labels in the range 0 to n-1, inclusive.
 *
 * \param howbig the size of one side of the square matrix
 * \return a pointer to a newly allocated AdjMatrix of the given size
 */
struct AdjMatrix *newAdjMatrix(int howbig);

/** \brief Clones an AdjMatrix to make an independent copy
 * This function simply clones an AdjMatrix and returns a new copy.
 *
 * \param inp a pointer to the AdjMatrix to be cloned
 * \return a pointer to a new copy of the input AdjMatrix
 */
struct AdjMatrix *cloneAdjMatrix(const struct AdjMatrix *inp);

/** \brief Deallocates memory used for an AdjMatrix
 *
 * This function frees memory used to store an AdjMatrix.
 * Do not use this pointer after the AdjMatrix has been freed.
 *
 * \param adj a pointer to the AdjMatrix that must be deallocated
 *
 */
void freeAdjMatrix(struct AdjMatrix *adj);

/** \brief Queries the connected status between two numbered nodes
 *
 * This function can be used to quickly determine if node i and node
 * j are connected.  It will return 1 if they are connected, 0 if they
 * are not.
 *
 * \param adj a pointer to the AdjMatrix to be queried
 * \param i One of the two node labels under consideration
 * \param j The other node label under consideration
 * \return 0 or 1, indicating if i and j are disconnected or connected,
 * respectively
 *
 */
int getConState(const struct AdjMatrix *adj, int i, int j);

/** \brief Adjusts the connected status between two numbered nodes
 *
 * This function connects or disconnects two nodes, depending on if
 * the third parameter is 1 or 0, respectively.
 *
 * \param adj a pointer to the AdjMatrix to be adjusted
 * \param i the first node label
 * \param j the second node label
 * \param conStatus an integer indicating whether to 0=disconnect or 1=connect
 */
void setConState(struct AdjMatrix *adj, int i, int j, int conState);

/** \brief Retrieves a list of neighbors for a given node
 *
 * This function stores, into a given buffer, the integer labels for each
 * neighbor of the given node.
 *
 * \param adj a pointer to the AdjMatrix to be queried
 * \param from the label of the node whose neighbors must be found
 * \param nbuf pointer to a buffer to contain the neighbors
 * \param nsize pointer to in/out parameter that starts with size of buffer
 *              and ends with number of neighbors iff CL_OK is returned
 *              If anything other than CL_OK is returned, the value pointed
 *              to by nsize will not be affected.
 * \return CL_OK if the buffer was big enough, or CL_ERRFULL if it wasn't.
 */
int getNeighbors(const struct AdjMatrix *adj, int from, int *nbuf, int *nsize);

/** \brief Counts the number of neighbors a given node contains
 *
 * This function counts the number of neighbors a given node has
 * within an AdjMatrix.  It will return a number between 0 and the
 * full size of one side of the AdjMatrix.
 *
 * \param adj a pointer to the AdjMatrix to be investigated
 * \param from the integer label for the node to be investigated
 * \return an integer representing the total count of neighbors for node from
 */
int getNeighborCount(const struct AdjMatrix *adj, int from);

/** \brief Returns the size of a given AdjMatrix
 *
 *  This function returns the size of a dimension of an AdjMatrix.  This
 *  number is the same value passed to newAdjMatrix() when a newly initialized
 *  AdjMatrix object is created.
 *
 *  \param adj a pointer to the AdjMatrix to be investigated
 *  \return an integer representing the size of an AdjMatrix
 */
int getAMSize(const struct AdjMatrix *adj);

/** \brief Prints a visual representation of an AdjMatrix to stdout
 *
 * This function prints an AdjMatrix where 1's represent a state of connection
 * between nodes, and 0's represent a state of disconnection.
 *
 * \param adj a pointer to the AdjMatrix to be printed
 */
void printAdjMatrix(const struct AdjMatrix *which);

struct AdjAdaptor *loadAdaptorAM(int howBig);

#endif
