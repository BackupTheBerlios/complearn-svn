#ifndef __PATHKEEPER_H
#define __PATHKEEPER_H

#include <complearn/cltypes.h>
struct AdjAdaptor;
struct CLNodeSet;
/*! \file pathkeeper.h */

/** \brief finds a path between any two nodes in the tree
 *
 * This function uses an SPMMap DRA to find the path quickly between
 * two nodes in a tree.
 *
 * To use this function, you must first have an array of integers
 * preallocated with space enough for the path.  For a tree of n nodes,
 * a buffer of size N integers will be sufficient for any path.  You
 * must also have space for one more integer to store the buffer size and
 * path length; prior to calling this function, set this integer to
 * indicate the size, measured in int-sized (4-byte) units, of the buffer
 * you have passed in.  If the function is successful, it will return
 * CL_OK and adjust the integer pointed to by bufsize to indicate the
 * size of the resultant path.  If the buffer was too small to hold the
 * path, the CL_ERRFULL will be returned and the value pointed to by
 * bufsize will be unaffected.
 *
 * \param ub pointer to the UnrootedBinary to be path queried
 * \param from starting node identifier
 * \param to destination node identifier
 * \param pathbuf pointer to integer array to hold resultant path
 * \param bufsize pointer to IN/OUT buffer size / return path size
 * \return CL_OK or CL_ERRFULL
 */
int pathFinder(struct AdjAdaptor *ad, qbase_t from, qbase_t to, int *pathbuf, int *bufsize);

/** \brief tests to determine if two trees are identical
 *
 * This function returns 0 to indicdate that two trees are different, and
 * 1 to indicate that they are identical.  This function is order insensitive.
 * It will only compare neighbor connectivity.
 *
 * \param ad1 pointer to one of the AdjAdaptor to compare
 * \param lab1 pointer to LabelPerm label assignments for ad1
 * \param ad2 pointer to another of the two AdjAdaptor to compare
 * \param lab2 pointer to LabelPerm label assignments for ad2
 * \return integer to be interpretted as a boolean indicating the trees are
 * identical
 */
int isIdenticalTree(struct AdjAdaptor *ad1, struct LabelPerm *lab1, struct AdjAdaptor *adj2, struct LabelPerm *lab2);

/** \brief PathKeeper holds an SPM for an AdjAdaptor as a decorator
 *
 * \struct PathKeeper
 *
 * An PathKeeper is a simple, fast data structure that allows for efficient
 * path calculation over a polymorphic adjacency container.
 *
 * \sa pathkeeper.h
 */
struct AdjAdaptor;

/** \brief Create a new PathKeeper with a given size
 *
 * This function creates a new PathKeeper with the given dimension on a side.
 * This memory should be freed using freePathKeeper when it is no longer needed.
 * An PathKeeper of size n supports node-labels in the range 0 to n-1, inclusive.
 *
 * \param basis the "real" underlying AdjAdaptor implementing the connections
 * \return a pointer to a newly allocated AdjAdaptor of the given size
 */
struct AdjAdaptor *newPathKeeper(struct AdjAdaptor *basis);

/** \brief Calculates a Shortest Path Map for the given binary tree and
 * starting from the given node
 *
 * This function calculates a Shortest Path Tree from a given starting
 * node and on a given tree.  A Shortest Path Tree is a set of paths
 * going to the given node, termed "from".  These paths are calculated
 * using the Dijktra-Prim shortest path tree algorithm.  The results are
 * stored in a DRA using the .i field of PCTypes.  In the result,
 * draGetValueAt(result, n).i
 * holds the node-identifier that one must move starting at node n in
 * order to get to the specified node "from".  For all node identifiers in
 * the tree i, draGetValueAt(result, i) will be defined.  Thus, this allows for
 * rapid path calculations during quartet scoring later.  This function is
 * used by pathFinder.
 *
 * \param ub pointer to the UnrootedBinary to be path mapped
 * \param from qbase_t node identifier indicating the destination node
 * \return pointer to a DRA holding an SPM for the node "from"
 */
struct DRA *makeSPMFor(struct AdjAdaptor *aa, qbase_t from);

/** \brief Calculates an All Points Shortest Path Map for the given binary tree
 *
 * This function computes an SPM using makeSPMFor for each node in this
 * tree.  The SPM for node i is stored in draGetValueAt(result, i).ar
 *
 * This function uses the .ar field in the PCTypes DRA to store a
 * nested set of SPM's, one for each node.  This is used in the pathFinder.
 *
 * \param ub pointer to the UnrootedBinary to be path mapped
 * \return pointer to a level-1 nested DRA containing SPM's
 */
struct DRA *makeSPMMap(struct AdjAdaptor *aa);

/** \brief Frees the memory associated with a full set of SPM Maps
 *
 * This function is useful in conjunction with makeSPMMap.
 *
 * \param spmmap pointer to the DRA containing nested DRA
 * \return nothing
 */
void freeSPMMap(struct DRA *spmmap);

void walkTree(struct AdjAdaptor *ad,
    struct DRA *result, struct DRA *border, struct CLNodeSet *done,
    int breadthFirst,
    struct CLNodeSet *flipped);
int countTrinaryDifferences(struct AdjAdaptor *ad1, struct LabelPerm *lab1, struct AdjAdaptor *ad2, struct LabelPerm *lab2);
int findConsistentIndex(struct AdjAdaptor *ad, struct LabelPerm *lab, qbase_t labels[4]);

struct DRA *simpleWalkTree(struct TreeAdaptor *ta, struct CLNodeSet *flips);

#endif
