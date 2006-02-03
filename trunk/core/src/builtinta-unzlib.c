#include <complearn/complearn.h>
#include <string.h>

#include <complearn/transadaptor.h>

static char *unzlib_shortname(void);
static void unzlib_transfree(struct TransformAdaptor *ta);
static int unzlib_predicate(struct DataBlock *db);
static struct DataBlock *unzlib_transform(struct DataBlock *src);

struct TransformAdaptor *clBuiltin_UNZLIB(void)
{
  struct ZlibDynamicAdaptor *zlib = clGrabZlibDA();
  struct TransformAdaptor *ptr;
	struct TransformAdaptor t =
	{
    sn:    unzlib_shortname,
    tfree: unzlib_transfree,
    pf:    unzlib_predicate,
    tf:    unzlib_transform,
    tptr:  NULL,
  };
  if (!zlib)
    return NULL;
  ptr = (struct TransformAdaptor*)clCalloc(sizeof(*ptr), 1);
  *ptr = t;
	return ptr;
}

static char *unzlib_shortname(void)
{
	return "unzlib";
}

static void unzlib_transfree(struct TransformAdaptor *ta)
{
  clFreeandclear(ta);
}

static int unzlib_predicate(struct DataBlock *db)
{
	return clDatablockSize(db) > 1 && clDatablockData(db)[0]==0x78 && clDatablockData(db)[1]==0xda;
}

static struct DataBlock *unzlib_transform(struct DataBlock *src)
{
  struct ZlibDynamicAdaptor *zlib = clGrabZlibDA();
	struct DataBlock *result;
  if (zlib) {
	int i;
	unsigned char *dbuff = NULL;
	int triedp;
	triedp = clDatablockSize(src) * 3.0 + 1;
	do {
    unsigned long p;
		if (dbuff != NULL)
			free(dbuff);
		dbuff = (unsigned char*)clMalloc(p);
    p = triedp;
		i = uncompress(dbuff, &p,clDatablockData(src),clDatablockSize(src));
		triedp = 2*triedp;
	} while (i == -5);  /* Z_BUF_ERROR */
  result = clDatablockNewFromBlock(dbuff, triedp);
	free(dbuff);
//	clDatablockFree(src); /* TODO: document this new non-free behavior */
   } else {
     assert ( 0 && "zlib not supported");
     exit(1);
   }
	return result;
}
