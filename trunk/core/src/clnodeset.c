#include <complearn/complearn.h>
#include <malloc.h>
#include <assert.h>

struct CLNodeSet
{
  struct DoubleA *da;
};

struct CLNodeSet *newCLNodeSet(int howbig)
{
  struct CLNodeSet *result = gcalloc(sizeof(struct CLNodeSet), 1);
  assert(howbig > 0);
  result->da = newDoubleDoubler();
  setValueAt(result->da, howbig-1, zeropct);
  return result;
}

struct CLNodeSet *cloneCLNodeSet(const struct CLNodeSet *cl)
{
  struct CLNodeSet *result = gcalloc(sizeof(struct CLNodeSet), 1);
  result->da = cloneDoubler(cl->da);
  return result;
}

void printCLNodeSet(struct CLNodeSet *cl)
{
  printNodeList(cl->da);
}

void freeCLNodeSet(struct CLNodeSet *cl)
{
  freeDoubleDoubler(cl->da);
  gfreeandclear(cl);
}

void addNodeToSet(struct CLNodeSet *cl, qbase_t which)
{
  setNodeStatusInSet(cl, which, 1);
}

void removeNodeFromSet(struct CLNodeSet *cl, qbase_t which)
{
  setNodeStatusInSet(cl, which, 0);
}

void setNodeStatusInSet(struct CLNodeSet *cl, qbase_t which, int status)
{
  union PCTypes p = zeropct;

  assert(status == 0 || status == 1);

  p.i = status;

  setValueAt(cl->da, which, p);
}

int isNodeInSet(const struct CLNodeSet *cl, qbase_t which)
{
  return getValueAt(cl->da, which).i;
}

struct DoubleA *CLNodeSetToDoubleA(const struct CLNodeSet *cl)
{
  return cloneDoubler(cl->da);
}

struct CLNodeSet *DoubleAToCLNodeSet(const struct DoubleA *da)
{
  struct CLNodeSet *result = gcalloc(sizeof(struct CLNodeSet), 1);
  result->da = cloneDoubler(da);
  return result;
}

int sizeCLNodeSet(const struct CLNodeSet *cl)
{
   return getSize(cl->da);
}
