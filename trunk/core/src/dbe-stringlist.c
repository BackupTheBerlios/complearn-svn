#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <malloc.h>

#include <complearn/complearn.h>

/** \brief Private enumeration instance for filelist enumeration.
 *  \struct DBEStringListEnumeration
 */
struct DBEStringListEnumeration
{
  char *filename;
};

/** \brief Iterator for a DBEStringListEnumeration.
 *  \struct DBEStringListEnumerationIterator
 */
#define MAXLABELSIZE 32

struct DBEStringListEnumerationIterator
{
  FILE *fp;
  char *linebuf;
  char curlabel[MAXLABELSIZE+1];
};

static void dbe_sl_istep(struct DataBlockEnumeration *dbe, struct DataBlockEnumerationIterator *dbi);

static struct DataBlockEnumerationIterator *dbe_sl_newenumiter(struct DataBlockEnumeration *ptr)
{
  struct DBEStringListEnumeration *dbe = (struct DBEStringListEnumeration *) (ptr->eptr);
  struct DBEStringListEnumerationIterator *dbi = gcalloc(sizeof(*dbi), 1);
  assert(dbi);
  dbi->fp = clfopen(dbe->filename, "rb");
  assert(dbi->fp);
  dbe_sl_istep(ptr, (struct DataBlockEnumerationIterator *) dbi);
  return (struct DataBlockEnumerationIterator *) dbi;
}

static void dbe_sl_iterfree(struct DataBlockEnumerationIterator *dbi)
{
  struct DBEStringListEnumerationIterator *fldbi = (struct DBEStringListEnumerationIterator *) dbi;
  gfreeifpresent(fldbi->linebuf);
  gfreeandclear(dbi);
}

static void dbe_sl_enumfree(struct DataBlockEnumeration *dbe)
{
  gfreeandclear(dbe->eptr);
  gfreeandclear(dbe);
}

static struct DataBlock *dbe_sl_istar(struct DataBlockEnumeration *dbe, struct DataBlockEnumerationIterator *dbi)
{
  struct DBEStringListEnumerationIterator *fldbi = (struct DBEStringListEnumerationIterator *) dbi;
  if (fldbi->linebuf) {
    struct DataBlock *db = gcalloc(sizeof(struct DataBlock), 1);
    *db = stringToDataBlock(fldbi->linebuf);
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
      while ((isspace(*ptr2) || *ptr2 == '\n' || *ptr2 == '\r') && ptr2 >= ptr)
      {
        *ptr2 = 0;
        ptr2--;
      }
      return strlen(ptr) ? ptr : NULL;
    }
  else
    return NULL;
}

static char *dbe_sl_ilabel(struct DataBlockEnumeration *dbe, struct DataBlockEnumerationIterator *dbi)
{
  struct DBEStringListEnumerationIterator *fldbi = (struct DBEStringListEnumerationIterator *) dbi;
  return fldbi->linebuf ? fldbi->curlabel : NULL;
}

static void dbe_sl_istep(struct DataBlockEnumeration *dbe, struct DataBlockEnumerationIterator *dbi)
{
  char linebuf[8192];
  char *goodstr;
  struct DBEStringListEnumerationIterator *fldbi = (struct DBEStringListEnumerationIterator *) dbi;
  linebuf[8191] = 0;
  /* Skip over lines starting with #, or just whitespace */
  for (;;) {
    if (fgets(linebuf, 8191, fldbi->fp)) {
      goodstr = convertLinebuf(linebuf);
      if (!goodstr)
        continue;
      goodstr = gstrdup(goodstr);
      gfreeifpresent(fldbi->linebuf);
      fldbi->linebuf = goodstr;
      strncpy(fldbi->curlabel, fldbi->linebuf, MAXLABELSIZE-1);
      fldbi->curlabel[MAXLABELSIZE] = 0;
    } else {
      fldbi->linebuf = NULL;
    }
    break;
  }
}

struct DataBlockEnumeration *dbeLoadStringList(const char *filename)
{
  struct DataBlockEnumeration c = {
    NULL, /* eptr, private enumeration instance */
    dbe_sl_newenumiter,
    dbe_sl_istep,
    dbe_sl_iterfree,
    dbe_sl_enumfree,
    dbe_sl_istar,
    dbe_sl_ilabel,
  };
  struct DataBlockEnumeration *dbe;
  struct DBEStringListEnumeration *fldbe;
  assert(filename);
  dbe = gcalloc(sizeof(struct DataBlockEnumeration),1);
  *dbe = c;
  dbe->eptr = gcalloc(sizeof(struct DBEStringListEnumeration), 1);
  fldbe = (struct DBEStringListEnumeration *) dbe->eptr;
  fldbe->filename = gstrdup(filename);
  assert(fldbe->filename);
  return dbe;
}
