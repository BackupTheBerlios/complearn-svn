#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <malloc.h>

#include <complearn/complearn.h>

static char *fname;

/** \brief Private enumeration instance for directory enumeration.
 *  \struct DBEDirEnumeration
 */
struct DBEDirEnumeration
{
  char *name;
};

/** \brief Iterator for a DBEDirEnumeration.
 *  \struct DBEDirEnumerationIterator
 */
struct DBEDirEnumerationIterator
{
  DIR *cur;
  char *curfilename;
//  struct dirent *curdirent;
  char curlabel[1024];
};

static void dbe_dir_istep(struct DataBlockEnumeration *dbe, struct DataBlockEnumerationIterator *dbi);

static struct DataBlockEnumerationIterator *dbe_dir_newenumiter(struct DataBlockEnumeration *ptr)
{
  struct DBEDirEnumeration *dbe = (struct DBEDirEnumeration *) (ptr->eptr);
  struct DBEDirEnumerationIterator *dbi = clCalloc(sizeof(*dbi), 1);
  assert(dbi);
  dbi->cur = opendir(dbe->name);
  assert(dbi->cur);
  dbe_dir_istep(ptr, (struct DataBlockEnumerationIterator *) dbi);
  return (struct DataBlockEnumerationIterator *) dbi;
}

static void dbe_dir_iterfree(struct DataBlockEnumerationIterator *dbi)
{
  struct DBEDirEnumerationIterator *dirdbi = (struct DBEDirEnumerationIterator *) dbi;
  closedir(dirdbi->cur);
  dirdbi->cur = NULL;
  clFreeandclear(dbi);
}

static void dbe_dir_enumfree(struct DataBlockEnumeration *dbe)
{
  struct DBEDirEnumeration *dirdbe = (struct DBEDirEnumeration *) dbe->eptr;
  clFreeandclear(dirdbe->name);
  clFreeandclear(dbe->eptr);
  clFreeandclear(dbe);
}
static char *dbe_get_pathname(struct DataBlockEnumeration *dbe, struct DataBlockEnumerationIterator *dbi)
{
  struct DBEDirEnumeration *dirdbe = (struct DBEDirEnumeration *) dbe->eptr;
  struct DBEDirEnumerationIterator *dirdbi = (struct DBEDirEnumerationIterator *) dbi;
	if (fname) {
		clFreeandclear(fname);
  }
	fname = clMalloc(strlen(dirdbe->name)+strlen(dirdbi->curfilename)+2);
  sprintf(fname, "%s/%s", dirdbe->name, dirdbi->curfilename);
  return fname;
}

static void freePath(char *fname)
{
  clFree(fname);
}

static struct DataBlock *dbe_dir_istar(struct DataBlockEnumeration *dbe, struct DataBlockEnumerationIterator *dbi)
{
  struct DBEDirEnumerationIterator *dirdbi = (struct DBEDirEnumerationIterator *) dbi;
  if (dirdbi->curfilename) {
    struct DataBlock *db = clCalloc(sizeof(struct DataBlock), 1);
    *db = fileToDataBlock(dbe_get_pathname(dbe,dbi));
    return db;
  } else
    return NULL;
}

static char *dbe_dir_ilabel(struct DataBlockEnumeration *dbe, struct DataBlockEnumerationIterator *dbi)
{
  struct DBEDirEnumerationIterator *dirdbi = (struct DBEDirEnumerationIterator *) dbi;
  if (dbi == NULL || dirdbi->curfilename == NULL)
    return NULL;
  sprintf(dirdbi->curlabel, "%s", dirdbi->curfilename);
  return dirdbi->curlabel;
}

static void dbe_dir_istep(struct DataBlockEnumeration *dbe, struct DataBlockEnumerationIterator *dbi)
{
  struct stat st;
  int retval;
  struct DBEDirEnumerationIterator *dirdbi = (struct DBEDirEnumerationIterator *) dbi;
  struct dirent *drt = NULL;
  /* Skip over files starting with ., or unreadables */
  do {
    drt = readdir(dirdbi->cur);
    if (drt == NULL) {
      if (dirdbi->curfilename) {
        freePath(dirdbi->curfilename);
        dirdbi->curfilename = NULL;
      }
      return;
    }
    if (dirdbi->curfilename)
      freePath(dirdbi->curfilename);
    dirdbi->curfilename = clStrdup(drt->d_name);
    fname = dbe_get_pathname(dbe,dbi);
    if (clIsDirectory(fname)) {
      dirdbi->curfilename = clStrdup(".notit");
      continue;
    }
    retval = stat(fname, &st);
    if (retval)
      continue;
    if (!S_ISREG(st.st_mode) || st.st_size == 0)
      continue;
  } while (dirdbi->curfilename[0] == '.');
}

struct DataBlockEnumeration *dbeLoadDirectory(const char *dirname)
{
  struct DataBlockEnumeration c = {
    NULL, /* eptr, private enumeration instance */
    dbe_dir_newenumiter,
    dbe_dir_istep,
    dbe_dir_iterfree,
    dbe_dir_enumfree,
    dbe_dir_istar,
    dbe_dir_ilabel,
  };
  struct DataBlockEnumeration *dbe;
  struct DBEDirEnumeration *dirdbe;
  assert(dirname);
  dbe = clCalloc(sizeof(struct DataBlockEnumeration),1);
  *dbe = c;
  dbe->eptr = clCalloc(sizeof(struct DBEDirEnumeration), 1);
  dirdbe = (struct DBEDirEnumeration *) dbe->eptr;
  dirdbe->name = clStrdup(dirname);
  return dbe;
}

int clIsDirectory(const char *dirname)
{
  int result = 0;
  DIR *cur;
  cur = opendir(dirname);
  if (cur)
    result = 1;
  closedir(cur);
  return result;
}

