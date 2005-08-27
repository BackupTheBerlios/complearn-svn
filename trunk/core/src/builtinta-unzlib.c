#include <complearn/complearn.h>
#if ZLIB_RDY
#include <zlib.h>
#include <malloc.h>
#include <string.h>

#include <complearn/transadaptor.h>

static char *unzlib_shortname(void);
static void unzlib_transfree(struct TransformAdaptor *ta);
static int unzlib_predicate(struct DataBlock db);
static struct DataBlock unzlib_transform(struct DataBlock src);

struct TransformAdaptor *builtin_UNZLIB(void)
{
	struct TransformAdaptor t =
	{
    sn:    unzlib_shortname,
    tfree: unzlib_transfree,
    pf:    unzlib_predicate,
    tf:    unzlib_transform,
    tptr:  NULL,
  };
  struct TransformAdaptor *ptr;
  ptr = (struct TransformAdaptor*)gcalloc(sizeof(*ptr), 1);
  *ptr = t;
	return ptr;
}

static char *unzlib_shortname(void)
{
	return "unzlib";
}

static void unzlib_transfree(struct TransformAdaptor *ta)
{
  gfreeandclear(ta);
}

static int unzlib_predicate(struct DataBlock db)
{
	return db.size > 1 && db.ptr[0]==0x78 && db.ptr[1]==0xda;
}

static struct DataBlock unzlib_transform(struct DataBlock src)
{
	struct DataBlock result;
	int i;
	unsigned char *dbuff = NULL;
	int triedp;
	triedp = src.size * 3.0 + 1;
	do {
    int p;
		if (dbuff != NULL)
			free(dbuff);
		dbuff = (unsigned char*)gmalloc(p);
    p = triedp;
		i = uncompress(dbuff,(uLongf *) &p,src.ptr,src.size);
		triedp = 2*triedp;
	} while (i == Z_BUF_ERROR);
	result.size = triedp;
	result.ptr = (unsigned char*)gmalloc(result.size);
	memcpy(result.ptr,dbuff,result.size);
	free(dbuff);
//	datablockFree(src); /* TODO: document this new non-free behavior */
	return result;
}
#else
struct TransformAdaptor *builtin_UNZLIB(void)
{
  return NULL;
}
#endif
