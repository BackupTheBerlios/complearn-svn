#ifndef __DATABLOCKENUMERATION_H
#define __DATABLOCKENUMERATION_H

#include <complearn/cltypes.h>

#define MAXDB 16

/** \brief The DataBlockEnumerationIterator indicates the current position of
 *   the DataBlock in a private enumeration instance.
 *  \struct DataBlockEnumerationIterator
 */
struct DataBlockEnumerationIterator;

/** \brief Enumeration interface.
 *
 *  DataBlockEnumeration contains 6 pointers: of 5 functions and a private
 *  enumeration instance.
 */
struct DataBlockEnumeration {
  void *eptr;                /*!< Pointer to a private enumeration instance */
  t_newenumiter newenumiter; /*!< Pointer to function which returns a pointer
                                  to a new DataBlockEnumerationIterator
                                  object */
  t_istep istep;             /*!< Pointer to function which advances the
                                  DataBlockEnumerationIterator object by 1 */
  t_iterfree ifree;          /*!< Pointer to function which frees the
                                  DataBlockEnumerationIterator object from
                                  memory */
  t_enumfree efree;          /*!< Pointer to function which frees the
                                  DataBlockEnumeration object from memory */
  t_istar istar;             /*!< Pointer to function which returns pointer to
                                  current DataBlock of a DataBlockEnumeration
                                  object; when end of DataBlockEnumeration
                                  reached, NULL returned */
  t_ilabel ilabel;           /*!< Pointer to a function which returns a label
                                  for the DataBlock indicated by an iterator */
};
/* TODO: wrap above */

/** \brief Container of DataBlockEnumerations
 *  \struct DBAccumulator
 */
struct DBAccumulator {
  struct DataBlockEnumeration *de[MAXDB]; /* array of dbe pointers */
  int desize; /* size of array de */
  struct DBEFactory *dbf; /* pointer to DBEFactory */
};
#endif
