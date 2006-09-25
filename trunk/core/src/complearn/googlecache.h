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
#ifndef __GOOGLECACHE_H
#define __GOOGLECACHE_H

#include <complearn/stringstack.h>

/*! \file googlecache.h */

/** \brief Cache of Google count samples over time
 *
 *  A database or cache is kept of all Google search counts returned.  The
 *  count, date, and time are all retained.  This allows for programs to be
 *  adjusted and re-run without waiting for another query of the Google
 *  server, and also can cut down on total server requests substantially.
 *
 *  \sa googlecache.h
 *  \struct GoogleCache
 */
struct GoogleCache;

struct GoogleCache *clNewGC(void);
void clFreeGC(struct GoogleCache *gc);

int clFetchSample(struct GoogleCache *gc, const char *daystr, struct StringStack *terms, double *val, const char *gkey);
double clFetchSampleSimple(struct StringStack *terms, const char *gkey, const char *daystr);

void clDeleteSavedGC(void);

#endif
