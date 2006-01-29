#include <complearn/complearn.h>
#include <assert.h>

#define MAXFAILS 100000

#if GSL_RDY
struct TreeBlaster {
  gsl_matrix *dm;
  int k;
  int failcount;
  struct TreeAdaptor *ta;
  int lastChanged;
  struct TreeMolder *best;
  double printedScore;
  struct TreeMolder *tm[64];
  struct TreeOrderObserver *tob;
};

static void setBestPtr(struct TreeBlaster *tm)
{
  int i;
  if (tm == NULL) {
    clogError("NULL ptr in setBestPtr()\n");
  }
  for (i = 0; i < tm->k; i += 1)
    if (i == 0 || treemolderScoreScaled(tm->best) < treemolderScoreScaled(tm->tm[i]))
      tm->best = tm->tm[i];
  if (treemolderScoreScaled(tm->best) > tm->printedScore) {
    tm->printedScore = treemolderScoreScaled(tm->best);
    if (tm->tob && tm->tob->treeorderimproved)
      tm->tob->treeorderimproved(tm->tob, tm->best, treemolderFlips(tm->best));
  }
}

struct TreeBlaster *treebNew(gsl_matrix *gsl, struct TreeAdaptor *ta)
{
  int i, howbig;
  struct TreeBlaster *tm = clCalloc(sizeof(struct TreeBlaster), 1);
  if (gsl == NULL || ta == NULL) {
    clogError("NULL ptr in treebNew()\n");
  }
  assert(gsl->size1 == gsl->size2);
  howbig = gsl->size1;
  tm->ta = ta;
  tm->dm = gslmatrixClone(gsl);
  tm->printedScore = -1;
  assert(tm->dm);
  if (gsl->size1 < 14)
    tm->k = 5;
  else
    tm->k = 3;
  if (gsl->size1 < 10) /* small trees are very quick yet very uncertain */
    tm->k += 1;
  for (i = 0; i < tm->k; i += 1) {
    tm->tm[i] = treemolderNew(tm->dm, tm->ta);
    printf("Tree holder %d starts with score %f\n", i, treemolderScore(tm->tm[i]));
    treemolderScramble(tm->tm[i]);
    printf("Tree holder %d scrmables to score %f\n", i, treemolderScore(tm->tm[i]));
  }
  setBestPtr(tm);
  return tm;
}

static int doStep(struct TreeBlaster *tm)
{
  //int whoseTurn = rand() % 2;
  int choseTree;
  int result;
  if (tm == NULL) {
    clogError("NULL ptr in doStep()\n");
  }
  choseTree = rand() % tm->k;
//  printf("Trying tree %d\n", choseTree);
  result = treemolderImprove(tm->tm[choseTree]);
  if (result) {
    tm->failcount = 0;
    setBestPtr(tm);
  } else {
    tm->failcount += 1;
//    printf("no improvement, trying another tree... (%d)\n", treehFailCount(tm->th[choseTree]));
  }
  return result;
}

static int checkDone(struct TreeBlaster *tm)
{
  int i;
  if (tm == NULL) {
    clogError("NULL ptr in checkDone()\n");
  }
  if (tm->failcount > MAXFAILS)
    return 1;
  for (i = 1; i < tm->k; ++i) {
    if (treemolderScoreScaled(tm->tm[i-1]) != treemolderScoreScaled(tm->tm[i]))
      return 0;
  }
  return 1;
}

struct CLNodeSet *treebFindTreeOrder(struct TreeBlaster *tm, double *s)
{
  int retval;
  if (tm == NULL) {
    clogError("NULL ptr in treebFindTreeOrder()\n");
  }
  if (tm->tob && tm->tob->treeordersearchstarted)
    tm->tob->treeordersearchstarted(tm->tob);
  while (!checkDone(tm)) {
    retval = doStep(tm);
  }
  if (tm->tob && tm->tob->treeorderdone)
    tm->tob->treeorderdone(tm->tob, tm->best, treemolderFlips(tm->best));
  *s = treemolderScoreScaled(tm->tm[0]);
  printf("Finished with tree, score is %f\n", *s);
  return treemolderFlips(tm->best); /* TODO: does this need clone? */
}

void treebFree(struct TreeBlaster *tm)
{
  int i;
  if (tm == NULL) {
    clogError("NULL ptr in treebFree()\n");
  }
  gsl_matrix_free(tm->dm);
  tm->dm = NULL;
  for (i = 0; i < tm->k; i += 1) {
    treemolderFree(tm->tm[i]);
  }
  clFreeandclear(tm);
}

void treebSetTreeOrderObserver(struct TreeBlaster *tm, struct TreeOrderObserver *tob)
{
  if (tm == NULL) {
    clogError("NULL ptr in treebSetTreeOrderObserver()\n");
  }
  tm->tob = tob;
}

int treebK(struct TreeBlaster *tbl)
{
  if (tbl == NULL) {
    clogError("NULL ptr in treebK()\n");
  }
  return tbl->k;
}

int getNodeCountTB(struct TreeBlaster *tbl)
{
  struct TreeMolder *tmo;
  if (tbl == NULL) {
    clogError("NULL ptr in getNodeCountTB()\n");
  }
  tmo = tbl->best;
  return treemolderNodeCount(tmo);
}

int treebLabelCount(struct TreeBlaster *tbl)
{
  if (tbl == NULL) {
    clogError("NULL ptr in treebLabelCount()\n");
  }
  return tbl->dm->size1;
}

#endif
