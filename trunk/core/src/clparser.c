#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include <complearn/complearn.h>

#define COMPLEARNDIR ".complearn"
#define CONFIGNAME "config.yml"

static char *readHomeVar(void)
{
  return getenv("HOME");
}
struct StringStack *getDefaultFileList(void)
{
  struct StringStack *ss;
  char *homedir = clCalloc(1, 2048);
  char *homeenv;
  ss = stringstackNew();
  stringstackPush(ss, SYSTEMDATAFILE);
  homeenv = readHomeVar();
  if (homeenv) {
    sprintf(homedir, "%s/%s/%s", homeenv, COMPLEARNDIR, CONFIGNAME);
    stringstackPush(ss, homedir);
  }
  free(homedir);
  return ss;
}

int doesFileExist(const char *fname)
{
  FILE *fp;
  fp = clFopen(fname, "rb");
  if (fp)
    clFclose(fp);
  return fp ? 1 : 0;
}

int readDefaultConfig(struct EnvMap *dest)
{
  struct StringStack *d = getDefaultFileList();
  assert(dest);
  assert(d);
  assert(stringstackSize(d) > 0);
  assert(stringstackSize(d) < 10);

  while (!stringstackIsEmpty(d)) {
    char *str = shiftSS(d);
    if (doesFileExist(str))
      readSpecificFile(dest, str);
    clFreeandclear(str);
  }
  stringstackFree(d);
  return CL_OK;
}

/* returns 1 iff there is a valid string in dst, otherwise 0 */
static int filterVal(const char *src, char *dst)
{
  int len, rs = 0, ei;
  int lc;
  len = strlen(src);
  if (len < 1)
    return 0;
  while (isspace(src[rs]))
    rs += 1;
  ei = len-1;
  while (isspace(src[ei]))
    ei -= 1;
  lc = src[ei];
  if ((lc == '"' || lc == '\'') && lc == src[rs]) {
    ei -= 1;
    rs += 1;
  }
  if (ei < rs)
    return 0;
  memcpy(dst, src+rs, ei-rs+1);
  dst[ei-rs+1] = '\0';
  return 1;
}

#define DELIMS "=:\r\n"

void handleLine(struct EnvMap *dest, const char *uline)
{
  char *key = NULL, *val = NULL;
  char *kbuf, *vbuf;
  char *line = clStrdup(uline);
  key = strtok(line, DELIMS);
  if (key == NULL)
    return;
  kbuf = key;
  val = strtok(NULL, DELIMS);
  if (val == NULL)
    return;
  vbuf = val;
  if (filterVal(key, kbuf)) {
    if (filterVal(val, vbuf)) {
      envmapSetKeyVal(dest, kbuf, vbuf);
    }
  }
  clFreeandclear(line);
}

/* Returns CL_OK or CL_ERRBADFILE if it cannot read the given file. */
int readSpecificFile(struct EnvMap *dest, const char *fname)
{
#define MAXLINESIZE 1024
  char linebuf[MAXLINESIZE];
  FILE *fp;
  fp = clFopen(fname, "rb");
  assert(fp);
  while (fgets(linebuf, MAXLINESIZE, fp)) {
    handleLine(dest, linebuf);
  }
  clFclose(fp);
  return CL_OK;
}
