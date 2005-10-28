#include <complearn/complearn.h>
#include "clmalloc.h"
#include <assert.h>

struct CLNodeSet
{
  struct DoubleA *da;
};

struct CLNodeSet *clnodesetNew(int howbig)
{
  struct CLNodeSet *result = clCalloc(sizeof(struct CLNodeSet), 1);
  assert(howbig > 0);
  result->da = doubleaNew();
  doubleaSetValueAt(result->da, howbig-1, zeropct);
  return result;
}

struct CLNodeSet *clnodesetClone(const struct CLNodeSet *cl)
{
  struct CLNodeSet *result = clCalloc(sizeof(struct CLNodeSet), 1);
  result->da = doubleaClone(cl->da);
  return result;
}

void clnodesetPrint(struct CLNodeSet *cl)
{
  doubleaPrintIntList(cl->da);
}

void clnodesetFree(struct CLNodeSet *cl)
{
  doubleaFree(cl->da);
  clFreeandclear(cl);
}

void clnodesetAddNode(struct CLNodeSet *cl, qbase_t which)
{
  clnodesetSetNodeStatus(cl, which, 1);
}

void clnodesetRemoveNode(struct CLNodeSet *cl, qbase_t which)
{
  clnodesetSetNodeStatus(cl, which, 0);
}

void clnodesetSetNodeStatus(struct CLNodeSet *cl, qbase_t which, int status)
{
  union PCTypes p = zeropct;

  assert(status == 0 || status == 1);

  p.i = status;

  doubleaSetValueAt(cl->da, which, p);
}

int clnodesetHasNode(const struct CLNodeSet *cl, qbase_t which)
{
  return doubleaGetValueAt(cl->da, which).i;
}

struct DoubleA *clnodesetToDoubleA(const struct CLNodeSet *cl)
{
  return doubleaClone(cl->da);
}

struct CLNodeSet *doubleaToCLNodeSet(const struct DoubleA *da)
{
  struct CLNodeSet *result = clCalloc(sizeof(struct CLNodeSet), 1);
  result->da = doubleaClone(da);
  return result;
}

int clnodesetSize(const struct CLNodeSet *cl)
{
   return doubleaSize(cl->da);
}
