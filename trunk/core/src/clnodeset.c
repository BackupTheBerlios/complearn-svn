#include <complearn/complearn.h>
#include <malloc.h>
#include <assert.h>

struct CLNodeSet
{
  struct DoubleA *da;
};

struct CLNodeSet *clnodesetNew(int howbig)
{
  struct CLNodeSet *result = gcalloc(sizeof(struct CLNodeSet), 1);
  assert(howbig > 0);
  result->da = doubleaNew();
  setValueAt(result->da, howbig-1, zeropct);
  return result;
}

struct CLNodeSet *clnodesetClone(const struct CLNodeSet *cl)
{
  struct CLNodeSet *result = gcalloc(sizeof(struct CLNodeSet), 1);
  result->da = cloneDoubler(cl->da);
  return result;
}

void clnodesetPrint(struct CLNodeSet *cl)
{
  printNodeList(cl->da);
}

void clnodesetFree(struct CLNodeSet *cl)
{
  freeDoubleDoubler(cl->da);
  gfreeandclear(cl);
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

  setValueAt(cl->da, which, p);
}

int clnodesetIsNodeInSet(const struct CLNodeSet *cl, qbase_t which)
{
  return getValueAt(cl->da, which).i;
}

struct DoubleA *clnodesetToDoubleA(const struct CLNodeSet *cl)
{
  return cloneDoubler(cl->da);
}

struct CLNodeSet *doubleaToCLNodeSet(const struct DoubleA *da)
{
  struct CLNodeSet *result = gcalloc(sizeof(struct CLNodeSet), 1);
  result->da = cloneDoubler(da);
  return result;
}

int clnodesetSize(const struct CLNodeSet *cl)
{
   return getSize(cl->da);
}
