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
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <complearn/complearn.h>

//void doParseTest(struct DataBlock *db);

int main(int argc, char **argv)
{
  const char *startfname, *dmfname, *outfname;
  struct DotParseTree *dpt;
  struct TreeHolder *th;
  struct TreeScore *ts;
  struct DataBlock *db, *matdb, *dotdb;
  struct GeneralConfig *cur = clLoadDefaultEnvironment();
  double score;
  if (argc != 4) {
    fprintf(stderr, "Usage: %s distmatrix.clb starttree.dot newtree.dot\n", argv[0]);
    exit(1);
  }
  dmfname = argv[1];
  startfname = argv[2];
  outfname = argv[3];
  fprintf(stderr, "Opening tree %s\n", startfname);
  /* All clFunc */
  db = clFileToDataBlockPtr(startfname);
  matdb = clFileToDataBlockPtr(dmfname);
  dpt = clParseDotDB(db, matdb);
  th = clTreehNew(dpt->dm, dpt->tree);
  ts = clInitTreeScore(dpt->dm);
  score = clScoreTree(ts, dpt->tree);
  clFreeTreeScore(ts);
  ts = NULL;
  fprintf(stderr, "initial score:%lf\n", score);
  dotdb = clConvertTreeToDot(dpt->tree, score, dpt->labels, NULL,
        cur,
        NULL,
        dpt->dm
        );
  clDatablockWriteToFile(dotdb, outfname);
  clDatablockFreePtr(dotdb);
  dotdb = NULL;
  for (;;) {
    clTreehImprove(th);
    if (score < clTreehScore(th)) {
      score = clTreehScore(th);
      fprintf(stderr, "improvement:%f\n", score);
      dotdb = clConvertTreeToDot(dpt->tree, score, dpt->labels, NULL,
              cur, NULL, dpt->dm);
      clDatablockWriteToFile(dotdb, outfname);
      clDatablockFreePtr(dotdb);
      dotdb = NULL;
    }
  }
  return 0;
}
