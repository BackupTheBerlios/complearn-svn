#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "newcomp.h"


static int specificInitCB(struct CompressionBase *cb)
{
  printf("(no specific initialization function given)\n");
}

struct CompressionBaseAdaptor cbsuper = {
  specificInitCB,
};

void **dvptr = (void **) &cbsuper;

struct CompressionBase *clInitCB(const char *shortName, const char *longName, int allocSize, struct CompressionBaseAdaptor *vptr)
{
  struct CompressionBase *cb = calloc(allocSize, 1);
  struct CompressionBaseInternal *cbi = calloc(sizeof(struct CompressionBaseInternal), 1);
  int cbas = sizeof(struct CompressionBaseAdaptor);
  int ps = sizeof(void *);
  void **vpn = calloc(cbas, 1);
  void **svptr = (void **) vptr;
  int i;
  for (i = 0; i < (cbas/ps); i += 1) {
    void *f = svptr[i];
    if (f == NULL)
      f = dvptr[i];
    vpn[i] = f;
  }
  cb->cbi = cbi;
  cbi->shortName = strdup(shortName);
  cbi->longName = strdup(longName);
  cbi->vptr = (struct CompressionBaseAdaptor *) vpn;
  cbi->cb = cb;
  cbi->vptr->specificInitCB(cb);
}
