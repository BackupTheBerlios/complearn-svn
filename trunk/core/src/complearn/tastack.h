#ifndef __TASTACK_H
#define __TASTACK_H

/*! \file tastack.h */

/** \brief Container of TransformAdaptors.
 *  \struct TransformAdaptorStack
 *
 *  Elements are pushed onto top of a TransformAdaptorStack and either pulled
 *  from bottom (element 0 is removed and rest move down 1) or popped from top
 *  (element size-1 is removed).
 *
 *  TransformAdaptorStack is a dynamically resizing container.
 */
struct TransformAdaptorStack;

/** \brief Returns pointer to a new TransformAdaptorStack.
 *
 *  Allocates memory and returns pointer to new TransformAdaptorStack object, which contains
 *  0 elements.
 */
struct TransformAdaptorStack *clNewTAStack(void);

/** \brief Frees TransformAdaptorStack object from memory.
 *  \param ts TransformAdaptorStack
 *  \return CL_OK on success
 */
int clFreeTS(struct TransformAdaptorStack *ts);

/** \brief TransformAdaptor object to top of TransformAdaptorStack.
 *  \param ts TransformAdaptorStack
 *  \param ta TransformAdaptor
 *  \return CL_OK on success
 */
int clPushTS(struct TransformAdaptorStack *ts, struct TransformAdaptor *ta);

/** \brief Removes and returns pointer to TransformAdaptor from bottom of TransformAdaptorStack.
 *
 *  A TransformAdaptor object is removed from the bottom of the TransformAdaptorStack and the
 *  rest of the elements move down by 1, e.g., element 1 becomes element 0,
 *  element 2 becomes element 1, and so forth. A pointer to the removed string
 *  is returned.
 *  \param ts TransformAdaptorStack
 *  \return pointer to TransformAdaptor object
 */
struct TransformAdaptor *clShiftTS(struct TransformAdaptorStack *ts);

/** \brief Removes and returns pointer to TransformAdaptor object from top of
 *  TransformAdaptorStack.
 *
 *  A TransformAdaptor object is removed from the top of the TransformAdaptorStack, i.e., for a
 *  TransformAdaptorStack of size 10, element 9 is removed.
 *  \param ts TransformAdaptorStack
 *  \return pointer to TransformAdaptor object
 */
struct TransformAdaptor *clPopTS(struct TransformAdaptorStack *ts);

/** \brief Returns true value if TransformAdaptorStack is empty.
 *  \param ts TransformAdaptorStack
 *  \return true if TransformAdaptorStack contains 0 elements
 */
int clIsEmptyTS(struct TransformAdaptorStack *ts);

/** \brief Returns number of elements in TransformAdaptorStack.
 *  \param ts TransformAdaptorStack
 *  \return size
 */
int clSizeTS(struct TransformAdaptorStack *ts);

/** \brief Searchs a TransformAdaptorStack and returns pointer to TransformAdaptor object.
 *
 *  s is the search term, and searchclFunc is a pointer to a clFunction which
 *  matches s against TransformAdaptorStack ts. A pointer to the matching TransformAdaptor object
 *  is returned.
 *  \param ts TransformAdaptorStack
 *  \param s search term
 *  \param searchclFunc search clFunction
 *  \return pointer to TransformAdaptor object
 */
struct TransformAdaptor *clSearchTS(void *ts, void *s, t_searchclFunc searchclFunc);

/** \brief Searches a TransformAdaptorStack using the return value of the shortname clFunction
 *  as a search term.
 *
 *  sequentialSearchTS() takes string s as a parameter and searches each
 *  element of the TransformAdaptorStack, starting at element 0, where s matches the
 *  return value of the TransformAdaptor object's shortname clFunction.  A pointer to
 *  the first matching TransformAdaptor object is returned.
 *  \param ts TransformAdaptorStack
 *  \param s string
 *  \return pointer to TransformAdaptor object
 */
struct TransformAdaptor *sequentialSearchTS(void *ts, void *s);

#endif
