#include <complearn/complearn.h>
#include "clalloc.h"
#include <assert.h>

struct CLNodeSet
{
  struct DRA *da;
};

struct CLNodeSet *clnodesetNew(int howbig)
{
  struct CLNodeSet *result = clCalloc(sizeof(struct CLNodeSet), 1);
  assert(howbig > 0);
  result->da = draNew();
  draSetValueAt(result->da, howbig-1, zeropct);
  return result;
}

struct CLNodeSet *clnodesetClone(const struct CLNodeSet *cl)
{
  struct CLNodeSet *result = clCalloc(sizeof(struct CLNodeSet), 1);
  result->da = draClone(cl->da);
  return result;
}

void clnodesetPrint(struct CLNodeSet *cl)
{
  draPrintIntList(cl->da);
}

void clnodesetFree(struct CLNodeSet *cl)
{
  draFree(cl->da);
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

  draSetValueAt(cl->da, which, p);
}

int clnodesetHasNode(const struct CLNodeSet *cl, qbase_t which)
{
  return draGetValueAt(cl->da, which).i;
}

struct DRA *clnodesetToDRA(const struct CLNodeSet *cl)
{
  return draClone(cl->da);
}

struct CLNodeSet *draToCLNodeSet(const struct DRA *da)
{
  struct CLNodeSet *result = clCalloc(sizeof(struct CLNodeSet), 1);
  result->da = draClone(da);
  return result;
}

int clnodesetSize(const struct CLNodeSet *cl)
{
   return draSize(cl->da);
}
