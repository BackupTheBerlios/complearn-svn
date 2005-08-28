#ifndef __CLTREE_H
#define __CLTREE_H

/*! \file unrootedbinary.h */

/* A suggested maximum number of neighbors per node.  This is not enforced. */
#define MAXNEIGHBORS 16

#include <complearn/quartet.h>

/*! \file unrootedbinary.h */

/** \brief An undirected, unrooted binary tree
 *
 *  \struct UnrootedBinary
 *
 * \sa unrootedbinary.h
 *
 * An UnrootedBinary holds the connection information associated with a
 * particular tree.  Each node has either 1 or 3 neighbors; leaves have only
 * 1, and the rest are kernel nodes with 3 neighbors each.  There is no
 * root.
 */
struct UnrootedBinary;
struct LabelPerm;
struct AdjAdaptor;
struct TreeAdaptor;

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
 * unrootedbinaryFree.
 *
 *  \param howManyLeaves an integer >=4 indicating how many leaves the tree
 *  should have.
 *  \return a pointer to a newly allocated tree
 */
struct UnrootedBinary *unrootedbinaryNew(int howManyLeaves);

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
 * \param ub pointer to the UnrootedBinary to be modified.
 * \return nothing
 */
void unrootedbinaryDoComplexMutation(struct UnrootedBinary *ub);

/** \brief Clones an UnrootedBinary tree, allocating new memory for the copy
 *
 * This function copies an UnrootedBinary tree object.  This can be used
 * in conjunction with unrootedbinaryDoComplexMutation for hill-climbing search.
 * This function allocates memory which must eventually be freed with
 * unrootedbinaryFree.
 * \param ub pointer to the UnrootedBinary that must be cloned
 * \return pointer to the new copy of the original tree
 */
struct UnrootedBinary *unrootedbinaryClone(const struct UnrootedBinary *ub);

/** \brief Indicates whether a given node identifier is a place for
 * a quartet-leaf object-label.
 *
 * This function allows the user to determine if a given node identifier
 * needs to have a label attached or not.  It returns a true value != 0
 * if a label should be placed at this node.
 *
 * \param ub pointer to the UnrootedBinary that must be examined
 * \param which qbase_t indicating the node identifier under investigation
 * \return an integer value to be interpretted in a boolean context
 */
int unrootedbinaryIsQuartetableNode(const struct UnrootedBinary *ub, qbase_t which);

/** \brief Indicates whether a given node has orderable children.
 *
 * This function allows the user to determine if a given node identifier
 * has at least 2 children that can be "flipped" in more than one order.
 * This bit may be adjusted using unrootedbinaryFlipNodeLayout
 * Leaf nodes are thus not flippable, but kernel nodes are.
 *
 * \param ub pointer to the UnrootedBinary that must be examined
 * \param which qbase_t indicating the node identifier under investigation
 * \return an integer value to be interpretted in a boolean context
 */
int isFlippableNode(struct UnrootedBinary *ub, qbase_t which);
/** \brief Inverts a single node's child ordering
 *
 * This function allows the user to invert the order that children are
 * traversed in this node.  This ordering applies only to walkTree traversals.
 * Thus, this order may or may not be significant in your application.
 * See also isFlippableNode
 * Leaf nodes are thus not flippable, but kernel nodes are.
 *
 * \param ub pointer to the UnrootedBinary that must be reordered
 * \param which qbase_t indicating the node identifier to flip
 * \return nothing
 */
void unrootedbinaryFlipNodeLayout(struct UnrootedBinary *ub, qbase_t which);

/** \brief Just flips a random flippable node
 *
 * More convenenient than unrootedbinaryFlipNodeLayout
 *
 * \param ub pointer to the UnrootedBinary that must be reordered
 * \return nothing
 */
void unrootedbinaryRandomFlipNodeLayout(struct UnrootedBinary *ub);

/** \brief Returns the first node returned in tree traversals
 *
 * This function allows the user to determine the starting node for
 * tree traversals on this tree.  This applies to the walkTree function.
 *
 * \param ub pointer to the UnrootedBinary that must be reordered
 * \return qbase_t indicating which node is first traversed in this tree
 */
