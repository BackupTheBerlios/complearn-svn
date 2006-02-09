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
#ifndef __GDBMHELPER_H
#define __GDBMHELPER_H

#include <complearn/datablock.h>
/*! \file gdbmhelper.h */

/** \brief a simple encapsulation of the GDBM database system
 * \struct GDBMHelper
 *
 * This structure allows for the convenient reading and writing of
 * GDBM files wihtin the complearn system.  All such files are referenced
 * relative to a hidden directory in $HOME/.complearn.  Basic exact-match
 * fetching and indexed storing of data are supported.  Both keys and
 * values are a simple DataBlock.
 */
struct GDBMHelper;

struct GDBMHelper *cldbopen(const char *filename);
void cldbunlink(const char *filename);
struct DataBlock *cldbfetch(struct GDBMHelper *gh, struct DataBlock *key);
void cldbstore(struct GDBMHelper *gh, struct DataBlock *key, struct DataBlock *val);
int cldbclose(struct GDBMHelper *gh);

#endif
