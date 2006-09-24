#include <complearn/complearn.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

struct PPMDXCompressionInstance {
  void *baseClass;
  int order;
};

static const char *ecmd;
static const char *fshortNameCB(void)
{
  return "ppmdx";
}

static const char *flongNameCB(void)
{
  return "external ppmd command";
}

static int fallocSizeCB(void)
{
  return sizeof(struct PPMDXCompressionInstance);
}

static double fcompressCB(struct CompressionBase *cb, struct DataBlock *src)
{
  struct PPMDXCompressionInstance *rci = (struct PPMDXCompressionInstance *) cb;
  struct StringStack *args = clStringstackNew();
  int dummy;
  struct DataBlock *nonblock;
  double result;
  char olddir[4096], tmpres[256];
  char goodopt[32];
  struct DataBlock *dbres;
  nonblock = clStringToDataBlockPtr(" ");
  clStringstackPush(args, "e");
  clStringstackPush(args, "-s");
  sprintf(goodopt, "-o%d", rci->order);
  clStringstackPush(args, goodopt);
  clStringstackPush(args, "-m256");
  clStringstackPush(args, "inp");
  int readfd;
  assert(ecmd);
  getcwd(olddir, sizeof(olddir));
  memset(tmpres, 0, sizeof(tmpres));
  tmpnam(tmpres);
  mkdir(tmpres, 0644);
  chdir(tmpres);
  clDatablockWriteToFile(src, "inp");
  readfd = clForkPipeExecAndFeedCB(nonblock, ecmd, args);
  clDatablockFree(nonblock);
  dummy = clCountBytesTillEOFThenCloseCB(readfd);
  dbres = clFileToDataBlockPtr("inp.pmd");
  result = 8.0 * clDatablockSize(dbres);
  clDatablockFree(dbres);
  unlink(clJoinAsPath(tmpres, "inp.pmd"));
  unlink(clJoinAsPath(tmpres, "inp"));
  chdir(olddir);
  rmdir(tmpres);
  return result;
}

static void ffreeCB(struct CompressionBase *cb)
{
  struct PPMDXCompressionInstance *rci = (struct PPMDXCompressionInstance *) cb;
}

static int fspecificInitCB(struct CompressionBase *cb)
{
  struct PPMDXCompressionInstance *rci = (struct PPMDXCompressionInstance *) cb;
  rci->order = 8;
  return 0;
}

static int fisAutoEnabledCB(void)
{
  return 0;
}

static int fprepareToCompressCB(struct CompressionBase *cb)
{
  const char *levp = clEnvmapValueForKey(clGetParametersCB(cb), "order");
  struct PPMDXCompressionInstance *rci = (struct PPMDXCompressionInstance *) cb;
  if (levp) {
    int wlev = atoi(levp);
    if (wlev < 2 || wlev > 16) {
      clSetLastErrorCB(cb, "Invalid order parameter");
      return 1;
    }
    rci->order = wlev;
  }
  return 0;
}

static int fisRuntimeProblemCB(void)
{
  const char *scmd = "ppmd";
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

void initPPMDX(void)
{
  clRegisterCB(&cba);
}

