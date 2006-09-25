#include <complearn/complearn.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define RCIBUFSIZE 1024

struct VirtualCompressionInstance {
  void *baseClass;
  char *ecmd;
  int curpt;
  int bytecount;
  char sbuf[RCIBUFSIZE];
};

static const char *fshortNameCB(void)
{
  return "virtual";
}

static const char *flongNameCB(void)
{
  return "virtual external process";
}

static int fdoesRoundWholeBytesCB(void)
{
  return 0;
}

static int fallocSizeCB(void)
{
  return sizeof(struct VirtualCompressionInstance);
}

static double fcompressCB(struct CompressionBase *cb, struct DataBlock *src)
{
  struct VirtualCompressionInstance *rci = (struct VirtualCompressionInstance *) cb;
#define READBLOCKSIZE 16384
  char ch;
  int readfd;
  float retval;

  rci->curpt = 0;
  assert(rci->ecmd);
  readfd = clForkPipeExecAndFeedCB(src, rci->ecmd, NULL);

  while (read(readfd, &ch, 1) == 1 && rci->curpt < RCIBUFSIZE) {
    rci->sbuf[rci->curpt++] = ch;
  }
  rci->sbuf[rci->curpt++] = 0;
  retval = atof(rci->sbuf);
  close(readfd);
  return retval;
}

static void ffreeCB(struct CompressionBase *cb)
{
  struct VirtualCompressionInstance *rci = (struct VirtualCompressionInstance *) cb;
  if (rci->ecmd) {
    free(rci->ecmd);
    rci->ecmd = NULL;
  }
}

static int fspecificInitCB(struct CompressionBase *cb)
{
  return 0;
}

static int fisAutoEnabledCB(void)
{
  return 0;
}

#define DELIMS ":"
static int fprepareToCompressCB(struct CompressionBase *cb)
{
  const char *scmd = clEnvmapValueForKey(clGetParametersCB(cb), "cmd");
  const char *ecmd;
  struct VirtualCompressionInstance *rci = (struct VirtualCompressionInstance *) cb;
  if (scmd == NULL) {
    clSetLastErrorCB(cb,"Error, virtual compressor must have cmd parameter");
    return 1;
  }
  ecmd = clExpandCommand(scmd);
  if (ecmd)
    rci->ecmd = strdup(ecmd);
  else {
    char buf[1024];
    sprintf(buf, "Cannot find command %s for virtual compressor.", scmd);
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
  VIRTFUNCEXPORT(doesRoundWholeBytesCB),
  VIRTFUNCEXPORT(allocSizeCB)
};

void clinitVirtual(void)
{
  clRegisterCB(&cba);
}

