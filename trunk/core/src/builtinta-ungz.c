#include <complearn/complearn.h>
#include <sys/stat.h>

#if ZLIB_RDY

#include <zlib.h>
#include <string.h>
#include "clalloc.h"

#include <complearn/transadaptor.h>

#define MAX_BYTES_READ 1000

static char *ungz_shortname(void);
static void ungz_transfree(struct TransformAdaptor *ta);
static int ungz_predicate(struct DataBlock *db);
static struct DataBlock *ungz_transform(struct DataBlock *src);

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
  ptr = (struct TransformAdaptor*)clCalloc(sizeof(*ptr), 1);
  *ptr = t;
	return ptr;
}

static char *ungz_shortname(void)
{
	return "ungz";
}

static void ungz_transfree(struct TransformAdaptor *ta)
{
  clFreeandclear(ta);
}

static int ungz_predicate(struct DataBlock *db)
{
	return datablockSize(db) > 1 && datablockData(db)[0]==0x1f && datablockData(db)[1]==0x8b;
}

static struct DataBlock *ungz_transform(struct DataBlock *src)
{
  const char *dirs[] = { "/tmp","/temp", "."};
	struct DataBlock *result;
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

//	tmpfile = (char*)clStrdup("/tmp/clgztmp-XXXXXX");
  sprintf(tmpfile, "%s/clgztmp-XXXXXX", pbuf);
  fd = mkstemp(tmpfile);
  close(fd);
 	fp = clFopen(tmpfile,"wb");
	written = fwrite(datablockData(src),1,datablockSize(src),fp);
	if (written == 0) {
		exit(1);
	}
	fclose(fp);
	gzfp = gzopen(tmpfile,"rb");
	dbuff = (unsigned char*)clMalloc(MAX_BYTES_READ);
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
  dbuff = clMalloc(p);
	err = gzread(gzfp,dbuff,p);
	if (err == Z_ERRNO) {
		printf("Error reading gz file\n");
		exit(1);
	}
	gzclose(gzfp);
	unlink(tmpfile);
  result = datablockNewFromBlock(dbuff,p);
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
