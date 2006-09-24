#include <complearn/complearn.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

struct LZMACompressionInstance {
  void *baseClass;
};

static const char *ecmd;
static const char *fshortNameCB(void)
{
  return "lzma";
}

static const char *flongNameCB(void)
{
  return "Lempel-Ziv Markov-Chain";
}

static int fallocSizeCB(void)
{
  return sizeof(struct LZMACompressionInstance);
}

static double fcompressCB(struct CompressionBase *cb, struct DataBlock *src)
{
  struct LZMACompressionInstance *rci = (struct LZMACompressionInstance *) cb;
  struct StringStack *args = clStringstackNew();
  clStringstackPush(args, "-9zcf");
  int readfd;

  assert(ecmd);
  readfd = clForkPipeExecAndFeedCB(src, ecmd, args);
  return 8.0 * clCountBytesTillEOFThenCloseCB(readfd);
}

static void ffreeCB(struct CompressionBase *cb)
{
  struct LZMACompressionInstance *rci = (struct LZMACompressionInstance *) cb;
}

static int fspecificInitCB(struct CompressionBase *cb)
{
  return 0;
}

static int fisAutoEnabledCB(void)
{
  return 1;
}

static int fisRuntimeProblemCB(void)
{
  const char *scmd = "lzma";
  ecmd = expandCommand(scmd);
  if (ecmd) {
   ;  /* all good */ ;
  } else {
    char buf[1024];
    sprintf(buf, "Cannot find command %s (please install)", scmd);
    clSetLastStaticErrorCB(fshortNameCB(), buf);
    return 1;
  }
  return 0;
}

static struct CompressionBaseAdaptor cba = {
  VIRTFUNCEXPORT(specificInitCB),
  VIRTFUNCEXPORT(isRuntimeProblemCB),
  VIRTFUNCEXPORT(compressCB),
  VIRTFUNCEXPORT(shortNameCB),
  VIRTFUNCEXPORT(longNameCB),
  VIRTFUNCEXPORT(isAutoEnabledCB),
  VIRTFUNCEXPORT(freeCB),
  VIRTFUNCEXPORT(allocSizeCB)
};

void initLZMA(void)
{
  clRegisterCB(&cba);
}

