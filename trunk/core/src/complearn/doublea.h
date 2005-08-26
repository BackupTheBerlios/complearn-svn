#ifndef __DOUBLEA_H
#define __DOUBLEA_H

#include <assert.h>
#include <complearn/datablock.h>
#include <complearn/cltypes.h>

/*! \file doublea.h */

/** \brief a dynamically resizing, doubling polymorphic array
 * \struct DoubleA
 *
 * This structure represents the primary mode of dynamic allocation in
 * the CompLearn system.  It keeps track of its highest-referenced
 * element index, and returns one more than this value for its size.
 * The DoubleA automatically doubles its sizes and reallocates with a
 * (flat, shallow) copy of all the old information whenever it is
 * necessary.  The DoubleA supports a variety of different types, of
 * sizes up to 8 bytes.  The union PCTypes contains all possible
 * value types that may be used within this dynamic container class.
 *
 * \sa doublea.h
 */
struct DoubleA;

/** \brief Tag added to a "dump" of a DoubleA.
 *  \struct DAHdr
 *
 *  This structure is embedded within the resulting DataBlock returned by the
 *  functions dumpDoubleDoubler() and dumpDeepDoubleDoubler(), both of which
 *  are used to write a DoubleA to file. DAHdr contains information necessary
 *  for the conversion of a "dumped" DataBlock, using loadDoubleDoubler(),
 *  back into a DoubleA.
 */
struct DAHdr;

/** \brief a basic key, value pair
 * \struct StringPair
 * This structure holds a key and an associated value as two associated pointers */
struct StringPair { char *key; char *val; };

/** \brief a integer pair
 * \struct IntPair
 * This structure holds two integers.
 */
struct IntPair { int x; int y; };

/** \brief holds the tagnum and a pointer to a DataBlock
 * \struct IntDBPair
 * This structure holds a tagnum and a pointer to a "dumped" DataBlock whose
 * type is described by tagnum.
 */
struct IntDBPair { int tnum ; struct DataBlock *db; };

/** \brief the basic polymorphic types supported by DoubleA
 * \union PCTypes
 *
 * a DoubleA can hold any number of different objects.  The choices include
 * a double <b>d</b> <br>
 * an integer <b>i</b> <br>
 * a character pointer or string <b>str</b> <br>
 * a pointer to a nested DoubleA as <b>ar</b> <br>
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
 */
