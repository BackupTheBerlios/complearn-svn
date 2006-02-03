#ifndef __DBE_WINDOWED_H
#define __DBE_WINDOWED_H

#include <complearn/complearn.h>

/*! \file dbe-windowed.h */

/** \brief Given a single DataBlock, returns pointer to newly initialized
 *  DataBlockEnumeration.
 *
 *  \param db pointer to DataBlock
 *  \return pointer to new DataBlockEnumeration
 */
struct DataBlockEnumeration *clDbeLoadWindowed(struct DataBlock *db,
    int firstpos, int stepsize, int width, int lastpos);

#endif
