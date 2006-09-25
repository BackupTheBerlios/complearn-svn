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
#ifndef __CLOG_H
#define __CLOG_H

/*! \file clLog.h */

/** \brief Unique and authentic type of wooden shoe
 *
 * This structure maintain CompLearn log information.  It may direct
 * output to a file, to stderr, or through a user-defined error
 * function depending on how it is used.
 *
 * \struct Clog
 */
struct Clog;

#include <stdio.h>

/* prints with msg */
void clLogGenericPrintFILE( FILE *outfp, const char *filename, int lineno, const char *msg, const char *fmt, ...);
#define clLogGenericPrint(x...) do { clLogGenericPrintFILE( stderr, __FILE__, __LINE__, "CompLearn Error", x); } while (0)
#define clLogError(x...) do { clLogGenericPrintFILE( stderr, __FILE__, __LINE__, "CompLearn Error",x); exit(1); } while (0)
/* prints */
#define clLogWarning(x...) do { clLogGenericPrintFILE(stderr, __FILE__, __LINE__, "CompLearn Warning", x); } while (0)
#define clLogLog(x...) do { clLogGenericPrintFILE(stdout, __FILE__, __LINE__, "complearnlog", x); } while (0)

#endif
