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

void clAdjaPrint(struct AdjAdaptor *aa)
{
  aa->vptr->adjaprint(aa);
}
int clAdjaSize(struct AdjAdaptor *aa)
{
  return aa->vptr->adjasize(aa);
}
void clAdjaFree(struct AdjAdaptor *aa)
{
  aa->vptr->adjafree(aa);
}
struct AdjAdaptor *clAdjaClone(struct AdjAdaptor *aa)
{
  return aa->vptr->adclJaclone(aa);
}
int clAdjaGetConState(struct AdjAdaptor *aa, int i, int j)
{
  return aa->vptr->adjagetconstate(aa, i, j);
}
void clAdjaSetConState(struct AdjAdaptor *aa, int i, int j, int which)
{
  aa->vptr->adjasetconstate(aa, i, j, which);
}
int clAdjaNeighborCount(struct AdjAdaptor *aa, int i)
{
  return aa->vptr->adjagetneighborcount(aa, i);
}
int clAdjaNeighbors(struct AdjAdaptor *aa, int i, int *nbuf, int *nsize)
{
  return aa->vptr->adjagetneighbors(aa, i, nbuf, nsize);
}
struct DRA *clAdjaSPMMap(struct AdjAdaptor *aa)
{
  return aa->vptr->adjaspmmap(aa);
}
