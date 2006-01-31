#include <complearn/complearn.h>

struct LabelPerm
{
  int size;
  struct DRA *coltonode;
  struct DRA *nodetocol;
};


struct LabelPerm *labelpermNew(struct DRA *labelledNodes)
{
  struct LabelPerm *lp = clCalloc(sizeof(*lp), 1);
  int i;
  assert(labelledNodes);
  assert(draSize(labelledNodes) > 0);
  lp->size = draSize(labelledNodes);
  lp->coltonode = draClone(labelledNodes);
  assert(lp->coltonode);
  assert(draSize(lp->coltonode) == draSize(labelledNodes));
  lp->nodetocol = draNew();
  for (i = 0; i < lp->size; i += 1) {
    union PCTypes p = draGetValueAt(labelledNodes, i);
    union PCTypes g = zeropct;
    g.i = i;
    draSetValueAt(lp->nodetocol, p.i, g);
  }
  return lp;
}

void labelpermFree(struct LabelPerm *lph)
{
  draFree(lph->coltonode);
  lph->coltonode = NULL;
  draFree(lph->nodetocol);
  lph->nodetocol = NULL;
  lph->size = 0;
  clFreeandclear(lph);
}

static void setColToNodeAndMore(struct LabelPerm *lph, int which, union PCTypes where)
{
  /* TODO: fix this to do 1/2 as many writes and be better */
  assert(where.i >= 0);
  assert(where.i < draSize(lph->nodetocol));
  assert(which >= 0);
  assert(which < draSize(lph->nodetocol));
  union PCTypes okey = zeropct;
  okey.i = which;
  draSetValueAt(lph->coltonode, which, where);
  draSetValueAt(lph->nodetocol, where.i, okey);
}

void labelpermMutate(struct LabelPerm *lph)
{
  int i, j;
  union PCTypes pi, pj;

  i = rand() % lph->size;
  do {
    j = rand() % lph->size;
  } while (j == i);

  pi = draGetValueAt(lph->coltonode, i);
  pj = draGetValueAt(lph->coltonode, j);

  setColToNodeAndMore(lph, i, pj);
  setColToNodeAndMore(lph, j, pi);

}

struct LabelPerm *labelpermClone(struct LabelPerm *lph)
{
  struct LabelPerm *lp = clCalloc(sizeof(*lp), 1);
  assert(lph);
  assert(lph->nodetocol);
  lp->nodetocol = draClone(lph->nodetocol);
  assert(lph->coltonode);
  lp->coltonode = draClone(lph->coltonode);
  lp->size = lph->size;
  return lp;
}

int labelpermSize(struct LabelPerm *lph)
{
  return lph->size;
}

int labelpermNodeIDForColIndex(struct LabelPerm *lph, int which)
{
  return draGetValueAt(lph->coltonode, which).i;
}

int labelpermColIndexForNodeID(struct LabelPerm *lph, int which)
{
  return draGetValueAt(lph->nodetocol, which).i;
}

int labelpermIdentical(struct LabelPerm *lpa, struct LabelPerm *lpb)
{
  int i, sz;
  sz = labelpermSize(lpa);
  if (sz != labelpermSize(lpb))
    return 0;
  for (i = 0; i < sz; i += 1)
    if (labelpermNodeIDForColIndex(lpa, i) != labelpermNodeIDForColIndex(lpb, i))
      return 0;
  return 1;
}

#if 0
static void printLabelPerm(struct LabelPerm *lp)
{
  printf("LABELPERM: %d (%p)\n", lp->size, lp);
  int i;
  for (i = 0; i < lp->size; i += 1) {
    int incn = draGetValueAt(lp->coltonode, i).i;
    int innc = draGetValueAt(lp->nodetocol, incn).i;
    printf("%d: incn:%d   innc: (nodetocol[%d]) %d\n", i, incn, incn, innc);
  }
}
#endif

void labelpermVerify(struct LabelPerm *lp)
{
  int i;
  assert(lp);
  assert(lp->nodetocol);
  assert(lp->coltonode);
  assert(lp->size == draSize(lp->coltonode));
  assert(draSize(lp->coltonode) > 0);
  for (i = 0; i < lp->size; i += 1) {
    int incn = draGetValueAt(lp->coltonode, i).i;
    if (incn < 0 || incn >= draSize(lp->nodetocol)) {
      printf("Bad entry in coltonode at position %d: %d\n", i, incn);
    }
    int innc = draGetValueAt(lp->nodetocol, incn).i;
    if (innc != i) {
      printf("Disagreement at position %d: nodetocol says %d but coltonode[%d] is %d\n", i, innc, i, incn);
    }
    assert(innc == i);
  }
}

void labelpermSetColumnIndexToNodeNumber(struct LabelPerm *lp, int col, int n)
{
  union PCTypes p1 = zeropct;
  union PCTypes p2 = zeropct;
  assert(col >= 0 && col <= draSize(lp->coltonode));
  assert(n >= 0 && n <= draSize(lp->nodetocol));
  p1.i = col;
  p2.i = n;
  draSetValueAt(lp->coltonode, col, p2);
  draSetValueAt(lp->nodetocol, n, p1);
}
