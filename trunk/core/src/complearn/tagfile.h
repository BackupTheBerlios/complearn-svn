#ifndef __TAGFILE_H
#define __TAGFILE_H

#include <complearn/tagtypes.h>
#include <complearn/cltypes.h>
#include <complearn/datablock.h>

/*! \file tagfile.h */

/** \brief Allows for easy parsing through DataBlocks in a DataBlock "package"
 *  \struct TagManager
 *
 *  A DataBlock "package" as created by package_DataBlocks() consists of
 *  sequential, dumped DataBlocks and can be accessed by a combination of
 *  calls to stepNextDataBlock() and getCurDataBlock().
 *
 *  A TagManager must first be created, taking a DataBlock "package" as input,
 *  by calling newTagManager().  An immediate call to getCurDataBlock() will
 *  retrieve the first dumped DataBlock.  Use stepNextDataBlock() to move the
 *  DataBlock pointer forward, and subsequentally call getCurDataBlock() to
 *  retrieve the next dumped DataBlock, and so on.
 *
 *  \sa tagfile.h
*/
struct TagManager;

/** \brief Header embedded within a dumped serialized DataBlock
 *
 * Within a dumped DataBlock there exists a TagHdr structure which holds two
 * pieces of information about the serialized object: the type of object
 * (in the form of a tagnum; see tagtypes.h for a complete list) and the size,
 * in bytes, of the object.  This \a size does not include the size of the
 * TagHdr.
 *
 * The \a tagnum in this embedded TagHdr can be used to trigger the correct
 * deserialization operation on the given DataBlock, or confirm that the
 * correct deserialization operation is being performed.
 *
 * \sa tagtypes.h
 */
struct TagHdr {
  t_tagtype tagnum;
  unsigned int size;
};

/** \brief Creates and returns a pointer to a new TagManager
 *
 *  newTagManager() takes as input a DataBlock "package" as created by
 *  package_DataBlocks(). Returns a pointer to the new TagManager which can
 *  then be immediately used to retrieve the first dumped DataBlock in the
 *  package.
 *  \param db DataBlock "package"
 *  \return pointer to new TagManager
 */
struct TagManager *newTagManager(struct DataBlock db);

/** \brief Steps the DataBlock pointer forward to the next DataBlock
 *  \param tm pointer to TagManager
 */
void stepNextDataBlock(struct TagManager *tm);

/** \brief Retrieves current DataBlock of a DataBlock "package"
 *
 *  The DataBlock returned by getCurDataBlock() is an independent copy and must
 *  be freed using freeDataBlockPtr().
 *
 *  \param tm pointer to TagManager
 *  \param cur pointer used for retrieved DataBlock
 *  \return 0 if no DataBlock to read; 1 if current DataBlock successfully read
 */
int getCurDataBlock(struct TagManager *tm, struct DataBlock *cur);

/** \brief Retrieve the tagnum of the current DataBlock
 *  \param tm pointer to TagManager
 *  \return tagnum of current DataBlock
 */
t_tagtype getCurTagNum(const struct TagManager *tm);

/** \brief Free TagManager from memory
 *  \param tm pointer to TagManager
 */
void freeTagManager(struct TagManager *tm);

/** \brief Serializes a set of dumped DataBlocks into a single DataBlock
 *
 *  package_DataBlocks() takes varionic args, a sequence of DataBlock pointers
 *  and the last is NULL.
 *
 *  \param overalltag describing the set of DataBlocks
 *  \param ... sequence of pointers to DataBlocks ending with NULL
 *  \return serialized DataBlock encompassing a series of DataBlocks
 */
struct DataBlock package_DataBlocks(t_tagtype overalltag, ...);

/** \brief Serializes a DoubleA of dumped DataBlocks into a singe DataBlock
 *
 *  package_dd_DataBlocks() is essentially the same as package_DataBlocks() but
 *  taking as an argument a DoubleA of pointers to dumped DataBlocks, rather
 *  than a varionic args list.
 *  \param overalltag describing the set of DataBlocks
 *  \param parts DoubleA of pointers to dumped DataBlocks
 *  \return serialized DataBlock encompassing a series of DataBlocks
 */
struct DataBlock package_dd_DataBlocks(t_tagtype tnum, struct DoubleA *parts);

/** \brief Given a DataBlock package, returns a DoubleA of tagnums & DataBlocks
 *
 *  load_DataBlock_package() is a high level function which will take a
 *  DataBlock "package," sequentially retrieve each dumped DataBlock in the
 *  package, and return a DoubleA of IntDBPair objects which hold the \a tagnum
 *  of each DataBlock and a pointer to the dumped DataBlock.
 *  \param db DataBlock "package" as created by package_DataBlocks()
 *  \return DoubleA of IntDBPair objects
 */
struct DoubleA *load_DataBlock_package(struct DataBlock db);

/** \brief Retrieves DataBlock from DoubleA of IntDBPairs
 *
 *  Another high level function, scanForTag() will, given a tagnum as defined
 *  in tagtypes.h, scan a DoubleA of IntDBPairs as produced by
 *  load_DataBlock_package(). This retrieved DataBlock is an independent copy
 *  any must be freed using freeDataBlock().
 *
 *  TODO: when switching db to db pointer, NULL should be returned if tagnum in
 *  question is not found.  Currently, a DataBlock is returned regardless, only
 *  a DataBlock size of 0 signifying a failure in finding the tagnum.
 *  \param dd DoubleA of IntDBPair objects
 *  \param tnum tagnum of DataBlock in question
 *  \return DataBlock
 */
struct DataBlock scanForTag(struct DoubleA *dd, int tnum);
#endif
