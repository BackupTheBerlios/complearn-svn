#ifndef __NEWCOMP_H
#define __NEWCOMP_H

#include <complearn/datablock.h>
#include <complearn/envmap.h>

#define APIVER_CLCOMP10 10
#define VIRTFUNCEXPORT(x) x : f##x

struct CompressionBase;
struct CompressionBaseAdaptor;

struct CompressionBase {
  struct CompressionBaseInternal *cbi;
};

void clRegisterCB(struct CompressionBaseAdaptor *vptr);

struct CompressionBase *clNewCompressorCB(const char *shortName);
const char *expandCommand(const char *inpcmd);
int clForkPipeExecAndFeedCB(struct DataBlock *inp, const char *cmd);
void clZombie_reaperCB(int q);
struct EnvMap *clGetParametersCB(struct CompressionBase *cb);
const char *clGetParamStringCB(struct CompressionBase *cb);
int clSetParameterCB(struct CompressionBase *cb, const char *key, const char *val, int isPrivate);
void clSetLastErrorCB(struct CompressionBase *cb, const char *errMsg);
void clSetLastStaticErrorCB(const char *shortName, const char *errMsg);
double clCompressCB(struct CompressionBase *cb, struct DataBlock *db);
struct DataBlock *clConcatCB(struct CompressionBase *cb, struct DataBlock *db1,
                            struct DataBlock *db2);
struct CompressionBaseAdaptor *clGetCBAsuper(void);
struct CompressionBaseAdaptor *clGetCBA(struct CompressionBase *cb);
const char *clLastErrorCB(struct CompressionBase *cb);
int clIsEnabledCB(const char *shortName);
const char *clLastStaticErrorCB(const char *shortName);
void clFreeCB(struct CompressionBase *cb);
void clSetStaticErrorMessage(const char *shortName, const char *msg);

struct CompressionBaseAdaptor {
  int (*specificInitCB)(struct CompressionBase *cb);
  void (*freeCB)(struct CompressionBase *cb);
  int (*getAPIVersionCB)(void);
  const char *(*longNameCB)(void);

  int (*isDisabledCB)(void);

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
  const char *(*shortNameCB)(void);
  int (*allocSizeCB)(void);

  const char *(*toStringCB)(struct CompressionBase *cb);

  const char *(*paramStringCB)(struct CompressionBase *cb);  // Should show params somehow

  /* Returns result in bits */
  double (*compressCB)(struct CompressionBase *cb, struct DataBlock *datum);
  struct DataBlock *(*concatCB)(struct CompressionBase *cb, struct DataBlock *dat1, struct DataBlock *dat2);
  int (*isAutoEnabledCB)(void);
};

#endif
