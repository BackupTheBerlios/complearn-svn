#include <stdio.h>
#include <assert.h>
#include <string.h>


#include <complearn/complearn.h>
#define MAXINDECES 10

struct EnvMap {
  struct DRA *d;
  struct CLNodeSet *marked;
  struct CLNodeSet *private;
};

struct EnvMap *clEnvmapLoad(struct DataBlock *db, int fmustbe)
{
  struct EnvMap *result = clEnvmapNew();
  struct DataBlock *cur = NULL;
  struct TagManager *tm;
  struct StringStack *keyparts;
  struct StringStack *valparts;
  struct TagHdr *h = (struct TagHdr *) clDatablockData(db);
  int i;

  if (db == NULL) {
    clogError("NULL ptr in clEnvmapLoad()\n");
  }

  if (h->tagnum != TAGNUM_ENVMAP) {
    if (fmustbe) {
      clogError("Error: expecting ENVMAP tagnum %x, got %x\n",
          TAGNUM_ENVMAP,h->tagnum);
      exit(1);
    }
    else
      return NULL;
  }
  result->marked = clnodesetNew(MAXINDECES);
  result->private = clnodesetNew(MAXINDECES);

  tm = clNewTagManager(db);

  cur = clGetCurDataBlock(tm);
  keyparts = clStringstackLoad(cur, 1);
  clStepNextDataBlock(tm);
  clDatablockFreePtr(cur);
  cur = clGetCurDataBlock(tm);
  valparts = clStringstackLoad(cur, 1);
  clDatablockFreePtr(cur);

  assert (clStringstackSize(keyparts)== clStringstackSize(valparts));
  for (i = 0; i < clStringstackSize(keyparts) ; i += 1)
    clEnvmapSetKeyVal(result, clStringstackReadAt(keyparts,i), clStringstackReadAt(valparts,i));

  for (i = 0; i < clDraSize(result->d); i += 1)
    clnodesetAddNode(result->marked, i);

  clFreeTagManager(tm);
  clStringstackFree(keyparts); clStringstackFree(valparts);
  return result;
}

struct DataBlock *clEnvmapDump(struct EnvMap *em)
{
  struct DataBlock *rr;
  struct DataBlock *keys, *vals;
  struct StringStack *keyparts = clStringstackNew();
  struct StringStack *valparts = clStringstackNew();
  int i;

  if (em == NULL) {
    clogError("NULL ptr in clEnvmapDump()\n");
  }

  for (i = 0; i < clEnvmapSize(em) ; i += 1) {
    clStringstackPush(keyparts, clDraGetValueAt(em->d,i).sp.key);
    clStringstackPush(valparts, clDraGetValueAt(em->d,i).sp.val);
  }
  keys = clStringstackDump(keyparts);
  vals = clStringstackDump(valparts);

  rr = clPackage_DataBlocks(TAGNUM_ENVMAP, keys,vals, NULL);

  clStringstackFree(keyparts); clStringstackFree(valparts);
  clDatablockFreePtr(keys); clDatablockFreePtr(vals);

  return rr;
}
#define MAXINDECES 10
struct EnvMap *clEnvmapNew() {
  struct EnvMap *em;
  em = clCalloc(sizeof(struct EnvMap), 1);
  em->d = clDraNew();
  em->marked = clnodesetNew(MAXINDECES);
  em->private = clnodesetNew(MAXINDECES);
  return em;
}

void clEnvmapPrint(struct EnvMap *uem)
{
  struct EnvMap *em;
  int i;
  if (uem == NULL) {
    clogError("NULL ptr in clEnvmapPrint()\n");
  }
  em = clEnvmapClone(uem);
  printf("ES:\n");
  for (i = 0; i < clDraSize(em->d); ++i)
    printf("%s->%s\n", clDraGetValueAt(em->d,i).sp.key, clDraGetValueAt(em->d,i).sp.val);
  printf("Marked: ");
  clnodesetPrint(em->marked);
  printf("Private: ");
  clnodesetPrint(em->private);
  printf("ES END.\n");
  clEnvmapFree(em);
}

static union PCTypes cloneStringPair(struct StringPair sp)
{
  union PCTypes p;
  p.sp.key = clStrdup(sp.key);
  p.sp.val = clStrdup(sp.val);
  return p;
}

struct EnvMap *clEnvmapClone(struct EnvMap *em)
{
  struct EnvMap *nem;
  int i;
  int sz;
  if (em == NULL) {
    clogError("NULL ptr in clEnvmapClone()\n");
  }
  sz = clEnvmapSize(em);
  nem = clCalloc(sizeof(struct EnvMap), 1);
  nem->d = clDraNew();
  for (i = 0; i < sz; ++i)
    clDraSetValueAt(nem->d, i, cloneStringPair(clDraGetValueAt(em->d, i).sp));
  nem->marked = clnodesetClone(em->marked);
  nem->private = clnodesetClone(em->private);
  return nem;
}

int clEnvmapIsEmpty(struct EnvMap *em)
{
  if (em == NULL) {
    clogError("NULL ptr in clEnvmapIsEmpty()\n");
  }
  return clDraSize(em->d) == 0;
}

int clEnvmapSize(struct EnvMap *em)
{
  if (em == NULL) {
    clogError("NULL ptr in clEnvmapSize()\n");
  }
  return clDraSize(em->d);
}

