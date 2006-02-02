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
  struct GeneralConfig *cur = loadDefaultEnvironment();
  double score;
  if (argc != 4) {
    fprintf(stderr, "Usage: %s distmatrix.clb starttree.dot newtree.dot\n", argv[0]);
    exit(1);
  }
  dmfname = argv[1];
  startfname = argv[2];
  outfname = argv[3];
  fprintf(stderr, "Opening tree %s\n", startfname);
  /* All func */
  db = fileToDataBlockPtr(startfname);
  matdb = fileToDataBlockPtr(dmfname);
  dpt = parseDotDB(db, matdb);
  th = treehNew(dpt->dm, dpt->tree);
  ts = initTreeScore(dpt->tree);
  score = scoreTree(ts, dpt->dm);
  freeTreeScore(ts);
  ts = NULL;
  fprintf(stderr, "initial score:%lf\n", score);
  dotdb = convertTreeToDot(dpt->tree, score, dpt->labels, NULL,
        cur,
        NULL,
        dpt->dm
        );
  datablockWriteToFile(dotdb, outfname);
  datablockFreePtr(dotdb);
  dotdb = NULL;
  for (;;) {
    treehImprove(th);
    if (score < treehScore(th)) {
      score = treehScore(th);
      fprintf(stderr, "improvement:%f\n", score);
      dotdb = convertTreeToDot(dpt->tree, score, dpt->labels, NULL,
              cur, NULL, dpt->dm);
      datablockWriteToFile(dotdb, outfname);
      datablockFreePtr(dotdb);
      dotdb = NULL;
    }
  }
  return 0;
}
