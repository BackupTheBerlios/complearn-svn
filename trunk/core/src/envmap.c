#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "clalloc.h"

#include <complearn/complearn.h>
#define MAXINDECES 10

struct EnvMap {
  struct DRA *d;
  struct CLNodeSet *marked;
  struct CLNodeSet *private;
};

struct EnvMap *envmapLoad(struct DataBlock *db, int fmustbe)
{
  struct EnvMap *result = envmapNew();
  struct DataBlock *cur = NULL;
  struct TagManager *tm;
  struct StringStack *keyparts;
  struct StringStack *valparts;
  struct TagHdr *h = (struct TagHdr *) datablockData(db);
  int i;

  if (db == NULL) {
    clogError("NULL ptr in envmapLoad()\n");
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

  tm = newTagManager(db);

  cur = getCurDataBlock(tm);
  keyparts = stringstackLoad(cur, 1);
  stepNextDataBlock(tm);
  datablockFreePtr(cur);
  cur = getCurDataBlock(tm);
  valparts = stringstackLoad(cur, 1);
  datablockFreePtr(cur);

  assert (stringstackSize(keyparts)== stringstackSize(valparts));
  for (i = 0; i < stringstackSize(keyparts) ; i += 1)
    envmapSetKeyVal(result, stringstackReadAt(keyparts,i), stringstackReadAt(valparts,i));

  for (i = 0; i < draSize(result->d); i += 1)
    clnodesetAddNode(result->marked, i);

  freeTagManager(tm);
  stringstackFree(keyparts); stringstackFree(valparts);
  return result;
}

struct DataBlock *envmapDump(struct EnvMap *em)
{
  struct DataBlock *rr;
  struct DataBlock *keys, *vals;
  struct StringStack *keyparts = stringstackNew();
  struct StringStack *valparts = stringstackNew();
  int i;

  if (em == NULL) {
    clogError("NULL ptr in envmapDump()\n");
  }

  for (i = 0; i < envmapSize(em) ; i += 1) {
    stringstackPush(keyparts, draGetValueAt(em->d,i).sp.key);
    stringstackPush(valparts, draGetValueAt(em->d,i).sp.val);
  }
  keys = stringstackDump(keyparts);
  vals = stringstackDump(valparts);

  rr = package_DataBlocks(TAGNUM_ENVMAP, keys,vals, NULL);

  stringstackFree(keyparts); stringstackFree(valparts);
  datablockFreePtr(keys); datablockFreePtr(vals);

