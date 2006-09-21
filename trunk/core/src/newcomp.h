#include <complearn/datablock.h>
#include <complearn/envmap.h>

#define APIVER_CLCOMP10 10

struct CompressionBase;
struct CompressionBaseAdaptor;

struct CompressionBase {
  struct CompressionBaseInternal *cbi;
};

void clRegisterCB(const char *shortName, int allocSize, struct CompressionBaseAdaptor *vptr);
#define REGTYPEFORNAME(name, typ, xcba) clRegisterCB(#name, sizeof(typ), &xcba)

struct CompressionBase *clNewCompressorCB(const char *shortName);
struct EnvMap *clGetParameters(struct CompressionBase *cb);
int clSetParameterCB(struct CompressionBase *cb, const char *key, const char *val, int isPrivate);
void clSetLastErrorCB(struct CompressionBase *cb, const char *errMsg);
double clCompressCB(struct CompressionBase *cb, struct DataBlock *db);
struct DataBlock *clConcatCB(struct CompressionBase *cb, struct DataBlock *db1,
                            struct DataBlock *db2);
struct CompressionBaseAdaptor *clGetCBAsuper(void);
struct CompressionBaseAdaptor *clGetCBA(struct CompressionBase *cb);
const char *clLastErrorCB(struct CompressionBase *cb);

struct CompressionBaseAdaptor {
  int (*specificInitCB)(struct CompressionBase *cb);
  void (*freeCB)(struct CompressionBase *cb);
  int (*getAPIVersionCB)(void);
  const char *(*getLongNameCB)(struct CompressionBase *cb);

  int (*isDisabledCB)(struct CompressionBase *cb);

    // Should read params and use them.   This will be called only once
    // prior to compression.  This should do any compressor-specific
    // initialization at this point and return 0 to indicate success or
    // nonzero to indicate an error.  Don't forget to clSetLastErrorCB().
  int (*prepareToCompressCB)(struct CompressionBase *cb);

  int (*getWindowSizeCB)(void); // Return 0 for "infinite window"
  int (*doesRoundWholeBytesCB)(void);

  int (*isCompileProblemCB)(void); // Returns != 0 iff compile-time problem
       // is detected that will prevent this compressor from working.
       // If != 0 is to be returned, a call to setLastErrorCB should first
       // occur to explain the reason for the error.
  int (*isRuntimeProblemCB)(void); // Returns != 0 iff runtime problem

  const char *(*toStringCB)(void);

  const char *(*paramStringCB)(void);  // Should show params somehow

  /* Returns result in bits */
  double (*compressCB)(struct CompressionBase *cb, struct DataBlock *datum);
  struct DataBlock *(*concatCB)(struct CompressionBase *cb, struct DataBlock *dat1, struct DataBlock *dat2);
};

