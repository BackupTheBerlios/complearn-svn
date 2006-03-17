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
#ifndef __DOUBLEA_H
#define __DOUBLEA_H

#include <assert.h>
#include <complearn/datablock.h>
#include <complearn/cltypes.h>

/*! \file dra.h */

/** \brief a dynamically resizing, doubling polymorphic array
 *
 * This structure represents the primary mode of dynamic allocation in
 * the CompLearn system.  It keeps track of its highest-referenced
 * element index, and returns one more than this value for its size.
 * The DRA automatically doubles its sizes and reallocates with a
 * (flat, shallow) copy of all the old information whenever it is
 * necessary.  The DRA supports a variety of different types, of
 * sizes up to 8 bytes.  The union PCTypes contains all possible
 * value types that may be used within this dynamic container class.
 *
 * \sa dra.h
 * \struct DRA
 */
struct DRA;

/** \brief Tag added to a "dump" of a DRA.
 *
 *  This structure is embedded within the resulting DataBlock returned by the
 *  functions clDraDump() and clDraDeepDump(), both of which
 *  are used to write a DRA to file. DRAHdr contains information necessary
 *  for the conversion of a "dumped" DataBlock, using clDraLoad(),
 *  back into a DRA.
 *
 *  \struct DRAHdr
 */
struct DRAHdr;

/** \brief a basic key, value pair
 *
 * This structure holds a key and an associated value as two associated pointers
 *
 * \struct StringPair
 */
struct StringPair { char *key; char *val; };

/** \brief a integer pair
 *
 * This structure holds two integers.
 *
 * \struct IntPair
 */
struct IntPair { int x; int y; };

/** \brief holds the tagnum and a pointer to a DataBlock
 *
 * This structure holds a tagnum and a pointer to a "dumped" DataBlock whose
 * type is described by tagnum.
 *
 * \struct IntDBPair
 */
struct IntDBPair { int tnum ; struct DataBlock *db; };

/** \brief the basic polymorphic types supported by DRA
 *
 * a DRA can hold any number of different objects.  The choices include
 * a double <b>d</b> <br>
 * an integer <b>i</b> <br>
 * a character pointer or string <b>str</b> <br>
 * a pointer to a nested DRA as <b>ar</b> <br>
 * a StringPair <b>sp</b> containing <b>sp.key</b> and <b>sp.val</b> <br>
 * a IntPair <b>ip</b> containing <b>x</b> and <b>y</b> <br>
 * a IntDBPair <b>idbp</b> containing <b>tnum</b> and <b>*db</b> <br>
 * a DataBlock <b>db</b>
 * a pointer to a TransformAdaptor <b>ta</b>
 *
 * When using a PCTypes, it is important to remember that you may only
 * store one of the above types (excepting the StringPair, wherein the pair
 * counts as a single object).  If you store using one type then attempt
 * to retrieve using a different type, then an uncertain future awaits.
 *
 * \union PCTypes
 */
union PCTypes {
  double d;
  int i;
  char *str;
  struct DRA *ar;
  struct StringPair sp;
  struct IntPair ip;
  struct IntDBPair idbp;
  struct DataBlock *dbp;
  struct TransformAdaptor *ta;
  void *ptr;
};

/**
 * These constants are for your convenience.
 * The first, zeropct, is simply a block of all-zeros that you may
 * conveniently copy to clear a PCTypes.  The second is an integer i
 * with the value 1.  You may use this for boolean, set, or other operations.
 */
const extern union PCTypes zeropct, onepcti;

/** \brief Creates a new DRA
 *
 *  Allocates memory and returns a pointer to for a new DRA.  Free memory
 *  allocated by a DRA using clDraFree().
 *  \return pointer to DRA
 */
struct DRA *clDraNew(void);

/** \brief Frees a DRA from memory.
 *  \param ptr pointer to DRA
 */
void clDraFree(struct DRA *ptr);

