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
#include <complearn/complearn.h>

struct IndDist {
  int index;
  double distance;
};

static int sidcompare(const void *a, const void *b)
{
  struct IndDist *ia = ((struct IndDist *)a), *ib = ((struct IndDist *)b);
  if (ia->distance < ib->distance)
    return -1;
  if (ia->distance > ib->distance)
    return 1;
  return 0;
}

/* Pass in an array of count strings in str and count entries in res; it
 * will return a sorted list with closest matches first.
 */
int clFindClosestMatchCB(struct CompressionBase *cb, char *target,  char **str, int count, struct StringWithDistance *res)
{
  int i;
  struct DataBlock *dbtar;
  struct IndDist *sid;
  if (count < 0)
    clLogError("Invalid negative count specified in approximate match.");
  if (count == 0)
    return 0;
  if (target == NULL)
    clLogError("Must pass in target string to find closest match.");
  if (str == NULL)
    clLogError("Must pass in array of strings to match.");
  if (res == NULL)
    clLogError("Must pass in array to hold results.");
  dbtar = clStringToDataBlockPtr(target);
  sid = clCalloc(sizeof(*sid), count);
  for (i = 0; i < count; i += 1) {
    struct DataBlock *dbcur;
    sid[i].index = i;
    dbcur = clStringToDataBlockPtr(str[i]);
    sid[i].distance = clNcdFuncCB(cb, dbtar, dbcur);
    clDatablockFreePtr(dbcur);
  }
  qsort(sid, count, sizeof(sid[0]), sidcompare);
  for (i = 0; i < count; i += 1) {
    res[i].str = str[sid[i].index];
    res[i].distance = sid[i].distance;
  }
  clFree(sid);
  clDatablockFreePtr(dbtar);
  return 0;
}

