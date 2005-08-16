#ifndef __TAGFILE_H
#define __TAGFILE_H

#include <complearn/tagtypes.h>
#include <complearn/cltypes.h>
#include <complearn/datablock.h>

/*! \file tagfile.h */

struct TagManager;

/** \brief Header embedded within a dumped serialized DataBlock
 *
 * Within a dumped DataBlock exists a tagHdr which holds two pieces of
 * information about the object just serialized: the type of object (in the
 * form of a tagnum; see tagtypes.h for a complete list) and the size, in
 * bytes, of the object.  This does not include the size of the tagHdr.
 *
 * \sa tagtypes.h
 */
struct tagHdr {
  t_tagtype tagnum;
  unsigned int size;
};

struct TagManager *newTagManager(struct DataBlock db);
void stepNextDataBlock(struct TagManager *tm);
int getCurDataBlock(struct TagManager *tm, struct DataBlock *cur);
t_tagtype getCurTagNum(const struct TagManager *tm);
int getCurTagHdrSize(const struct TagManager *tm);
void freeTagManager(struct TagManager *tm);

/* this function takes varionic args, ending in NULL */
/* it must have DataBlock *'s for all args after overalltag but before NULL */
struct DataBlock package_DataBlocks(t_tagtype overalltag, ...);
struct DataBlock package_dd_DataBlocks(t_tagtype tnum, struct DoubleA *parts);
struct DoubleA *load_DataBlock_package(struct DataBlock db);
struct DataBlock scanForTag(struct DoubleA *dd, int tnum);
#endif
