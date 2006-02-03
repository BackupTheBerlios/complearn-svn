#ifndef __DBE_FILELIST_H
#define __DBE_FILELIST_H

#include <complearn/complearn.h>

/*! \file dbe-filelist.h */

/** \brief Given a filename whose contents contain a list of files to be read,
 *  returns pointer to newly initialized DataBlockEnumeration.
 *
 *  \param filename pointer to string filename
 *  \return pointer to new DataBlockEnumeration
 */
struct DataBlockEnumeration *clDbeLoadFileList(const char *filename);

#endif
