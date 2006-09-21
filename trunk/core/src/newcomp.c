#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "newcomp.h"

struct CompressionBaseInternal {
  int fHavePrepared;
  char *shortName;
  char *errorMessage;
  struct CompressionBase *cb;
  struct EnvMap *em;
  struct CompressionBaseAdaptor *vptr;
};

#define VFI(xcbi,xfunc) (*(xcbi->vptr->xfunc))
#define VF(xcb, xfunc) VFI(xcb->cbi, xfunc)

struct CLCompressionInfo {
  int allocSize;
  const char *shortName;
  struct CompressionBaseAdaptor cba;
  struct CLCompressionInfo *next;
};

static struct CLCompressionInfo *clciHead = NULL;
static struct CLCompressionInfo **clciHeadPtr = &clciHead;

static struct CLCompressionInfo *findCompressorInfo(const char *name);
static struct CLCompressionInfo **findPointerTo(struct CLCompressionInfo *t);

void deregisterCompressorCL(const char *shortName)
{
  struct CLCompressionInfo *ci = findCompressorInfo(shortName);
  assert(ci != NULL);
  struct CLCompressionInfo **pc = findPointerTo(ci);
  *pc = ci->next;
  free(ci); // shortName must be owned by Compressor instance
}

static void addToCLCIList(struct CLCompressionInfo *t)
{
  t->next = clciHead;
  clciHeadPtr = &t->next;
  clciHead = t;
}

static void checkPrepared(struct CompressionBase *cb)
{
  if (cb->cbi->fHavePrepared == 0) {
    VF(cb, prepareToCompressCB)(cb);
    cb->cbi->fHavePrepared = 1;
  }
}

static struct CLCompressionInfo **findPointerTo(struct CLCompressionInfo *t)
{
  struct CLCompressionInfo *c = clciHead;
  struct CLCompressionInfo **pc = clciHeadPtr;
  while (c != NULL) {
    if (c->next == t)
      return pc;
    pc = &(c->next);
    c = c->next;
  }
  return NULL;
}

static struct CLCompressionInfo *findCompressorInfo(const char *name)
{
  struct CLCompressionInfo *c;
  for (c = clciHead; c; c = c->next) {
    if (strcmp(name, c->shortName) == 0)
      return c;
  }
  return NULL;
}

static double fcompressCB(struct CompressionBase *cb, struct DataBlock *db)
{
  printf("(no specific compression function given)\n");
  return clDatablockSize(db) * 8.0;
}

static int fspecificInitCB(struct CompressionBase *cb)
{
  printf("(no specific initialization function given)\n");
}

struct CompressionBaseAdaptor cbsuper = {
  specificInitCB : fspecificInitCB,
  compressCB : fcompressCB,
};

void **dvptr = (void **) &cbsuper;
struct CompressionBase *clNewCompressorCB(const char *shortName)
{
  struct CLCompressionInfo *ci;
  ci = findCompressorInfo(shortName);
  assert(ci != NULL);
  struct CompressionBase *cb = calloc(ci->allocSize, 1);
  struct CompressionBaseInternal *cbi = calloc(sizeof(struct CompressionBaseInternal), 1);
  cbi->cb = cb;
  cbi->vptr = &ci->cba;
  cb->cbi = cbi;
  cbi->em = clEnvmapNew();
  VF(cb, specificInitCB)(cb);
  return cb;
}

int clSetParameterCB(struct CompressionBase *cb, const char *key, const char *val, int isPrivate)
{
  clEnvmapSetKeyVal(cb->cbi->em, key, val);
  if (isPrivate)
    clEnvmapSetKeyPrivate(cb->cbi->em, key);
}

void clRegisterCB(const char *shortName, int allocSize, struct CompressionBaseAdaptor *vptr)
{
  struct CLCompressionInfo *ci = calloc(sizeof(struct CLCompressionInfo), 1);
  int cbas = sizeof(struct CompressionBaseAdaptor);
  int ps = sizeof(void *);
  assert(findCompressorInfo(shortName) == NULL);
  void **vpn = (void **) &ci->cba;
  void **svptr = (void **) vptr;
  int i;
  for (i = 0; i < (cbas/ps); i += 1) {
    void *f = svptr[i];
    if (f == NULL)
      f = dvptr[i];
    vpn[i] = f;
  }
  ci->shortName = strdup(shortName);
  ci->allocSize = allocSize;
  addToCLCIList(ci);
}

double clCompressCB(struct CompressionBase *cb, struct DataBlock *db)
{
  checkPrepared(cb);
  return VF(cb, compressCB)(cb, db);
}

struct DataBlock *clConcatCB(struct CompressionBase *cb, struct DataBlock *db1,
                            struct DataBlock *db2)
{
  checkPrepared(cb);
  return VF(cb, concatCB)(cb, db1, db2);
}

struct EnvMap *clGetParameters(struct CompressionBase *cb)
{
  return cb->cbi->em;
}
