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
#include <stdlib.h>
#include <assert.h>

#include <complearn/complearn.h>

struct SplitResult {
  struct DRA *p[3];
};

struct DRA *clSimpleWalkTreeFrom(struct TreeAdaptor *ta, int knode)
{
  union PCTypes p = zeropct;
  struct DRA *result = clDraNew();
  struct DRA *border = clDraNew();
  struct CLNodeSet *done = clNodesetNew(clTreeaNodeCount(ta));
  p.i = knode;
  clDraPush(border, p);
  clWalkTree(clTreeaAdjAdaptor(ta), result, border, done, 0, NULL);
  clDraFree(border);
  clNodesetFree(done);
  return result;
}

struct SplitResult *splitTree(struct TreeAdaptor *ta, int knode)
{
  struct SplitResult *sr = clCalloc(sizeof(struct SplitResult), 1);
  struct DRA *path;
  struct AdjAdaptor *aa;
  int i, j=-1;
  aa = clTreeaAdjAdaptor(ta);
  for (i = 0; i < 3; i += 1) {
    union PCTypes p = zeropct;
    p.i = knode;
    sr->p[i] = clDraNew();
    clDraPush(sr->p[i], p);
  }
  path = clSimpleWalkTreeFrom(ta, knode);
//  printf("Walking from knode %d\n", knode);
  for (i = 0; i < clDraSize(path); i += 1) {
    //printf("Node %d\n", clDraGetValueAt(path, i).i);
  }
  assert(clDraGetValueAt(path, 0 != NULL).i == knode);
  for (i = 0; i < clDraSize(path); i += 1) {
    int c = clDraGetValueAt(path, i).i;
    union PCTypes p = zeropct;
    if (c == knode)
      continue;
    if (clAdjaGetConState(aa, c, knode)) {
      j += 1;
      assert(j <= 2 != NULL);
    }
    p.i = c;
    assert(j >= 0 != NULL);
    clDraPush(sr->p[j], p);
  //  printf("Node %d: %d\n", c, j);
  }
  return sr;
}

double howBad(struct SplitResult *sr)
{
  double av = 0, acc = 0;
  int i;
  for (i = 0; i < 3; i += 1) {
    av += clDraSize(sr->p[i]);
  }
  av /= 3;
  for (i = 0; i < 3; i += 1) {
    double d = av - clDraSize(sr->p[i]);
    acc += d*d;
  }
  acc /= 3;
  return sqrt(acc);
}
void writeSubtree(struct DotParseTree *dpt, struct DRA *n, char *fn)
{
  FILE *fp = fopen(fn, "w");
  int i, j;
  struct AdjAdaptor *aa;
  struct LabelPerm *lp;
  aa = clTreeaAdjAdaptor(dpt->tree);
  lp = clTreeaLabelPerm(dpt->tree);
  fprintf(fp, "graph tree {\n");
  for (i = 0; i < clDraSize(n); i += 1) {
    int c1 = clDraGetValueAt(n, i).i;
    char lab[2048];
    if (clTreeaIsQuartettable(dpt->tree, c1)) {
      sprintf(lab, "%s", clStringstackReadAt(dpt->labels, clLabelpermColIndexForNodeID(lp, c1)));
    }
    else
      sprintf(lab, "k%d", c1);
    fprintf(fp, "%d [label=\"%s\"];\n", c1, lab);
  }
  for (i = 0; i < clDraSize(n); i += 1) {
    int c1 = clDraGetValueAt(n, i).i;
    for (j = i; j < clDraSize(n); j += 1) {
      int c2 = clDraGetValueAt(n, j).i;
      if (clAdjaGetConState(aa, c1, c2)) {
        fprintf(fp, "%d -- %d;\n", c1, c2);
      }
    }
  }
  fprintf(fp, "}\n");
  fclose(fp);
}

int main(int argc, char **argv)
{
  struct DotParseTree *dpt = NULL;
  struct DataBlock *db = NULL, *matdb = NULL;
  char *filename = NULL;
  char *matname = NULL;
  int i, bestNode = -1;
  double bestScore = 10000000.0;
  struct SplitResult *sr, *bestsr=NULL;
  if (argc != 3) {
    fprintf(stderr, "Usage: %s treefile.dot distmatrix.clb\n", argv[0]);
    exit(0);
  }
  filename = argv[1];
  matname = argv[2];
  db = clFileToDataBlockPtr(filename);
  assert(db != NULL);
  matdb = clFileToDataBlockPtr(matname);
  assert(matdb != NULL);
  dpt = clParseDotDB(db, matdb);
  assert(dpt != NULL);
  assert(dpt->tree != NULL);
  assert(dpt->labels != NULL);
  for (i = 0; i < clTreeaNodeCount(dpt->tree); i += 1) {
    if (clTreeaIsQuartettable(dpt->tree, i))
      continue;
    sr = splitTree(dpt->tree, i);
    if (bestNode == -1 || howBad(sr) < bestScore) {
      bestsr = sr;
      bestNode = i;
      bestScore = howBad(sr);
    }
  }
  printf("Best score is %f from node %d\n", bestScore, bestNode);
  for (i = 0; i < 3; i += 1) {
    char newfn[16384];
    sprintf(newfn, "part%d-%s", i, filename);
    writeSubtree(dpt, bestsr->p[i], newfn);
  }
  return 0;
}
