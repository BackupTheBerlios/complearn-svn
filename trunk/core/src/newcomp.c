#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "newcomp.h"
#include "ncbase.h"
#include "nccbi.h"
#include <complearn/complearn.h>

struct CLCompressionInfo {
  struct CompressionBaseAdaptor cba;
  struct CLCompressionInfo *next;
  char *staticErrorMsg;
};

static struct CLCompressionInfo *clciHead = NULL;
static struct CLCompressionInfo **clciHeadPtr = &clciHead;

static struct CLCompressionInfo *findCompressorInfo(const char *name);
static struct CLCompressionInfo **findPointerTo(struct CLCompressionInfo *t);
static void staticErrorExitIfBad(int retval, struct CompressionBase *cb);

const char *clLastStaticErrorCB(const char *shortName)
{
  struct CLCompressionInfo *ci = findCompressorInfo(shortName);
  assert(ci != NULL);
  return ci->staticErrorMsg;
}

void clSetStaticErrorMessage(const char *shortName, const char *msg)
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
    int retval;
    retval = VF(cb, prepareToCompressCB)(cb);
    if (retval != 0)
      cb->cbi->fHaveFailed = 1;
    cb->cbi->fHavePrepared = 1;
  }
  staticErrorExitIfBad(cb->cbi->fHaveFailed, cb);
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

void **dvptr = (void **) &cbsuper;
struct CompressionBase *clNewCompressorCB(const char *shortName)
{
  struct CLCompressionInfo *ci;
  ci = findCompressorInfo(shortName);
  assert(ci != NULL);
  if (!clIsEnabledCB(shortName))
    return NULL;
  struct CompressionBase *cb = calloc(ci->cba.allocSizeCB(), 1);
  struct CompressionBaseInternal *cbi = calloc(sizeof(struct CompressionBaseInternal), 1);
  cbi->cb = cb;
  cbi->vptr = &ci->cba;
  cb->cbi = cbi;
  cbi->em = clEnvmapNew();
  int retval;
  retval = VF(cb, specificInitCB)(cb);
  staticErrorExitIfBad(retval, cb);
  return cb;
}
static void staticErrorExitIfBad(int retval, struct CompressionBase *cb)
{
  if (retval != 0) {
    fprintf(stderr, "Error in %s: %s\n", VF(cb,shortNameCB)(), clLastStaticErrorCB(VF(cb,shortNameCB)()));
    exit(1);
  }
}

int clSetParameterCB(struct CompressionBase *cb, const char *key, const char *val, int isPrivate)
{
  assert(cb->cbi->fHavePrepared == 0 && "Parameters already locked.");
  clEnvmapSetKeyVal(cb->cbi->em, key, val);
  if (isPrivate)
    clEnvmapSetKeyPrivate(cb->cbi->em, key);
  return 0;
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
  return cb->cbi->errorMessage;
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
    const char *erm = NULL;
    if (!clIsEnabledCB(c->cba.shortNameCB()))
      erm = clLastStaticErrorCB(c->cba.shortNameCB());
    printf("%1s %10s:%12s:%8d%9s:%30s:%s%s\n",
      c->cba.isAutoEnabledCB() ? "A" : "_",
      clIsEnabledCB(c->cba.shortNameCB())?"(enabled)" : "(disabled)",
      c->cba.shortNameCB(),
      c->cba.getWindowSizeCB(),
      c->cba.doesRoundWholeBytesCB() ? "(int)" : "(double)",
      c->cba.longNameCB(),
      erm ? "\nreason:" : "",
      erm ? erm : ""
    );
  }
}

const char *clGetParamStringCB(struct CompressionBase *cb)
{
  checkPrepared(cb);
  return VF(cb, paramStringCB)(cb);
  return "(no parmstring yet)";
}
