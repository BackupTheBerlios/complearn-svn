#include <complearn/complearn.h>

#if ZLIB_RDY

#include <zlib.h>
#include <string.h>
#include <malloc.h>

#include <complearn/transadaptor.h>

#define MAX_BYTES_READ 1000

static char *ungz_shortname(void);
static void ungz_transfree(struct TransAdaptor *ta);
static int ungz_predicate(struct DataBlock db);
static struct DataBlock ungz_transform(struct DataBlock src);

struct TransAdaptor *builtin_UNGZ(void)
{
	struct TransAdaptor t =
	{
    sn:    ungz_shortname,
    tfree: ungz_transfree,
    pf:    ungz_predicate,
    tf:    ungz_transform,
    tptr:  NULL,
  };
  struct TransAdaptor *ptr;
  ptr = (struct TransAdaptor*)gcalloc(sizeof(*ptr), 1);
  *ptr = t;
	return ptr;
}

static char *ungz_shortname(void)
{
	return "ungz";
}

static void ungz_transfree(struct TransAdaptor *ta)
{
  gfreeandclear(ta);
}

static int ungz_predicate(struct DataBlock db)
{
	return db.size > 1 && db.ptr[0]==0x1f && db.ptr[1]==0x8b;
}

static struct DataBlock ungz_transform(struct DataBlock src)
{
	struct DataBlock result;
	unsigned char *dbuff=NULL;
	int p = 0;
  int fd;
	char *tmpfile;
	int err, written;
	FILE *fp;
	gzFile gzfp;
	tmpfile = (char*)gstrdup("/tmp/clgztmp-XXXXXX");
  fd = mkstemp(tmpfile);
  close(fd);
 	fp = clfopen(tmpfile,"wb");
  assert(fp != NULL && "Error opening tmp file!");
	written = fwrite(src.ptr,1,src.size,fp);
	if (written == 0) {
		exit(1);
	}
	fclose(fp);
	gzfp = gzopen(tmpfile,"rb");
	dbuff = (unsigned char*)gmalloc(MAX_BYTES_READ);
	do {
		err = gzread(gzfp,dbuff,MAX_BYTES_READ);
		if (err == Z_ERRNO) {
			printf("Error reading gz file\n");
			exit(1);
		}
		p += err;
  } while (err != 0);
	
	gzrewind(gzfp);
  free(dbuff);
  dbuff = gmalloc(p);
	err = gzread(gzfp,dbuff,p);
	if (err == Z_ERRNO) {
		printf("Error reading gz file\n");
		exit(1);
	}
	gzclose(gzfp);
	unlink(tmpfile);
	free(tmpfile);
	result.size = p;
	result.ptr = gmalloc(result.size);
	memcpy(result.ptr,dbuff,result.size);
	free(dbuff);
	// freeDataBlock(src); /* TODO: document me */
	return result;
}
#else
struct TransAdaptor *builtin_UNGZ(void)
{
  return NULL;
}
#endif