union PCTypes {
  double d;
  int i;
  char *str;
  struct DoubleA *ar;
  struct StringPair sp;
  struct IntPair ip;
  struct IntDBPair idbp;
  struct DataBlock db;
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

/** \brief Creates a new DoubleA
 *
 *  Allocates memory and returns a pointer to for a new DoubleA.  Free memory
 *  allocated by a DoubleA using freeDoubleDoubler().
 *  \return pointer to DoubleA
 */
struct DoubleA *newDoubleDoubler(void);

/** \brief Frees a DoubleA from memory.
 *  \param ptr pointer to DoubleA
 */
void freeDoubleDoubler(struct DoubleA *ptr);

/** \brief Converts a "dumped" DoubleA DataBlock back into a DoubleA
 *
 *  loadDoubleDoubler() will take as an argument a DataBlock, which was created
 *  by the dumpDoubleDoubler() or the dumpDeepDoubleDoubler() function, and
 *  convert the DataBlock into a DoubleA, even if originally a multi-level
 *  DoubleA. A pointer to the DoubleA is returned.
 *
 *  An option to loadDoubleDoubler() is the fmustbe flag, which, if set to 1,
 *  forces the function to exit when check for the special DoubleA tag created
 *  by dumpDoubleDoubler() or dumpDeepDoubleDoubler() fails.  If the tag is not
 *  found, an error message is printed to stdout.  Set fmustbe to 0 to return
 *  NULL instead.
 *
 *  \param db pointer to DataBlock
 *  \param fmustbe 1 if the DataBlock must contain the identifying DoubleA flag;
 *  0 if not
 *  \return pointer to new DoubleA
 */
struct DoubleA *loadDoubleDoubler(struct DataBlock d, int fmustbe);

/** \brief Serializes a single-level DoubleA into a DataBlock
 *
 *  dumpDoubleDoubler() returns a DataBlock which then can be
 *  written to a file using the function writeDataBlockToFile().  This
 *  resulting DataBlock is also appropriate when using the function
 *  package_DataBlocks().
 *
 *  To convert the resulting DataBlock back into a DoubleA, use
 *  loadDoubleDoubler() function.
 *
 *  Same as using dumpDeepDoubleDoubler(da,0).
 *  \param d pointer to DoubleA
 *  \return a DataBlock which can be written to file
 */
struct DataBlock dumpDoubleDoubler(const struct DoubleA *d);

/** \brief Serializes a multi-level DoubleA into a DataBlock
 *
 *  dumpDeepDoubleDoubler() returns a DataBlock which then can be
 *  written to a file using the function writeDataBlockToFile().  This
 *  resulting DataBlock is also appropriate when using the function
 *  package_DataBlocks().
 *
 *  To convert the resulting DataBlock back into a DoubleA, use
 *  loadDoubleDoubler() function.
 *  \param em pointer to DoubleA
 *  \param level number of levels starting at 0; 0 indicates a single level, 1
 *  indicates a 2-level and so on
 *  \return a DataBlock which can be written to file
 *
 */
struct DataBlock dumpDeepDoubleDoubler(const struct DoubleA *d, int level);

/** \brief Frees a multi-level DoubleA from memory
 *  \param ptr pointer to DoubleA
 *  \param lvl number of levels starting at 0; 0 indicates a single level, 1
 *  indidicates a 2-level, and so on
 */
void freeDeepDoubleDoubler(struct DoubleA *ptr, int lvl);

/** \brief Returns a double, for a DoubleA of doubles, at a given index
 *
 *  This function is a shortcut used for a DoubleA of doubles.  Same as
 *  using getValueAt(da, where).d
 *  \param da pointer to DoubleA
 *  \param where index
 */
double getDValueAt(struct DoubleA *da, int where);

/** \brief Sets a double, for a Doublea of doubles, at a given index
 *  \param a pointer to DoubleA
 *  \param where index
 *  \param val double to set
 */
void setDValueAt(struct DoubleA *a, int where, double val);

/** \brief Returns number of elements in DoubleA.
 *  \param a pointer to DoubleA
 *  \return size
 */
int getSize(const struct DoubleA *a);

/** \brief Creates a copy of a multi-level DoubleA
 *  \param ptr pointer to DoubleA to be copied
 *  \param level number of levels starting at 0
 *  \return pointer to new DoubleA
 */
struct DoubleA *deepCopyLvl(const struct DoubleA *ptr, int lvl);

/** \brief Creates a copy of a single-level DoubleA
 *
 *  Same as using deepCopyLvl(da,0)
 *  \param ptr pointer to DoubleA to be copied
 *  \return pointer to new DoubleA
 */
struct DoubleA *cloneDoubler(const struct DoubleA *ptr);

/** \brief Returns element at given index
 *  \param da pointer to DoubleA
 *  \param where index
 *  \return pctype instance
 */
union PCTypes getValueAt(const struct DoubleA *da, int where);

/** \brief Sets a PCTypes instance at a given index
 *  \param da pointer to DoubleA
 *  \param where index
 *  \param val PCTypes instance to set
 */
void setValueAt(struct DoubleA *da, int where, union PCTypes p);

/** \brief Adds a PCTypes instance to bottom of array (at index 0)
 *  \param da pointer to DoubleA
 *  \param p PCTypes instance to set
 */
void unshiftValue(struct DoubleA *da, union PCTypes p);

/** \brief Adds a PCTypes instance to top of array (at index size)
 *  \param da pointer to DoubleA
 *  \param p PCTypes instance to set
 */
void pushValue(struct DoubleA *da, union PCTypes p);

/** \brief Removes PCTypes instance from bottom of array (at index 0)
 *  \param da pointer to DoubleA
 *  \return PCTypes instance from bottom of array
 */
union PCTypes shiftDoubleDoubler(struct DoubleA *da);

/** \brief Removes PCTypes instance from top of array (at index size)
 *  \param da pointer to DoubleA
 *  \return PCTypes instance from top of array
 */
union PCTypes popDoubleDoubler(struct DoubleA *da);

/** \brief Returns random PCTypes instance from array
 *  \param da pointer to DoubleA
 *  \return PCTypes instance randomly chosen from array
 */
union PCTypes getRandomElement(const struct DoubleA *da);

/** \brief Swaps values at given two indeces
 *
 *  swapValues() will take two indeces of an array and swap their contents.
 *  Returns CL_OK upon success.
 *  \param da pointer to DoubleA
 *  \param inda first index
 *  \param indb second index
 *  \return CL_OK
 */
int swapValues(struct DoubleA *da, int inda, int indb);

/** \brief Prints to stdout list of integers stored in DoubleA
 *  printNodeList() only works if the DoubleA consists of integers
 *  \param da pointer to DoubleA
 */
void printNodeList(const struct DoubleA *da);

/** \brief Converts a character string to a file-writable DataBlock
 *
 *  dumpString() returns a DataBlock which then can be written to a file using
 *  the function writeDataBlockToFile().  This resulting DataBlock is also
 *  appropriate when using the function package_DataBlocks().
 *
 *  To convert the resulting DataBlock back into a character string, use
 *  loadString() function.
 *
 *  \param s pointer to character string
 *  \return a DataBlock which can be written to file
 */
struct DataBlock dumpString(const char *s);

/** \brief Converts a "dumped" string DataBlock back into a string
 *
 *  loadString() will take as an argument a pointer to a DataBlock,
 *  which was created by the dumpString() function
 *  and convert the DataBlock into a chracter string. A pointer to the string
 *  is returned.
 *
 *  An option to loadString() is the fmustbe flag, which, if set to 1,
 *  forces the function to exit when the check for the special string tag
 *  created by dumpString() fails.  If the tag is not found, an error message
 *  is printed to stdout.  Set fmustbe to 0 to return NULL instead.
 *
 *  \param db pointer to DataBlock
 *  \param fmustbe 1 if the DataBlock must contain the identifying DoubleA flag;
 *  0 if not
 *  \return pointer to new DoubleA
 */
char *loadString(struct DataBlock d, int fmustbe);

/** \brief Consistency function, to ensure a DoubleA is memory safe
 *  If input DoubleA is memory corrupt in any way, program will exit and an
 *  error message will be printed to stdout.
 *  \param da pointer to DoubleA
 */
void verifyDoubleDoubler(const struct DoubleA *da);

/** \brief Checks if a qbase_t node label is in DoubleA
 *  If qbase_t node label is found in DoubleA, returns 1. If not, returns 0.
 *  \param da pointer to DoubleA
 *  \param which node label in question
 *  \returns 1 if node label is stored in DoubleA, 0 if not
 */
int isInDAqb(const struct DoubleA *da, qbase_t which);

/** \brief Prints to stdout a DoubleA of IntPairs separated by spaces
 *
 *  printInPairList() will only work with a DoubleA of IntPair objects.
 *  \param da pointer to DoubleA
 */
void printIntPairList(const struct DoubleA *da);

/** \brief Adds node label to DoubleA if not already there
 *  \param which node label to be added
 */
void addIfNewqb(struct DoubleA *da, qbase_t which);

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