  return rr;
}
#define MAXINDECES 10
struct EnvMap *envmapNew() {
  struct EnvMap *em;
  em = clCalloc(sizeof(struct EnvMap), 1);
  em->d = draNew();
  em->marked = clnodesetNew(MAXINDECES);
  em->private = clnodesetNew(MAXINDECES);
  return em;
}

void envmapPrint(struct EnvMap *uem)
{
  struct EnvMap *em;
  int i;
  if (uem == NULL) {
    clogError("NULL ptr in envmapPrint()\n");
  }
  em = envmapClone(uem);
  printf("ES:\n");
  for (i = 0; i < draSize(em->d); ++i)
    printf("%s->%s\n", draGetValueAt(em->d,i).sp.key, draGetValueAt(em->d,i).sp.val);
  printf("Marked: ");
  clnodesetPrint(em->marked);
  printf("Private: ");
  clnodesetPrint(em->private);
  printf("ES END.\n");
  envmapFree(em);
}

static union PCTypes cloneStringPair(struct StringPair sp)
{
  union PCTypes p;
  p.sp.key = clStrdup(sp.key);
  p.sp.val = clStrdup(sp.val);
  return p;
}

struct EnvMap *envmapClone(struct EnvMap *em)
{
  struct EnvMap *nem;
  int i;
  int sz;
  if (em == NULL) {
    clogError("NULL ptr in envmapClone()\n");
  }
  sz = envmapSize(em);
  nem = clCalloc(sizeof(struct EnvMap), 1);
  nem->d = draNew();
  for (i = 0; i < sz; ++i)
    draSetValueAt(nem->d, i, cloneStringPair(draGetValueAt(em->d, i).sp));
  nem->marked = clnodesetClone(em->marked);
  nem->private = clnodesetClone(em->private);
  return nem;
}

int envmapIsEmpty(struct EnvMap *em)
{
  if (em == NULL) {
    clogError("NULL ptr in envmapIsEmpty()\n");
  }
  return draSize(em->d) == 0;
}

int envmapSize(struct EnvMap *em)
{
  if (em == NULL) {
    clogError("NULL ptr in envmapSize()\n");
  }
  return draSize(em->d);
}

static int setKeyValAt(struct EnvMap *em, int where, char *key, char *val)
{
  union PCTypes p;
  if (em == NULL || key == NULL || val == NULL) {
    clogError("NULL ptr in envmapSize()\n");
  }
  p.sp.key = clStrdup(key);
  p.sp.val = clStrdup(val);
  draSetValueAt(em->d, where, p);
  return CL_OK;
}

void envmapSetKeyMarked(struct EnvMap *em, const char *key)
{
  if (em == NULL || key == NULL) {
    clogError("NULL ptr in envmapSetKeyMarked()\n");
  }
  clnodesetAddNode(em->marked, envmapIndexForKey(em, key));
}

int envmapSetKeyVal(struct EnvMap *em, char *key, char *val)
{
  int i;
  if (em == NULL || key == NULL || val == NULL) {
    clogError("NULL ptr in envmapSetKeyVal()\n");
  }
  i = envmapIndexForKey(em,key);
  if (i >= 0)
    setKeyValAt(em, i, key, val);
  else {
    union PCTypes p;
    p.sp.key = clStrdup(key);
    p.sp.val = clStrdup(val);
    draPush(em->d, p);
  }
  /* to ensure NodeSets are big enough */
  clnodesetRemoveNode(em->marked, envmapSize(em)+1);
  clnodesetRemoveNode(em->private, envmapSize(em)+1);
  return CL_OK;
}

int envmapFree(struct EnvMap *em)
{
  int i, sz;
  static union PCTypes zeroblock;

  if (em == NULL) {
    clogError("NULL ptr in envmapFree()\n");
  }
  sz = envmapSize(em);

  for (i = 0; i < sz; ++i) {
    union PCTypes p = draGetValueAt(em->d, i);
    clFreeandclear(p.sp.key);
    clFreeandclear(p.sp.val);
    draSetValueAt(em->d, i, zeroblock);
  }
  draFree(em->d);
  em->d = NULL;
  clnodesetFree(em->marked);
  em->marked = NULL;
  clnodesetFree(em->private);
  em->private = NULL;
  clFreeandclear(em);
  return CL_OK;
}

char *envmapValueForKey(struct EnvMap *em, const char *key)
{
  int i;
  char *val = NULL;
  if (em == NULL || key == NULL) {
    clogError("NULL ptr in envmapValueForKey()\n");
  }
  i = envmapIndexForKey(em,key);
  if (i >= 0) {
    union PCTypes p = envmapKeyValAt(em,envmapIndexForKey(em,key));
    val = p.sp.val;
    envmapSetKeyMarked(em, key);
  }
  return val;
}

union PCTypes envmapKeyValAt(struct EnvMap *em, int where)
{
  if (em == NULL) {
    clogError("NULL ptr in envmapKeyValAt()\n");
  }
  assert(where >= 0);
  return draGetValueAt(em->d, where);
}

int envmapIndexForKey(struct EnvMap *em, const char *key)
{
  int i;
  if (em == NULL || key == NULL) {
    clogError("NULL ptr in envmapIndexForKey()\n");
  }
  for (i = 0; i < envmapSize(em); i += 1) {
    union PCTypes p = envmapKeyValAt(em,i);
    if (strcmp(p.sp.key, key) == 0) {
      return i;
    }
  }
  return -1;
}

void envmapSetKeyPrivate(struct EnvMap *em, const char *key)
{
  if (em == NULL || key == NULL) {
    clogError("NULL ptr in envmapSetKeyPrivate()\n");
  }
  clnodesetAddNode(em->private, envmapIndexForKey(em,key));
}

int envmapIsMarkedAt(struct EnvMap *em, int where)
{
  if (em == NULL) {
    clogError("NULL ptr in envmapIsMarkedAt()\n");
  }
  return clnodesetHasNode(em->marked, where);
}

int envmapIsPrivateAt(struct EnvMap *em, int where)
{
  if (em == NULL) {
    clogError("NULL ptr in envmapIsPrivateAt()\n");
  }
  return clnodesetHasNode(em->private, where);
}

struct EnvMap *clbEnvMap(char *fname)
{
  struct DataBlock *db, *dbem;
  struct DRA *dd;
  struct EnvMap *result = NULL;

  db = fileToDataBlockPtr(fname);
  dd = load_DataBlock_package(db);
  dbem = scanForTag(dd, TAGNUM_ENVMAP);
  result = envmapLoad(dbem, 1);
  datablockFreePtr(dbem);

  datablockFreePtr(db);
  draFree(dd);
  return result;
}

int envmapMerge(struct EnvMap *dest, struct EnvMap *src)
{
  union PCTypes p;
  int i;
  if (src == NULL) {
    clogError("NULL ptr in envmapMerge()\n");
  }
  for (i = 0; i < envmapSize(src); i += 1) {
    p = envmapKeyValAt(src,i);
    envmapSetKeyVal(dest, p.sp.key, p.sp.val);
    if (clnodesetHasNode(src->marked, i))
      envmapSetKeyMarked(dest, p.sp.key);
  }
  return CL_OK;
}
