
#include "newcomp.h"

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

