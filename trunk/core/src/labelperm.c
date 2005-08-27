#include <complearn/complearn.h>

struct LabelPerm
{
  int size;
  struct DoubleA *coltonode;
  struct DoubleA *nodetocol;
};


struct LabelPerm *newLabelPerm(struct DoubleA *labelledNodes)
{
  struct LabelPerm *lp = gcalloc(sizeof(*lp), 1);
  int i;
  assert(labelledNodes);
  assert(doubleaSize(labelledNodes) > 0);
  lp->size = doubleaSize(labelledNodes);
  lp->coltonode = doubleaClone(labelledNodes);
  assert(lp->coltonode);
  assert(doubleaSize(lp->coltonode) == doubleaSize(labelledNodes));
  lp->nodetocol = doubleaNew();
  for (i = 0; i < lp->size; i += 1) {
    union PCTypes p = doubleaGetValueAt(labelledNodes, i);
    union PCTypes g = zeropct;
    g.i = i;
    doubleaSetValueAt(lp->nodetocol, p.i, g);
  }
  return lp;
}

void freeLabelPerm(struct LabelPerm *lph)
{
  doubleaFree(lph->coltonode);
  lph->coltonode = NULL;
  doubleaFree(lph->nodetocol);
  lph->nodetocol = NULL;
  lph->size = 0;
  gfreeandclear(lph);
}

static void setColToNodeAndMore(struct LabelPerm *lph, int which, union PCTypes where)
{
  /* TODO: fix this to do 1/2 as many writes and be better */
  assert(where.i >= 0);
  assert(where.i < doubleaSize(lph->nodetocol));
  assert(which >= 0);
  assert(which < doubleaSize(lph->nodetocol));
  union PCTypes okey = zeropct;
  okey.i = which;
  doubleaSetValueAt(lph->coltonode, which, where);
  doubleaSetValueAt(lph->nodetocol, where.i, okey);
}

void mutateLabelPerm(struct LabelPerm *lph)
{
  int i, j;
  union PCTypes pi, pj;

  i = rand() % lph->size;
  do {
    j = rand() % lph->size;
  } while (j == i);

  pi = doubleaGetValueAt(lph->coltonode, i);
  pj = doubleaGetValueAt(lph->coltonode, j);

  setColToNodeAndMore(lph, i, pj);
  setColToNodeAndMore(lph, j, pi);

}

struct LabelPerm *cloneLabelPerm(struct LabelPerm *lph)
{
  struct LabelPerm *lp = gcalloc(sizeof(*lp), 1);
  assert(lph);
  assert(lph->nodetocol);
  lp->nodetocol = doubleaClone(lph->nodetocol);
  assert(lph->coltonode);
  lp->coltonode = doubleaClone(lph->coltonode);
  lp->size = lph->size;
  return lp;
}

int doubleaSizeLP(struct LabelPerm *lph)
{
  return lph->size;
}

int getNodeIDForColumnIndexLP(struct LabelPerm *lph, int which)
{
  return doubleaGetValueAt(lph->coltonode, which).i;
}

int getColumnIndexForNodeIDLP(struct LabelPerm *lph, int which)
{
  return doubleaGetValueAt(lph->nodetocol, which).i;
}

int isLabelPermIdentical(struct LabelPerm *lpa, struct LabelPerm *lpb)
{
  int i, sz;
  sz = doubleaSizeLP(lpa);
  if (sz != doubleaSizeLP(lpb))
    return 0;
  for (i = 0; i < sz; i += 1)
    if (getNodeIDForColumnIndexLP(lpa, i) != getNodeIDForColumnIndexLP(lpb, i))
      return 0;
  return 1;
}

#if 0
static void printLabelPerm(struct LabelPerm *lp)
{
  printf("LABELPERM: %d (%p)\n", lp->size, lp);
  int i;
  for (i = 0; i < lp->size; i += 1) {
    int incn = doubleaGetValueAt(lp->coltonode, i).i;
    int innc = doubleaGetValueAt(lp->nodetocol, incn).i;
    printf("%d: incn:%d   innc: (nodetocol[%d]) %d\n", i, incn, incn, innc);
  }
}
#endif

void verifyLabelPerm(struct LabelPerm *lp)
{
  int i;
  assert(lp);
  assert(lp->nodetocol);
  assert(lp->coltonode);
  assert(lp->size == doubleaSize(lp->coltonode));
  assert(doubleaSize(lp->coltonode) > 0);
  for (i = 0; i < lp->size; i += 1) {
    int incn = doubleaGetValueAt(lp->coltonode, i).i;
    if (incn < 0 || incn >= doubleaSize(lp->nodetocol)) {
      printf("Bad entry in coltonode at position %d: %d\n", i, incn);
    }
    int innc = doubleaGetValueAt(lp->nodetocol, incn).i;
    if (innc != i) {
      printf("Disagreement at position %d: nodetocol says %d but coltonode[%d] is %d\n", i, innc, i, incn);
    }
    assert(innc == i);
  }
}
