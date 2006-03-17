/*
* Copyright (c) 2006 Rudi Cilibrasi, Rulers of the RHouse
* All rights reserved.     cilibrar@cilibrar.com
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the RHouse nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE RULERS AND CONTRIBUTORS "AS IS" AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE RULERS AND CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef __TAGFILE_H
#define __TAGFILE_H

#include <complearn/tagtypes.h>
#include <complearn/cltypes.h>
#include <complearn/datablock.h>

/*! \file tagfile.h */

/** \brief Allows for easy parsing through DataBlocks in a DataBlock "package"
 *
 *  A DataBlock "package" as created by clPackage_DataBlocks() consists of
 *  sequential, dumped DataBlocks and can be accessed by a combination of
 *  calls to clStepNextDataBlock() and clGetCurDataBlock().
 *
 *  A TagManager must first be created, taking a DataBlock "package" as input,
 *  by calling clNewTagManager().  An immediate call to clGetCurDataBlock() will
 *  retrieve the first dumped DataBlock.  Use clStepNextDataBlock() to move the
 *  DataBlock pointer forward, and subsequentally call clGetCurDataBlock() to
 *  retrieve the next dumped DataBlock, and so on.
 *
 *  \sa tagfile.h
 *  \struct TagManager
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
 *  clNewTagManager() takes as input a DataBlock "package" as created by
 *  clPackage_DataBlocks(). Returns a pointer to the new TagManager which can
 *  then be immediately used to retrieve the first dumped DataBlock in the
 *  package.
 *  \param db pointer to the DataBlock to "package"
 *  \return pointer to new TagManager
 */
struct TagManager *clNewTagManager(struct DataBlock *db);

/** \brief Steps the DataBlock pointer forward to the next DataBlock
 *  \param tm pointer to TagManager
 */
void clStepNextDataBlock(struct TagManager *tm);

/** \brief Retrieves current DataBlock of a DataBlock "package"
 *
 *  The DataBlock returned by clGetCurDataBlock() is an independent copy and must
 *  be freed using clDatablockFreePtr().
 *
 *  \param tm pointer to TagManager
 *  \param cur pointer used for retrieved DataBlock
 *  \return 0 if no DataBlock to read; 1 if current DataBlock successfully read
 */
struct DataBlock *clGetCurDataBlock(struct TagManager *tm);

/** \brief Retrieve the tagnum of the current DataBlock
 *  \param tm pointer to TagManager
 *  \return tagnum of current DataBlock
 */
t_tagtype clGetCurTagNum(const struct TagManager *tm);

/** \brief Free TagManager from memory
 *  \param tm pointer to TagManager
 */
void clFreeTagManager(struct TagManager *tm);

/** \brief Serializes a set of dumped DataBlocks into a single DataBlock
 *
 *  clPackage_DataBlocks() takes varionic args, a sequence of DataBlock pointers
 *  and the last is NULL.
 *
 *  \param overalltag describing the set of DataBlocks
 *  \param ... sequence of pointers to DataBlocks ending with NULL
 *  \return pointer to serialized DataBlock encompassing a series of DataBlocks
 */
struct DataBlock *clPackage_DataBlocks(t_tagtype overalltag, ...);

/** \brief Serializes a DRA of dumped DataBlocks into a singe DataBlock
 *
 *  clPackage_dd_DataBlocks() is essentially the same as clPackage_DataBlocks() but
 *  taking as an argument a DRA of pointers to dumped DataBlocks, rather
 *  than a varionic args list.
 *  \param overalltag describing the set of DataBlocks
 *  \param parts DRA of pointers to dumped DataBlocks
 *  \return serialized DataBlock encompassing a series of DataBlocks
 */
struct DataBlock *clPackage_dd_DataBlocks(t_tagtype tnum, struct DRA *parts);

/** \brief Given a DataBlock package, returns a DRA of tagnums & DataBlocks
 *
 *  clLoad_DataBlock_package() is a high level function which will take a
 *  DataBlock "package," sequentially retrieve each dumped DataBlock in the
 *  package, and return a DRA of IntDBPair objects which hold the \a tagnum
 *  of each DataBlock and a pointer to the dumped DataBlock.
 *  \param db DataBlock "package" as created by clPackage_DataBlocks()
 *  \return DRA of IntDBPair objects
 */
struct DRA *clLoad_DataBlock_package(struct DataBlock *db);

/** \brief Don't forget to free your DataBlock packages.
 *
 *  \param da DRA returned from clLoad_DataBlock_package
 */
void clFree_DataBlock_package ( struct DRA *da );

/** \brief Retrieves DataBlock from DRA of IntDBPairs
 *
 *  Another high level function, clScanForTag() will, given a tagnum as defined
 *  in tagtypes.h, scan a DRA of IntDBPairs as produced by
 *  clLoad_DataBlock_package(). This retrieved DataBlock is an independent copy
 *  any must be freed using clDatablockFree().
 *
 *  TODO: when switching db to db pointer, NULL should be returned if tagnum in
 *  question is not found.  Currently, a DataBlock is returned regardless, only
 *  a DataBlock size of 0 signifying a failure in finding the tagnum.
 *  \param dd DRA of IntDBPair objects
 *  \param tnum tagnum of DataBlock in question
 *  \return DataBlock
 */
struct DataBlock *clScanForTag(struct DRA *dd, int tnum);
#endif
