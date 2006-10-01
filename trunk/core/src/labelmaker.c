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
#include <string.h>
#include <complearn/complearn.h>

/* TODO: needs new home; does not belong here */
struct DataBlock *clStringStackDumpTagged(struct StringStack *ss, int tagnum)
{
  struct DataBlock *db, *dblabels;
  db = clStringstackDump(ss);
  dblabels = clPackageDataBlocks(tagnum,db,NULL);
  clDatablockFreePtr(db);
  return dblabels;
}

/* TODO: needs new home; does not belong here */
struct DataBlock *clCommandsDump(struct StringStack *ss)
{
  return clStringStackDumpTagged(ss, TAGNUM_COMMANDS);
}

struct DataBlock *clLabelsDump(struct StringStack *ss)
{
  return clStringStackDumpTagged(ss, TAGNUM_DMLABELS);
}

/* TODO: needs new home; does not belong here */
struct StringStack *clLoadTaggedStringStack(struct DataBlock *db, int fmustbe, const char *tagname, int tagnum)
{
  struct StringStack *ss;
  struct DataBlock *dbss;
  struct DRA *results;
  struct TagHdr *h = (struct TagHdr *) clDatablockData(db);

  if (h->tagnum != tagnum) {
    if (fmustbe) {
      clLogError("Error: expecting %s tagnum %x, got %x\n",
          tagname, tagnum,h->tagnum);
      exit(1);
    }
    else
      return NULL;
  }
  results = clLoadDatablockPackage(db);
  dbss = clScanForTag(results, TAGNUM_STRINGSTACK);
  ss = clStringstackLoad(dbss, 1);
  clDatablockFreePtr(dbss);
  clFreeDataBlockpackage(results);
  return ss;
}

struct StringStack *clLabelsLoad(struct DataBlock *db, int fmustbe)
{
  return clLoadTaggedStringStack(db, fmustbe, "DMLABELS", TAGNUM_DMLABELS);
}

struct DataBlock *clbLabelsDataBlock(char *fname)
{
  struct DataBlock *db, *dblabels;
  struct DRA *dd;

  db = clFileToDataBlockPtr(fname);
  dd = clLoadDatablockPackage(db);
  dblabels = clScanForTag(dd, TAGNUM_DMLABELS);

  clDatablockFreePtr(db);
  clDraFree(dd);

  return dblabels;
}

struct StringStack *clbLabelsLoad(struct DataBlock *db)
{
  struct StringStack *labels;
  labels = clLabelsLoad(db, 1);
  return labels;
}

struct StringStack *clbDBLabels(struct DataBlock *db)
{
  struct CLDistMatrix *clb;
  clb = clReadCLBDistMatrix(db);
  if (clb && clb->labels && clb->labels[0])
    return clb->labels[0];
  return NULL;
}

struct StringStack *clbLabels(char *fname)
{
  struct DataBlock *db;
  struct StringStack *result;

  db = clbLabelsDataBlock(fname);
  result = clbLabelsLoad(db);

  clDatablockFreePtr(db);
  return result;
}

/* TODO: needs new home; does not belong here */
struct StringStack *clbCommands(char *fname)
{
  struct DataBlock *db, *dbem;
  struct DRA *dd;
  struct StringStack *result;

  db = clFileToDataBlockPtr(fname);
  dd = clLoadDatablockPackage(db);
  dbem = clScanForTag(dd, TAGNUM_COMMANDS);
  result = clLoadTaggedStringStack(dbem, 1, "COMMANDS", TAGNUM_COMMANDS);

  clDatablockFreePtr(db);
  clDatablockFreePtr(dbem);
  clDraFree(dd);
  return result;
}
