#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <malloc.h>

#include <complearn/complearn.h>
#define MAXINDECES 10

struct EnvMap {
  struct DoubleA *d;
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

  if (h->tagnum != TAGNUM_ENVMAP) {
    if (fmustbe) {
      fprintf(stderr,"Error: expecting ENVMAP tagnum %x, got %x\n",
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

  for (i = 0; i < doubleaSize(result->d); i += 1)
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

  for (i = 0; i < envmapSize(em) ; i += 1) {
    stringstackPush(keyparts, doubleaGetValueAt(em->d,i).sp.key);
    stringstackPush(valparts, doubleaGetValueAt(em->d,i).sp.val);
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
  em->d = doubleaNew();
  em->marked = clnodesetNew(MAXINDECES);
  em->private = clnodesetNew(MAXINDECES);
  return em;
}

void envmapPrint(struct EnvMap *uem)
{
  struct EnvMap *em = envmapClone(uem);
  int i;
  printf("ES:\n");
  for (i = 0; i < doubleaSize(em->d); ++i)
    printf("%s->%s\n", doubleaGetValueAt(em->d,i).sp.key, doubleaGetValueAt(em->d,i).sp.val);
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
  int sz = envmapSize(em);
  nem = clCalloc(sizeof(struct EnvMap), 1);
  nem->d = doubleaNew();
  for (i = 0; i < sz; ++i)
    doubleaSetValueAt(nem->d, i, cloneStringPair(doubleaGetValueAt(em->d, i).sp));
  nem->marked = clnodesetClone(em->marked);
  nem->private = clnodesetClone(em->private);
  return nem;
}

int envmapIsEmpty(struct EnvMap *em)
{
  return doubleaSize(em->d) == 0;
}

int envmapSize(struct EnvMap *em)
{
  return doubleaSize(em->d);
}

static int setKeyValAt(struct EnvMap *em, int where, char *key, char *val)
{
  union PCTypes p;
  p.sp.key = clStrdup(key);
  p.sp.val = clStrdup(val);
  doubleaSetValueAt(em->d, where, p);
  return CL_OK;
}

void envmapSetKeyMarked(struct EnvMap *em, const char *key)
{
  clnodesetAddNode(em->marked, envmapIndexForKey(em, key));
}

int envmapSetKeyVal(struct EnvMap *em, char *key, char *val)
{
  int i = envmapIndexForKey(em,key);
  if (i >= 0)
    setKeyValAt(em, i, key, val);
  else {
    union PCTypes p;
    p.sp.key = clStrdup(key);
    p.sp.val = clStrdup(val);
    doubleaPush(em->d, p);
  }
  /* to ensure NodeSets are big enough */
  clnodesetRemoveNode(em->marked, envmapSize(em)+1);
  clnodesetRemoveNode(em->private, envmapSize(em)+1);
  return CL_OK;
}

int envmapFree(struct EnvMap *em)
{
  int i;
  int sz = envmapSize(em);
  static union PCTypes zeroblock;

  for (i = 0; i < sz; ++i) {
    union PCTypes p = doubleaGetValueAt(em->d, i);
    clFreeandclear(p.sp.key);
    clFreeandclear(p.sp.val);
    doubleaSetValueAt(em->d, i, zeroblock);
  }
  doubleaFree(em->d);
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
  assert(where >= 0);
  return doubleaGetValueAt(em->d, where);
}

int envmapIndexForKey(struct EnvMap *em, const char *key)
{
  int i;
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
  clnodesetAddNode(em->private, envmapIndexForKey(em,key));
}

int envmapIsMarkedAt(struct EnvMap *em, int where)
{
  return clnodesetHasNode(em->marked, where);
}

int envmapIsPrivateAt(struct EnvMap *em, int where)
{
  return clnodesetHasNode(em->private, where);
}

struct EnvMap *clbEnvMap(char *fname)
{
  struct DataBlock *db, *dbem;
  struct DoubleA *dd;
  struct EnvMap *result = NULL;

  db = fileToDataBlockPtr(fname);
  dd = load_DataBlock_package(db);
  dbem = scanForTag(dd, TAGNUM_ENVMAP);
  result = envmapLoad(dbem, 1);
  datablockFreePtr(dbem);

  datablockFreePtr(db);
  doubleaFree(dd);
  return result;
}

int envmapMerge(struct EnvMap *dest, struct EnvMap *src)
{
  union PCTypes p;
  int i;
  for (i = 0; i < envmapSize(src); i += 1) {
    p = envmapKeyValAt(src,i);
    envmapSetKeyVal(dest, p.sp.key, p.sp.val);
    if (clnodesetHasNode(src->marked, i))
      envmapSetKeyMarked(dest, p.sp.key);
  }
  return CL_OK;
}