static int setKeyValAt(struct EnvMap *em, int where, char *key, char *val)
{
  union PCTypes p;
  if (em == NULL || key == NULL || val == NULL) {
    clogError("NULL ptr in clEnvmapSize()\n");
  }
  p.sp.key = clStrdup(key);
  p.sp.val = clStrdup(val);
  clDraSetValueAt(em->d, where, p);
  return CL_OK;
}

void clEnvmapSetKeyMarked(struct EnvMap *em, const char *key)
{
  if (em == NULL || key == NULL) {
    clogError("NULL ptr in clEnvmapSetKeyMarked()\n");
  }
  clnodesetAddNode(em->marked, clEnvmapIndexForKey(em, key));
}

int clEnvmapSetKeyVal(struct EnvMap *em, char *key, char *val)
{
  int i;
  if (em == NULL || key == NULL || val == NULL) {
    clogError("NULL ptr in clEnvmapSetKeyVal()\n");
  }
  i = clEnvmapIndexForKey(em,key);
  if (i >= 0)
    setKeyValAt(em, i, key, val);
  else {
    union PCTypes p;
    p.sp.key = clStrdup(key);
    p.sp.val = clStrdup(val);
    clDraPush(em->d, p);
  }
  /* to ensure NodeSets are big enough */
  clnodesetRemoveNode(em->marked, clEnvmapSize(em)+1);
  clnodesetRemoveNode(em->private, clEnvmapSize(em)+1);
  return CL_OK;
}

int clEnvmapFree(struct EnvMap *em)
{
  int i, sz;
  static union PCTypes zeroblock;

  if (em == NULL) {
    clogError("NULL ptr in clEnvmapFree()\n");
  }
  sz = clEnvmapSize(em);

  for (i = 0; i < sz; ++i) {
    union PCTypes p = clDraGetValueAt(em->d, i);
    clFreeandclear(p.sp.key);
    clFreeandclear(p.sp.val);
    clDraSetValueAt(em->d, i, zeroblock);
  }
  clDraFree(em->d);
  em->d = NULL;
  clnodesetFree(em->marked);
  em->marked = NULL;
  clnodesetFree(em->private);
  em->private = NULL;
  clFreeandclear(em);
  return CL_OK;
}

char *clEnvmapValueForKey(struct EnvMap *em, const char *key)
{
  int i;
  char *val = NULL;
  if (em == NULL || key == NULL) {
    clogError("NULL ptr in clEnvmapValueForKey()\n");
  }
  i = clEnvmapIndexForKey(em,key);
  if (i >= 0) {
    union PCTypes p = clEnvmapKeyValAt(em,clEnvmapIndexForKey(em,key));
    val = p.sp.val;
    clEnvmapSetKeyMarked(em, key);
  }
  return val;
}

union PCTypes clEnvmapKeyValAt(struct EnvMap *em, int where)
{
  if (em == NULL) {
    clogError("NULL ptr in clEnvmapKeyValAt()\n");
  }
  assert(where >= 0);
  return clDraGetValueAt(em->d, where);
}

int clEnvmapIndexForKey(struct EnvMap *em, const char *key)
{
  int i;
  if (em == NULL || key == NULL) {
    clogError("NULL ptr in clEnvmapIndexForKey()\n");
  }
  for (i = 0; i < clEnvmapSize(em); i += 1) {
    union PCTypes p = clEnvmapKeyValAt(em,i);
    if (strcmp(p.sp.key, key) == 0) {
      return i;
    }
  }
  return -1;
}

void clEnvmapSetKeyPrivate(struct EnvMap *em, const char *key)
{
  if (em == NULL || key == NULL) {
    clogError("NULL ptr in clEnvmapSetKeyPrivate()\n");
  }
  clnodesetAddNode(em->private, clEnvmapIndexForKey(em,key));
}

int clEnvmapIsMarkedAt(struct EnvMap *em, int where)
{
  if (em == NULL) {
    clogError("NULL ptr in clEnvmapIsMarkedAt()\n");
  }
  return clnodesetHasNode(em->marked, where);
}

int clEnvmapIsPrivateAt(struct EnvMap *em, int where)
{
  if (em == NULL) {
    clogError("NULL ptr in clEnvmapIsPrivateAt()\n");
  }
  return clnodesetHasNode(em->private, where);
}

struct EnvMap *clbEnvMap(char *fname)
{
  struct DataBlock *db, *dbem;
  struct DRA *dd;
  struct EnvMap *result = NULL;

  db = clFileToDataBlockPtr(fname);
  dd = clLoad_DataBlock_package(db);
  dbem = clScanForTag(dd, TAGNUM_ENVMAP);
  result = clEnvmapLoad(dbem, 1);
  clDatablockFreePtr(dbem);

  clDatablockFreePtr(db);
  clFree_DataBlock_package(dd);
  return result;
}

int clEnvmapMerge(struct EnvMap *dest, struct EnvMap *src)
{
  union PCTypes p;
  int i;
  if (src == NULL) {
    clogError("NULL ptr in clEnvmapMerge()\n");
  }
  for (i = 0; i < clEnvmapSize(src); i += 1) {
    p = clEnvmapKeyValAt(src,i);
    clEnvmapSetKeyVal(dest, p.sp.key, p.sp.val);
    if (clnodesetHasNode(src->marked, i))
      clEnvmapSetKeyMarked(dest, p.sp.key);
  }
  return CL_OK;
}
