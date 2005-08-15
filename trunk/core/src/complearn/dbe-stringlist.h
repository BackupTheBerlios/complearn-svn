#ifndef __DBE_STRINGLIST_H
#define __DBE_STRINGLIST_H

#include <complearn/complearn.h>

/*! \file dbe-stringlist.h */

/** \brief Given a filename whose contents contain a list of strings,
 * one per line, returns pointer to newly initialized DataBlockEnumeration
 * made by converting each line to a (chomp'd string) DataBlock.
 *
 *  \param filename pointer to string filename for list
 *  \return pointer to new DataBlockEnumeration
 */
struct DataBlockEnumeration *loadStringListDBE(const char *filename);

#endif
