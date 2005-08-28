#include <complearn/complearn.h>
#include <complearn/treeholder.h>
#include <assert.h>
#include <string.h>

#if GSL_RDY
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>

struct TreeHolder {
  gsl_matrix *dm;
  struct TreeAdaptor *best;
  double bestscore;
  int totalCount;
  int failedCount;
  int treeindex;
};

struct TreeHolder *cloneTreeHolder(const struct TreeHolder *th)
{
  assert(th);
  assert(th->best && "th->best is NULL");
  struct TreeHolder *result;
  result =  gcalloc(sizeof(*th), 1);
  result->best = treecloneTRA(th->best);
  result->dm = cloneGSLMatrix(th->dm);
  result->bestscore = th->bestscore;
  result->totalCount = th->totalCount;
  result->failedCount = th->failedCount;
  result->treeindex = th->treeindex;
  return result;
}

static double calculateScore(struct TreeHolder *th, struct TreeAdaptor *ta)
{
  double result;
  struct TreeScore *ts;
  ts = initTreeScore(ta);
  result = scoreTree(ts, th->dm);
  freeTreeScore(ts);
  th->totalCount += 1;
  return result;
}

struct TreeHolder *newTreeHolder(const gsl_matrix *distmat, struct TreeAdaptor *tra)
{
  struct TreeHolder *th = gcalloc(sizeof(*th), 1);
  struct LabelPerm *lp;
  assert(tra);
  assert(distmat->size1 >= 4);
  assert(distmat->size1 == distmat->size2);
  th->best = treecloneTRA(tra);
  lp = treegetlabelpermTRA(th->best);
  assert(labelpermSize(lp) == distmat->size1);
  assert(th->best);
  assert(th->best->ptr);
  th->dm = cloneGSLMatrix(distmat);
  th->bestscore = calculateScore(th, th->best);
  th->treeindex = -1;
  return th;
}

void scrambleTreeHolder(struct TreeHolder *th)
{
  int i, mutnum = 10;
  assert(th->best);
  for (i = 0; i < mutnum; i += 1)
    treemutateTRA(th->best);
  th->bestscore = calculateScore(th, th->best);
}

double getCurScore(const struct TreeHolder *th)
{
  return th->bestscore;
}

void setTreeIndexTH(struct TreeHolder *th, int treeind)
{
  th->treeindex = treeind;
}

int getTreeIndexTH(struct TreeHolder *th)
{
  return th->treeindex;
}

struct TreeAdaptor *getCurTree(const struct TreeHolder *th)
{
  return th->best;
}

/* returns 1 if it did improve, 0 otherwise */
int tryToImprove(struct TreeHolder *th)
{
  int itWorked;
  double candscore;
  struct TreeAdaptor *cand = treecloneTRA(th->best);
  treemutateTRA(cand);
  candscore = calculateScore(th, cand);
//  if (candscore == 1 || candscore > th->bestscore) {
  if (candscore > th->bestscore) {
    th->failedCount = 0;
    itWorked = 1;
    treefreeTRA(th->best);
    th->best = cand;
    th->bestscore = candscore;
  } else {
    th->failedCount += 1;
//    printf("failedcount: %d\n",th->failedCount);
    itWorked = 0;
    treefreeTRA(cand);
  }
  return itWorked;
}

int getTotalTreeCount(const struct TreeHolder *th)
{
  return th->totalCount;
}

int getSuccessiveFailCount(const struct TreeHolder *th)
{
  return th->failedCount;
}

gsl_matrix *getDistMatrixTH(const struct TreeHolder *th)
{
  return th->dm;
}

void freeTreeHolder(struct TreeHolder *th)
{
  if (th->best) {
    treefreeTRA(th->best);
    th->best = NULL;
  }
  gsl_matrix_free(th->dm);
  th->dm = NULL;
  gfreeandclear(th);
}


#endif
