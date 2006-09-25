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
#ifndef __NCBASE_H
#define __NCBASE_H

/* COMPLEARN COMPRESSION MODULE INTERFACE BASE CLASS */
/*
 *
 * Users may override any or all functions listed in the structure below.
 * Follow the example in libart.c.
 */
struct CompressionBaseAdaptor {

/* Any Compressor-specific initialization code goes here */
  int (*specificInitCB)(struct CompressionBase *cb);
/* Any deallocation functions other than the already-handled freeing of the
 * main memory block pointed to by cb.  Only fill this in if you have deep
 * pointers in your structure that need to be freed.
 */
  void (*freeCB)(struct CompressionBase *cb);

/* For future backwards compatibility */
  int (*getAPIVersionCB)(void);

/* A longer (10-80 characters) description of this compressor */
  const char *(*longNameCB)(void);

/* Returns 0 iff this compressor is "Enabled".  If it returns 1, it must
 * have already called clSetLastStaticErrorCB before returning.
 */
  int (*isDisabledCB)(void);

/* Should read params and use them.   This will be called only once
 * prior to compression.  This should do any compressor-specific
 * initialization at this point and return 0 to indicate success or nonzero
 * to indicate an error.  Don't forget to clSetLastErrorCB().
 */
  int (*prepareToCompressCB)(struct CompressionBase *cb);

/* Returns the windows size measured *in bytes*.  0 means no window.  */
  int (*getWindowSizeCB)(void);

/* Returns 0 if the compressor can return fractional bit results, nonzero
 * otherwise.
 */
  int (*doesRoundWholeBytesCB)(void);

/* Returns 0 if all compile time checks succeeded.  Alternatively, it
 * calls clSetLastStaticErrorCB to describe a problem and then returns
 * nonzero, disabling the compressor at initialization.
 */
  int (*isCompileProblemCB)(void);

/* Returns 0 if all classwise runtime checks succeeded.  Alternatively, it
 * calls clSetLastStaticErrorCB to describe a problem and then returns
 * nonzero, disabling the compressor at initialization.
 */
  int (*isRuntimeProblemCB)(void);

  /* Returns the shortNameCB, used to identify the compressor.
   * For example, "zlib" is a shortName that is used in CompLearn.
   */
  const char *(*shortNameCB)(void);

  /* Returns the size of the initial allocated memory block for
   * this compressor instance.  It must include the void *base pointer
   * as the first element and you should never access this field yourself
   * directly but instead rely on the framework to manage it.
   */
  int (*allocSizeCB)(void);

  /* Returns a simple text description useful for debugging of the
   * compressor instance.
   */
  const char *(*toStringCB)(struct CompressionBase *cb);

  /* Returns a simple text description of the parameters for this instance.  */
  const char *(*paramStringCB)(struct CompressionBase *cb);

  /* Returns compressed size of datum measured in bits */
  double (*compressCB)(struct CompressionBase *cb, struct DataBlock *datum);

  /* Concatenates the two input blocks in a compressor-defined way */
  struct DataBlock *(*concatCB)(struct CompressionBase *cb, struct DataBlock *dat1, struct DataBlock *dat2);

  /* Returns 0 if this compressor is "special" and should be skipped when
   * doing 'automatic scan' type runs.  Returns 1 if the compressor is
   * considered normal enough to be included in an ensemble run.
   */
  int (*isAutoEnabledCB)(void);
};

extern struct CompressionBaseAdaptor cbsuper;

#endif

