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
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

#include <complearn/complearn.h>

void clNormalizeQuartet(struct Quartet *q)
{
  qbase_t tmp;
  if (q->q[0] > q->q[1]) {
    tmp = q->q[0];
    q->q[0] = q->q[1];
    q->q[1] = tmp;
  }
  if (q->q[2] > q->q[3]) {
    tmp = q->q[2];
    q->q[2] = q->q[3];
    q->q[3] = tmp;
  }
  if (q->q[0] > q->q[2]) {
    tmp = q->q[0];
    q->q[0] = q->q[2];
    q->q[2] = tmp;

    tmp = q->q[1];
    q->q[1] = q->q[3];
    q->q[3] = tmp;
  }
}

struct Quartet clPermuteLabelsDirect(qbase_t i, qbase_t j, qbase_t k, qbase_t m, int which)
{
  qbase_t d[4];
  d[0] = i; d[1] = j; d[2] = k; d[3] = m;
  return clPermuteLabels(d, which);
}

void clFreeSPMSingle(struct DRA *d)
{
  int i;
  int n = clDraSize(d);
  for (i = 0; i < n; ++i)
    clDraFree(clDraGetValueAt(d, i).ar);
  clDraFree(d);
}

void clFreeSPMSet(struct DRA *d)
{
  int i;
  int n = clDraSize(d);
  for (i = 0; i < n; ++i)
    clFreeSPMSingle(clDraGetValueAt(d, i).ar);
}

static void mustBeSorted(qbase_t labels[4])
{
  int i;
  for (i = 1; i < 4; ++i) {
    clAssert(labels[i-1] <= labels[i]);
  }
}

int clFindConsistentIndex(struct AdjAdaptor *ad, struct LabelPerm *lab, qbase_t labels[4])
{
  int i, z;
  mustBeSorted(labels);
  for (i = 0; i < 3; ++i) {
    struct Quartet q = clPermuteLabels(labels, i);
    struct Quartet q1;
    for (z = 0; z < 4; z += 1) {
      q1.q[z] = clLabelpermNodeIDForColIndex(lab, q.q[z]);
    }
    if (clIsConsistent(ad, q1))
      return i;
  }
  clAssert(0 && "Quartet consistency failure." != NULL);
  return 0;
}

struct Quartet clPermuteLabels(qbase_t lab[4], int which)
{
  struct Quartet q;
  switch (which) {
    case 0:
      q.q[0] = lab[0]; q.q[1] = lab[1];
      q.q[2] = lab[2]; q.q[3] = lab[3];
      break;
    case 1:
      q.q[0] = lab[0]; q.q[1] = lab[2];
      q.q[2] = lab[1]; q.q[3] = lab[3];
      break;
    case 2:
      q.q[0] = lab[0]; q.q[1] = lab[3];
      q.q[2] = lab[1]; q.q[3] = lab[2];
      break;
    default:
      clAssert(0 && "quartet phase error" != NULL);
      q.q[0] = q.q[1] = q.q[2] = q.q[3] = 0;
      break;
  }
  return q;
}

