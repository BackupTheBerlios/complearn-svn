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
#ifndef __DATABLOCKENUMERATION_H
#define __DATABLOCKENUMERATION_H

#include <complearn/cltypes.h>

#define MAXDB 16

/*! \file datablockenum.h */

/** \brief The DataBlockEnumerationIterator indicates the current position of
 *   the DataBlock in a private enumeration instance.
 *  \struct DataBlockEnumerationIterator
 */
struct DataBlockEnumerationIterator;

/** \brief Enumeration interface.
 *
 *  DataBlockEnumeration contains 6 pointers: of 5 clFunctions and a private
 *  enumeration instance.
 */
struct DataBlockEnumeration {
  void *eptr;                /*!< Pointer to a private enumeration instance */
  t_newenumiter newenumiter; /*!< Pointer to clFunction which returns a pointer
                                  to a new DataBlockEnumerationIterator
                                  object */
  t_istep istep;             /*!< Pointer to clFunction which advances the
                                  DataBlockEnumerationIterator object by 1 */
  t_iterfree ifree;          /*!< Pointer to clFunction which frees the
                                  DataBlockEnumerationIterator object from
                                  memory */
  t_enumfree efree;          /*!< Pointer to clFunction which frees the
                                  DataBlockEnumeration object from memory */
  t_istar istar;             /*!< Pointer to clFunction which returns pointer to
                                  current DataBlock of a DataBlockEnumeration
                                  object; when end of DataBlockEnumeration
                                  reached, NULL returned */
  t_ilabel ilabel;           /*!< Pointer to a clFunction which returns a label
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
