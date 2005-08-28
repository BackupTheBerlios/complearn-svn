#ifndef __TREEADAPTOR_H
#define __TREEADAPTOR_H

#include <complearn/cltypes.h>

/*! \file treeadaptor.h */

/** \brief holds adjacency information and label placement information for trees
 *
 * \struct TreeAdaptor
 *
 * The TreeAdaptor holds the basic tree topology and allows for manipulation.
 *
 * The adaptor provides a simple direct polymorphism capability to extend
 * tree capabilities over certain restricted classes of simple graphs.  For
 * most users the basic newTreeTRA() constructor should be sufficient with
 * its basic rooted or unrooted tree implementations.
 *
 * \sa treeadaptor.h
 *
 */
struct TreeAdaptor {
  void *ptr;
  t_treemutate treemutate;
  t_treefree treefree;
  t_treeclone treeclone;
  t_treegetlabelperm treegetlabelperm;
  t_treegetadja treegetadja;
  t_treenodepred treeisquartetable;
  t_treenodepred treeisflippable;
  t_treenodepred treeisroot;
  t_treemutecount treemutecount;
  t_treeperimpairs treeperimpairs;
};

/** \brief mutates a tree using a complex mutation
 *
 * This function performs a complex mutation, changing the connections in
 * the tree in some amount that is usually small.  This is composed of 1 or
 * more steps of a random simple mutation.  There is no return value.
 *
 * \param tra pointer to a TreeAdaptor to be changed randomly
 */
void treemutateTRA(struct TreeAdaptor *tra);

/** \brief frees memory associated with the TreeAdaptor
 *
 * This function frees the memory allocated from newTreeTRA() or treecloneTRA().
 * There is no return.
 *
 * \param tra pointer to a TreeAdaptor to be deallocated
 */
void treefreeTRA(struct TreeAdaptor *tra);

/** \brief constructs a new (unrooted = 0, rooted = 1) simple TreeAdaptor
 *
 * This function constructs a basic "starting tree" in the shape of a
 * catterpillar of the given number of leaf nodes.
 *
 * \param isRooted integer indicating unrooted (0) or rooted (nonzero) tree
 * \param howbig integer specifying the number of leaf nodes
 * \return pointer to new TreeAdaptor of specified leaf-size and rootedness
 */
struct TreeAdaptor *newTreeTRA(int isRooted, int howbig);

/** \brief create an exact duplicate copy of a TreeAdaptor
 *
 * This function clones a TreeAdaptor.  It is useful in conjunction with
 * treemutateTRA() in order to conditionally keep a random mutation.
 *
 * \param tra pointer to a TreeAdaptor to be copied
 * \return pointer to new TreeAdaptor clone
 */
struct TreeAdaptor *treecloneTRA(struct TreeAdaptor *tra);

/** \brief fetch a LabelPerm indicating current leaf label-placement
 *
 * This function provides access to the permutation that places each
 * distance-matrix leaf-index number at a given node-number in the tree.
 * Although the indices in a distance matrix are 0 through n - 1 for n
 * different labels, this does not mean that they are placed on tree nodes
 * numbered 0 to n - 1.  Instead LabelPerm stores this mapping to and from
 * column (or row) indices and tree node numbers.
 * \param tra pointer to a TreeAdaptor to be inspected
 * \return pointer to the LabelPerm indicating label placement positions
 */
struct LabelPerm *treegetlabelpermTRA(struct TreeAdaptor *tra);

/** \brief retrieves the underlying adjacency adaptor for the TreeAdaptor
 *
 * This function allows the user to fetch the underlying AdjAdaptor (or adjacency
 * adaptor) for this TreeAdaptor.  This provides the ability to find out
 * whether any pair of nodes is connected or disconnected in the tree.
 *
 * \param tra pointer to a TreeAdaptor to be inspected
 * \return pointer to the AdjAdaptor showing connection information for this tree
 */
struct AdjAdaptor *treegetadjaTRA(struct TreeAdaptor *tra);