/** \brief Converts a "dumped" DRA DataBlock back into a DRA
 *
 *  clDraLoad() will take as an argument a DataBlock, which was created
 *  by the clDraDump() or the clDraDeepDump() function, and
 *  convert the DataBlock into a DRA, even if originally a multi-level
 *  DRA. A pointer to the DRA is returned.
 *
 *  An option to clDraLoad() is the fmustbe flag, which, if set to 1,
 *  forces the function to exit when check for the special DRA tag created
 *  by clDraDump() or clDraDeepDump() fails.  If the tag is not
 *  found, an error message is printed to stdout.  Set fmustbe to 0 to return
 *  NULL instead.
 *
 *  \param db pointer to DataBlock
 *  \param fmustbe 1 if the DataBlock must contain the identifying DRA flag;
 *  0 if not
 *  \return pointer to new DRA
 */
struct DRA *clDraLoad(struct DataBlock *d, int fmustbe);

/** \brief Serializes a single-level DRA into a DataBlock
 *
 *  clDraDump() returns a DataBlock which then can be
 *  written to a file using the function clDatablockWriteToFile().  This
 *  resulting DataBlock is also appropriate when using the function
 *  clPackage_DataBlocks().
 *
 *  To convert the resulting DataBlock back into a DRA, use
 *  clDraLoad() function.
 *
 *  Same as using clDraDeepDump(da,0).
 *  \param d pointer to DRA
 *  \return a DataBlock which can be written to file
 */
struct DataBlock *clDraDump(const struct DRA *d);

/** \brief Serializes a multi-level DRA into a DataBlock
 *
 *  clDraDeepDump() returns a DataBlock which then can be
 *  written to a file using the function clDatablockWriteToFile().  This
 *  resulting DataBlock is also appropriate when using the function
 *  clPackage_DataBlocks().
 *
 *  To convert the resulting DataBlock back into a DRA, use
 *  clDraLoad() function.
 *  \param em pointer to DRA
 *  \param level number of levels starting at 0; 0 indicates a single level, 1
 *  indicates a 2-level and so on
 *  \return a DataBlock which can be written to file
 *
 */
struct DataBlock *clDraDeepDump(const struct DRA *d, int level);

/** \brief Frees a multi-level DRA from memory
 *  \param ptr pointer to DRA
 *  \param lvl number of levels starting at 0; 0 indicates a single level, 1
 *  indidicates a 2-level, and so on
 */
void clDraDeepFree(struct DRA *ptr, int lvl);

/** \brief Returns a double, for a DRA of doubles, at a given index
 *
 *  This function is a shortcut used for a DRA of doubles.  Same as
 *  using clDraGetValueAt(da, where).d
 *  \param da pointer to DRA
 *  \param where index
 */
double clDraGetDValueAt(struct DRA *da, int where);

/** \brief Sets a double, for a DRA of doubles, at a given index
 *  \param a pointer to DRA
 *  \param where index
 *  \param val double to set
 */
void clDraSetDValueAt(struct DRA *a, int where, double val);

/** \brief Returns number of elements in DRA.
 *  \param a pointer to DRA
 *  \return size
 */
int clDraSize(const struct DRA *a);

/** \brief Creates a copy of a multi-level DRA
 *  \param ptr pointer to DRA to be copied
 *  \param level number of levels starting at 0
 *  \return pointer to new DRA
 */
struct DRA *clDraDeepClone(const struct DRA *ptr, int lvl);

/** \brief Creates a copy of a single-level DRA
 *
 *  Same as using clDraDeepClone(da,0)
 *  \param ptr pointer to DRA to be copied
 *  \return pointer to new DRA
 */
struct DRA *clDraClone(const struct DRA *ptr);

/** \brief Returns element at given index
 *  \param da pointer to DRA
 *  \param where index
 *  \return pctype instance
 */
union PCTypes clDraGetValueAt(const struct DRA *da, int where);

/** \brief Sets a PCTypes instance at a given index
 *  \param da pointer to DRA
 *  \param where index
 *  \param val PCTypes instance to set
 */
void clDraSetValueAt(struct DRA *da, int where, union PCTypes p);

/** \brief Adds a PCTypes instance to bottom of array (at index 0)
 *  \param da pointer to DRA
 *  \param p PCTypes instance to set
 */
void clDraUnshift(struct DRA *da, union PCTypes p);

/** \brief Adds a PCTypes instance to top of array (at index size)
 *  \param da pointer to DRA
 *  \param p PCTypes instance to set
 */
void clDraPush(struct DRA *da, union PCTypes p);

/** \brief Removes PCTypes instance from bottom of array (at index 0)
 *  \param da pointer to DRA
 *  \return PCTypes instance from bottom of array
 */
