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

struct EnvMap *loadEnvMap(struct DataBlock db, int fmustbe)
{
  struct EnvMap *result = newEnvMap();
  struct DataBlock cur;
  struct TagManager *tm;
  struct StringStack *keyparts;
  struct StringStack *valparts;
  struct tagHdr *h = (struct tagHdr *) db.ptr;
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
  result->marked = newCLNodeSet(MAXINDECES);
  result->private = newCLNodeSet(MAXINDECES);

  tm = newTagManager(db);

  getCurDataBlock(tm, &cur);
  keyparts = loadStringStack(cur, 1);
  stepNextDataBlock(tm);
  getCurDataBlock(tm, &cur);
  valparts = loadStringStack(cur, 1);
  
  assert (sizeSS(keyparts)== sizeSS(valparts));
  for (i = 0; i < sizeSS(keyparts) ; i += 1)
    setKeyValEM(result, readAtSS(keyparts,i), readAtSS(valparts,i));

  for (i = 0; i < getSize(result->d); i += 1)
    addNodeToSet(result->marked, i);

  freeTagManager(tm);
  freeSS(keyparts); freeSS(valparts);
  return result;
}

struct DataBlock dumpEnvMap(struct EnvMap *em)
{
  struct DataBlock result;
  struct DataBlock keys, vals;
  struct StringStack *keyparts = newStringStack();
  struct StringStack *valparts = newStringStack();
  int i;

  for (i = 0; i < sizeEM(em) ; i += 1) {
    pushSS(keyparts, getValueAt(em->d,i).sp.key);
    pushSS(valparts, getValueAt(em->d,i).sp.val);
  }
  keys = dumpStringStack(keyparts);
  vals = dumpStringStack(valparts);

  result = package_DataBlocks(TAGNUM_ENVMAP, &keys,&vals, NULL);

  freeSS(keyparts); freeSS(valparts);
  freeDataBlock(keys); freeDataBlock(vals);

  return result;
}
#define MAXINDECES 10
struct EnvMap *newEnvMap() {
  struct EnvMap *em;
  em = gcalloc(sizeof(struct EnvMap), 1);
  em->d = newDoubleDoubler();
  em->marked = newCLNodeSet(MAXINDECES);
  em->private = newCLNodeSet(MAXINDECES);
  return em;
}

void printEM(struct EnvMap *uem)
{
  struct EnvMap *em = cloneEM(uem);
  int i;
  printf("ES:\n");
  for (i = 0; i < getSize(em->d); ++i)
    printf("%s->%s\n", getValueAt(em->d,i).sp.key, getValueAt(em->d,i).sp.val);
  printf("Marked: ");
  printCLNodeSet(em->marked);
  printf("Private: ");
  printCLNodeSet(em->private);
  printf("ES END.\n");
  freeEM(em);
}

static union pctypes clonestrpair(struct strpair sp)
{
  union pctypes p;
  p.sp.key = gstrdup(sp.key);
  p.sp.val = gstrdup(sp.val);
  return p;
}

struct EnvMap *cloneEM(struct EnvMap *em)
{
  struct EnvMap *nem;
  int i;
  int sz = sizeEM(em);
  nem = gcalloc(sizeof(struct EnvMap), 1);
  nem->d = newDoubleDoubler();
  for (i = 0; i < sz; ++i)
    setValueAt(nem->d, i, clonestrpair(getValueAt(em->d, i).sp));
  nem->marked = cloneCLNodeSet(em->marked);
  nem->private = cloneCLNodeSet(em->private);
  return nem;
}

int isEmptyEM(struct EnvMap *em)
{
  return getSize(em->d) == 0;
}

int sizeEM(struct EnvMap *em)
{
  return getSize(em->d);
}

static int setKeyValAt(struct EnvMap *em, int where, char *key, char *val)
{
  union pctypes p;
  p.sp.key = gstrdup(key);
  p.sp.val = gstrdup(val);
  setValueAt(em->d, where, p);
  return CL_OK;
}

void setKeyMarkedEM(struct EnvMap *em, const char *key)
{
  addNodeToSet(em->marked, findIndexForKey(em, key));
}

int setKeyValEM(struct EnvMap *em, char *key, char *val)
{
  int i = findIndexForKey(em,key);
  if (i >= 0)
    setKeyValAt(em, i, key, val);
  else {
    union pctypes p;
    p.sp.key = gstrdup(key);
    p.sp.val = gstrdup(val);
    pushValue(em->d, p);
  }
  /* to ensure NodeSets are big enough */
  removeNodeFromSet(em->marked, sizeEM(em)+1);
  removeNodeFromSet(em->private, sizeEM(em)+1);
  return CL_OK;
}

int freeEM(struct EnvMap *em)
{
  int i;
  int sz = sizeEM(em);
  static union pctypes zeroblock;

  for (i = 0; i < sz; ++i) {
    union pctypes p = getValueAt(em->d, i);
    gfreeandclear(p.sp.key);
    gfreeandclear(p.sp.val);
    setValueAt(em->d, i, zeroblock);
  }
  freeDoubleDoubler(em->d);
  em->d = NULL;
  freeCLNodeSet(em->marked);
  em->marked = NULL;
  freeCLNodeSet(em->private);
  em->private = NULL;
  gfreeandclear(em);
  return CL_OK;
}

char *readValForEM(struct EnvMap *em, const char *key)
{
  int i;
  char *val = NULL;
  i = findIndexForKey(em,key);
  if (i >= 0) {
    union pctypes p = getKeyValAt(em,findIndexForKey(em,key));
    val = p.sp.val;
    setKeyMarkedEM(em, key);
  }
  return val;
}

union pctypes getKeyValAt(struct EnvMap *em, int where)
{
  assert(where >= 0);
  return getValueAt(em->d, where);
}

int findIndexForKey(struct EnvMap *em, const char *key)
{
  int i;
  for (i = 0; i < sizeEM(em); i += 1) {
    union pctypes p = getKeyValAt(em,i);
    if (strcmp(p.sp.key, key) == 0) {
      return i;
    }
  }
  return -1;
}

void setKeyPrivateEM(struct EnvMap *em, const char *key)
{
  addNodeToSet(em->private, findIndexForKey(em,key));
}

int isMarkedAtEM(struct EnvMap *em, int where)
{
  return isNodeInSet(em->marked, where);
}

int isPrivateAtEM(struct EnvMap *em, int where)
{
  return isNodeInSet(em->private, where);
}

struct EnvMap *get_clem_from_clb(char *fname)
{
  struct DataBlock db, dbem;
  struct DoubleA *dd;
  struct EnvMap *result = NULL;

  db = convertFileToDataBlock(fname);
  dd = load_DataBlock_package(db);
  dbem = scanForTag(dd, TAGNUM_ENVMAP);
  result = loadEnvMap(dbem, 1);
  freeDataBlock(dbem);

  freeDataBlock(db);
  freeDoubleDoubler(dd);
  return result;
}

int mergeEM(struct EnvMap *dest, struct EnvMap *src)
{
  union pctypes p;
  int i;
  for (i = 0; i < sizeEM(src); i += 1) {
    p = getKeyValAt(src,i);
    setKeyValEM(dest, p.sp.key, p.sp.val);
    if (isNodeInSet(src->marked, i))
      setKeyMarkedEM(dest, p.sp.key);
  }
  return CL_OK;
}
