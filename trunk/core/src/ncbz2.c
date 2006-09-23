
#include <complearn/complearn.h>

#ifdef HAVE_BZLIB_H
#include <bzlib.h>
#endif

#include "newcomp.h"
#include "ncabz2.h"

struct BZ2DynamicAdaptorCB *clGrabBZ2DACB(void);

struct BZipCompressionInstance {
  void *baseClass;
  int blocksize;
  int verbosity;
  int workfactor;
};

static int fgetWindowSizeCB(void)
{
  return 900*1024;
}

static const char *fshortNameCB(void)
{
  return "bzip2";
}

static const char *flongNameCB(void)
{
  return "popular block-sorting bzip2";
}

static int fallocSizeCB(void)
{
  return sizeof(struct BZipCompressionInstance);
}

static int fisCompileProblemCB(void)
{
#if HAVE_BZLIB_H
  return 0;
#endif
  if (clGrabBZ2DACB() != NULL)
    return 0;
  clSetStaticErrorMessage(fshortNameCB(), "Cannot open bzip2 library");
  return 1;
}

static double fcompressCB(struct CompressionBase *cb, struct DataBlock *src)
{
  struct BZipCompressionInstance *bzci = (struct BZipCompressionInstance *) cb;
  struct BZ2DynamicAdaptorCB *bzip2 = clGrabBZ2DACB();
  int s;

  unsigned char *dbuff;
  unsigned long p;

  if (bzip2 == NULL) {
    clogError("Cannot do bzip2 compression: no bzip2 library available.");
  }
  p = clDatablockSize(src)*1.5+600;
  dbuff = (unsigned char*)clMalloc(p);
  s = (bzip2->buftobufcompress)((char *) dbuff,(unsigned int *) &p,(char *) clDatablockData(src),clDatablockSize(src),
      bzci->blocksize, bzci->verbosity, bzci->workfactor);
  if (s != BZ_OK) {
    printf ("error code %d\n", s);
    exit(1);
  }
  free(dbuff);
  return (double) p*8.0;
}

static int fspecificInitCB(struct CompressionBase *cb)
{
  struct BZipCompressionInstance *bzci = (struct BZipCompressionInstance *) cb;
  bzci->blocksize = 9; // Best compression by default
  bzci->workfactor = 30; // Default provides "ok" performance
  bzci->verbosity = 0; // Silent
  return 0;
}

static int fprepareToCompressCB(struct CompressionBase *cb)
{
  const char *sbs = clEnvmapValueForKey(clGetParametersCB(cb), "blocksize");
  const char *swf = clEnvmapValueForKey(clGetParametersCB(cb), "workfactor");
  const char *sv = clEnvmapValueForKey(clGetParametersCB(cb), "verbosity");
  struct BZipCompressionInstance *bzci = (struct BZipCompressionInstance *) cb;
  if (sbs != NULL)
    bzci->blocksize = atoi(sbs);
  if (swf != NULL)
    bzci->workfactor = atoi(swf);
  if (sv != NULL)
    bzci->verbosity = atoi(sv);
  return 0;
}

static struct CompressionBaseAdaptor cba = {
  VIRTFUNCEXPORT(specificInitCB),
  VIRTFUNCEXPORT(prepareToCompressCB),
  VIRTFUNCEXPORT(compressCB),
  VIRTFUNCEXPORT(isCompileProblemCB),
  VIRTFUNCEXPORT(getWindowSizeCB),
  VIRTFUNCEXPORT(shortNameCB),
  VIRTFUNCEXPORT(longNameCB),
  VIRTFUNCEXPORT(allocSizeCB)
};

void initBZ2(void)
{
  clRegisterCB(&cba);
}

