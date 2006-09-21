#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "newcomp.h"
#include <complearn/complearn.h>

struct CompressionBaseInternal {
  int fHavePrepared;
  char *shortName;
  char *madeupLongname;
  char *madeupToString;
  char *madeupParamString;
  char *errorMessage;
  struct CompressionBase *cb;
  struct EnvMap *em;
  struct CompressionBaseAdaptor *vptr;
};

#define VFI(xcbi,xfunc) (*(xcbi->vptr->xfunc))
#define VF(xcb, xfunc) VFI(xcb->cbi, xfunc)

struct CLCompressionInfo {
  struct CompressionBaseAdaptor cba;
  struct CLCompressionInfo *next;
  char *staticErrorMsg;
};

static struct CLCompressionInfo *clciHead = NULL;
static struct CLCompressionInfo **clciHeadPtr = &clciHead;

static struct CLCompressionInfo *findCompressorInfo(const char *name);
static struct CLCompressionInfo **findPointerTo(struct CLCompressionInfo *t);

const char *clLastStaticErrorCB(const char *shortName)
{
  struct CLCompressionInfo *ci = findCompressorInfo(shortName);
  assert(ci != NULL);
  return ci->staticErrorMsg;
}

static void setStaticErrorMessage(const char *shortName, const char *msg)
{
  struct CLCompressionInfo *ci = findCompressorInfo(shortName);
  assert(ci != NULL);
  ci->staticErrorMsg = strdup(msg);
}

void clFreeCB(struct CompressionBase *cb)
{
  VF(cb, freeCB)(cb);
  if (cb->cbi->madeupParamString != NULL) {
    free(cb->cbi->madeupParamString);
    cb->cbi->madeupParamString = NULL;
  }
  if (cb->cbi->madeupToString != NULL) {
    free(cb->cbi->madeupToString);
    cb->cbi->madeupToString = NULL;
  }
  if (cb->cbi->madeupLongname != NULL) {
    free(cb->cbi->madeupLongname);
    cb->cbi->madeupLongname = NULL;
  }
  free(cb->cbi);
  cb->cbi = NULL;
  free(cb);
}

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
    if (strcmp(name, c->cba.shortNameCB()) == 0)
      return c;
  }
  return NULL;
}

// START OF BASE-CLASS FUNCTIONS
// ----------------------------------------------------------
//
// By default, do nothing
//
static int fSpecificInitCB(struct CompressionBase *cb)
{
}

// By default, do nothing.  (the block itself, cb, will automatically be freed)
//
// If there are other pointers however, they should be freed here.
static void ffreeCB(struct CompressionBase *cb)
{
}

static int fgetAPIVersionCB(void)
{
  return APIVER_CLCOMP10;
}

static const char *fgetLongNameCB(struct CompressionBase *cb)
{
  if (cb->cbi->madeupLongname == NULL) {
    cb->cbi->madeupLongname = calloc(strlen(cb->cbi->shortName)+80, 1);
    sprintf(cb->cbi->madeupLongname, "(no description for %s)",
            cb->cbi->shortName);
  }
  return cb->cbi->madeupLongname;
}

static int fisDisabledCB(void)
{
  return 0;
}

static const char *fshortNameCB(void)
{
  fprintf(stderr, "Error, no short name defined for compressor\n");
  exit(0);
  return NULL;
}

static int fallocSize(void)
{
  return sizeof(void *);
}


static const char *ftoStringCB(struct CompressionBase *cb)
{
  if (cb->cbi->madeupToString == NULL) {
    const char *sn = VF(cb, shortNameCB)();
    int sl = strlen(sn);
    cb->cbi->madeupToString = calloc(20 + sl, 1);
    sprintf(cb->cbi->madeupToString, "%s:%08x", sn, cb);
  }
  return cb->cbi->madeupToString;
}

static struct DataBlock *fconcatCB(struct CompressionBase *cb, struct DataBlock *dat1, struct DataBlock *dat2)
{
  return clDatablockCatPtr(dat1, dat2);
}

