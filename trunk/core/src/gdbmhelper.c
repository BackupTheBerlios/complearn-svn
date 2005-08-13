#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <complearn/complearn.h>

#if GDBM_RDY

#include <complearn/gdbmhelper.h>
#include <stdlib.h>
#include <malloc.h>

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

datum convertDataBlockToDatum(struct DataBlock d)
{
  datum gd;
  gd.dptr = (char *) d.ptr;
  gd.dsize = d.size;
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

struct GDBMHelper *cldbopen(const char *userfilename)
{
  char *filename;
  struct GDBMHelper *gh;
  filename = makefilename(userfilename);
  gh = gcalloc(sizeof(struct GDBMHelper), 1);
  gh->db = gdbm_open(filename, 0, GDBM_WRCREAT, 0664, 0);
  return gh;
}

/* Allocates a new DataBlock and returns pointer to new DataBlock
 */
struct DataBlock *cldbfetch(struct GDBMHelper *gh, struct DataBlock key)
{
  datum result;
  result = gdbm_fetch(gh->db, convertDataBlockToDatum(key));
  if (result.dptr) {
    struct DataBlock *dbr = gcalloc(sizeof(struct DataBlock), 1);
    dbr->ptr = (unsigned char *) result.dptr;
    dbr->size = result.dsize;
    return dbr;
  }
  return NULL;
}

struct DataBlock *cldbFetchString(struct GDBMHelper *gh, const char *str)
{
  struct DataBlock db, *result;
  db = convertStringToDataBlock("dog");
  result = cldbfetch(gh, db);
  freeDataBlock(db);
  return result;
}

void cldbstore(struct GDBMHelper *gh, struct DataBlock key, struct DataBlock val)
{
  datum gkey;
  gkey.dptr = (char *) key.ptr;
  gdbm_store(gh->db,
      convertDataBlockToDatum(key),
      convertDataBlockToDatum(val),
      GDBM_REPLACE);
}
void cldbStoreStrings(struct GDBMHelper *gh, const char *s1, const char *s2)
{
  struct DataBlock db1, db2;
  db1 = convertStringToDataBlock(s1);
  db2 = convertStringToDataBlock(s2);
  cldbstore(gh, db1, db2);
  freeDataBlock(db1);
  freeDataBlock(db2);
}

int cldbclose(struct GDBMHelper *gh)
{
  gdbm_close(gh->db);
  gh->db = NULL;
  gfreeandclear(gh);
  return 0;
}

int cldbdelete(struct GDBMHelper *gh, struct DataBlock key)
{
  return gdbm_delete(gh->db, convertDataBlockToDatum(key));
}

void cldbDeleteString(struct GDBMHelper *gh, const char *str)
{
  struct DataBlock db;
  db = convertStringToDataBlock(str);
  cldbdelete(gh, db);
  freeDataBlock(db);
}

#else

#include <complearn/complearn.h>
struct GDBMHelper { int ignoreMe; };

int cldbdelete(struct GDBMHelper *gh, struct DataBlock key)
{
  assert("No GDBM installed." && 0);
}
struct GDBMHelper *cldbopen(const char *userfilename)
{
  assert("No GDBM installed." && 0);
}
int cldbclose(struct GDBMHelper *gh)
{
  assert("No GDBM installed." && 0);
}
struct DataBlock *cldbfetch(struct GDBMHelper *gh, struct DataBlock key)
{
  assert("No GDBM installed." && 0);
}
void cldbstore(struct GDBMHelper *gh, struct DataBlock key, struct DataBlock val)
{
  assert("No GDBM installed." && 0);
}
void cldbunlink(const char *userfilename)
{
  assert("No GDBM installed." && 0);
}
#endif
