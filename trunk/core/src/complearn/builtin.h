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
#ifndef __BUILTIN_H
#define __BUILTIN_H

#include <complearn/uclconfig.h>
#include <complearn/environment.h>

/*! \file builtin.h */

/** \brief Returns pointer to a builtin compressor interface.
 *
 * The clCompaLoadBuiltin() clFunction takes a char *name and matches it against a
 * list of builtin compressor interfaces which are available.  Availability of
 * an interface is dependent on whether its corresponding compression library
 * is installed during compile time.  Currently, CompLearn has 2 builtin
 * compressor adaptors: "bzip" and "zlib".
 *
 * If name matches an available compressor interface, a pointer to that
 * CompAdaptor interface is returned. If name is not found in the list, NULL is
 * returned.
 * \param name name of compressor interface
 * \returns pointer to CompAdaptor or NULL
 */
struct CompAdaptor *clCompaLoadBuiltin(const char *name);

/** \brief Returns pointer to StringStack of available builtin compressors.
 *
 */
struct StringStack *clCompaListBuiltin(void);

/** \brief Returns pointer to builtin bzip2 compressor interface.
 *
 *  Equivalent to calling load BuiltinCA("bzip").
 * \returns pointer to CompAdaptor or NULL
 */
struct CompAdaptor *clCompaLoadBzip2(void);

/** \brief Returns pointer to builtin blocksort compressor interface.
 *
 *  Equivalent to calling load BuiltinCA("blocksort").
 * \returns pointer to CompAdaptor
 */
struct CompAdaptor *clCompaLoadBlockSort(void);

/** \brief Returns pointer to builtin google compressor interface.
 *
 *  Equivalent to calling load BuiltinCA("google").
 * \returns pointer to CompAdaptor or NULL
 */
struct CompAdaptor *clCompaLoadGoogle(void);

/** \brief Returns pointer to builtin zlib compressor interface.
 *
 *  Equivalent to calling load BuiltinCA("zlib").
 * \returns pointer to CompAdaptor or NULL
 */
struct CompAdaptor *clCompaLoadZlib(void);

/** \brief Prints a list of builtin compressors.
 *
 *  clCompaPrintBuiltin() prints to stdout a list of builtin compressors
 *  supported by CompLearn.  This list is dependent on the compression
 *  libraries detected during source compilation.
 */
void clCompaPrintBuiltin(void);

/* TODO:  Following clFunctions belong somewhere else.  They are not true
 * builtin compressors.  Must also rename the files builtinca-realcomp.c,
 * builtinca-virtcomp.c, and builtinca-soap.c */

/** \brief Returns pointer to a real compressor interface.
 *
 *  The real compressor interface requires a parameter cmd in the EnvMap for
 *  newcompinst. cmd specifies the filename of a command to execute to compress
 *  data. This executable is expected to read the input data from STDIN and
 *  output compressed data to STDOUT. The compressed bytes will be counted and
 *  converted to bits.
 * \returns pointer to CompAdaptor
 */
struct CompAdaptor *clCompaLoadReal(const char *cmd);

/** \brief Returns pointer to a virtual compressor interface.
 *
 * The virtual compressor interface requires a parameter cmd in the EnvMap
 * for newcompinst. cmd specifies the filename of a command to execute to
 * compress data. This executable is expected to read the input data from STDIN
 * and output the floating point number representing the compressed size, in
 * bits, to STDOUT. This number should be printed in standard ASCII-format as a
 * decimal.
 * \returns pointer to CompAdaptor
 */
struct CompAdaptor *clCompaLoadVirtual(const char *cmd);

/** \brief Returns pointer to a SOAP compressor server interface.
 *
 *  The SOAP compressor server interace requires a url of the form
 *  "http://localhost:2000/" and urn of the form "urn:hws". The CompLearn
 *  system will connect to the specified SOAP server and call a single clFunction
 *  called compclFunc and pass it one string. The compression server is expected
 *  to return a double precision floating-point value indicating the compressed
 *  size, in bits, of the string.
 * \returns pointer to CompAdaptor
 */
struct CompAdaptor *clCompaLoadSOAP(const char *url, const char *urn);

/** Dynamic Adaptors to support dual-mode loading */
struct BZ2DynamicAdaptor {
  int (*buftobufcompress)(char *dbuff,unsigned int *p,
    char *src, unsigned int sz, int blocksize, int verblevel, int workfactor);
  int (*buftobufdecompress)(char *dbuff,unsigned int *p,
    char *src, unsigned int sz, int small, int verblevel);
};

struct BZ2DynamicAdaptor *clGrabBZ2DA(void);

struct ZlibDynamicAdaptor {
  int (*compress2)(unsigned char *dbuff,unsigned long *p,
    unsigned char *src, unsigned long sz, int level);
  int (*uncompress)(unsigned char *dbuff,unsigned long *p, unsigned char *src, unsigned long sz);
};

struct ZlibDynamicAdaptor *clGrabZlibDA(void);

#endif
