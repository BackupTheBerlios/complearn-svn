#ifndef __ROOTEDBINARY_H
#define __ROOTEDBINARY_H

#include <complearn/quartet.h>

/*! \file rootedbinary.h */

/** \brief An undirected, rooted binary tree
 *
 *  \struct RootedBinary
 *
 * \sa rootedbinary.h
 *
 * A RootedBinary holds the connection information associated with a
 * particular tree.  Each node has either 1 or 3 neighbors; leaves have only
 * 1, and the rest are kernel nodes with 3 neighbors each.
 */
struct RootedBinary;
struct LabelPerm;
struct AdjAdaptor;

struct StringStack;

/** \brief Allocates and initializes a new unrooted binary tree with enough
 * room to hold the specified number of leaf-nodes.
 *
 * If an unrooted binary tree
 * is to support k leaves, then it must have k-2 kernel nodes, for a total
 * of 2 * k - 2 nodes.  When a new tree is created, it is made by a simple
 * caterpillar-style pattern.  To get random variation, use the
 * unrootedbinaryDoComplexMutation function with the unrootedbinaryClone function.
 * This function allocates memory which must eventually be freed with
 * rootedbinaryFree.
 *
 *  \param howManyLeaves an integer >=4 indicating how many leaves the tree
 *  should have.
 *  \param uaa an optional parameter specifying an AdjAdaptor (without pathing)
 *  to use for initial connectivity.  This may be NULL to specify a default
 *  catterpillar type connection pattern.
 *  \param ulabelperm an optional parameter specifying which leaves are placed where in the tree.  This may be NULL as well.
 *  \return a pointer to a newly allocated tree
 */
struct RootedBinary *rootedbinaryNew(int howManyLeaves, struct AdjAdaptor *uaa, struct LabelPerm *ulabelperm);

/** \brief Applies a complex mutation to a tree.
 *
 * This function is the most convenient source of variation in trees.
 * It uses a coin flip to determine how many simple mutations to do, as
 * specified in the Quartet Tree paper by Cilibrasi and Vitanyi.
 * It then applies the requisite number of simple mutations, using the
 * builtin random-number generator rand().  This function may be used
 * in combination with unrootedbinaryClone to allow several different tries from
 * the same tree starting point.  It may also be used with the TreeScore
 * module which can allow for quartet-based hill-climbing algorithms.
 *
 * \param ub pointer to the RootedBinary to be modified.
 * \return nothing
 */
void rootedbinaryComplexMutation(struct RootedBinary *ub);

/** \brief Clones an RootedBinary tree, allocating new memory for the copy
 *
 * This function copies an RootedBinary tree object.  This can be used
 * in conjunction with unrootedbinaryDoComplexMutation for hill-climbing search.
 * This function allocates memory which must eventually be freed with
 * rootedbinaryFree.
 * \param ub pointer to the RootedBinary that must be cloned
 * \return pointer to the new copy of the original tree
 */
struct RootedBinary *rootedbinaryClone(const struct RootedBinary *ub);

/** \brief Indicates whether a given node identifier is a place for
 * a quartet-leaf object-label.
 *
 * This function allows the user to determine if a given node identifier
 * needs to have a label attached or not.  It returns a true value != 0
 * if a label should be placed at this node.
 *
 * \param ub pointer to the RootedBinary that must be examined
 * \param which qbase_t indicating the node identifier under investigation
 * \return an integer value to be interpretted in a boolean context
 */
int rootedbinaryIsQuartetableNode(const struct RootedBinary *ub, qbase_t which);

/** \brief Indicates whether a given node has orderable children.
 *
 * This function allows the user to determine if a given node identifier
 * has at least 2 children that can be flipped in more than one order.
 * This bit may be adjusted using unrootedbinaryFlipNodeLayout
 * Leaf nodes are thus not flippable, but kernel nodes are.
 *
 * \param ub pointer to the RootedBinary that must be examined
 * \param which qbase_t indicating the node identifier under investigation
 * \return an integer value to be interpretted in a boolean context
 */
int rootedbinaryIsFlippableNode(struct RootedBinary *ub, qbase_t which);

/** \brief Returns the first node returned in tree traversals
 *
 * This function allows the user to determine the starting node for
 * tree traversals on this tree.  This applies to the walkTree function.
 *
 * \param ub pointer to the RootedBinary that must be reordered
 * \return qbase_t indicating which node is first traversed in this tree
 */
