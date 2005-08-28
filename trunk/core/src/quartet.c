#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <malloc.h>
#include <sys/time.h>

#include <complearn/complearn.h>

void normalizeQuartet(struct Quartet *q)
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

struct Quartet permuteLabelsDirect(qbase_t i, qbase_t j, qbase_t k, qbase_t m, int which)
{
  qbase_t d[4];
  d[0] = i; d[1] = j; d[2] = k; d[3] = m;
  return permuteLabels(d, which);
}

void freeSPMSingle(struct DoubleA *d)
{
  int i;
  int n = doubleaSize(d);
  for (i = 0; i < n; ++i)
    doubleaFree(doubleaGetValueAt(d, i).ar);
  doubleaFree(d);
}

void freeSPMSet(struct DoubleA *d)
{
  int i;
  int n = doubleaSize(d);
  for (i = 0; i < n; ++i)
    freeSPMSingle(doubleaGetValueAt(d, i).ar);
}

struct DoubleA *findPath(const struct CLTree *clt, struct DoubleA *spmset, qbase_t qfrom, qbase_t qto)
{
  return doubleaGetValueAt(doubleaGetValueAt(spmset, qfrom).ar, qto).ar;
}

static void mustBeSorted(qbase_t labels[4])
{
  int i;
  for (i = 1; i < 4; ++i) {
    assert(labels[i-1] <= labels[i]);
  }
}

int findConsistentIndex(struct AdjAdaptor *ad, struct LabelPerm *lab, qbase_t labels[4])
{
  int i, z;
  mustBeSorted(labels);
  for (i = 0; i < 3; ++i) {
    struct Quartet q = permuteLabels(labels, i);
    struct Quartet q1;
    for (z = 0; z < 4; z += 1) {
      q1.q[z] = labelpermNodeIDForColIndex(lab, q.q[z]);
    }
    if (isConsistent(ad, q1))
      return i;
  }
  assert(0 && "Quartet consistency failure.");
  return 0;
}

struct Quartet permuteLabels(qbase_t lab[4], int which)
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
      assert(0 && "quartet phase error");
      q.q[0] = q.q[1] = q.q[2] = q.q[3] = 0;
      break;
  }
  return q;
}

