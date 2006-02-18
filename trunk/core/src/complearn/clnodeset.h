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
#ifndef __CLNODESET_H
#define __CLNODESET_H
/*! \file clnodeset.h */

/** \brief A dynamically resizing set of integers
 * \struct CLNodeSet
 *
 * This structure represents a set of nodes commonly, or more generally,
 * a set of integers.  The set is not ordered.  Each integer is either
 * in or out of a given CLNodeSet.
 */
struct CLNodeSet;

/** \brief Allocates a new struct CLNodeSet
 * This function allocates a new CLNodeSet.
 * \param howbig an optional value indicating how big this set should
 * initially be sized.  This value is not a limit.
 */
struct CLNodeSet *clnodesetNew(int howbig);
struct CLNodeSet *clnodesetClone(const struct CLNodeSet *cl);
void clnodesetFree(struct CLNodeSet *cl);

/** \brief Adds a node label to a CLNodeSet
 *  \param cl CLNodeSet
 *  \param which node label to add
 */
void clnodesetAddNode(struct CLNodeSet *cl, qbase_t which);
void clnodesetRemoveNode(struct CLNodeSet *cl, qbase_t which);
void clnodesetSetNodeStatus(struct CLNodeSet *cl, qbase_t which, int status);
int clnodesetHasNode(const struct CLNodeSet *cl, qbase_t which);
void clnodesetPrint(struct CLNodeSet *cl);
struct DRA *clnodesetToDRA(const struct CLNodeSet *cl);
struct CLNodeSet *clDraToCLNodeSet(const struct DRA *da);
int clnodesetSize(const struct CLNodeSet *cl);

#endif

