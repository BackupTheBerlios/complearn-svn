#include <complearn/complearn.h>
#include <string.h>

#include "newcomp.h"
#include "ncabz2.h"

struct RealCompressionInstance {
  void *baseClass;
  char *cmd;
};

static const char *fshortNameCB(void)
{
  return "real";
}

static const char *flongNameCB(void)
{
  return "external process";
}

static int fallocSizeCB(void)
{
  return sizeof(struct RealCompressionInstance);
}

static double fcompressCB(struct CompressionBase *cb, struct DataBlock *src)
{
  //struct RealCompressionInstance *rci = (struct RealCompressionInstance *) cb;
  return clDatablockSize(src)*8.0;
}

static void ffreeCB(struct CompressionBase *cb)
{
  struct RealCompressionInstance *rci = (struct RealCompressionInstance *) cb;
  free(rci->cmd);
  rci->cmd = NULL;
}

static int fspecificInitCB(struct CompressionBase *cb)
{
  struct RealCompressionInstance *rci = (struct RealCompressionInstance *) cb;
  rci->cmd = strdup("cat");
  return 0;
}

static int fisAutoEnabledCB(void)
{
  return 0;
}

static int fprepareToCompressCB(struct CompressionBase *cb)
{
  const char *scmd = clEnvmapValueForKey(clGetParametersCB(cb), "cmd");
  struct RealCompressionInstance *rci = (struct RealCompressionInstance *) cb;
  if (scmd == NULL) {
    clogError("Error, real compressor must have cmd parameter");
  }
  rci->cmd = strdup(scmd);
  return 0;
}

static struct CompressionBaseAdaptor cba = {
  VIRTFUNCEXPORT(specificInitCB),
  VIRTFUNCEXPORT(prepareToCompressCB),
  VIRTFUNCEXPORT(compressCB),
  VIRTFUNCEXPORT(shortNameCB),
  VIRTFUNCEXPORT(longNameCB),
  VIRTFUNCEXPORT(isAutoEnabledCB),
  VIRTFUNCEXPORT(freeCB),
  VIRTFUNCEXPORT(allocSizeCB)
};

void initReal(void)
{
  clRegisterCB(&cba);
}

