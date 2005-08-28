#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <complearn/complearn.h>

#if BZIP2_RDY
#include <bzlib.h>

static void bz2a_clsetenv(struct CompAdaptor *ca);
static double bz2a_compfunc(struct CompAdaptor *ca, struct DataBlock src);
static void bz2a_freecompfunc(struct CompAdaptor *ca);
static char *bz2a_shortname(void);
static char *bz2a_longname(void);
static int bz2a_apiver(void);

/* bzip2 compression interface */

/** \brief The BZIP2 compression adaptor instance
 *  \struct BZ2CompInstance
 *
 *  This structure holds all the persistent configuration information
 *  governing how the block-sorting compressor behaves.  Several parameters
 *  that effect the behavior are listed as fields below.
 */
struct BZ2CompInstance {
  int blocksize;  /*!< the size of each block, in multiples of 100K; this must
                       be between 1 and 9 inclusive.  The default, 9, is the
                       largest block, measuring just under one megabyte. */
  int workfactor; /*!< a performance-tuning parameter that can effect
                       compression speed drastically for certain types of
                       highly repetitive low-entropy files.  This parameter
                       should now affect results, but may effect the time it
                       takes to reach them. */
	int verbosity;  /*!< a bzip-only verbosity flag, either 0 or 1, to suppress
                       or encourage the printing of status or other messages.*/
};

/** \brief Initializes a BZIP2 CompAdaptor instance
 *
 *  builtin_BZIP() allocates memory to a BZIP2 CompAdaptor instance. The
 *  BZIP2 CompAdaptor is required for use in computing NCDs with the bzip2
 *  compression library.
 *
 *  There are 3 options that can be set for the bzip2 CompAdaptor: blocksize,
 *  workfactor, and verbosity.  By default, blocksize is set to 9, workfactor
 *  is set to 30, and verbosity is set to 0.
 *
 *  builtin_BZIP() will also read a CompLearn configuration file to override
 *  the option defaults. For details on how to create a configuration file, see
 *  http://www.complearn.org/config.html
 *
 *  \return pointer to newly initialized BZIP2 CompAdaptor instance
 */
struct CompAdaptor *builtin_BZIP(void)
{
	struct CompAdaptor c =
	{
    cptr: NULL,
//    se:   bz2a_clsetenv,
    cf:   bz2a_compfunc,
    fcf:  bz2a_freecompfunc,
    sn:   bz2a_shortname,
    ln:   bz2a_longname,
    apiv: bz2a_apiver,
  };
  struct CompAdaptor *ca;
  struct BZ2CompInstance *bzci;
  ca = clCalloc(sizeof(*ca), 1);
  *ca = c;
  ca->cptr = clCalloc(sizeof(struct BZ2CompInstance), 1);
  bzci = (struct BZ2CompInstance *) ca->cptr;

  /* default compressor options */
  bzci->blocksize = 9;
  bzci->workfactor = 30;
  bzci->verbosity = 0;

  bz2a_clsetenv(ca);
  return ca;
}

static void bz_setIntValueMaybe(struct EnvMap *srcenv, const char *keyname, int *placeToSet) {
  char *val;
  val = envmapValueForKey(srcenv,keyname);
  if (val)
    *placeToSet = atoi(val);
}

static void bz2a_clsetenv(struct CompAdaptor *ca)
{
  struct BZ2CompInstance *bzci = (struct BZ2CompInstance *) ca->cptr;
  struct EnvMap *em = loadDefaultEnvironment()->em;

  bz_setIntValueMaybe(em, "blocksize", &bzci->blocksize);
  bz_setIntValueMaybe(em, "workfactor", &bzci->workfactor);
  bz_setIntValueMaybe(em, "bzverbosity", &bzci->verbosity);
}

static double bz2a_compfunc(struct CompAdaptor *ca, struct DataBlock src)
{
#if BZIP2_RDY
	struct BZ2CompInstance *bzci = (struct BZ2CompInstance *) ca->cptr;
	int s;

  unsigned char *dbuff;
	int p;

	p = src.size*1.02+600;
	dbuff = (unsigned char*)clMalloc(p);
	s = BZ2_bzBuffToBuffCompress((char *) dbuff,(unsigned int *) &p,(char *) src.ptr,src.size,
			bzci->blocksize, bzci->verbosity, bzci->workfactor);
	if (s == BZ_OUTBUFF_FULL) {
		printf ("destLen not big enough!\n");
		exit(1);
	}
	if (s != BZ_OK) {
		printf ("Unknown error: bzBuff returned %d\n",s);
		exit(1);
	}
	free(dbuff);
	return (double) p*8.0;
#else
	return -1.0;
#endif
}

static void bz2a_freecompfunc(struct CompAdaptor *ca)
{
  clFreeandclear(ca->cptr);
	clFreeandclear(ca);
}

static char *bz2a_shortname(void)
{
	return "bzip";
}

static char *bz2a_longname(void)
{
	return "Block-sorting compressor bzip2";
}

static int bz2a_apiver(void)
{
	return APIVER_V1;
}

#else

struct CompAdaptor *builtin_BZIP(void)
{
  return NULL;
}

#endif
