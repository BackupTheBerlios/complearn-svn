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
#include <complearn/complearn.h>
#include <assert.h>

double clMndf (double ca, double cb, double cab, double cba) {
  double maxk = ca < cb ? cb : ca;
	double kab = cba - cb;
	double kba = cab - ca;
  double maxck;

	maxck = kab < kba ? kba : kab;

	return ( maxck / maxk );
}

double clNcdFunc(struct DataBlock *a, struct DataBlock *b, struct GeneralConfig *cur)
{
  double result;
  if (cur->fJustSize) {
    struct DataBlock *c;
    assert(cur->ca != NULL);
    c = clDatablockCatPtr(a, b);
    result = clCompressCB(cur->ca, c);
    clDatablockFreePtr(c);
  }
  else
    result = clNcdFuncCB(cur->ca, a, b);
  return result;
}

double clNcdFuncCB(struct CompressionBase *cb, struct DataBlock *a, struct DataBlock *b)
{
  double result;
  struct DataBlock *c;
  double x, y, xy;
  assert(cb != NULL);

  c = clDatablockCatPtr(a, b);

  x = clCompressCB(cb, a);
  y = clCompressCB(cb, b);
  xy = clCompressCB(cb, c);
  result = clMndf (x, y, xy, xy);

  clDatablockFreePtr(c);

  return result;
}
