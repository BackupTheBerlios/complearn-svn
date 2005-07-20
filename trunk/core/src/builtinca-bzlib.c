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

/**
 *  \brief The BZIP2 compression adaptor instance
 *  \struct bz2aCompInstance
 *
 *  This structure holds all the persistent configuration information
 *  governing how the block-sorting compressor behaves.  Several parameters
 *  that effect the behavior are listed as fields below.
 */
struct bz2aCompInstance {
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
  struct bz2aCompInstance *bzci;
  ca = gcalloc(sizeof(*ca), 1);
  *ca = c;
  ca->cptr = gcalloc(sizeof(struct bz2aCompInstance), 1);
  bzci = (struct bz2aCompInstance *) ca->cptr;

  /* default compressor options */
  bzci->blocksize = 9;
  bzci->workfactor = 30;
  bzci->verbosity = 0;

  bz2a_clsetenv(ca);
  return ca;
}

static void bz_setIntValueMaybe(struct EnvMap *srcenv, const char *keyname, int *placeToSet) {
  char *val;
  val = readValForEM(srcenv,keyname);
  if (val)
    *placeToSet = atoi(val);
}

static void bz2a_clsetenv(struct CompAdaptor *ca)
{
  struct bz2aCompInstance *bzci = (struct bz2aCompInstance *) ca->cptr;
  struct EnvMap *em = loadDefaultEnvironment()->em;

  bz_setIntValueMaybe(em, "blocksize", &bzci->blocksize);
  bz_setIntValueMaybe(em, "workfactor", &bzci->workfactor);
  bz_setIntValueMaybe(em, "bzverbosity", &bzci->verbosity);
}

static double bz2a_compfunc(struct CompAdaptor *ca, struct DataBlock src)
{
#if BZIP2_RDY
	struct bz2aCompInstance *bzci = (struct bz2aCompInstance *) ca->cptr;
	int s;

  unsigned char *dbuff;
	int p;

	p = src.size*1.02+600; 
	dbuff = (unsigned char*)gmalloc(p);
	s = BZ2_bzBuffToBuffCompress(dbuff,&p,src.ptr,src.size,
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
  gfreeandclear(ca->cptr);
	gfreeandclear(ca);
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
