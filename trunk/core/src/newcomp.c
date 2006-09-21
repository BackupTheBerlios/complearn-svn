#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "newcomp.h"

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

static int specificInitCB(struct CompressionBase *cb)
{
  printf("(no specific initialization function given)\n");
}

struct CompressionBaseAdaptor cbsuper = {
  specificInitCB,
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
  cbi->vptr->specificInitCB(cb);
  return cb;
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
