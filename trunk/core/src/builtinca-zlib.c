#include <complearn/complearn.h>

#if ZLIB_RDY
#include <zlib.h>

#include <stdlib.h>
#include <malloc.h>

static void zlib_clsetenv(struct CompAdaptor *ca);
static double zlib_compfunc(struct CompAdaptor *ca, struct DataBlock src);
static void zlib_freecompfunc(struct CompAdaptor *ca);
static char *zlib_shortname(void);
static char *zlib_longname(void);
static int zlib_apiver(void);

/** \brief zlib compression state information
 * \struct ZlibCompInstance
 *
 * This structure holds all zlib compression information.  At this time
 * it only allows for the adjustment of the compression level, which
 * is defaulted to the maximum level of 9.
 */
struct ZlibCompInstance {
	int level; // 0 - 9
};

/** \brief Initializes a ZLIB CompAdaptor instance
 *
 *  builtin_ZLIB() allocates memory to a ZLIB CompAdaptor instance. The
 *  ZLIB CompAdaptor is required for use in computing NCDs with the zlib
 *  compression library.
 *
 *  One option can be set for the ZLIB CompAdaptor: level. By default, level is
 *  set to 9.
 *
 *  builtin_ZLIB() will also read a CompLearn configuration file to override
 *  the option defaults. For details on how to create a configuration file, see
 *  http://www.complearn.org/config.html
 *
 *  \return pointer to newly initialized ZLIB CompAdaptor instance
 */
struct CompAdaptor *builtin_ZLIB(void)
{
	struct CompAdaptor c =
	{
    cptr: NULL,
//    se:   zlib_clsetenv,
    cf:   zlib_compfunc,
    fcf:  zlib_freecompfunc,
    sn:   zlib_shortname,
    ln:   zlib_longname,
    apiv: zlib_apiver
  };
  struct CompAdaptor *ca;
  struct ZlibCompInstance *zci;
  ca = gcalloc(sizeof(*ca), 1);
  *ca = c;
  ca->cptr = gcalloc(sizeof(struct ZlibCompInstance), 1);
  zci = (struct ZlibCompInstance *) ca->cptr;


  /* default compressor options */
	zci->level = 9;

  zlib_clsetenv(ca);

  return ca;
}

static void zlib_setIntValueMaybe(struct EnvMap *srcenv, const char *keyname, int *placeToSet) {
  char *val;
  val = readValForEM(srcenv,keyname);
  if (val)
    *placeToSet = atoi(val);
}

static void zlib_clsetenv(struct CompAdaptor *ca)
{
	struct ZlibCompInstance *ci = (struct ZlibCompInstance *) ca->cptr;
  struct EnvMap *em = loadDefaultEnvironment()->em;

  zlib_setIntValueMaybe(em, "zliblevel", &ci->level);
}

static double zlib_compfunc(struct CompAdaptor *ca, struct DataBlock src)
{
	struct ZlibCompInstance *ci = (struct ZlibCompInstance *) ca->cptr;
	int s;

  unsigned char *dbuff;
	int p;

	p = src.size*1.001 + 12;
	dbuff = (unsigned char*)gmalloc(p);
	s = compress2(dbuff,(uLongf *) &p,src.ptr,src.size,ci->level);

	if (s == Z_BUF_ERROR) {
		printf ("destLen not big enough!\n");
		exit(1);
	}
	if (s != Z_OK) {
		printf ("Unknown error: zlibBuff returned %d\n",s);
		exit(1);
	}
	free(dbuff);
	return (double) p*8.0;
}

static void zlib_freecompfunc(struct CompAdaptor *ca)
{
  gfreeandclear(ca->cptr);
	gfreeandclear(ca);
}

static char *zlib_shortname(void)
{
	return "zlib";
}

static char *zlib_longname(void)
{
	return "General purpose compression library zlib";
}

static int zlib_apiver(void)
{
	return APIVER_V1;
}

#else
struct CompAdaptor *builtin_ZLIB(void)
{
  return NULL;
}
#endif
