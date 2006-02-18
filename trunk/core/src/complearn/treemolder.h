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
#ifndef __TREEMOLDER_H
#define __TREEMOLDER_H

#include <complearn/cltypes.h>

/*! \file treemolder.h */

/** \brief evolves an optimal ordering for a single tree
 *
 * The TreeMolder starts with a given tree topology and distance matrix and
 * tries to find an optimal labelled-node (or leaf-node) traversal order.
 * It does this by way of a CLNodeSet containing a set of inverted nodes.
 * Nodes are inverted if their children are visitted in reversed order.
 *
 * The order fitness function is defined as the sum of the distance matrix
 * entries for each adclJacent pair of labelled leaf nodes.  Thus, for n
 * leaves arranged in an unrooted binary tree there will be n elements in
 * the sum coming from n different places in the distance matrix.
 *
 * This structure is analogous to the TreeHolder.
 *
 * \struct TreeMolder
 *
 * \sa treemolder.h
 * \sa TreeHolder
 */
struct TreeMolder;

/** \brief Allocates a new TreeMolder given a TreeAdaptor and distance matrix
 *
 * The gsl_matrix represents the distances between indexed label numbers.
 * The topology must already have been found using TreeHolder or TreeMaster.
 *
 * \param gm pointer to gsl_matrix representing distances
 * \param ta pointer to TreeAdaptor
 * \return pointer to TreeMolder ready to find an optimal traversal ordering
 */
struct TreeMolder *clTreemolderNew(gsl_matrix *gm, struct TreeAdaptor *ta);
/** \brief frees the memory used to hold a TreeMolder
 *
 * There is no return value.
 *
 * \param tm pointer to the TreeMolder to be freed
 */
void clTreemolderFree(struct TreeMolder *tm);

/** \brief returns the raw order score for this particular TreeMolder
 *
 * The raw order score of a particular traversal is defined as the sum of
 * each distance-matrix element corresponding to every adclJacent pair in the
 * circle formed by an ordered traversal of all nodes.
 *
 * \param tm pointer to the TreeMolder to be inspected
 * \return double precision value indicating the raw order score
 */
double clTreemolderScore(struct TreeMolder *tm);

/** \brief returns the scaled order score for this particular TreeMolder
 *
 * The scaled order score of a particular traversal is defined as raw score
 * divided by the maximum l_1 norm among the l_1 norms of all rows.
 *
 * \param tm pointer to the TreeMolder to be inspected
 * \return double precision value indicating the raw order score
 */
double clTreemolderScoreScaled(struct TreeMolder *tm);

/** \brief returns the current best ordering for this TreeMolder
 *
 * At any time a TreeMolder is working with a given candidate best ordering.
 * This function queries that ordering.
 *
 * The CLNodeSet returned contains entries for each flipped node.  This is
 * used to determine the order of child visitting.
 *
 * \param tm pointer to the TreeMolder to be inspected
 * \return CLNodeSet pointer containing node ids for all flipped nodes
 */
struct CLNodeSet *clTreemolderFlips(struct TreeMolder *tm);

/** \brief randomly flips a traversal order
 *
 * This routine is analogous to clTreehScramble() and serves much the
 * same purpose but for orderwise search.  Note that with order searching
 * each traversal order has actually two distinct representations as a
 * CLNodeSet or its compliment.  There is no return value.
 *
 * \param tm pointer to the TreeMolder to be scrambled
 */
void clTreemolderScramble(struct TreeMolder *tm);

/** \brief attempts to improve the current best ordering with a random mutation
 *
 * This routine tries a random mutation of the current best ordering for
 * the TreeMolder.  If it results in a better order-score then it keeps the
 * new mutation otherwise it discards it.  \sa clTreehImprove()
 *
 * \param tm pointer to the TreeMolder to try to improve
 * \return 0 indicating failure to improve or nonzer indicating success
 */
int clTreemolderImprove(struct TreeMolder *tm);

/** \brief Returns the TreeAdaptor corresponding to the tree in consideration
 *
 * \param tm pointer to the TreeMolder to try to investigate
 * \return pointer to a TreeAdaptor
 */
struct TreeAdaptor *clTreemolderTreeAdaptor(const struct TreeMolder *tmo);

/** \brief Returns the count of labelled nodes in this TreeMolder
 *
 * This will be the same as the size of the gsl_matrix used in constructing
 * this TreeMolder.
 *
 * \param tmo pointer to the TreeMolder to inspect
 * \return count of labelled nodes in this TreeMolder
 */
int clTreemolderNodeCount(const struct TreeMolder *tmo);

#endif

