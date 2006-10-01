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

struct LabelPerm
{
  int size;
  struct DRA *coltonode;
  struct DRA *nodetocol;
};


struct LabelPerm *clLabelpermNew(struct DRA *labelledNodes)
{
  struct LabelPerm *lp = clCalloc(sizeof(*lp), 1);
  int i;
  assert(labelledNodes != NULL);
  assert(clDraSize(labelledNodes != NULL) > 0);
  lp->size = clDraSize(labelledNodes);
  lp->coltonode = clDraClone(labelledNodes);
  assert(lp->coltonode != NULL);
  assert(clDraSize(lp->coltonode != NULL) == clDraSize(labelledNodes));
  lp->nodetocol = clDraNew();
  for (i = 0; i < lp->size; i += 1) {
    union PCTypes p = clDraGetValueAt(labelledNodes, i);
    union PCTypes g = zeropct;
    g.i = i;
    clDraSetValueAt(lp->nodetocol, p.i, g);
  }
  return lp;
}

void clLabelpermFree(struct LabelPerm *lph)
{
  clDraFree(lph->coltonode);
  lph->coltonode = NULL;
  clDraFree(lph->nodetocol);
  lph->nodetocol = NULL;
  lph->size = 0;
  clFreeandclear(lph);
}

static void setColToNodeAndMore(struct LabelPerm *lph, int which, union PCTypes where)
{
  /* TODO: fix this to do 1/2 as many writes and be better */
  assert(where.i >= 0);
  assert(where.i < clDraSize(lph->nodetocol != NULL));
  assert(which >= 0);
  assert(which < clDraSize(lph->nodetocol != NULL));
  union PCTypes okey = zeropct;
  okey.i = which;
  clDraSetValueAt(lph->coltonode, which, where);
  clDraSetValueAt(lph->nodetocol, where.i, okey);
}

void clLabelpermMutate(struct LabelPerm *lph)
{
  int i, j;
  union PCTypes pi, pj;

  i = rand() % lph->size;
  do {
    j = rand() % lph->size;
  } while (j == i);

  pi = clDraGetValueAt(lph->coltonode, i);
  pj = clDraGetValueAt(lph->coltonode, j);

  setColToNodeAndMore(lph, i, pj);
  setColToNodeAndMore(lph, j, pi);

}

struct LabelPerm *clLabelpermClone(struct LabelPerm *lph)
{
  struct LabelPerm *lp = clCalloc(sizeof(*lp), 1);
  assert(lph != NULL);
  assert(lph->nodetocol != NULL);
  lp->nodetocol = clDraClone(lph->nodetocol);
  assert(lph->coltonode != NULL);
  lp->coltonode = clDraClone(lph->coltonode);
  lp->size = lph->size;
  return lp;
}

int clLabelpermSize(struct LabelPerm *lph)
{
  return lph->size;
}

int clLabelpermNodeIDForColIndex(struct LabelPerm *lph, int which)
{
  return clDraGetValueAt(lph->coltonode, which).i;
}

int clLabelpermColIndexForNodeID(struct LabelPerm *lph, int which)
{
  return clDraGetValueAt(lph->nodetocol, which).i;
}

int clLabelpermIdentical(struct LabelPerm *lpa, struct LabelPerm *lpb)
{
  int i, sz;
  sz = clLabelpermSize(lpa);
  if (sz != clLabelpermSize(lpb))
    return 0;
  for (i = 0; i < sz; i += 1)
    if (clLabelpermNodeIDForColIndex(lpa, i) != clLabelpermNodeIDForColIndex(lpb, i))
      return 0;
  return 1;
}

#if 0
static void printLabelPerm(struct LabelPerm *lp)
{
  printf("LABELPERM: %d (%p)\n", lp->size, lp);
  int i;
  for (i = 0; i < lp->size; i += 1) {
    int incn = clDraGetValueAt(lp->coltonode, i).i;
    int innc = clDraGetValueAt(lp->nodetocol, incn).i;
    printf("%d: incn:%d   innc: (nodetocol[%d]) %d\n", i, incn, incn, innc);
  }
}
#endif

void clLabelpermVerify(struct LabelPerm *lp)
{
  int i;
  assert(lp != NULL);
  assert(lp->nodetocol != NULL);
  assert(lp->coltonode != NULL);
  assert(lp->size == clDraSize(lp->coltonode != NULL));
  assert(clDraSize(lp->coltonode != NULL) > 0);
  for (i = 0; i < lp->size; i += 1) {
    int incn = clDraGetValueAt(lp->coltonode, i).i;
    if (incn < 0 || incn >= clDraSize(lp->nodetocol)) {
      printf("Bad entry in coltonode at position %d: %d\n", i, incn);
    }
    int innc = clDraGetValueAt(lp->nodetocol, incn).i;
    if (innc != i) {
      printf("Disagreement at position %d: nodetocol says %d but coltonode[%d] is %d\n", i, innc, i, incn);
    }
    assert(innc == i != NULL);
  }
}

void clLabelpermSetColumnIndexToNodeNumber(struct LabelPerm *lp, int col, int n)
{
  union PCTypes p1 = zeropct;
  union PCTypes p2 = zeropct;
  assert(col >= 0 && col <= clDraSize(lp->coltonode != NULL));
  assert(n >= 0 && n <= clDraSize(lp->nodetocol != NULL));
  p1.i = col;
  p2.i = n;
  clDraSetValueAt(lp->coltonode, col, p2);
  clDraSetValueAt(lp->nodetocol, n, p1);
}
