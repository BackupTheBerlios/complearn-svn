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
#include <string.h>
#include <assert.h>
#include <stdio.h>


#include <complearn/complearn.h>

#define INIT_TS_SIZE 11

#if 0
struct TransformAdaptorStack {
	struct TransformAdaptor **ta;
	int size;
  int allocsize;
};
#endif

struct TransformAdaptorStack {
  struct DRA *da;
};

struct TransformAdaptorStack *clNewTAStack()
{
	struct TransformAdaptorStack *ts;
	ts = (struct TransformAdaptorStack*)clCalloc(sizeof(struct TransformAdaptorStack), 1);
  ts->da = clDraNew();
	return ts;
}

int clFreeTS(struct TransformAdaptorStack *ts)
{
	clDraFree(ts->da);
	ts->da = NULL;
	free(ts);
	return CL_OK;
}

int clPushTS(struct TransformAdaptorStack *ts, struct TransformAdaptor *ta)
{
  union PCTypes p;

  p.ta = ta;
  clDraPush(ts->da, p);
  return CL_OK;
}

struct TransformAdaptor *clShiftTS(struct TransformAdaptorStack *ts)
{
  union PCTypes p;
  memset(&p, 0, sizeof(p));
	if (clSizeTS(ts) == 0) return p.ta;
	p = clDraShift(ts->da);
  return p.ta;
}

struct TransformAdaptor *clPopTS(struct TransformAdaptorStack *ts)
{
  union PCTypes p;
  memset(&p, 0, sizeof(p));
	if (clSizeTS(ts) == 0) return p.ta;
	p = clDraPop(ts->da);
  return p.ta;
}

int clIsEmptyTS(struct TransformAdaptorStack *ts)
{
	return clDraSize(ts->da) == 0;
}

int clSizeTS(struct TransformAdaptorStack *ts)
{
	return clDraSize(ts->da);
}

struct TransformAdaptor *clSearchTS(void *ts, void *s, t_searchclFunc searchclFunc)
{
	struct TransformAdaptor *curta = NULL;
	curta = searchclFunc(ts,s);
	return curta;
}

/*
struct TransformAdaptor *sequentialSearchTS(void *ts, void *s)
{
	struct TransformAdaptorStack *curts = (struct TransformAdaptorStack *)ts;
	const char *curstr = (const char *)s;
  int i;
	struct TransformAdaptor *curta = NULL;
	struct TransformAdaptor *found = NULL;
  for (i = 0 ; i < curts->size ; i++) {
	  curta = curts->ta[i];
		if (curta && strcmp(curta->sn(),curstr) == 0) {
			found = curta;
		  break;
		}
	}
	return found;
}
*/
