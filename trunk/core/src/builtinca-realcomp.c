#include <complearn/complearn.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

struct RealCompressionInstance {
  void *baseClass;
  char *ecmd;
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
  struct RealCompressionInstance *rci = (struct RealCompressionInstance *) cb;
  int readfd;

  clAssert(rci->ecmd != NULL);
  readfd = clForkPipeExecAndFeedCB(src, rci->ecmd, NULL);
  return 8.0 * clCountBytesTillEOFThenCloseCB(readfd);
}

static void ffreeCB(struct CompressionBase *cb)
{
  struct RealCompressionInstance *rci = (struct RealCompressionInstance *) cb;
  if (rci->ecmd) {
    free(rci->ecmd);
    rci->ecmd = NULL;
  }
}

static int fspecificInitCB(struct CompressionBase *cb)
{
//  clSetParameterCB(cb, "cmd", "catbzip", 0);
  return 0;
}

static int fisAutoEnabledCB(void)
{
  return 0;
}

static int fprepareToCompressCB(struct CompressionBase *cb)
{
  const char *scmd = clEnvmapValueForKey(clGetParametersCB(cb), "cmd");
  const char *ecmd;
  struct RealCompressionInstance *rci = (struct RealCompressionInstance *) cb;
  if (scmd == NULL) {
    clSetLastErrorCB(cb,"Error, real compressor must have cmd parameter");
    return 1;
  }
  ecmd = clExpandCommand(scmd);
  if (ecmd)
    rci->ecmd = strdup(ecmd);
  else {
    char buf[1024];
    sprintf(buf, "Cannot find command %s for real compressor.", scmd);
    clSetLastErrorCB(cb, buf);
    return 1;
  }
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

void clinitReal(void)
{
  clRegisterCB(&cba);
}

