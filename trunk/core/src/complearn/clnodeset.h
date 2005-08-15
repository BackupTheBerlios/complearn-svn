#ifndef __CLNODESET_H
#define __CLNODESET_H
/*! \file clnodeset.h */

/** \brief A dynamically resizing set of integers
 * \struct CLNodeSet
 *
 * This structure represents a set of nodes commonly, or more generally,
 * a set of integers.  The set is not ordered.  Each integer is either
 * in or out of a given CLNodeSet.
 */
struct CLNodeSet;

/** \brief Allocates a new struct CLNodeSet
 * This function allocates a new CLNodeSet.
 * \param howbig an optional value indicating how big this set should
 * initially be sized.  This value is not a limit.
 */
struct CLNodeSet *newCLNodeSet(int howbig);
struct CLNodeSet *cloneCLNodeSet(const struct CLNodeSet *cl);
void freeCLNodeSet(struct CLNodeSet *cl);

/** \brief Adds a node label to a CLNodeSet
 *  \param cl CLNodeSet
 *  \param which node label to add
 */
void addNodeToSet(struct CLNodeSet *cl, qbase_t which);
void removeNodeFromSet(struct CLNodeSet *cl, qbase_t which);
void setNodeStatusInSet(struct CLNodeSet *cl, qbase_t which, int status);
int isNodeInSet(const struct CLNodeSet *cl, qbase_t which);
void printCLNodeSet(struct CLNodeSet *cl);
struct DoubleA *CLNodeSetToDoubleA(const struct CLNodeSet *cl);
struct CLNodeSet *DoubleAToCLNodeSet(const struct DoubleA *da);
int sizeCLNodeSet(const struct CLNodeSet *cl);

#endif

