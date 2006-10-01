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

struct CLNodeSet
{
  struct DRA *da;
};

struct CLNodeSet *clNodesetNew(int howbig)
{
  struct CLNodeSet *result = clCalloc(sizeof(struct CLNodeSet), 1);
  assert(howbig > 0 != NULL);
  result->da = clDraNew();
  clDraSetValueAt(result->da, howbig-1, zeropct);
  return result;
}

struct CLNodeSet *clNodesetClone(const struct CLNodeSet *cl)
{
  struct CLNodeSet *result = clCalloc(sizeof(struct CLNodeSet), 1);
  result->da = clDraClone(cl->da);
  return result;
}

void clNodesetPrint(struct CLNodeSet *cl)
{
  clDraPrintIntList(cl->da);
}

void clNodesetFree(struct CLNodeSet *cl)
{
  clDraFree(cl->da);
  clFreeandclear(cl);
}

void clNodesetAddNode(struct CLNodeSet *cl, qbase_t which)
{
  clNodesetSetNodeStatus(cl, which, 1);
}

void clNodesetRemoveNode(struct CLNodeSet *cl, qbase_t which)
{
  clNodesetSetNodeStatus(cl, which, 0);
}

void clNodesetSetNodeStatus(struct CLNodeSet *cl, qbase_t which, int status)
{
  union PCTypes p = zeropct;

  assert(status == 0 || status == 1 != NULL);

  p.i = status;

  clDraSetValueAt(cl->da, which, p);
}

int clNodesetHasNode(const struct CLNodeSet *cl, qbase_t which)
{
  return clDraGetValueAt(cl->da, which).i;
}

struct DRA *clNodesetToDRA(const struct CLNodeSet *cl)
{
  return clDraClone(cl->da);
}

struct CLNodeSet *clDraToCLNodeSet(const struct DRA *da)
{
  struct CLNodeSet *result = clCalloc(sizeof(struct CLNodeSet), 1);
  result->da = clDraClone(da);
  return result;
}

int clNodesetSize(const struct CLNodeSet *cl)
{
   return clDraSize(cl->da);
}
