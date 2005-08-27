#include <assert.h>
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <complearn/complearn.h>

struct DBEFactory {
  int mode;
};

struct DBEFactory *dbefactoryNew(void)
{
  struct DBEFactory *dbf;
  dbf = gcalloc(sizeof(struct DBEFactory), 1);
  dbf->mode = DBF_MODE_FILE;
  return dbf;
}

void dbefactoryFree(struct DBEFactory *dbf)
{
  dbf->mode = 0;
  gfreeandclear(dbf);
}

int dbefactorySetMode(struct DBEFactory *dbf, int newMode)
{
  assert(newMode >= 1 && newMode <= DBF_MODE_MAX);
  dbf->mode = newMode;
  return 0;
}

int dbefactoryGetMode(struct DBEFactory *dbf)
{
  return dbf->mode;
}

const char *dbefactoryModeString(struct DBEFactory *dbf)
{
  switch (dbf->mode) {
    case DBF_MODE_QUOTED: return "quoted";
    case DBF_MODE_FILE: return "file";
    case DBF_MODE_FILELIST: return "filelist";
    case DBF_MODE_STRINGLIST: return "stringlist";
    case DBF_MODE_DIRECTORY: return "directory";
    case DBF_MODE_WINDOWED: return "windowed";
    default:
      fprintf(stderr, "Bad mode %d\n", dbf->mode);
      exit(1);
  }
  return "(error)";
}
static struct DataBlockEnumeration *dbef_handleWindowedDBE(struct DBEFactory *dbf, const char *str)
{
  char *fname = NULL;
  int stepsize = 1;
  int width = 1;
  int startpos = 0;
  int lastpos = 0;
  char *cstr, *cur;
  struct DataBlock *db;
  db = gcalloc(sizeof(struct DataBlock), 1);
#define WINDELIMS ","
  cstr = gstrdup(str);
  fname = strtok(cstr, WINDELIMS);
  assert(fname && "Must specify filename for window");
  *db = fileToDataBlock(fname);
  lastpos = db->size - 1;
  cur = strtok(NULL, WINDELIMS);
  if (cur) {
    width = atoi(cur);
    stepsize = width;
    cur = strtok(NULL, WINDELIMS);
    if (cur) {
      stepsize = atoi(cur);
      cur = strtok(NULL, WINDELIMS);
      if (cur) {
        startpos = atoi(cur);
        cur = strtok(NULL, WINDELIMS);
        if (cur) {
          lastpos = atoi(cur);
        }
      }
    }
  }
  return dbeLoadWindowed(db, startpos, stepsize, width, lastpos);
}

struct DataBlockEnumeration *dbefactoryNewDBE(struct DBEFactory *dbf, const char
 *str)
{
  struct DataBlock *db;
  switch (dbf->mode) {
    case DBF_MODE_QUOTED:
      db = gcalloc(sizeof(struct DataBlock), 1);
      *db = stringToDataBlock(gstrdup(str));
      return dbeLoadSingleton(db);
    case DBF_MODE_FILE:
      db = gcalloc(sizeof(struct DataBlock), 1);
      *db = fileToDataBlock(str);
      return dbeLoadSingleton(db);
    case DBF_MODE_FILELIST:
      return dbeLoadFileList(str);
    case DBF_MODE_STRINGLIST:
      return dbeLoadStringList(str);
    case DBF_MODE_DIRECTORY:
      return dbeLoadDirectory(str);
    case DBF_MODE_WINDOWED:
      return dbef_handleWindowedDBE(dbf, str);
    default:
      fprintf(stderr, "Bad mode %d\n", dbf->mode);
      exit(1);
  }
}
