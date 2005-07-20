#include <complearn/complearn.h>

#include <string.h>

#if GSL_RDY

struct TreeMolder {
  int nodecount;
  double score, minscore, maxscore;
  gsl_matrix *dm;
  struct TreeAdaptor *ta;
  struct CLNodeSet *flips;
};

/*
  if (tm->nodecount < 20)
    tm->k = 24 - tm->nodecount;
  int k;
*/

void freeTreeMolder(struct TreeMolder *tm)
{
  freeCLNodeSet(tm->flips);
  tm->flips = NULL;
  memset(tm, 0, sizeof(*tm));
  gfreeandclear(tm);
}

static void calcRangesTM(struct TreeMolder *tm)
{
  int i;
  gsl_vector_view currow;
  tm->minscore = 0;
  tm->maxscore = 0;
  for (i = 0; i < tm->dm->size1; i += 1) {
    currow = gsl_matrix_row(tm->dm, i);
    tm->minscore += gsl_vector_min((gsl_vector *) &currow);
    tm->maxscore += gsl_vector_max((gsl_vector *) &currow);
  }
}

struct TreeMolder *newTreeMolder(gsl_matrix *gm, struct TreeAdaptor *ta)
{
  struct TreeMolder *tm = gcalloc(sizeof(*tm), 1);
  struct AdjA *aa;
  assert(gm->size1 > 0 && gm->size1 == gm->size2);
  aa = treegetadjaTRA(ta);
  assert(aa);
  tm->nodecount = adjaSize(aa);
  tm->ta = ta;
  tm->dm = gm;
  tm->flips = newCLNodeSet(tm->nodecount);
  tm->score = -1;
  calcRangesTM(tm);
  return tm;
}

static double scorePerimeter(const gsl_matrix *dm, struct TreeAdaptor *ts, struct CLNodeSet *flips)
{
  double acc = 0.0;
  struct DoubleA *pairs = treeperimpairsTRA(ts, flips);
  int i;
  struct LabelPerm *lph = treegetlabelpermTRA(ts);
  for (i = 0; i < getSize(pairs); i += 1) {
    union pctypes p = getValueAt(pairs, i);
    double x, y;
    x = getColumnIndexForNodeIDLP(lph, p.ip.x);
    y = getColumnIndexForNodeIDLP(lph, p.ip.y);
    acc += gsl_matrix_get(dm, x, y);
  }
  freeDoubleDoubler(pairs);
  return acc;
}

double getScoreScaledTM(struct TreeMolder *tm)
{
  double rawscore = getScoreTM(tm);
  return 1.0 - ((rawscore - tm->minscore) / (tm->maxscore-tm->minscore));
}

double getScoreTM(struct TreeMolder *tm)
{
  if (tm->score == -1)
    tm->score = scorePerimeter(tm->dm, tm->ta, tm->flips);
  return tm->score;
}

struct CLNodeSet *getFlips(struct TreeMolder *tm)
{
  return tm->flips;
}

static void mutateFlipArray(struct TreeMolder *tm, struct CLNodeSet *dst)
{
  int whichNode, oldStatus;
  do {
    do {
      whichNode = rand() % tm->nodecount;
    } while (!treeIsFlippable(tm->ta, whichNode));
    oldStatus = isNodeInSet(dst, whichNode);
    //printf("About to switch node %d flip from %d to %d.\n", whichNode, oldStatus, !oldStatus);
    setNodeStatusInSet(dst, whichNode, !oldStatus);
  } while ((rand() % 2) == 0);
}

void scrambleTreeMolder(struct TreeMolder *tm)
{
  int i;
  for (i = 0; i < tm->nodecount; i += 1)
    mutateFlipArray(tm, tm->flips);
  tm->score = -1;
}

int tryToImproveTM(struct TreeMolder *tm)
{
  struct CLNodeSet *cand = cloneCLNodeSet(tm->flips);
  double candscore;
  mutateFlipArray(tm, cand);
  candscore = scorePerimeter(tm->dm, tm->ta, cand);
  if (candscore < getScoreTM(tm)) {
//    printf("In treemolder %p, raw score impr from %f to %f\n", tm, tm->score, candscore );
    tm->score = candscore;
    freeCLNodeSet(tm->flips);
    tm->flips = cand;
    return 1;
  }
  else {
    freeCLNodeSet(cand);
    return 0;
  }
}

struct TreeAdaptor *getCurTreeTM(const struct TreeMolder *tmo)
{
  return treecloneTRA(tmo->ta);
}

int getNodeCountTMO(const struct TreeMolder *tmo)
{
  return tmo->nodecount;
}
#endif

