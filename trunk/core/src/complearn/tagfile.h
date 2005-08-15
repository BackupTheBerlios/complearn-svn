#ifndef __TAGFILE_H
#define __TAGFILE_H

#include <complearn/tagtypes.h>
#include <complearn/cltypes.h>
#include <complearn/datablock.h>

/*! \file tagfile.h */

struct tagHdr {
  t_tagtype tagnum;
  unsigned int size;
};

struct TagManager;

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
