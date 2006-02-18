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
 * clUnrootedbinaryDoComplexMutation function with the clUnclRootedbinaryClone function.
 * This function allocates memory which must eventually be freed with
 * clUnrootedbinaryFree.
 *
 *  \param howManyLeaves an integer >=4 indicating how many leaves the tree
 *  should have.
 *  \return a pointer to a newly allocated tree
 */
struct UnrootedBinary *clUnclRootedbinaryNew(int howManyLeaves);

/** \brief Applies a complex mutation to a tree.
 *
 * This function is the most convenient source of variation in trees.
 * It uses a coin flip to determine how many simple mutations to do, as
 * specified in the Quartet Tree paper by Cilibrasi and Vitanyi.
 * It then applies the requisite number of simple mutations, using the
 * builtin random-number generator rand().  This function may be used
 * in combination with clUnclRootedbinaryClone to allow several different tries from
 * the same tree starting point.  It may also be used with the TreeScore
 * module which can allow for quartet-based hill-climbing algorithms.
 *
 * \param ub pointer to the UnrootedBinary to be modified.
 * \return nothing
 */
void clUnrootedbinaryDoComplexMutation(struct UnrootedBinary *ub);

/** \brief Clones an UnrootedBinary tree, allocating new memory for the copy
 *
 * This function copies an UnrootedBinary tree object.  This can be used
 * in conjunction with clUnrootedbinaryDoComplexMutation for hill-climbing search.
 * This function allocates memory which must eventually be freed with
 * clUnrootedbinaryFree.
 * \param ub pointer to the UnrootedBinary that must be cloned
 * \return pointer to the new copy of the original tree
 */
struct UnrootedBinary *clUnclRootedbinaryClone(const struct UnrootedBinary *ub);

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
int clUnclRootedbinaryIsQuartetableNode(const struct UnrootedBinary *ub, qbase_t which);

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
int clIsFlippableNode(struct UnrootedBinary *ub, qbase_t which);
/** \brief Inverts a single node's child ordering
 *
 * This function allows the user to invert the order that children are
 * traversed in this node.  This ordering applies only to clWalkTree traversals.
 * Thus, this order may or may not be significant in your application.
 * See also clIsFlippableNode
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
 * tree traversals on this tree.  This applies to the clWalkTree function.
 *
 * \param ub pointer to the UnrootedBinary that must be reordered
 * \return qbase_t indicating which node is first traversed in this tree
 */
qbase_t clUnclRootedbinaryStartingNode(const struct UnrootedBinary *ub);

/** \brief Returns a DRA contained an ordered traversal of all
 * the nodes in the tree.
 *
 * This function allows the user to get a dynamically-allocated list of
 * all the nodes in this tree.  They are returned in depth first order.
 * This is equivalent to the clWalkTree function with a 0 breadthFirst
 * parameter; thus the order returned by clUnclRootedbinaryNodes will be affected
 * by the node child order flip bits.
 *
 * The DRA returned by this function use the .i field of PCTypes:
 *
 * struct DRA *da = clUnclRootedbinaryNodes(ub, NULL);
 * int i;
 * for (i = 0; i < clDraSize(da); i += 1)
 *   printf("Got node %d\n", clDraGetValueAt(da, i).i);
 *
 * \param ub pointer to the UnrootedBinary that must be reordered
 * \return pointer to a DRA holding the list of all tree nodes
 */
struct DRA *clUnclRootedbinaryNodes(const struct UnrootedBinary *ub, struct CLNodeSet *flips);


/** \brief Returns a list of all adclJacent border-pair node identifiers
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
 * struct DRA *da = clUnrootedbinaryPerimPairs(ub, NULL);
 * int i;
 * for (i = 0; i < clDraSize(da); i += 1) {
 *   union PCTypes pct = clDraGetValueAt(da, i);
 *   printf("perimeter pair: node %d and node %d\n", pct.ip.x, pct.ip.y);
 * }
 *
 * \param ub pointer to the UnrootedBinary that must be reordered
 * \return pointer to a DRA holding the list of all perimeter pairs
 */
struct DRA *clUnrootedbinaryPerimPairs(const struct UnrootedBinary *ub, struct CLNodeSet *flips);

/** \brief Frees the memory associated with an UnrootedBinary
 *
 * This function frees the memory associated with an UnrootedBinary.
 * After calling this function it is safest to set the pointer to NULL,
 * as you shouldn't try to use it after deallocation.  This function must
 * be called for every time clUnclRootedbinaryNew, clUnclRootedbinaryClone, or loadTree is called.
 *
 * \param ub pointer to the UnrootedBinary that must be freed.
 * \return nothing
 */
void clUnrootedbinaryFree(struct UnrootedBinary *ub);

/** \brief Returns a permutation array holding the column-index to leaf-node
 * identifier mapping in effect for this tree.
 *
 * When mapping a set of leaf-labels to a graph or tree, there is always
 * the possibility to permute the leaf labels.  This is handled with a
 * permutation array.  The entry at
 * clDraGetValueAt(leaflabels, r).i
 * indicates the node-identifier of the leaf where datamatrix column index
 * indicator r should be placed.
 *
 * \param ub pointer to the UnrootedBinary to be examined
 * \return pointer to a DRA containing leaf label positions
 */
struct DRA *clUnclRootedbinaryLeafLabels(const struct UnrootedBinary *ub);

/** \brief This function returns the number of simple mutations used in
 * the most recent complex mutation step taken with clUnrootedbinaryDoComplexMutation
 *
 * \param ub pointer to the UnrootedBinary to be path queried
 * \return integer indicating the number of simple mutation steps last used
 */
int clUnclRootedbinaryLastMutationCount(const struct UnrootedBinary *ub);

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

struct LabelPerm *clUnclRootedbinaryLabelPerm(struct UnrootedBinary *ub);

void clUnclRootedbinaryLabelPermSetter(struct UnrootedBinary *ub, int j, int i);


struct AdjAdaptor *clGetAdjAdaptorForUB(struct UnrootedBinary *ub);

struct TreeAdaptor *clTreeaLoadUnrooted(int howBig);

#endif

