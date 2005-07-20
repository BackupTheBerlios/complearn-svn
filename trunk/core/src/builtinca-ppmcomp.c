#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <complearn/ppmcomp.h>
#include <complearn/complearn.h>

static double ppm_compfunc(struct CompAdaptor *ca, struct DataBlock src);
static void ppm_freecompfunc(struct CompAdaptor *ca);
static char *ppm_shortname(void);
static char *ppm_longname(void);
static int ppm_apiver(void);

/* bzip2 compression interface */

/**
 *  \brief The PPM compression adaptor instance
 *  \struct ppmCompInstance
 *
 *  This structure holds all the persistent configuration information
 *  governing how the block-sorting compressor behaves.  Several parameters
 *  that effect the behavior are listed as fields below.
 */
struct ppmCompInstance {
  int maxOrder;  /*!< the size of each block, in multiples of 100K; this must
                       be between 1 and 9 inclusive.  The default, 9, is the
                       largest block, measuring just under one megabyte. */
  int maxMem; /*!< a performance-tuning parameter that can effect
                       compression speed drastically for certain types of
                       highly repetitive low-entropy files.  This parameter
                       should now affect results, but may effect the time it
                       takes to reach them. */
};

struct CompAdaptor *builtin_PPM(void)
{
	struct CompAdaptor c =
	{
    cptr: NULL,
    cf:   ppm_compfunc,
    fcf:  ppm_freecompfunc,
    sn:   ppm_shortname,
    ln:   ppm_longname,
    apiv: ppm_apiver,
  };
  struct CompAdaptor *ca;
  struct ppmCompInstance *ppmci;
  ca = gcalloc(sizeof(*ca), 1);
  *ca = c;
  ca->cptr = gcalloc(sizeof(struct ppmCompInstance), 1);
  ppmci = (struct ppmCompInstance *) ca->cptr;

  /* default compressor options */
  ppmci->maxOrder = 4;
  ppmci->maxMem = 512*1024*1024;

  return ca;
}

static double ppm_compfunc(struct CompAdaptor *ca, struct DataBlock src)
{
	//struct ppmCompInstance *bzci = (struct ppmCompInstance *) ca->cptr;
  struct PPMComp *ppmc;
  double result;

  ppmc = newPPMComp();
  encodeSymbolArray(ppmc, src.ptr, src.size);

  result = getPPMCompSize(ppmc);
  freePPMComp(ppmc);
//  printf("PPM returning %f for input size %f\n", result, src.size * 8.0);
	return result;
}

static void ppm_freecompfunc(struct CompAdaptor *ca)
{
  gfreeandclear(ca->cptr);
	gfreeandclear(ca);
}

static char *ppm_shortname(void)
{
	return "ppm";
}

static char *ppm_longname(void)
{
	return "Prediction by Partial Matching (PPM) compressor";
}

static int ppm_apiver(void)
{
	return APIVER_V1;
}

