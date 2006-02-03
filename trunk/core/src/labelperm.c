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
  assert(labelledNodes);
  assert(clDraSize(labelledNodes) > 0);
  lp->size = clDraSize(labelledNodes);
  lp->coltonode = clDraClone(labelledNodes);
  assert(lp->coltonode);
  assert(clDraSize(lp->coltonode) == clDraSize(labelledNodes));
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
  assert(where.i < clDraSize(lph->nodetocol));
  assert(which >= 0);
  assert(which < clDraSize(lph->nodetocol));
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
  assert(lph);
  assert(lph->nodetocol);
  lp->nodetocol = clDraClone(lph->nodetocol);
  assert(lph->coltonode);
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
  assert(lp);
  assert(lp->nodetocol);
  assert(lp->coltonode);
  assert(lp->size == clDraSize(lp->coltonode));
  assert(clDraSize(lp->coltonode) > 0);
  for (i = 0; i < lp->size; i += 1) {
    int incn = clDraGetValueAt(lp->coltonode, i).i;
    if (incn < 0 || incn >= clDraSize(lp->nodetocol)) {
      printf("Bad entry in coltonode at position %d: %d\n", i, incn);
    }
    int innc = clDraGetValueAt(lp->nodetocol, incn).i;
    if (innc != i) {
      printf("Disagreement at position %d: nodetocol says %d but coltonode[%d] is %d\n", i, innc, i, incn);
    }
    assert(innc == i);
  }
}

void clLabelpermSetColumnIndexToNodeNumber(struct LabelPerm *lp, int col, int n)
{
  union PCTypes p1 = zeropct;
  union PCTypes p2 = zeropct;
  assert(col >= 0 && col <= clDraSize(lp->coltonode));
  assert(n >= 0 && n <= clDraSize(lp->nodetocol));
  p1.i = col;
  p2.i = n;
  clDraSetValueAt(lp->coltonode, col, p2);
  clDraSetValueAt(lp->nodetocol, n, p1);
}
