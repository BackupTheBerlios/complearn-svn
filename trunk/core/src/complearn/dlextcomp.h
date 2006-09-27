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
#ifndef __DLEXTCOMP_H
#define __DLEXTCOMP_H
/*! \file dlextcomp.h */

#if DLFCN_RDY
#include <dlfcn.h>
#endif

#include <complearn/datablock.h>
#include <complearn/envmap.h>
#include <complearn/compadaptor.h>
/**
 * \brief initialization hook function for CompLearn compression modules
 *
 * INITFUNCNAME is the name of the function that a shared object
 * compression module must export in order to be used by CompLearn.
 * The CompLearn system will call this function to register any number
 * of compressors by name using teh clRegisterCB() function.
 *
 * \sa clRegisterCB()
 */
#define INITFUNCNAME        initCompLearnModule

/**
 * \brief double-quoted string version of INITFUNCNAME
 *
 * INITFUNCQUOTED is just another variation of the same concept as
 * INITFUNCNAME.
 */
#define INITFUNCQUOTED     "initCompLearnModule"

/**
 * \brief correct declaration to use for the initialization function
 *
 * INITFUNCNAME must have the correct type signature, as given by
 * INITFUNCTYPE here.  There are no parameters nor any return values.
 *
 * \sa INITFUNCNAME
 */
#define INITFUNCTYPE() void initCompLearnModule(void)

/** \brief Loads custom compressor module through dynamic library.
 *
 * \param libraryname name of dynamic library
 * \returns 0 on success
 */
int clCompaLoadDynamicLib(const char *libraryname);

/** \brief Loads all compressor modules in a directory
 *
 * This function will automatically load every compressor module
 * in the named diretory; this will mean caling each compression
 * module's initialization function.
 *
 * \param dirname pathname of directory to search
 * \cur may be NULL, or may be a pointer to a struct GeneralConfig
 * that contains further environment information to control the
 * printing options such as fVerbosity.
 * \returns nothing
 */
void clScanDirForModules(const char *dirname, struct GeneralConfig *cur);

/**
 * \breif BZip2 dynamic adaptor to support dual-mode loading
 *
 * This structure allows for dynamic bzip2 library utilization without
 * as much compile / linktime support.
 */
struct BZ2DynamicAdaptor {
  int (*buftobufcompress)(char *dbuff,unsigned int *p,
    char *src, unsigned int sz, int blocksize, int verblevel, int workfactor);
  int (*buftobufdecompress)(char *dbuff,unsigned int *p,
    char *src, unsigned int sz, int small, int verblevel);
};
/**
 * \brief Returns a pointer to the bzip2 singleton adaptor object.
 *
 * This function will try to use static or dynamic symbols to link
 * to the bzip2 library functions.  It returns NULL if it cannot
 * fill in the functions.
 *
 * If the pointers are non-NULL, then they are valid.  Therefore the
 * functions in the adaptor may be called if it succeeds in order to compress.
 */
struct BZ2DynamicAdaptor *clGrabBZ2DA(void);

/**
 * \breif ZLib dynamic adaptor to support dual-mode loading
 *
 * This structure allows for dynamic zlib library utilization without
 * as much compile / linktime support.
 */
struct ZlibDynamicAdaptor {
  int (*compress2)(unsigned char *dbuff,unsigned long *p,
    unsigned char *src, unsigned long sz, int level);
  int (*uncompress)(unsigned char *dbuff,unsigned long *p, unsigned char *src, unsigned long sz);
};

/**
 * \brief Returns a pointer to the singleton ZLib adaptor object.
 *
 * This function will try to use static or dynamic symbols to link
 * to the zlib library functions.  It returns NULL if it cannot
 * fill in the functions.
 *
 * If the pointers are non-NULL, then they are valid.  Therefore the
 * functions in the adaptor may be called if it succeeds in order to compress.
 */
struct ZlibDynamicAdaptor *clGrabZlibDA(void);


#endif
