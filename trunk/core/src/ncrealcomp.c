#include <complearn/complearn.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "newcomp.h"
#include "ncabz2.h"

struct RealCompressionInstance {
  void *baseClass;
  char *ecmd;
  int bytecount;
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
#define READBLOCKSIZE 16384
  static char dummy[READBLOCKSIZE];
  int readfd, readlen;

  rci->bytecount = 0;
  assert(rci->ecmd);
  readfd = clForkPipeExecAndFeedCB(src, rci->ecmd);
  while ((readlen = read(readfd, &dummy[0], READBLOCKSIZE)) > 0) {
    rci->bytecount += readlen;
  }
  close(readfd);
  return rci->bytecount * 8.0;
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
  //clSetParameterCB(cb, "cmd", "cat", 0);
  return 0;
}

static int fisAutoEnabledCB(void)
{
  return 0;
}

#define DELIMS ":"
static const char *expandCommand(const char *inpcmd)
{
  static char *buf;
  struct stat st;
  char *path, *p;
  int inplen, pathlen;
  if (inpcmd[0] == '/')
    return inpcmd;
  inplen = strlen(inpcmd);
  path = getenv("PATH");
  if (path == NULL)
    path = "/bin:/usr/bin";
  pathlen = strlen(path);
  if (buf)
    free(buf);
  buf = calloc(1, inplen + pathlen + 16);
  path = strdup(path);
  for (p = strtok(path, DELIMS); p; p = strtok(NULL, DELIMS)) {
    sprintf(buf, "%s/%s", p, inpcmd);
    if (stat(buf, &st) == 0)
      return buf;
  }
  free(path);
  return NULL;
}

static int fprepareToCompressCB(struct CompressionBase *cb)
{
  const char *scmd = clEnvmapValueForKey(clGetParametersCB(cb), "cmd");
  const char *ecmd;
  struct RealCompressionInstance *rci = (struct RealCompressionInstance *) cb;
  if (scmd == NULL) {
    clogError("Error, real compressor must have cmd parameter");
  }
  ecmd = expandCommand(scmd);
  if (ecmd)
    rci->ecmd = strdup(ecmd);
  else {
    char buf[1024];
    sprintf(buf, "Cannot find command %s for real compressor.", scmd);
    clogError(buf);
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

void initReal(void)
{
  clRegisterCB(&cba);
}