static const char *fparamStringCB(struct CompressionBase *cb)
{
  if (cb->cbi->madeupParamString == NULL) {
    struct EnvMap *em = cb->cbi->em;
    int i;
    int als = 1;
    char *curs;
    for (i = 0; i < clEnvmapSize(em); i += 1) {
      union PCTypes p;
      if (clEnvmapIsPrivateAt(em, i))
        continue;
      p = clEnvmapKeyValAt(em, i);
      als += 2 + strlen(p.sp.key) + strlen(p.sp.val);
    }
    cb->cbi->madeupParamString = calloc(als, 1);
    curs = cb->cbi->madeupParamString;
    for (i = 0; i < clEnvmapSize(em); i += 1) {
      union PCTypes p;
      if (clEnvmapIsPrivateAt(em, i))
        continue;
      p = clEnvmapKeyValAt(em, i);
      curs += sprintf(curs, "%s=%s,",p.sp.key, p.sp.val);
    }
    curs[-1] = 0;
  }
  return cb->cbi->madeupParamString;
}

static int fgetWindowSizeCB(void)
{
  return 0;
}

static int fdoesRoundWholeBytesCB(void)
{
  return 1;
}

static int fisCompileProblemCB(void)
{
  return 0;
}

static int fisRuntimeProblemCB(void)
{
  return 0;
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
  getAPIVersionCB : fgetAPIVersionCB,
  specificInitCB : fspecificInitCB,
  freeCB : ffreeCB,
  compressCB : fcompressCB,
  specificInitCB : fspecificInitCB,
  getWindowSizeCB : fgetWindowSizeCB,
  doesRoundWholeBytesCB : fdoesRoundWholeBytesCB,
  isCompileProblemCB : fisCompileProblemCB,
  isRuntimeProblemCB : fisRuntimeProblemCB,
  getLongNameCB : fgetLongNameCB,
  shortNameCB : fshortNameCB,
  toStringCB : ftoStringCB,
  paramStringCB : fparamStringCB,
  concatCB : fconcatCB,
  isDisabledCB : fisDisabledCB
};

void **dvptr = (void **) &cbsuper;
struct CompressionBase *clNewCompressorCB(const char *shortName)
{
  struct CLCompressionInfo *ci;
  ci = findCompressorInfo(shortName);
  assert(ci != NULL);
  struct CompressionBase *cb = calloc(ci->cba.allocSizeCB(), 1);
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

void clRegisterCB(struct CompressionBaseAdaptor *vptr)
{
  struct CLCompressionInfo *ci = calloc(sizeof(struct CLCompressionInfo), 1);
  int cbas = sizeof(struct CompressionBaseAdaptor);
  int ps = sizeof(void *);
  assert(vptr != NULL);
  assert(vptr->shortNameCB != NULL);
  const char *shortName = vptr->shortNameCB();
  assert(shortName != NULL);
  assert(strlen(shortName) > 0);
  assert(findCompressorInfo(shortName) == NULL);
  void **vpn = (void **) &ci->cba;
  void **svptr = (void **) vptr;
  int i;
  for (i = 0; i < (cbas/ps); i += 1) {
    void *f = svptr[i];
    if (f == NULL)
      f = dvptr[i];
    if (f == NULL) {
      fprintf(stderr, "Function %d undefined.\n", i);
      exit(1);
    }
    vpn[i] = f;
  }
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

struct EnvMap *clGetParametersCB(struct CompressionBase *cb)
{
  return cb->cbi->em;
}

const char *clLastErrorCB(struct CompressionBase *cb)
{
}

int clIsEnabledCB(const char *shortName)
{
  struct CLCompressionInfo *ci;
  ci = findCompressorInfo(shortName);
  if (ci == NULL)
    return 0;
  if (ci->cba.isDisabledCB())
    return 0;
  if (ci->cba.isCompileProblemCB())
    return 0;
  if (ci->cba.isRuntimeProblemCB())
    return 0;
  return 1;
}

void printCompressors(void)
{
  struct CLCompressionInfo *c;
  for (c = clciHead; c; c = c->next) {
    printf("%10s:%15s: %8d  %10s %s\n", 
      clIsEnabledCB(c->cba.shortNameCB())?"(enabled)" : clLastStaticErrorCB(c->cba.shortNameCB()), c->cba.shortNameCB(),  c->cba.getWindowSizeCB(),
      c->cba.doesRoundWholeBytesCB() ? "(int)" : "(double)");
  }
}

const char *clGetParamStringCB(struct CompressionBase *cb)
{
  checkPrepared(cb);
  return VF(cb, paramStringCB)(cb);
  return "(no parmstring yet)";
}
