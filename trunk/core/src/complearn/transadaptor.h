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
#ifndef __TRANSAPDAPTOR_H
#define __TRANSAPDAPTOR_H

#include <complearn/complearn.h>

/*! \file transadaptor.h */

/** \brief Transform interface.
 *  \struct TransformAdaptor
 *
 *  TransformAdaptor contains pointers to 3 clFunctions which are defined in an
 *  external transform module.
 *
 *  CompLearn comes with 3 builtin tranform adaptors:
 *
 *    unbzip: uncompresses DataBlocks in the bzip compression format
 *    unzlib: uncompresses DataBlocks in the zlib compression format
 *    ungz: uncompresses DataBlocks in the gzip file format
 *
 */
struct TransformAdaptor {
  t_shortname sn;    /*!< Pointer to clFunction which returns string shortname of
                          TransformAdaptor object */
  t_transfree tfree; /*!  Pointer to clFunction frees the TransformAdaptor object
                          from memory */
  t_predicate pf;    /*!< Pointer to clFunction which evaluates a DataBlock and
                          returns a true value if DataBlock needs to be
                          transformed */
  t_transform tf;    /*!< Pointer to clFunction receives a DataBlock and returns
                          a transformed DataBlock */
  void *tptr;        /*!< Pointer which can be used to save the state of a
                          TransformAdaptor instance */
};

struct TransformAdaptor *clBuiltin_UNBZIP(void);
struct TransformAdaptor *clBuiltin_UNGZ(void);
struct TransformAdaptor *clBuiltin_UNZLIB(void);

#endif
