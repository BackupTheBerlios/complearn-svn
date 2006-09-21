#include <complearn/datablock.h>

struct CompressionBase {
  struct CompressionBaseInternal *cbi;
};

struct CompressionBaseInternal {
  char *shortName, *longName;
  char *errorMessage;
  struct CompressionBase *cb;
  struct CompressionBaseAdaptor *vptr;
};


struct CompressionBase *clInitCB(const char *shortName, const char *longName, int allocSize, struct CompressionBaseAdaptor *vptr);
int clSetParameterCB(struct CompressionBase *cb, const char *key, const char *val, int isPrivate);
void clSetLastErrorCB(struct CompressionBase *cb, const char *errMsg);
struct CompressionBaseAdaptor *clGetCBAsuper(void);
struct CompressionBaseAdaptor *clGetCBA(struct CompressionBase *cb);
const char *clLastErrorCB(struct CompressionBase *cb);

struct CompressionBaseAdaptor {
  int (*specificInitCB)(struct CompressionBase *cb);
  void (*freeCB)(struct CompressionBase *cb);
  int (*getAPIVersionCB)(struct CompressionBase *cb);
  const char (*getShortNameCB)(struct CompressionBase *cb);
  const char (*getLongNameCB)(struct CompressionBase *cb);

  int (*isDisabledCB)(struct CompressionBase *cb);
  const char *(*getReasonDisabledCB)(struct CompressionBase *cb);

  int (*prepareToCompressCB)(struct CompressionBase *cb);
  const char *(*getReasonPrepareFailedCB)(struct CompressionBase *cb);

  int (*getWindowSizeCB)(void);
  int (*doesRoundWholeBytesCB)(void);

  const char *(*toStringCB)(void);

  /* Returns result in bits */
  float (*compressCB)(struct CompressionBase *cb, struct DataBlock *datum);
  struct DataBlock *(*concatCB)(struct CompressionBase *cb, struct DataBlock *dat1, struct DataBlock *dat2);
};