union PCTypes clDraShift(struct DRA *da);

/** \brief Removes PCTypes instance from top of array (at index size)
 *  \param da pointer to DRA
 *  \return PCTypes instance from top of array
 */
union PCTypes clDraPop(struct DRA *da);

/** \brief Returns random PCTypes instance from array
 *  \param da pointer to DRA
 *  \return PCTypes instance randomly chosen from array
 */
union PCTypes clDraRandom(const struct DRA *da);

/** \brief Swaps values at given two indeces
 *
 *  clDraSwapAt() will take two indeces of an array and swap their contents.
 *  Returns CL_OK upon success.
 *  \param da pointer to DRA
 *  \param inda first index
 *  \param indb second index
 *  \return CL_OK
 */
int clDraSwapAt(struct DRA *da, int inda, int indb);

/** \brief Prints to stdout list of integers stored in DRA
 *
 *  clDraPrintIntList() only works if the DRA consists of integers
 *  \param da pointer to DRA
 */
void clDraPrintIntList(const struct DRA *da);

/* TODO: following 2 functions, clStringDump() and clStringLoad() obviously belong
 * somewhere else.
 */

/** \brief Converts a character string to a file-writable DataBlock
 *
 *  clStringDump() returns a DataBlock which then can be written to a file using
 *  the function clDatablockWriteToFile().  This resulting DataBlock is also
 *  appropriate when using the function clPackage_DataBlocks().
 *
 *  To convert the resulting DataBlock back into a character string, use
 *  clStringLoad() function.
 *
 *  \param s pointer to character string
 *  \return a pointer to a DataBlock which can be written to file
 */
struct DataBlock *clStringDump(const char *s);

/** \brief Converts a "dumped" string DataBlock back into a string
 *
 *  clStringLoad() will take as an argument a pointer to a DataBlock,
 *  which was created by the clStringDump() function
 *  and convert the DataBlock into a chracter string. A pointer to the string
 *  is returned.
 *
 *  An option to clStringLoad() is the fmustbe flag, which, if set to 1,
 *  forces the function to exit when the check for the special string tag
 *  created by clStringDump() fails.  If the tag is not found, an error message
 *  is printed to stdout.  Set fmustbe to 0 to return NULL instead.
 *
 *  \param db pointer to DataBlock
 *  \param fmustbe 1 if the DataBlock must contain the identifying DRA flag;
 *  0 if not
 *  \return pointer to new DRA
 */
char *clStringLoad(struct DataBlock *d, int fmustbe);

/** \brief Consistency function, to ensure a DRA is memory safe
 *
 *  If input DRA is memory corrupt in any way, program will exit and an
 *  error message will be printed to stdout.
 *  \param da pointer to DRA
 */
void clDraVerify(const struct DRA *da);

/** \brief Checks if a qbase_t node label is in DRA
 *
 *  If qbase_t node label is found in DRA, returns 1. If not, returns 0.
 *  \param da pointer to DRA
 *  \param which node label in question
 *  \returns 1 if node label is stored in DRA, 0 if not
 */
int clDraHasQB(const struct DRA *da, qbase_t which);

/** \brief Prints to stdout a DRA of IntPairs separated by spaces
 *
 *  printInPairList() will only work with a DRA of IntPair objects.
 *  \param da pointer to DRA
 */
void clDraPrintIntPairList(const struct DRA *da);

/** \brief Adds node label to DRA if not already there
 *  \param which node label to be added
 */
void clDraAddQBIfNew(struct DRA *da, qbase_t which);

#define ALLNODES(sz, i) \
  for (i = 0; i < sz; i += 1)

#define ALLPAIRS(sz, i, j) \
  for (i = 0; i < sz; i += 1) \
    for (j = i+1; j < sz; j += 1)

#define ALLTRIPLETS(sz, i, j, k) \
  for (i = 0; i < sz; i += 1) \
    for (j = i+1; j < sz; j += 1) \
      for (k = j+1; k < sz; k += 1)

#define ALLQUARTETS(sz, i, j, k, m) \
  for (i = 0; i < sz; i += 1) \
    for (j = i+1; j < sz; j += 1) \
      for (k = j+1; k < sz; k += 1) \
        for (m = k+1; m < sz; m += 1)

#endif
