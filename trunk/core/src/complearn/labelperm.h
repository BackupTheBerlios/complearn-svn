#ifndef __LABELPERM_H
#define __LABELPERM_H

/*! \file labelperm.h */

/** \brief holds a label-index placement permutation
 *
 * A LabelPerm stores the placements or positions of each of the n labelled
 * object placements in each of the labelled nodes of a tree.  Leaf nodes are
 * labellable.  The LabelPerm maps between NodeID (normal TreeAdaptor node
 * numbering) and ColumnIndex.  The ColumnIndex refers to the entries in
 * the distance matrix.  This permutation and its inverse are kept in sync.
 * The LabelPerm supports a simple and generic label permutation operation
 * in labelpermMutate() to support nondeterministic searches.
 *
 * \struct LabelPerm
 *
 * \sa labelperm.h
 */
struct LabelPerm;

/** \brief create a new LabelPerm
 *
 * the labelpermNew function creates a new LabelPerm object given a list of
 * labelled nodes as DRA of integers (.i) .  These integers are supposed
 * to represent node numbers as used in the TreeAdaptor.  The initial
 * permutation will map the column indexes in order to these integers in order.
 *
 * \param labelledNodes pointer to DRA containing integers (.i) for nodes
 * \return pointer to a new LabelPerm
 */
struct LabelPerm *labelpermNew(struct DRA *labelledNodes);

/** \brief deallocate memory associated with a LabelPerm
 *
 * \param lph pointer to LabelPerm to deallocate
 */
void labelpermFree(struct LabelPerm *lph);

/** \brief mutate a LabelPerm randomly
 *
 * This function applies a mutation step on the given LabelPerm.
 * There is no return value.
 *
 * \param lph pointer to the LabelPerm to mutate
 */
void labelpermMutate(struct LabelPerm *lph);

/** \brief returns the size of this LabelPerm, or (the maximum column index) + 1
 *
 * \param lph pointer to the LabelPerm to mutate
 * \return integer number of places in this LabelPerm
 */
int labelpermSize(struct LabelPerm *lph);

/** \brief creates a new LabelPerm as a copy of a pre-existing one
 *
 * \param lph pointer to LabelPerm to be copied
 * \return pointer to a new cloned LabelPerm just like the original
 */
struct LabelPerm *labelpermClone(struct LabelPerm *lph);

/** \brief fetches the node number placement for a given column index label
 *
 * \param lph pointer to LabelPerm to be queried
 * \param which the integer column index between 0 to n-1 (n == size of matrix)
 * \return the integer node number (for TreeAdaptor) placement of the label
 */
int labelpermNodeIDForColIndex(struct LabelPerm *lph, int which);

/** \brief fetches the node number placement for a given column index label
 *
 * \param lph pointer to LabelPerm to be queried
 * \param which the integer node number where unknown label is placed
 * \return the integer column index between 0 and n-1 (for n x n matrix)
 */
int labelpermColIndexForNodeID(struct LabelPerm *lph, int which);

/** \brief compares two LabelPerm to see if they are the same or different
 *
 * This function compares to label placement assignments, lpa and lpb.
 * If they are identical it returns a nonzero value.  If they differ
 * at all, 0 is returned.
 *
 * \param lpa pointer to one LabelPerm to compare
 * \param lpb pointer to another LabelPerm to compare
 * \return integer that is nonzero only if lpa and lpb are identical
 */
int labelpermIdentical(struct LabelPerm *lpa, struct LabelPerm *lpb);

/** \brief verifies that a LabelPerm is self-consistent
 *
 * This function verifies that the LabelPerm passes internal consistency
 * checks.  There is no return value.
 *
 * \param pointer to LabelPerm to verify
 */
void labelpermVerify(struct LabelPerm *lp);

/** \brief Sets a given column index indicator to point at a specific node
 *
 * This function simply sets an entry in the label permutation table.
 * It is the user's responsibility to ensure that the permutation remains
 * consistent before further use. \sa labelpermVerify
 *
 * \param pointer to LabelPerm to adjust
 * \param col integer index between 0 and columns-1
 * \param n integer index indicating node in tree
 *
 */
void labelpermSetColumnIndexToNodeNumber(struct LabelPerm *lp, int col, int n);

#endif
