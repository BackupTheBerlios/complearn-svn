#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <complearn/complearn.h>

#if GDBM_RDY

#include <complearn/gdbmhelper.h>
#include <stdlib.h>
#include "clalloc.h"

#include <gdbm.h>

struct GDBMHelper {
  GDBM_FILE db;
};

const char *getHomeDir(void)
{
  char *result;
  result = getenv("HOME");
  assert(result);
  return result;
}

datum convertDataBlockToDatum(struct DataBlock *d)
{
  datum gd;
  gd.dptr = (char *) datablockData(d);
  gd.dsize = datablockSize(d);
  return gd;
}

static char *makefilename(const char *userfilename)
{
  static char filename[1024];
  const char *dirname = "gdbm";
  sprintf(filename, "%s/.complearn", getHomeDir());
  mkdir(filename, 0775);
  sprintf(filename, "%s/.complearn/%s", getHomeDir(), dirname);
  mkdir(filename, 0775);
  sprintf(filename, "%s/.complearn/%s/%s", getHomeDir(), dirname, userfilename);
  return filename;
}

void cldbunlink(const char *userfilename)
{
  unlink(makefilename(userfilename));
}

static void printfunc(const char *str)
{
  clogError( "GDBM error: %s\n", str);
}

struct GDBMHelper *cldbopen(const char *userfilename)
{
  char *filename;
  struct GDBMHelper *gh;
  struct stat buf;
  filename = makefilename(userfilename);
  gh = clCalloc(sizeof(struct GDBMHelper), 1);
  if(stat(filename, &buf)) {
    GDBM_FILE db;
    db = gdbm_open(filename, 0, GDBM_WRCREAT, 0664, printfunc);
    gdbm_close(db);
  }
  gh->db = gdbm_open(filename, 0, GDBM_READER, 0664, printfunc);
  if (gh->db)
    return gh;
  clFree(gh);
  return NULL;
}

/* Allocates a new DataBlock and returns pointer to new DataBlock
 */
struct DataBlock *cldbfetch(struct GDBMHelper *gh, struct DataBlock *key)
{
  datum result;
  assert(gh);
  assert(gh->db);
  result = gdbm_fetch(gh->db, convertDataBlockToDatum(key));
  if (result.dptr)
    return datablockNewFromBlock(result.dptr, result.dsize);
  return NULL;
}

void cldbstore(struct GDBMHelper *gh, struct DataBlock *key, struct DataBlock *val)
{
  GDBM_FILE db;
  datum gkey;
  assert(gh);
  assert(gh->db);
  gkey.dptr = (char *) datablockData(key);
  while(1) {
    db = gdbm_open(makefilename("gsamp"), 0, GDBM_WRCREAT, 0664, printfunc);
    if (db)
      break;
    clSleepMillis(100);
  }
  gdbm_store(db,
      convertDataBlockToDatum(key),
      convertDataBlockToDatum(val),
      GDBM_REPLACE);
  gdbm_close(db);
}

int cldbclose(struct GDBMHelper *gh)
{
  assert(gh);
  assert(gh->db);
  gdbm_close(gh->db);
  gh->db = NULL;
  clFreeandclear(gh);
  return 0;
}

#else

#include <complearn/complearn.h>
struct GDBMHelper { int ignoreMe; };

struct GDBMHelper *cldbopen(const char *userfilename)
{
  assert("No GDBM installed." && 0);
}
int cldbclose(struct GDBMHelper *gh)
{
  assert("No GDBM installed." && 0);
}
struct DataBlock *cldbfetch(struct GDBMHelper *gh, struct DataBlock *key)
{
  assert("No GDBM installed." && 0);
}
void cldbstore(struct GDBMHelper *gh, struct DataBlock *key, struct DataBlock *val)
{
  assert("No GDBM installed." && 0);
}
void cldbunlink(const char *userfilename)
{
  assert("No GDBM installed." && 0);
}
#endif
