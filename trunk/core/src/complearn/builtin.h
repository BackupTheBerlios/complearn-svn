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
#include <complearn/stringstack.h>

/** \brief Returns pointer to a SOAP compressor server interface.
 *
 *  The SOAP compressor server interace requires a url of the form
 *  "http://localhost:2000/" and urn of the form "urn:hws". The CompLearn
 *  system will connect to the specified SOAP server and call a single function
 *  called compfunc and pass it one string. The compression server is expected
 *  to return a double precision floating-point value indicating the compressed
 *  size, in bits, of the string.
 * \returns pointer to CompAdaptor
 */
struct CompAdaptor *clCompaLoadSOAP(const char *url, const char *urn);

struct ParamList *clGetParameterListCB(struct CompressionBase *cb);

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

#ifndef __NEWCOMP_H
#define __NEWCOMP_H

#include <complearn/datablock.h>
#include <complearn/envmap.h>

#define APIVER_CLCOMP10 10
#define VIRTFUNCEXPORT(x) x : f##x

struct CompressionBase;
struct CompressionBaseAdaptor;

struct CompressionBase {
  struct CompressionBaseInternal *cbi;
};

void clRegisterCB(struct CompressionBaseAdaptor *vptr);

struct CompressionBase *clNewCompressorCB(const char *shortName);
const char *clShortNameCB(struct CompressionBase *cb);
const char *clLongNameCB(struct CompressionBase *cb);
const char *expandCommand(const char *inpcmd);
int clForkPipeExecAndFeedCB(struct DataBlock *inp, const char *cmd, struct StringStack *afterCmdArgs);
void clZombieReaperCB(int q);
struct EnvMap *clGetParametersCB(struct CompressionBase *cb);
const char *clGetParamStringCB(struct CompressionBase *cb);
int clSetParameterCB(struct CompressionBase *cb, const char *key, const char *val, int isPrivate);
void clSetLastErrorCB(struct CompressionBase *cb, const char *errMsg);
void clSetLastStaticErrorCB(const char *shortName, const char *errMsg);
double clCompressCB(struct CompressionBase *cb, struct DataBlock *db);
struct DataBlock *clConcatCB(struct CompressionBase *cb, struct DataBlock *db1,
                            struct DataBlock *db2);
struct CompressionBaseAdaptor *clGetCBAsuper(void);
struct CompressionBaseAdaptor *clGetCBA(struct CompressionBase *cb);
const char *clLastErrorCB(struct CompressionBase *cb);
int clIsEnabledCB(const char *shortName);
const char *clLastStaticErrorCB(const char *shortName);
void clFreeCB(struct CompressionBase *cb);
void clSetStaticErrorMessage(const char *shortName, const char *msg);
struct StringStack *clListBuiltinsCB(int fWithDisabled);
void clPrintCompressors(void);

struct CompressionBaseAdaptor {
  int (*specificInitCB)(struct CompressionBase *cb);
  void (*freeCB)(struct CompressionBase *cb);
  int (*getAPIVersionCB)(void);
  const char *(*longNameCB)(void);

  int (*isDisabledCB)(void);

    // Should read params and use them.   This will be called only once
    // prior to compression.  This should do any compressor-specific
    // initialization at this point and return 0 to indicate success or
    // nonzero to indicate an error.  Don't forget to clSetLastErrorCB().
  int (*prepareToCompressCB)(struct CompressionBase *cb);

  int (*getWindowSizeCB)(void); // Return 0 for "infinite window"
  int (*doesRoundWholeBytesCB)(void);

  int (*isCompileProblemCB)(void); // Returns != 0 iff compile-time problem
       // is detected that will prevent this compressor from working.
       // If != 0 is to be returned, a call to setLastErrorCB should first
       // occur to explain the reason for the error.
  int (*isRuntimeProblemCB)(void); // Returns != 0 iff runtime problem
  const char *(*shortNameCB)(void);
  int (*allocSizeCB)(void);

  const char *(*toStringCB)(struct CompressionBase *cb);

  const char *(*paramStringCB)(struct CompressionBase *cb);  // Should show params somehow

  /* Returns result in bits */
  double (*compressCB)(struct CompressionBase *cb, struct DataBlock *datum);
  struct DataBlock *(*concatCB)(struct CompressionBase *cb, struct DataBlock *dat1, struct DataBlock *dat2);
  int (*isAutoEnabledCB)(void);
};

#endif
#endif
