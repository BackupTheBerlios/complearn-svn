#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <complearn/complearn.h>

//void doParseTest(struct DataBlock *db);

int main(int argc, char **argv)
{
  const char *fname, *dmfname;
  struct DotParseTree *dpt;
  struct TreeHolder *th;
  struct TreeScore *ts;
  struct DataBlock *db, *matdb, *dotdb;
  struct GeneralConfig *cur = loadDefaultEnvironment();
  double score;
  if (argv[1])
    fname = argv[1];
  else
    fname = "treefile.dot";
  if (argc > 3)
    dmfname = argv[2];
  else
    dmfname = "distmatrix.clb";
  fprintf(stderr, "Opening tree %s\n", fname);
  db = fileToDataBlockPtr(fname);
  matdb = clbDMDataBlock(dmfname);
  dpt = parseDotDB(db, matdb);
  th = treehNew(dpt->dm, dpt->tree);
  ts = initTreeScore(dpt->tree);
  score = scoreTree(ts, dpt->dm);
  freeTreeScore(ts);
  ts = NULL;
  printf("The tree score is %lf\n", score);
  dotdb = convertTreeToDot(dpt->tree, score, dpt->labels, NULL,
        cur,
        NULL,
        dpt->dm
        );
  datablockWriteToFile(dotdb, "newtree.dot");;
  datablockFreePtr(dotdb);
  dotdb = NULL;
  for (;;) {
    treehImprove(th);
    if (score < treehScore(th)) {
      score = treehScore(th);
      printf("Got new tree with score %f\n", score);
      dotdb = convertTreeToDot(dpt->tree, score, dpt->labels, NULL,
      cur,
      NULL,
      dpt->dm
      );
    datablockWriteToFile(dotdb, "newtree.dot");;
    datablockFreePtr(dotdb);
    dotdb = NULL;
    }
  }
  return 0;
}
