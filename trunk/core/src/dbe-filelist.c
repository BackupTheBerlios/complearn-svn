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
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>


#include <complearn/complearn.h>

/** \brief Private enumeration instance for filelist enumeration.
 *
 * This DBE enumerates through a list of filenames.
 *
 *  \struct DBEFileListEnumeration
 */
struct DBEFileListEnumeration
{
  char *filename;
};

/** \brief Iterator for a DBEFileListEnumeration.
 *
 * This DBE iterator enumerates through a list of filenames.
 *
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
  clAssert(dbi != NULL);
  dbi->fp = clFopen(dbe->filename, "rb");
  if (dbi->fp == NULL)
    clLogError("Cannot open %s\n", dbe->filename?dbe->filename:"(no filename)");
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
    db = clFileToDataBlockPtr(fldbi->linebuf);
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

struct DataBlockEnumeration *clBlockEnumerationLoadFileList(const char *filename)
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
  clAssert(filename != NULL);
  dbe = clCalloc(sizeof(struct DataBlockEnumeration),1);
  *dbe = c;
  dbe->eptr = clCalloc(sizeof(struct DBEFileListEnumeration), 1);
  fldbe = (struct DBEFileListEnumeration *) dbe->eptr;
  fldbe->filename = clStrdup(filename);
  clAssert(fldbe->filename != NULL);
  return dbe;
}
