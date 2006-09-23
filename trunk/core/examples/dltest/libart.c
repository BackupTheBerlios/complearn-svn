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
#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <complearn/complearn.h>

#include "newcomp.h"

struct ARTCompressionInstance {
  void *baseClass;
  int c[256];
};

static int fgetAPIVersionCB(void)
{
  return APIVER_CLCOMP10;
}

static const char *flongNameCB(void)
{
	return "order-0 adaptive arithmetic";
}

static const char *fshortNameCB(void)
{
  return "art";
}

static int fallocSizeCB(void)
{
  return sizeof(struct ARTCompressionInstance);
}

static double fcompressCB(struct CompressionBase *cb, struct DataBlock *src)
{
  struct ARTCompressionInstance *artci = (struct ARTCompressionInstance *) cb;
	int sum = 0;
	int i;
	double codelen = 0.0;

	for (i = 0; i < 256 ; i++) {
		artci->c[i] = 1;
	}
	sum = 256;

  unsigned char *data = clDatablockData(src);
	for (i = 0; i < clDatablockSize(src); i++) {
		unsigned char cur;
		double prob;
		cur = data[i];
		prob = ((double) artci->c[cur]) / ((double) sum);
		codelen -= log(prob);
		artci->c[cur] += 1;
		sum += 1;
	}
	return codelen/log(2);
}

static int fspecificInitCB(struct CompressionBase *cb)
{
  //struct ARTCompressionInstance *artci = (struct ARTCompressionInstance *) cb;
  return 0;
}

static int fprepareToCompressCB(struct CompressionBase *cb)
{
  //struct ARTCompressionInstance *artci = (struct ARTCompressionInstance *) cb;
  return 0;
}

static struct CompressionBaseAdaptor cba = {
  VIRTFUNCEXPORT(specificInitCB),
  VIRTFUNCEXPORT(prepareToCompressCB),
  VIRTFUNCEXPORT(compressCB),
  VIRTFUNCEXPORT(shortNameCB),
  VIRTFUNCEXPORT(longNameCB),
  VIRTFUNCEXPORT(getAPIVersionCB),
  VIRTFUNCEXPORT(allocSizeCB)
};

void initCL(void)
{
  clRegisterCB(&cba);
}