qbase_t rootedbinaryStartingNode(const struct RootedBinary *ub);

/** \brief Returns a DRA contained an ordered traversal of all
 * the nodes in the tree.
 *
 * This function allows the user to get a dynamically-allocated list of
 * all the nodes in this tree.  They are returned in depth first order.
 * This is equivalent to the walkTree function with a 0 breadthFirst
 * parameter; thus the order returned by unrootedbinaryNodes will be affected
 * by the node child order flip bits.
 *
 * The DRA returned by this function use the .i field of PCTypes:
 *
 * struct DRA *da = unrootedbinaryNodes(ub);
 * int i;
 * for (i = 0; i < draSize(da); i += 1)
 *   printf("Got node %d\n", draGetValueAt(da, i).i);
 *
 * \param ub pointer to the RootedBinary that must be reordered
 * \return pointer to a DRA holding the list of all tree nodes
 */
struct DRA *rootedbinaryNodes(const struct RootedBinary *ub);


/** \brief Returns a list of all adjacent border-pair node identifiers
 * on the tree.
 *
 * In any given ordered binary tree, we may start at node 0 and perform
 * a depth-first traversal to accumulate a list of nodes.  We may discard
 * all kernel nodes, leaving only a list of leaf-nodes.  In an unrooted
 * binary tree, this list may be thought of as a circle comprised of
 * leaf nodes.  For a tree with k leaves, there are k leaf-node pairs
 * formed in this way.  This function allows the user to retrieve a list
 * of node identifier pairs along the entire tree perimeter.
 *
 * The node identifiers are accessed using the .ip.x and .ip.y members of
 * PCTypes.  For example:
 *
 * struct DRA *da = unrootedbinaryPerimPairs(ub);
 * int i;
 * for (i = 0; i < draSize(da); i += 1) {
 *   union PCTypes pct = draGetValueAt(da, i);
 *   printf("perimeter pair: node %d and node %d\n", pct.ip.x, pct.ip.y);
 * }
 *
 * \param ub pointer to the RootedBinary that must be reordered
 * \return pointer to a DRA holding the list of all perimeter pairs
 */
struct DRA *rootedbinaryPerimeterPairs(const struct RootedBinary *rb, struct CLNodeSet *flips);

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

/** \brief Frees the memory associated with an RootedBinary
 *
 * This function frees the memory associated with an RootedBinary.
 * After calling this function it is safest to set the pointer to NULL,
 * as you shouldn't try to use it after deallocation.  This function must
 * be called for every time rootedbinaryNew, unrootedbinaryClone, or loadTree is called.
 *
 * \param ub pointer to the RootedBinary that must be freed.
 * \return nothing
 */
void rootedbinaryFree(struct RootedBinary *ub);

/** \brief Returns a permutation array holding the column-index to leaf-node
 * identifier mapping in effect for this tree.
 *
 * When mapping a set of leaf-labels to a graph or tree, there is always
 * the possibility to permute the leaf labels.  This is handled with a
 * permutation array.  The entry at
 * draGetValueAt(leaflabels, r).i
 * indicates the node-identifier of the leaf where datamatrix column index
 * indicator r should be placed.
 *
 * \param ub pointer to the RootedBinary to be examined
 * \return pointer to a DRA containing leaf label positions
 */
struct DRA *rootedbinaryLeafLabels(const struct RootedBinary *ub);

/** \brief This function returns the number of simple mutations used in
 * the most recent complex mutation step taken with unrootedbinaryDoComplexMutation
 *
 * \param ub pointer to the RootedBinary to be path queried
 * \return integer indicating the number of simple mutation steps last used
 */
int rootedbinaryLastMutationCount(const struct RootedBinary *ub);

/** \brief Indicates whether or not a given pair of nodes are connected.
 *
 * This function returns a true value if and only if the two nodes with the
 * given node identifiers are connected.  A node may not connect to itself.
 *
 * \param ub pointer to the RootedBinary to be path queried
 * \param a first node identifier
 * \param b second node identifier
 * \return integer to be interpretted in a boolean context
 */
int rootedbinaryAreNodesConnected(const struct RootedBinary *ub, qbase_t a, qbase_t b);

struct LabelPerm *rootedbinaryLabelPerm(struct RootedBinary *ub);


struct AdjAdaptor *rootedbinaryAdjAdaptor(struct RootedBinary *ub);

struct TreeAdaptor *treeaLoadRootedBinary(int howBig);
#endif

