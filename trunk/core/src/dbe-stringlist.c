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
 * This DBE enumerates through a list of files.
 *
 *  \struct DBEStringListEnumeration
 */
struct DBEStringListEnumeration
{
  char *filename;
};

/** \brief Iterator for a DBEStringListEnumeration.
 *
 * This DBE enumerates through a list of literal strings.
 *
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
  struct DBEStringListEnumerationIterator *dbi = clCalloc(sizeof(*dbi), 1);
  assert(dbi);
  dbi->fp = clFopen(dbe->filename, "rb");
  if (dbi->fp == NULL) {
    clLogErrorPrintf("Cannot open stringlist file %s", dbe->filename);
  }
  dbe_sl_istep(ptr, (struct DataBlockEnumerationIterator *) dbi);
  return (struct DataBlockEnumerationIterator *) dbi;
}

static void dbe_sl_iterfree(struct DataBlockEnumerationIterator *dbi)
{
  struct DBEStringListEnumerationIterator *fldbi = (struct DBEStringListEnumerationIterator *) dbi;
  clFreeifpresent(fldbi->linebuf);
  clFreeandclear(dbi);
}

static void dbe_sl_enumfree(struct DataBlockEnumeration *dbe)
{
  clFreeandclear(dbe->eptr);
  clFreeandclear(dbe);
}

static struct DataBlock *dbe_sl_istar(struct DataBlockEnumeration *dbe, struct DataBlockEnumerationIterator *dbi)
{
  struct DBEStringListEnumerationIterator *fldbi = (struct DBEStringListEnumerationIterator *) dbi;
  if (fldbi->linebuf) {
    struct DataBlock *db;
    db = clStringToDataBlockPtr(fldbi->linebuf);
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
  ptr = linebuf; /* TODO: check Unicode handling here, is \n etc bad? */
  char *ptr2 = ptr + strlen(ptr) - 1;
  while ((*ptr2 == '\n' || *ptr2 == '\r') && ptr2 >= ptr)
  {
    *ptr2 = 0;
    ptr2--;
  }
  return strlen(ptr) ? ptr : NULL;
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
      goodstr = clStrdup(goodstr);
      clFreeifpresent(fldbi->linebuf);
      fldbi->linebuf = goodstr;
      strncpy(fldbi->curlabel, fldbi->linebuf, MAXLABELSIZE-1);
      fldbi->curlabel[MAXLABELSIZE] = 0;
    } else {
      fldbi->linebuf = NULL;
    }
    break;
  }
}

struct DataBlockEnumeration *clBlockEnumerationLoadStringList(const char *filename)
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
  dbe = clCalloc(sizeof(struct DataBlockEnumeration),1);
  *dbe = c;
  dbe->eptr = clCalloc(sizeof(struct DBEStringListEnumeration), 1);
  fldbe = (struct DBEStringListEnumeration *) dbe->eptr;
  fldbe->filename = clStrdup(filename);
  assert(fldbe->filename);
  return dbe;
}
