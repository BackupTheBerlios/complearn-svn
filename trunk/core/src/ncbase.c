#include <complearn/complearn.h>
#include <string.h>
#include "ncbase.h"
#include "newcomp.h"
#include "nccbi.h"

// START OF BASE-CLASS FUNCTIONS
// ----------------------------------------------------------

// By default, do nothing.  (the block itself, cb, will automatically be freed)
//
// If there are other pointers however, they should be freed here.
static void ffreeCB(struct CompressionBase *cb)
{
}

static int fgetAPIVersionCB(void)
{
  return APIVER_CLCOMP10;
}

static const char *flongNameCB(void)
{
  fprintf(stderr, "Error, no long name defined for compressor\n");
  exit(0);
  return NULL;
}

static int fisDisabledCB(void)
{
  return 0;
}

static const char *fshortNameCB(void)
{
  fprintf(stderr, "Error, no short name defined for compressor\n");
  exit(0);
  return NULL;
}

static int fallocSizeCB(void)
{
  return sizeof(void *);
}


static const char *ftoStringCB(struct CompressionBase *cb)
{
  if (cb->cbi->madeupToString == NULL) {
    const char *sn = VF(cb, shortNameCB)();
    int sl = strlen(sn);
    cb->cbi->madeupToString = calloc(20 + sl, 1);
    sprintf(cb->cbi->madeupToString, "%s:%08x", sn, ((unsigned int) cb));
  }
  return cb->cbi->madeupToString;
}

static struct DataBlock *fconcatCB(struct CompressionBase *cb, struct DataBlock *dat1, struct DataBlock *dat2)
{
  return clDatablockCatPtr(dat1, dat2);
}

static const char *fparamStringCB(struct CompressionBase *cb)
{
  if (cb->cbi->madeupParamString == NULL) {
    struct EnvMap *em = cb->cbi->em;
    int i;
    int als = 1;
    char *curs;
    for (i = 0; i < clEnvmapSize(em); i += 1) {
      union PCTypes p;
      if (clEnvmapIsPrivateAt(em, i))
        continue;
      p = clEnvmapKeyValAt(em, i);
      als += 2 + strlen(p.sp.key) + strlen(p.sp.val);
    }
    cb->cbi->madeupParamString = calloc(als, 1);
    curs = cb->cbi->madeupParamString;
    for (i = 0; i < clEnvmapSize(em); i += 1) {
      union PCTypes p;
      if (clEnvmapIsPrivateAt(em, i))
        continue;
      p = clEnvmapKeyValAt(em, i);
      curs += sprintf(curs, "%s=%s,",p.sp.key, p.sp.val);
    }
    curs[-1] = 0;
  }
  return cb->cbi->madeupParamString;
}

static int fgetWindowSizeCB(void)
{
  return 0;
}

static int fdoesRoundWholeBytesCB(void)
{
  return 1;
}

static int fisCompileProblemCB(void)
{
  return 0;


static ixnt fisAutoEnabled(void)
{
  return 1;
}

static int fisRuntimeProblemCB(void)
{
  return 0;
}

static double fcompressCB(struct CompressionBase *cb, struct DataBlock *db)
{
  printf("(no specific compression function given)\n");
  return clDatablockSize(db) * 8.0;
}

static int fspecificInitCB(struct CompressionBase *cb)
{
  printf("(no specific initialization function given)\n");
  return 0;
}

struct CompressionBaseAdaptor cbsuper = {
  VIRTFUNCEXPORT(getAPIVersionCB),
  VIRTFUNCEXPORT(specificInitCB),
  VIRTFUNCEXPORT(freeCB),
  VIRTFUNCEXPORT(compressCB),
  VIRTFUNCEXPORT(specificInitCB),
  VIRTFUNCEXPORT(getWindowSizeCB),
  VIRTFUNCEXPORT(doesRoundWholeBytesCB),
  VIRTFUNCEXPORT(isCompileProblemCB),
  VIRTFUNCEXPORT(isRuntimeProblemCB),
  VIRTFUNCEXPORT(longNameCB),
  VIRTFUNCEXPORT(shortNameCB),
  VIRTFUNCEXPORT(allocSizeCB),
  VIRTFUNCEXPORT(toStringCB),
  VIRTFUNCEXPORT(paramStringCB),
  VIRTFUNCEXPORT(concatCB),
  VIRTFUNCEXPORT(isDisabledCB),
  VIRTFUNCEXPORT(isAutoEnabledCB)
};

