#ifndef __STRINGSTACK_H
#define __STRINGSTACK_H

#include <complearn/datablock.h>

/*! \file stringstack.h */

/** \brief Container of strings.
 *  \struct StringStack
 *
 *  Elements are pushed onto top of StringStack and either pulled
 *  from bottom (element 0 is removed and rest move down 1) or popped from top
 *  (element size-1 is removed).
 *
 *  The maximum size of a StringStack is MAX_SS_SIZE.
 */
struct StringStack;

/** \brief Returns pointer to a new StringStack.
 *
 *  Allocates memory and returns pointer to new StringStack object, which
 *  contains 0 elemments.
 */
struct StringStack *newStringStack(void);

/** \brief Returns pointer to a new StringStack and add one element
 *
 *  Allocates memory and returns pointer to new StringStack object, which
 *  contains just 1 elemment.  Note that the string itself is not copied.
 *  \param str a string to put in the StringStack
 *  \return pointer to the new StringStack
 */
struct StringStack *newSingleSS(const char *str);

/** \brief Loads a StringStack from a serialized format inside a DataBlock
 *
 * Allocates memory and returns a pointer to a deserialized StringStack
 * instance.  The DataBlock passed in to this function should have been made
 * using a dumpStringStack call earlier.
 *
 * \param db a DataBlock containing the serialized format StringStack
 * \return pointer to the newly allocated deserialized StringStack
 */
struct StringStack *loadStringStack(struct DataBlock db, int fmustbe);

/** \brief Dumps a StringStack into a serialized format and returns a DataBlock
 *
 * This function saves or serializes a StringStack, preparing it to be saved
 * within a file, transmitted over the network, etc.
 *
 * \param ss pointer to the StringStack instance to be saved / serialized
 * \return DataBlock containing the serialized information
 */
struct DataBlock dumpStringStack(const struct StringStack *ss);

/** \brief Duplicates StringStack and returns a pointer to a new StringStack.
 *  \param ss StringStack to be duplicated
 *  \return pointer to new StringStack
 */
struct StringStack *cloneSS(struct StringStack *ss);

/** \brief Frees StringStack object from memory.
 *  \param ss StringStack
 *  \return CL_OK on success
 */
int freeSS(struct StringStack *ss);

/** \brief Adds new element to StringStack.
 *  \param ss StringStack
 *  \param string
 *  \return CL_OK on success; CL_ERRFULL if stack has reached MAX_SS_SIZE
 */
int pushSS(struct StringStack *ss, const char *str);

/** \brief Returns true value if StringStack is empty
 *  \param ss StringStack
 *  \return true if StringStack contains 0 elements
 */
int isEmptySS(struct StringStack *ss);

/** \brief Returns number of elements in StringStack.
 *  \param ss StringStack
 *  \return size
 */
int sizeSS(const struct StringStack *ss);

/** \brief Sorts a StringStack lexicographically.
 *  \param ss StringStack
 *  \return nothing
 */
int sortSS(struct StringStack *ss);

int unshiftSS(struct StringStack *ss, const char *str);

/** \brief Removes and returns string from bottom of StringStack.
 *
 *  A string is removed from the bottom of the StringStack and the rest of the
 *  elements move down by 1, e.g., element 1 becomes element 0, element 2
 *  becomes element 1, and so forth. A pointer to the removed string is
 *  returned.
 *  \param ss StringStack
 *  \return pointer to string
 */
char *shiftSS(struct StringStack *ss);

/** \brief Removes and returns string from top of StringStack.
 *
 *  A string is removed from the top of the StringStack, i.e., for a
 *  StringStack of size 10, element 9 is removed.
 *  \param ss StringStack
 *  \return pointer to string
 */
char *popSS(struct StringStack *ss);

/** \brief Returns element at index i.
 *  \param ss StringStack
 *  \param i string array index
 *  \return pointer to string
 */
char *readAtSS(struct StringStack *ss, int i);

/** \brief merges two StringStacks.
 *  \param ssa StringStack
 *  \param ssb StringStack
 *  \return pointer to new StringStack containing combination
 */
struct StringStack *mergeSS(struct StringStack *ssa, struct StringStack *ssb);

/** \brief Prints the contents of a StringStack, newline separated, to stdout
 *  \param ss pointer to StringStack
 */
void printSS(struct StringStack *ss);

#endif

