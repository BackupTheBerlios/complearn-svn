
#ifndef __NCCBI_H
#define __NCCBI_H

#include <complearn/builtin.h>

struct CompressionBaseInternal {
  int fHavePrepared;
  int fHaveFailed;
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

#endif
