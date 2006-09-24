#include <complearn/complearn.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

struct BZip2XCompressionInstance {
  void *baseClass;
  char levChar;
};

static const char *ecmd;
static const char *fshortNameCB(void)
{
  return "bzip2x";
}

static const char *flongNameCB(void)
{
  return "external bzip2 command";
}

static int fallocSizeCB(void)
{
  return sizeof(struct BZip2XCompressionInstance);
}

static double fcompressCB(struct CompressionBase *cb, struct DataBlock *src)
{
  struct BZip2XCompressionInstance *rci = (struct BZip2XCompressionInstance *) cb;
  struct StringStack *args = clStringstackNew();
  char goodopt[32];
  strcpy(goodopt, "-9cf");
  goodopt[1] = rci->levChar;
  clStringstackPush(args, goodopt);
  int readfd;

  assert(ecmd);
  readfd = clForkPipeExecAndFeedCB(src, ecmd, args);
  return 8.0 * clCountBytesTillEOFThenCloseCB(readfd);
}

static void ffreeCB(struct CompressionBase *cb)
{
  struct BZip2XCompressionInstance *rci = (struct BZip2XCompressionInstance *) cb;
}

static int fspecificInitCB(struct CompressionBase *cb)
{
  struct BZip2XCompressionInstance *rci = (struct BZip2XCompressionInstance *) cb;
  rci->levChar = '9';
  return 0;
}

static int fisAutoEnabledCB(void)
{
  return 0;
}

static int fprepareToCompressCB(struct CompressionBase *cb)
{
  const char *levp = clEnvmapValueForKey(clGetParametersCB(cb), "blocksize");
  struct BZip2XCompressionInstance *rci = (struct BZip2XCompressionInstance *) cb;
  if (levp) {
    if (levp[1] == 0 && ( levp[0] >= '1' && levp[0] <= '9')) {
      rci->levChar = levp[0];
      return 0;
    }
    clSetLastErrorCB(cb, "Invalid blocksize parameter");
    return 1;
  }
  return 0;
}

static int fisRuntimeProblemCB(void)
{
  const char *scmd = "bzip2";
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
  VIRTFUNCEXPORT(prepareToCompressCB),
  VIRTFUNCEXPORT(compressCB),
  VIRTFUNCEXPORT(shortNameCB),
  VIRTFUNCEXPORT(longNameCB),
  VIRTFUNCEXPORT(isAutoEnabledCB),
  VIRTFUNCEXPORT(freeCB),
  VIRTFUNCEXPORT(allocSizeCB)
};

void initBZip2X(void)
{
  clRegisterCB(&cba);
}

