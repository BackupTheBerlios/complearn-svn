#include <complearn/complearn.h>

#include <assert.h>

struct CLNodeSet
{
  struct DRA *da;
};

struct CLNodeSet *clnodesetNew(int howbig)
{
  struct CLNodeSet *result = clCalloc(sizeof(struct CLNodeSet), 1);
  assert(howbig > 0);
  result->da = clDraNew();
  clDraSetValueAt(result->da, howbig-1, zeropct);
  return result;
}

struct CLNodeSet *clnodesetClone(const struct CLNodeSet *cl)
{
  struct CLNodeSet *result = clCalloc(sizeof(struct CLNodeSet), 1);
  result->da = clDraClone(cl->da);
  return result;
}

void clnodesetPrint(struct CLNodeSet *cl)
{
  clDraPrintIntList(cl->da);
}

void clnodesetFree(struct CLNodeSet *cl)
{
  clDraFree(cl->da);
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

  clDraSetValueAt(cl->da, which, p);
}

int clnodesetHasNode(const struct CLNodeSet *cl, qbase_t which)
{
  return clDraGetValueAt(cl->da, which).i;
}

struct DRA *clnodesetToDRA(const struct CLNodeSet *cl)
{
  return clDraClone(cl->da);
}

struct CLNodeSet *clDraToCLNodeSet(const struct DRA *da)
{
  struct CLNodeSet *result = clCalloc(sizeof(struct CLNodeSet), 1);
  result->da = clDraClone(da);
  return result;
}

int clnodesetSize(const struct CLNodeSet *cl)
{
   return clDraSize(cl->da);
}
