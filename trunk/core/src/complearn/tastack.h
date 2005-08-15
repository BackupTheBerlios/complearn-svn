#ifndef __TASTACK_H
#define __TASTACK_H

/*! \file tastack.h */

/** \brief Container of TransAdaptors.
 *  \struct TAstack
 *
 *  Elements are pushed onto top of a TAstack and either pulled
 *  from bottom (element 0 is removed and rest move down 1) or popped from top
 *  (element size-1 is removed).
 *
 *  TAstack is a dynamically resizing container.
 */
struct TAstack;

/** \brief Returns pointer to a new TAstack.
 *
 *  Allocates memory and returns pointer to new TAstack object, which contains
 *  0 elements.
 */
struct TAstack *newTAStack(void);

/** \brief Frees TAstack object from memory.
 *  \param ts TAstack
 *  \return CL_OK on success
 */
int freeTS(struct TAstack *ts);

/** \brief TransAdaptor object to top of TAstack.
 *  \param ts TAstack
 *  \param ta TransAdaptor
 *  \return CL_OK on success
 */
int pushTS(struct TAstack *ts, struct TransAdaptor *ta);

/** \brief Removes and returns pointer to TransAdaptor from bottom of TAstack.
 *
 *  A TransAdaptor object is removed from the bottom of the TAstack and the
 *  rest of the elements move down by 1, e.g., element 1 becomes element 0,
 *  element 2 becomes element 1, and so forth. A pointer to the removed string
 *  is returned.
 *  \param ts TAstack
 *  \return pointer to TransAdaptor object
 */
struct TransAdaptor *shiftTS(struct TAstack *ts);

/** \brief Removes and returns pointer to TransAdaptor object from top of
 *  TAstack.
 *
 *  A TransAdaptor object is removed from the top of the TAstack, i.e., for a
 *  TAstack of size 10, element 9 is removed.
 *  \param ts TAstack
 *  \return pointer to TransAdaptor object
 */
struct TransAdaptor *popTS(struct TAstack *ts);

/** \brief Returns true value if TAstack is empty.
 *  \param ts TAstack
 *  \return true if TAstack contains 0 elements
 */
int isEmptyTS(struct TAstack *ts);

/** \brief Returns number of elements in TAstack.
 *  \param ts TAstack
 *  \return size
 */
int sizeTS(struct TAstack *ts);

/** \brief Searchs a TAstack and returns pointer to TransAdaptor object.
 *
 *  s is the search term, and searchfunc is a pointer to a function which
 *  matches s against TAstack ts. A pointer to the matching TransAdaptor object
 *  is returned.
 *  \param ts TAstack
 *  \param s search term
 *  \param searchfunc search function
 *  \return pointer to TransAdaptor object
 */
struct TransAdaptor *searchTS(void *ts, void *s, t_searchfunc searchfunc);

/** \brief Searches a TAstack using the return value of the shortname function
 *  as a search term.
 *
 *  sequentialSearchTS() takes string s as a parameter and searches each
 *  element of the TAstack, starting at element 0, where s matches the
 *  return value of the TransAdaptor object's shortname function.  A pointer to
 *  the first matching TransAdaptor object is returned.
 *  \param ts TAstack
 *  \param s string
 *  \return pointer to TransAdaptor object
 */
struct TransAdaptor *sequentialSearchTS(void *ts, void *s);

#endif
