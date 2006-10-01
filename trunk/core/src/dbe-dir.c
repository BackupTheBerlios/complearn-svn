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
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#if WINCHOICE
#include <sys/dirent.h>
#else
#include <dirent.h>
#endif
#include <stdio.h>


#include <complearn/complearn.h>

static char *fname;

/** \brief Private enumeration instance for directory enumeration.
 *
 *  \struct DBEDirEnumeration
 */
struct DBEDirEnumeration
{
  char *name;
};

/** \brief Iterator for a DBEDirEnumeration.
 *
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
  assert(dbi != NULL);
  dbi->cur = opendir(dbe->name);
  assert(dbi->cur != NULL);
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
    struct DataBlock *db;
    db = clFileToDataBlockPtr(dbe_get_pathname(dbe,dbi));
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

struct DataBlockEnumeration *clBlockEnumerationLoadDirectory(const char *dirname)
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
  assert(dirname != NULL);
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
  if (cur) {
    result = 1;
    closedir(cur);
    cur = NULL;
  }
  return result;
}

