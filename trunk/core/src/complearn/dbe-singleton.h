#ifndef __DBE_SINGLETON_H
#define __DBE_SINGLETON_H

#include <complearn/complearn.h>

/** \brief Given a single DataBlock, returns pointer to newly initialized
 *  DataBlockEnumeration.
 *
 *  \param db pointer to DataBlock
 *  \return pointer to new DataBlockEnumeration
 */
struct DataBlockEnumeration *loadSingletonDBE(struct DataBlock *db);

#endif
