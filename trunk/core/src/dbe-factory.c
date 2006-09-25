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
#include <stdio.h>
#include <complearn/complearn.h>

struct DBEFactory {
  int mode;
};

struct DBEFactory *clBlockEnumerationFactoryNew(void)
{
  struct DBEFactory *dbf;
  dbf = clCalloc(sizeof(struct DBEFactory), 1);
  dbf->mode = DBF_MODE_FILE;
  return dbf;
}

void clBlockEnumerationFactoryFree(struct DBEFactory *dbf)
{
  dbf->mode = 0;
  clFreeandclear(dbf);
}

int clBlockEnumerationFactorySetMode(struct DBEFactory *dbf, int newMode)
{
  assert(newMode >= 1 && newMode <= DBF_MODE_MAX);
  dbf->mode = newMode;
  return 0;
}

int clBlockEnumerationFactoryGetMode(struct DBEFactory *dbf)
{
  return dbf->mode;
}

const char *clBlockEnumerationFactoryModeString(struct DBEFactory *dbf)
{
  switch (dbf->mode) {
    case DBF_MODE_QUOTED: return "quoted";
    case DBF_MODE_FILE: return "file";
    case DBF_MODE_FILELIST: return "filelist";
    case DBF_MODE_STRINGLIST: return "stringlist";
    case DBF_MODE_DIRECTORY: return "directory";
    case DBF_MODE_WINDOWED: return "windowed";
    default:
      clogError( "Bad mode %d\n", dbf->mode);
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
  db = clCalloc(sizeof(struct DataBlock *), 1);
#define WINDELIMS ","
  cstr = clStrdup(str);
  fname = strtok(cstr, WINDELIMS);
  assert(fname && "Must specify filename for window");
  db = clFileToDataBlockPtr(fname);
  lastpos = clDatablockSize(db) - 1;
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
  return clBlockEnumerationLoadWindowed(db, startpos, stepsize, width, lastpos);
}

struct DataBlockEnumeration *clBlockEnumerationFactoryNewDBE(struct DBEFactory *dbf, const char
 *str)
{
  struct DataBlock *db;
  switch (dbf->mode) {
    case DBF_MODE_QUOTED:
      db = clStringToDataBlockPtr(str);
      return clBlockEnumerationLoadSingleton(db);
    case DBF_MODE_FILE:
      db = clFileToDataBlockPtr(str);
      return clBlockEnumerationLoadSingleton(db);
    case DBF_MODE_FILELIST:
      return clBlockEnumerationLoadFileList(str);
    case DBF_MODE_STRINGLIST:
      return clBlockEnumerationLoadStringList(str);
    case DBF_MODE_DIRECTORY:
      return clBlockEnumerationLoadDirectory(str);
    case DBF_MODE_WINDOWED:
      return dbef_handleWindowedDBE(dbf, str);
    default:
      clogError( "Bad mode %d\n", dbf->mode);
      exit(1);
  }
}
