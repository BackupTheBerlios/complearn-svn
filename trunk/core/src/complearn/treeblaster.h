#ifndef __TREEBLASTER_H
#define __TREEBLASTER_H

#include <complearn/cltypes.h>

#if GSL_RDY
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>
#endif

/*! \file treeblaster.h */

/** \brief searches for an optimal order for traversing a given TreeAdaptor
 *
 * \struct TreeBlaster
 *
 * The TreeBlaster permutes child-ordering in the nodes of a TreeAdaptor
 * randomly and searches for the lowest perimiter-pair NCD sum.  For a given
 * traversal, the leaf nodes will form a ring and every adjacent pair of
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
 * This function creates a new TreeBlaster for determining an optimal
 * child traversal ordering for the given tree topology.  It is normally
 * called after treemasterFindTree() has determined a best-matching topology.
 * The result of an order-search is a CLNodeSet of internal node numbers.
 * These node-numbers correspond to kernel nodes in the TreeAdaptor.
 * Children are either visitted in order or in opposite order if the
 * corresponding node-number is stored within the CLNodeSet.
 *
 * A gsl_matrix representing distances is necessary to judge which orders
 * are better.
 *
 * \sa treemasterFindTree()
 *
 * \param gsl distance matrix as a pointer to a gsl_matrix
 * \param ta pointer to a TreeAdaptor
 * \return pointer to a newly allocated TreeBlaster
 */
struct TreeBlaster *treebNew(gsl_matrix *gsl, struct TreeAdaptor *ta);

/** \brief finds a best traversal ordering for the given tree
 *
 * This function first determines which nodes in the given TreeAdaptor are
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
struct CLNodeSet *treebFindTreeOrder(struct TreeBlaster *tbl, double *s);

/** \brief deallocates a TreeBlaster
 *
 * This function deallocates the memory associated with a TreeBlaster.
 * There is no return value.
 *
 * \param tbl pointer to the TreeBlaster to be freed
 */
void treebFree(struct TreeBlaster *tbl);

/** \brief provides realtime feedback callbacks for tree order search
 *
 * This structure supports the treebSetTreeOrderObserver() function of TreeBlaster.
 * There are analogous functions in the TreeObserver for TreeMaster.
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
 * This function is only useful for multithreaded applications that want
 * to calculate in one thread and display the intermediate results in
 * realtime in another thread.
 *
 * \sa treemasterSetTreeObserver()
 * \param tbl pointer to the TreeBlaster to observe
 * \param tob pointer to the TreeOrderObserver to connect to tbl
 */
void treebSetTreeOrderObserver(struct TreeBlaster *tbl, struct TreeOrderObserver *tob);

/** \brief returns the number of trees used in orderwise agreement testing
 *
 * This function returns the "k" count of trees for orderwise search
 * agreement termination condition.
 *
 * \param tbl pointer to the TreeBlaster to inspect
 * \return number of trees that must agree before termination
 */
int treebK(struct TreeBlaster *tbl);

/** \brief returns the number of labelled nodes in this TreeBlaster
 *
 * This function returns the number of labelled nodes in the TreeAdaptor
 * contained within the given TreeBlaster.
 *
 * \param tbl pointer to the TreeBlaster to inspect
 * \return number of labelled nodes in this TreeBlaster
 */
int treebLabelCount(struct TreeBlaster *tbl);

#endif