/** \brief tests whether a given node is "quartettable", e.g. a labellable node
 *
 * This function tests whether a given node is "quartettable" or not.
 * For unrooted binary trees this is the same as asking if a node is a leaf
 * node.  For rooted binary trees this asks if the node is a leaf or the
 * root, as the root receives a label in these cases.  Quartettable is
 * synonymous with "labellable".  This may be used to determine which node
 * numbers are used in LabelPerm; 0 values indicate that the node is not
 * quartettable, nonzero values indicate that it is.
 *
 * \sa treeIsFlippable(), treeIsRoot()
 *
 * \param tra pointer to a TreeAdaptor to be inspected
 * \return boolean indicating if the node number is quartettable or not
 */
int treeIsQuartettable(struct TreeAdaptor *tra, int which);
/** \brief tests whether a given node is "flippable" with left-right ordering
 *
 * This function tests whether a given node is "flippable" in a left-right
 * child ordering sense or not.  This is used in the best order search for
 * trees after the best topology is determinedl.
 *
 * \sa treeIsQuartettable(), treeIsRoot()
 *
 * \param tra pointer to a TreeAdaptor to be inspected
 * \return boolean indicating if the node number is flippable or not
 */
int treeIsFlippable(struct TreeAdaptor *tra, int which);
/** \brief tests whether a given node is the root node or not
 *
 * This function tests whether a given node is the root or not.
 * For unrooted binary trees this always returns 0.
 * For rooted binary trees this returns nonzero only for the root node and
 * 0 everywhere else.
 *
 * \sa treeIsQuartettable(), treeIsFlippable()
 *
 * \param tra pointer to a TreeAdaptor to be inspected
 * \return boolean indicating if the node number is the root node or not
 */
int treeIsRoot(struct TreeAdaptor *tra, int which);

/** \brief returns the number of simple mutations in the last treemutateTRA()
 *
 * Each complex mutation is composed of one or more simple mutations.  For
 * statistical purposes, the number of simple mutations comprising the most
 * recent complex mutation (done through treemutateTRA()) is saved and
 * treemutecountTRA() accesses this register.
 *
 * \param tra pointer to a TreeAdaptor to be inspected
 * \return integer indicating the number of simple mutations used last mutation
 */
int treemutecountTRA(struct TreeAdaptor *tra);

/** \brief returns the number of (both leaf and kernel) nodes in this tree
 *
 * This function indicates how many node numbers there are in this tree.
 * Thus, the nodes are numbered from 0 to 1 less than this function's return
 * value.
 *
 * \param tra pointer to a TreeAdaptor to be inspected
 * \return integer indicating the number of nodes in this tree in total
 */
int treeGetNodeCountTRA(struct TreeAdaptor *tra);

/** \brief returns a quartet-difference coefficient between two trees
 *
 * This function can be used to determine if two trees are the same or
 * not.  It computes the percentage of quartet groupings that disagree
 * in their quartet topologies between the two trees.  If two trees are
 * identical then the value will be 0.  Any nonzero value indicates that
 * the trees differ in some way.
 *
 * \param tra1 pointer to first TreeAdaptor to be compared
 * \param tra2 pointer to second TreeAdaptor to be compared
 * \return double precision value between 0.0 and 1.0 and 0.0 means identical.
 */
double getTreeDifferenceScore(struct TreeAdaptor *tra1, struct TreeAdaptor *tra2);

/** \brief Walks the tree perimeter in the depth-first order specified by the CLNodeSet
 *
 * This function allows for customized tree-walk ordering.  It uses an array
 * of node numbers to control its ordering.  By default node children are
 * visitted in ascending order, but if a node number is within the flips
 * object, then instead its children are visitted in reverse order.  Only
 * nodes who are true with regards to treeIsFlippable() should ever appear
 * in the flips CLNodeSet object.  If no customized ordering is necessary,
 * NULL may be passed in for flips and will result in the same traversal
 * as an empty flips set would obtain.  This function is only useful for
 * obtaining the set of tree perimeter pair nodes going all the way around.
 * It will not provide you with kernel nodes.
 *
 * The returned DoubleA will contain IntPair entries where x and y
 * are consecutive labelled node numbers in the ordered traversal of the tree.
 *
 * \param tra pointer to the TreeAdaptor to be traversed
 * \param flips pointer to CLNodeSet controlling traversal or NULL for default
 * \return DoubleA containing IntPair with adjacent node numbers in order
 */
struct DoubleA *treeperimpairsTRA(struct TreeAdaptor *tra, struct CLNodeSet *flips);

struct DoubleA *getTreeNodesTRA(struct TreeAdaptor *ta);
#endif
