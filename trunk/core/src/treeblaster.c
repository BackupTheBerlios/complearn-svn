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
  for (i = 0; i < tm->k; i += 1)
    if (i == 0 || getScoreScaledTM(tm->best) < getScoreScaledTM(tm->tm[i]))
      tm->best = tm->tm[i];
  if (getScoreScaledTM(tm->best) > tm->printedScore) {
    tm->printedScore = getScoreScaledTM(tm->best);
    if (tm->tob && tm->tob->treeorderimproved)
      tm->tob->treeorderimproved(tm->tob, tm->best, getFlips(tm->best));
  }
}

struct TreeBlaster *newTreeBlaster(gsl_matrix *gsl, struct TreeAdaptor *ta)
{
  int i, howbig;
  struct TreeBlaster *tm = gcalloc(sizeof(struct TreeBlaster), 1);
  assert(gsl);
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
    tm->tm[i] = newTreeMolder(tm->dm, tm->ta);
    printf("Tree holder %d starts with score %f\n", i, getScoreTM(tm->tm[i]));
    scrambleTreeMolder(tm->tm[i]);
    printf("Tree holder %d scrmables to score %f\n", i, getScoreTM(tm->tm[i]));
  }
  setBestPtr(tm);
  return tm;
}

static int doStep(struct TreeBlaster *tm)
{
  //int whoseTurn = rand() % 2;
  int choseTree;
  int result;
  choseTree = rand() % tm->k;
//  printf("Trying tree %d\n", choseTree);
  result = tryToImproveTM(tm->tm[choseTree]);
  if (result) {
    tm->failcount = 0;
    setBestPtr(tm);
  } else {
    tm->failcount += 1;
//    printf("no improvement, trying another tree... (%d)\n", getSuccessiveFailCount(tm->th[choseTree]));
  }
  return result;
}

static int checkDone(struct TreeBlaster *tm)
{
  int i;
  if (tm->failcount > MAXFAILS)
    return 1;
  for (i = 1; i < tm->k; ++i) {
    if (getScoreScaledTM(tm->tm[i-1]) != getScoreScaledTM(tm->tm[i]))
      return 0;
  }
  return 1;
}

struct CLNodeSet *findTreeOrder(struct TreeBlaster *tm, double *s)
{
  int retval;
  if (tm->tob && tm->tob->treeordersearchstarted)
    tm->tob->treeordersearchstarted(tm->tob);
  while (!checkDone(tm)) {
    retval = doStep(tm);
  }
  if (tm->tob && tm->tob->treeorderdone)
    tm->tob->treeorderdone(tm->tob, tm->best, getFlips(tm->best));
  *s = getScoreScaledTM(tm->tm[0]);
  printf("Finished with tree, score is %f\n", *s);
  return getFlips(tm->best); /* TODO: does this need clone? */
}

void freeTreeBlaster(struct TreeBlaster *tm)
{
  int i;
  gsl_matrix_free(tm->dm);
  tm->dm = NULL;
  for (i = 0; i < tm->k; i += 1) {
    freeTreeMolder(tm->tm[i]);
  }
  gfreeandclear(tm);
}

void setTreeOrderObserver(struct TreeBlaster *tm, struct TreeOrderObserver *tob)
{
  tm->tob = tob;
}

int getKTB(struct TreeBlaster *tbl)
{
  assert(tbl);
  return tbl->k;
}

int getNodeCountTB(struct TreeBlaster *tbl)
{
  assert(tbl);
  struct TreeMolder *tmo = tbl->best;
  return getNodeCountTMO(tmo);
}

int getLabelCountTB(struct TreeBlaster *tbl)
{
  assert(tbl);
  return tbl->dm->size1;
}

#endif