qbase_t unrootedbinaryStartingNode(const struct UnrootedBinary *ub);

/** \brief Returns a DoubleA contained an ordered traversal of all
 * the nodes in the tree.
 *
 * This function allows the user to get a dynamically-allocated list of
 * all the nodes in this tree.  They are returned in depth first order.
 * This is equivalent to the walkTree function with a 0 breadthFirst
 * parameter; thus the order returned by unrootedbinaryNodes will be affected
 * by the node child order flip bits.
 *
 * The DoubleA returned by this function use the .i field of PCTypes:
 *
 * struct DoubleA *da = unrootedbinaryNodes(ub, NULL);
 * int i;
 * for (i = 0; i < doubleaSize(da); i += 1)
 *   printf("Got node %d\n", doubleaGetValueAt(da, i).i);
 *
 * \param ub pointer to the UnrootedBinary that must be reordered
 * \return pointer to a DoubleA holding the list of all tree nodes
 */
struct DoubleA *unrootedbinaryNodes(const struct UnrootedBinary *ub, struct CLNodeSet *flips);


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
 * struct DoubleA *da = unrootedbinaryPerimPairs(ub, NULL);
 * int i;
 * for (i = 0; i < doubleaSize(da); i += 1) {
 *   union PCTypes pct = doubleaGetValueAt(da, i);
 *   printf("perimeter pair: node %d and node %d\n", pct.ip.x, pct.ip.y);
 * }
 *
 * \param ub pointer to the UnrootedBinary that must be reordered
 * \return pointer to a DoubleA holding the list of all perimeter pairs
 */
struct DoubleA *unrootedbinaryPerimPairs(const struct UnrootedBinary *ub, struct CLNodeSet *flips);

/** \brief Frees the memory associated with an UnrootedBinary
 *
 * This function frees the memory associated with an UnrootedBinary.
 * After calling this function it is safest to set the pointer to NULL,
 * as you shouldn't try to use it after deallocation.  This function must
 * be called for every time unrootedbinaryNew, unrootedbinaryClone, or loadTree is called.
 *
 * \param ub pointer to the UnrootedBinary that must be freed.
 * \return nothing
 */
void unrootedbinaryFree(struct UnrootedBinary *ub);

/** \brief Returns a permutation array holding the column-index to leaf-node
 * identifier mapping in effect for this tree.
 *
 * When mapping a set of leaf-labels to a graph or tree, there is always
 * the possibility to permute the leaf labels.  This is handled with a
 * permutation array.  The entry at
 * doubleaGetValueAt(leaflabels, r).i
 * indicates the node-identifier of the leaf where datamatrix column index
 * indicator r should be placed.
 *
 * \param ub pointer to the UnrootedBinary to be examined
 * \return pointer to a DoubleA containing leaf label positions
 */
struct DoubleA *unrootedbinaryLeafLabels(const struct UnrootedBinary *ub);

/** \brief This function returns the number of simple mutations used in
 * the most recent complex mutation step taken with unrootedbinaryDoComplexMutation
 *
 * \param ub pointer to the UnrootedBinary to be path queried
 * \return integer indicating the number of simple mutation steps last used
 */
int unrootedbinaryLastMutationCount(const struct UnrootedBinary *ub);

/** \brief Indicates whether or not a given pair of nodes are connected.
 *
 * This function returns a true value if and only if the two nodes with the
 * given node identifiers are connected.  A node may not connect to itself.
 *
 * \param ub pointer to the UnrootedBinary to be path queried
 * \param a first node identifier
 * \param b second node identifier
 * \return integer to be interpretted in a boolean context
 */
int unrootedbinaryIsConnected(const struct UnrootedBinary *ub, qbase_t a, qbase_t b);

struct LabelPerm *unrootedbinaryLabelPerm(struct UnrootedBinary *ub);


struct AdjAdaptor *getAdjAdaptorForUB(struct UnrootedBinary *ub);

struct TreeAdaptor *treeaLoadUnrooted(int howBig);

#endif

