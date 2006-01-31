#include <string.h>
#include <complearn/complearn.h>

static char *unbz2a_shortname(void);
static void unbz2a_transfree(struct TransformAdaptor *ta);
static int unbz2a_predicate(struct DataBlock *db);
static struct DataBlock *unbz2a_transform(struct DataBlock *src);

struct TransformAdaptor *builtin_UNBZIP(void)
{
  struct BZ2DynamicAdaptor *bzlib = grabBZ2DA();
  struct TransformAdaptor *ptr;
  struct TransformAdaptor t =
  {
    sn:    unbz2a_shortname,
    tfree: unbz2a_transfree,
    pf:    unbz2a_predicate,
    tf:    unbz2a_transform,
    tptr:  NULL,
  };
  if (!bzlib)
    return NULL;
  ptr = (struct TransformAdaptor*)clCalloc(sizeof(*ptr), 1);
  *ptr = t;
  return ptr;
}

static char *unbz2a_shortname(void)
{
	return "unbzip";
}

static void unbz2a_transfree(struct TransformAdaptor *ta)
{
  clFreeandclear(ta);
}

static int unbz2a_predicate(struct DataBlock *db)
{
	return datablockSize(db) > 2 && datablockData(db)[0]==0x42 && datablockData(db)[1]==0x5a && datablockData(db)[2]==0x68;
}

static struct DataBlock *unbz2a_transform(struct DataBlock *src)
{
  struct BZ2DynamicAdaptor *bzlib = grabBZ2DA();
  struct DataBlock *result = NULL;
  if (bzlib) {
  int i;
  unsigned char *dbuff = NULL;
  int p;
  p = datablockSize(src) * 3.0 + 1;
  do {
    if (dbuff != NULL)
      clFreeandclear(dbuff);
    dbuff = (unsigned char*)clMalloc(p);
    i = (bzlib->buftobufdecompress)((char *) dbuff,(unsigned int *) &p, (char *) datablockData(src),datablockSize(src), 0, 0);
    p = 2*p;
  } while (i != 0);
  result = datablockNewFromBlock(dbuff,p);
  clFreeandclear(dbuff);
  // datablockFree(src); /* TODO: document this */
   } else {
	assert ( 0 && "bzip not supported");
	exit(1);
   }
  return result;
}

