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
  return "lzmax";
}

static const char *flongNameCB(void)
{
  return "Lempel-Ziv Markov-Chain cmd";
}

static int fallocSizeCB(void)
{
  return sizeof(struct LZMACompressionInstance);
}

static double fcompressCB(struct CompressionBase *cb, struct DataBlock *src)
{
  struct StringStack *args = clStringstackNew();
  /* The Cygwin and Linux versions of lzma seem to take different options */
#ifdef __CYGWIN__
  clStringstackPush(args, "e");
  clStringstackPush(args, "-d27");
  clStringstackPush(args, "-si");
  clStringstackPush(args, "-so");
#else
  clStringstackPush(args, "-9zcf");
#endif
  int readfd;

  assert(ecmd != NULL);
  readfd = clForkPipeExecAndFeedCB(src, ecmd, args);
  return 8.0 * clCountBytesTillEOFThenCloseCB(readfd);
}

static void ffreeCB(struct CompressionBase *cb)
{
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
  ecmd = clExpandCommand(scmd);
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

#if STATICLOADEXTRAMODS
void clinitLZMAX(void)
{
  clRegisterCB(&cba);
}
#else
INITFUNCTYPE()
{
  clRegisterCB(&cba);
}
#endif
