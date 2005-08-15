#ifndef __DBE_ARRAY_H
#define __DBE_ARRAY_H

#include <complearn/complearn.h>

/*! \file dbe-array.h */

/** \brief Given an array of DataBlocks, returns pointer to newly initialized
 *  DataBlockEnumeration.
 *
 *  \param db pointer to an array of DataBlocks
 *  \param size number of elements in DataBlock array
 *  \return pointer to new DataBlockEnumeration
 */
struct DataBlockEnumeration *loadArrayDBE(struct DataBlock *db, int size);

#endif
