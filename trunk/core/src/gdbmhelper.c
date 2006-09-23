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
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <complearn/complearn.h>

#if HAVE_GDBM_H

#include <complearn/gdbmhelper.h>
#include <stdlib.h>

#include <gdbm.h>

struct GDBMHelper {
//  GDBM_FILE db;
  char *filename;
};

const char *clGetHomeDir(void)
{
  char *result;
  result = getenv("HOME");
  assert(result);
  return result;
}

datum clConvertDataBlockToDatum(struct DataBlock *d)
{
  datum gd;
  gd.dptr = (char *) clDatablockData(d);
  gd.dsize = clDatablockSize(d);
  return gd;
}

static char *makefilename(const char *userfilename)
{
  static char filename[1024];
  const char *dirname = "gdbm";
  sprintf(filename, "%s/.complearn", clGetHomeDir());
  mkdir(filename, 0775);
  sprintf(filename, "%s/.complearn/%s", clGetHomeDir(), dirname);
  mkdir(filename, 0775);
  sprintf(filename, "%s/.complearn/%s/%s", clGetHomeDir(), dirname, userfilename);
  return filename;
}

void cldbunlink(const char *userfilename)
{
  unlink(makefilename(userfilename));
}

static void printclFunc(const char *str)
{
  clogError( "GDBM error: %s\n", str);
}

struct GDBMHelper *cldbopen(const char *userfilename)
{
  struct GDBMHelper *gh;
  struct stat buf;
  gh = clCalloc(sizeof(struct GDBMHelper), 1);
  gh->filename = clStrdup(makefilename(userfilename));
  if(stat(gh->filename, &buf)) {
    GDBM_FILE db;
    db = gdbm_open(gh->filename, 0, GDBM_WRCREAT, 0664, printclFunc);
    gdbm_close(db);
  }
//  gh->db = gdbm_open(gh->filename, 0, GDBM_READER | GDBM_SYNC, 0664, printclFunc);
//  if (gh->db)
  return gh;
//  clFree(gh);
//  return NULL;
}

/* Allocates a new DataBlock and returns pointer to new DataBlock
 */
struct DataBlock *cldbfetch(struct GDBMHelper *gh, struct DataBlock *key)
{
  GDBM_FILE db;
  datum result;
  result.dptr = NULL;
  assert(gh);
//  assert(gh->db);
  db = gdbm_open(gh->filename, 0, GDBM_READER, 0664, printclFunc);
  assert(db);
  result = gdbm_fetch(db, clConvertDataBlockToDatum(key));
//  clogWarning("KEY<%s:%d>FETCH to %p:%d\n", clDatablockToString(key), clDatablockSize(key),  result.dptr, result.dsize);
  gdbm_close(db);
  if (result.dptr) {
    return clDatablockNewFromBlock(result.dptr, result.dsize);
  }
  return NULL;
}

void cldbstore(struct GDBMHelper *gh, struct DataBlock *key, struct DataBlock *val)
{
  GDBM_FILE db;
  assert(gh);
  //assert(gh->db);
// clogWarning("KEY<%s:%d>STORE value size <%d>\n", clDatablockToString(key), clDatablockSize(key), clDatablockSize(val));
  while(1) {
    db = gdbm_open(gh->filename, 0, GDBM_WRCREAT | GDBM_SYNC, 0664, printclFunc);
    if (db)
      break;
    clSleepMillis(100);
  }
  gdbm_store(db,
      clConvertDataBlockToDatum(key),
      clConvertDataBlockToDatum(val),
      GDBM_REPLACE);
  gdbm_close(db);
}

int cldbclose(struct GDBMHelper *gh)
{
  assert(gh);
  //assert(gh->db);
  //gdbm_close(gh->db);
  //gh->db = NULL;
  clFree(gh->filename);
  clFreeandclear(gh);
  return 0;
}

int cldbIsGDBMProblem(void)
{
	return 0;
}

const char *cldbReasonWhy(void)
{
	return NULL;
}

#else

#include <complearn/complearn.h>

int cldbIsGDBMProblem(void)
{
	return 1;
}

const char *cldbReasonWhy(void)
{
	return "Cannot find #include <gdbm.h>";
}

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
