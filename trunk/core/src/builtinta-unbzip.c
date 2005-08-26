#include <string.h>
#include <malloc.h>
#include <complearn/complearn.h>

#include <complearn/transadaptor.h>

#if BZIP2_RDY
#include <bzlib.h>

static char *unbz2a_shortname(void);
static void unbz2a_transfree(struct TransformAdaptor *ta);
static int unbz2a_predicate(struct DataBlock db);
static struct DataBlock unbz2a_transform(struct DataBlock src);

struct TransformAdaptor *builtin_UNBZIP(void)
{
  struct TransformAdaptor *ptr;
  struct TransformAdaptor t =
  {
    sn:    unbz2a_shortname,
    tfree: unbz2a_transfree,
    pf:    unbz2a_predicate,
    tf:    unbz2a_transform,
    tptr:  NULL,
  };
  ptr = (struct TransformAdaptor*)gcalloc(sizeof(*ptr), 1);
  *ptr = t;
  return ptr;
}

static char *unbz2a_shortname(void)
{
	return "unbzip";
}

static void unbz2a_transfree(struct TransformAdaptor *ta)
{
  gfreeandclear(ta);
}

static int unbz2a_predicate(struct DataBlock db)
{
	return db.size > 2 && db.ptr[0]==0x42 && db.ptr[1]==0x5a && db.ptr[2]==0x68;
}

static struct DataBlock unbz2a_transform(struct DataBlock src)
{
  struct DataBlock result;
#if BZIP2_RDY
  int i;
  unsigned char *dbuff = NULL;
  int p;
  p = src.size * 3.0 + 1;
  do {
    if (dbuff != NULL)
      gfreeandclear(dbuff);
    dbuff = (unsigned char*)gmalloc(p);
    i = BZ2_bzBuffToBuffDecompress((char *) dbuff,(unsigned int *) &p, (char *) src.ptr,src.size, 0, 0);
    p = 2*p;
  } while (i == BZ_OUTBUFF_FULL);
  result.size = p;
  result.ptr = (unsigned char*)gmalloc(result.size);
  memcpy(result.ptr,dbuff,result.size);
  gfreeandclear(dbuff);
  // freeDataBlock(src); /* TODO: document this */
# else
	assert ( 0 && "bzip not supported");
	exit(1);
	result.ptr = NULL;
	result.size = 0;
#endif
  return result;
}
#else

struct TransformAdaptor *builtin_UNBZIP(void)
{
  return NULL;
}

#endif
