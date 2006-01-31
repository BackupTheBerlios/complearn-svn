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
struct CLNodeSet *clnodesetNew(int howbig);
struct CLNodeSet *clnodesetClone(const struct CLNodeSet *cl);
void clnodesetFree(struct CLNodeSet *cl);

/** \brief Adds a node label to a CLNodeSet
 *  \param cl CLNodeSet
 *  \param which node label to add
 */
void clnodesetAddNode(struct CLNodeSet *cl, qbase_t which);
void clnodesetRemoveNode(struct CLNodeSet *cl, qbase_t which);
void clnodesetSetNodeStatus(struct CLNodeSet *cl, qbase_t which, int status);
int clnodesetHasNode(const struct CLNodeSet *cl, qbase_t which);
void clnodesetPrint(struct CLNodeSet *cl);
struct DRA *clnodesetToDRA(const struct CLNodeSet *cl);
struct CLNodeSet *draToCLNodeSet(const struct DRA *da);
int clnodesetSize(const struct CLNodeSet *cl);

#endif

