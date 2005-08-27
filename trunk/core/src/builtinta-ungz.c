#include <complearn/complearn.h>
#include <sys/stat.h>

#if ZLIB_RDY

#include <zlib.h>
#include <string.h>
#include <malloc.h>

#include <complearn/transadaptor.h>

#define MAX_BYTES_READ 1000

static char *ungz_shortname(void);
static void ungz_transfree(struct TransformAdaptor *ta);
static int ungz_predicate(struct DataBlock db);
static struct DataBlock ungz_transform(struct DataBlock src);

struct TransformAdaptor *builtin_UNGZ(void)
{
	struct TransformAdaptor t =
	{
    sn:    ungz_shortname,
    tfree: ungz_transfree,
    pf:    ungz_predicate,
    tf:    ungz_transform,
    tptr:  NULL,
  };
  struct TransformAdaptor *ptr;
  ptr = (struct TransformAdaptor*)gcalloc(sizeof(*ptr), 1);
  *ptr = t;
	return ptr;
}

static char *ungz_shortname(void)
{
	return "ungz";
}

static void ungz_transfree(struct TransformAdaptor *ta)
{
  gfreeandclear(ta);
}

static int ungz_predicate(struct DataBlock db)
{
	return db.size > 1 && db.ptr[0]==0x1f && db.ptr[1]==0x8b;
}

static struct DataBlock ungz_transform(struct DataBlock src)
{
  const char *dirs[] = { "/tmp","/temp", "."};
	struct DataBlock result;
	unsigned char *dbuff=NULL;
	int p = 0;
  int fd, i, err, written;
	FILE *fp;
	gzFile gzfp;
  struct stat sbuf;
	char tmpfile[1024];
  char pbuf[1024];

  for(i = 0; dirs[i]; i += 1) {
    sprintf(pbuf,"%s",dirs[i]);
     if (stat(pbuf, &sbuf) == 0)
       break;
  }

//	tmpfile = (char*)gstrdup("/tmp/clgztmp-XXXXXX");
  sprintf(tmpfile, "%s/clgztmp-XXXXXX", pbuf);
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
	result.size = p;
	result.ptr = gmalloc(result.size);
	memcpy(result.ptr,dbuff,result.size);
	free(dbuff);
	// datablockFree(src); /* TODO: document me */
	return result;
}
#else
struct TransformAdaptor *builtin_UNGZ(void)
{
  return NULL;
}
#endif
