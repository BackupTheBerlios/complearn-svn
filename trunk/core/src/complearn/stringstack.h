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
#ifndef __STRINGSTACK_H
#define __STRINGSTACK_H

#include <complearn/datablock.h>

/*! \file stringstack.h */

/** \brief Container of strings.
 *
 *  Elements are pushed onto top of StringStack and either pulled
 *  from bottom (element 0 is removed and rest move down 1) or popped from top
 *  (element size-1 is removed).
 *
 *  The maximum size of a StringStack is MAX_SS_SIZE.
 *  \struct StringStack
 */
struct StringStack;

/** \brief Returns pointer to a new StringStack.
 *
 *  Allocates memory and returns pointer to new StringStack object, which
 *  contains 0 elemments.
 */
struct StringStack *clStringstackNew(void);

/** \brief Returns pointer to a new StringStack and add one element
 *
 *  Allocates memory and returns pointer to new StringStack object, which
 *  contains just 1 elemment.  Note that the string itself is not copied.
 *  \param str a string to put in the StringStack
 *  \return pointer to the new StringStack
 */
struct StringStack *clStringstackNewSingle(const char *str);

/** \brief Duplicates StringStack and returns a pointer to a new StringStack.
 *  \param ss StringStack to be duplicated
 *  \return pointer to new StringStack
 */
struct StringStack *clStringstackClone(struct StringStack *ss);

/** \brief Frees StringStack object from memory.
 *  \param ss StringStack
 *  \return CL_OK on success
 */
int clStringstackFree(struct StringStack *ss);

/** \brief Adds new element to StringStack.
 *  \param ss StringStack
 *  \param string
 *  \return CL_OK on success; CL_ERRFULL if stack has reached MAX_SS_SIZE
 */
int clStringstackPush(struct StringStack *ss, const char *str);

/** \brief Returns true value if StringStack is empty
 *  \param ss StringStack
 *  \return true if StringStack contains 0 elements
 */
int clStringstackIsEmpty(struct StringStack *ss);

/** \brief Returns number of elements in StringStack.
 *  \param ss StringStack
 *  \return size
 */
int clStringstackSize(const struct StringStack *ss);

/** \brief Sorts a StringStack lexicographically.
 *  \param ss StringStack
 *  \return nothing
 */
int clStringstackSort(struct StringStack *ss);

int clStringstackUnshift(struct StringStack *ss, const char *str);

/** \brief Removes and returns string from bottom of StringStack.
 *
 *  A string is removed from the bottom of the StringStack and the rest of the
 *  elements move down by 1, e.g., element 1 becomes element 0, element 2
 *  becomes element 1, and so forth. A pointer to the removed string is
 *  returned.
 *  \param ss StringStack
 *  \return pointer to string
 */
char *clShiftSS(struct StringStack *ss);

/** \brief Removes and returns string from top of StringStack.
 *
 *  A string is removed from the top of the StringStack, i.e., for a
 *  StringStack of size 10, element 9 is removed.
 *  \param ss StringStack
 *  \return pointer to string
 */
char *clStringstackPop(struct StringStack *ss);

/** \brief Returns element at index i.
 *  \param ss StringStack
 *  \param i string array index
 *  \return pointer to string
 */
char *clStringstackReadAt(struct StringStack *ss, int i);

/** \brief merges two StringStacks.
 *  \param ssa StringStack
 *  \param ssb StringStack
 *  \return pointer to new StringStack containing combination
 */
struct StringStack *clStringstackMerge(struct StringStack *ssa, struct StringStack *ssb);

/** \brief Prints the contents of a StringStack, newline separated, to stdout
 *  \param ss pointer to StringStack
 */
void clStringstackPrint(struct StringStack *ss);

int clStringstackLongestLength(struct StringStack *ss);

#endif

