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
#ifndef __TREEBLASTER_H
#define __TREEBLASTER_H

#include <complearn/cltypes.h>

#include <gsl/gsl_blas.h>

/*! \file treeblaster.h */

/** \brief searches for an optimal order for traversing a given TreeAdaptor
 *
 * \struct TreeBlaster
 *
 * The TreeBlaster permutes child-ordering in the nodes of a TreeAdaptor
 * randomly and searches for the lowest perimiter-pair NCD sum.  For a given
 * traversal, the leaf nodes will form a ring and every adclJacent pair of
 * leaf nodes in this traversal is summed.  The traversal order is determined
 * by a flips array that contains only those nodes who are visitted in reversed
 * order.
 *
 * \sa treeblaster.h
 *
 */
struct TreeBlaster;

/** \brief creates a new TreeBlaster
 *
 * This clFunction creates a new TreeBlaster for determining an optimal
 * child traversal ordering for the given tree topology.  It is normally
 * called after clTreemasterFindTree() has determined a best-matching topology.
 * The result of an order-search is a CLNodeSet of internal node numbers.
 * These node-numbers correspond to kernel nodes in the TreeAdaptor.
 * Children are either visitted in order or in opposite order if the
 * corresponding node-number is stored within the CLNodeSet.
 *
 * A gsl_matrix representing distances is necessary to judge which orders
 * are better.
 *
 * \sa clTreemasterFindTree()
 *
 * \param gsl distance matrix as a pointer to a gsl_matrix
 * \param ta pointer to a TreeAdaptor
 * \return pointer to a newly allocated TreeBlaster
 */
struct TreeBlaster *clTreebNew(gsl_matrix *gsl, struct TreeAdaptor *ta);

/** \brief finds a best traversal ordering for the given tree
 *
 * This clFunction first determines which nodes in the given TreeAdaptor are
 * flippable.  Then it randomly mutates a set of flippable nodes to try to
 * find the optimal child-order for traversal.  The resultant ordering is
 * represented in the returned flips CLNodeSet.  Only flippable node numbers
 * can appear here.  Additionally, an out-parameter s (a pointer to a double)
 * will hold the scaled order score.
 *
 * \param tbl pointer to a TreeBlaster to find a best ordering
 * \param s out-parameter to hold scaled order score
 * \return CLNodeSet pointer to array of flipped nodes for best ordering
 *
 * \sa treePerimPairsTRA()
 */
struct CLNodeSet *clTreebFindTreeOrder(struct TreeBlaster *tbl, double *s);

/** \brief deallocates a TreeBlaster
 *
 * This clFunction deallocates the memory associated with a TreeBlaster.
 * There is no return value.
 *
 * \param tbl pointer to the TreeBlaster to be freed
 */
void clTreebFree(struct TreeBlaster *tbl);

/** \brief provides realtime feedback callbacks for tree order search
 *
 * This structure supports the clTreebSetTreeOrderObserver() clFunction of TreeBlaster.
 * There are analogous clFunctions in the TreeObserver for TreeMaster.
 *
 * \sa TreeObserver
 */
struct TreeOrderObserver {
  void *ptr;
  t_treeordersearchstarted treeordersearchstarted;
  t_treeorderimproved treeorderimproved;
  t_treeorderdone treeorderdone;
};

/** \brief sets the TreeBlaster order observer to allow realtime feedback
 *
 * This clFunction is only useful for multithreaded applications that want
 * to calculate in one thread and display the intermediate results in
 * realtime in another thread.
 *
 * \sa clTreemasterSetTreeObserver()
 * \param tbl pointer to the TreeBlaster to observe
 * \param tob pointer to the TreeOrderObserver to connect to tbl
 */
void clTreebSetTreeOrderObserver(struct TreeBlaster *tbl, struct TreeOrderObserver *tob);

/** \brief returns the number of trees used in orderwise agreement testing
 *
 * This clFunction returns the "k" count of trees for orderwise search
 * agreement termination condition.
 *
 * \param tbl pointer to the TreeBlaster to inspect
 * \return number of trees that must agree before termination
 */
int clTreebK(struct TreeBlaster *tbl);

/** \brief returns the number of labelled nodes in this TreeBlaster
 *
 * This clFunction returns the number of labelled nodes in the TreeAdaptor
 * contained within the given TreeBlaster.
 *
 * \param tbl pointer to the TreeBlaster to inspect
 * \return number of labelled nodes in this TreeBlaster
 */
int clTreebLabelCount(struct TreeBlaster *tbl);

#endif

