#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include "clalloc.h"

#include <complearn/complearn.h>

/** \brief Private enumeration instance for filelist enumeration.
 *  \struct DBEFileListEnumeration
 */
struct DBEFileListEnumeration
{
  char *filename;
};

/** \brief Iterator for a DBEFileListEnumeration.
 *  \struct DBEFileListEnumerationIterator
 */
struct DBEFileListEnumerationIterator
{
  FILE *fp;
  char *linebuf;
};

static void dbe_fl_istep(struct DataBlockEnumeration *dbe, struct DataBlockEnumerationIterator *dbi);

static struct DataBlockEnumerationIterator *dbe_fl_newenumiter(struct DataBlockEnumeration *ptr)
{
  struct DBEFileListEnumeration *dbe = (struct DBEFileListEnumeration *) (ptr->eptr);
  struct DBEFileListEnumerationIterator *dbi = clCalloc(sizeof(*dbi), 1);
  assert(dbi);
  dbi->fp = clFopen(dbe->filename, "rb");
  dbe_fl_istep(ptr, (struct DataBlockEnumerationIterator *) dbi);
  return (struct DataBlockEnumerationIterator *) dbi;
}

static void dbe_fl_iterfree(struct DataBlockEnumerationIterator *dbi)
{
  struct DBEFileListEnumerationIterator *fldbi = (struct DBEFileListEnumerationIterator *) dbi;
  clFreeifpresent(fldbi->linebuf);
  clFreeandclear(dbi);
}

static void dbe_fl_enumfree(struct DataBlockEnumeration *dbe)
{
  clFreeandclear(dbe->eptr);
  clFreeandclear(dbe);
}

static char *dbe_fl_ilabel(struct DataBlockEnumeration *dbe, struct DataBlockEnumerationIterator *dbi)
{
  struct DBEFileListEnumerationIterator *fldbi = (struct DBEFileListEnumerationIterator *) dbi;
  return fldbi->linebuf;
}

static struct DataBlock *dbe_fl_istar(struct DataBlockEnumeration *dbe, struct DataBlockEnumerationIterator *dbi)
{
  struct DBEFileListEnumerationIterator *fldbi = (struct DBEFileListEnumerationIterator *) dbi;
  if (fldbi->linebuf) {
    struct DataBlock *db;
    db = fileToDataBlockPtr(fldbi->linebuf);
    return db;
  } else
    return NULL;
}

static char *convertLinebuf(char *linebuf)
{
  char *ptr;
  ptr = linebuf;
  while (*ptr) {
    if (*ptr == '#') {
      *ptr = 0;
      break;
    }
    ptr += 1;
  }
  ptr = linebuf;
  while (isspace(*ptr))
    ptr++;
  if (isprint(*ptr))
    {
      char *ptr2 = ptr + strlen(ptr) - 1;
      while (isspace(*ptr2) && ptr2 > ptr)
        *ptr2-- = 0;
      return strlen(ptr) ? ptr : NULL;
    }
  else
    return NULL;
}

static void dbe_fl_istep(struct DataBlockEnumeration *dbe, struct DataBlockEnumerationIterator *dbi)
{
  char linebuf[8192];
  char *goodstr;
  struct DBEFileListEnumerationIterator *fldbi = (struct DBEFileListEnumerationIterator *) dbi;
  linebuf[8191] = 0;
  /* Skip over lines starting with #, or just whitespace */
  for (;;) {
    if (fgets(linebuf, 8191, fldbi->fp)) {
      goodstr = convertLinebuf(linebuf);
      if (!goodstr)
        continue;
      goodstr = clStrdup(goodstr);
      clFreeifpresent(fldbi->linebuf);
      fldbi->linebuf = goodstr;
    } else {
      fldbi->linebuf = NULL;
    }
    break;
  }
}

struct DataBlockEnumeration *dbeLoadFileList(const char *filename)
{
  struct DataBlockEnumeration c = {
    NULL, /* eptr, private enumeration instance */
    dbe_fl_newenumiter,
    dbe_fl_istep,
    dbe_fl_iterfree,
    dbe_fl_enumfree,
    dbe_fl_istar,
    dbe_fl_ilabel,
  };
  struct DataBlockEnumeration *dbe;
  struct DBEFileListEnumeration *fldbe;
  assert(filename);
  dbe = clCalloc(sizeof(struct DataBlockEnumeration),1);
  *dbe = c;
  dbe->eptr = clCalloc(sizeof(struct DBEFileListEnumeration), 1);
  fldbe = (struct DBEFileListEnumeration *) dbe->eptr;
  fldbe->filename = clStrdup(filename);
  assert(fldbe->filename);
  return dbe;
}
