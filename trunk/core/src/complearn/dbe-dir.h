#ifndef __DBE_DIR_H
#define __DBE_DIR_H

#include <complearn/complearn.h>

/*! \file dbe-dir.h */

/** \brief Given a directory name whose contents contains files to be read,
 *  returns pointer to newly initialized DataBlockEnumeration.
 *
 *  \param dirname pointer to string directory name
 *  \return pointer to new DataBlockEnumeration
 */
struct DataBlockEnumeration *clDbeLoadDirectory(const char *dirname);

int clIsDirectory(const char *dirname);

#endif
