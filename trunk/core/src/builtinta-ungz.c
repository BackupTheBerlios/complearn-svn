/*
* Copyright (c) 2006 Rudi Cilibrasi, Rulers of the RHouse
* All rights reserved.     cilibrar@cilibrar.com
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the RHouse nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE RULERS AND CONTRIBUTORS "AS IS" AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE RULERS AND CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <complearn/complearn.h>
#include <sys/stat.h>
#include <string.h>

#if HAVE_ZLIB_H
#include <zlib.h>

#define MAX_BYTES_READ 1000

static char *ungz_shortname(void);
static void ungz_transfree(struct TransformAdaptor *ta);
static int ungz_predicate(struct DataBlock *db);
static struct DataBlock *ungz_transform(struct DataBlock *src);

struct TransformAdaptor *clBuiltin_UNGZ(void)
{
  struct TransformAdaptor *ptr;
  struct TransformAdaptor t =
  {
    sn:    ungz_shortname,
    tfree: ungz_transfree,
    pf:    ungz_predicate,
    tf:    ungz_transform,
    tptr:  NULL,
  };
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
	return clDatablockSize(db) > 1 && clDatablockData(db)[0]==0x1f && clDatablockData(db)[1]==0x8b;
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

  dirs[0] = getLikelyTmpPrefix();

  for(i = 0; dirs[i]; i += 1) {
    sprintf(pbuf,"%s",dirs[i]);
     if (stat(pbuf, &sbuf) == 0)
       break;
  }

  sprintf(tmpfile, "%s/clgztmp-XXXXXX", pbuf);
  fd = mkstemp(tmpfile);
  close(fd);
 	fp = clFopen(tmpfile,"wb");
	written = fwrite(clDatablockData(src),1,clDatablockSize(src),fp);
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
  result = clDatablockNewFromBlock(dbuff,p);
	free(dbuff);
	// clDatablockFree(src); /* TODO: document me */
	return result;
}
#else
struct TransformAdaptor *clBuiltin_UNGZ(void)
{
  return NULL;
}
#endif
