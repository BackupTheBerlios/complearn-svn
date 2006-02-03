#include <complearn/complearn.h>
#include <string.h>
#include <unistd.h>

static FILE *makeTmpCopyStdin(void)
{
  static char tmpfile[128];
  int fd;
  FILE *fp;
  if (tmpfile[0] == 0) {
    struct DataBlock *db = clFilePtrToDataBlockPtr(stdin);
    strcpy(tmpfile, "/tmp/clstdintmp-XXXXXX");
    fd = mkstemp(tmpfile);
    close(fd);
    clDatablockWriteToFile(db, tmpfile);
    clDatablockFreePtr(db);
  }
  fp = fopen(tmpfile,"r");
  return fp;
}

FILE *clFopen(const char *fname, char *mode)
{
  if (strcmp(fname,"-") == 0) {
    return (mode[0] == 'r' ? makeTmpCopyStdin() : stdout);
  }
  else
    return fopen(fname, mode);
}

void clFclose(FILE *fp)
{
  if ( (fp == stdin) || (fp == stdout) )
    return;
  else {
    fclose(fp);
    return;
  }
}
