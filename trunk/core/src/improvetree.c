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
