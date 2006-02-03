#include <complearn/complearn.h>
#include <complearn/treeholder.h>
#include <assert.h>
#include <string.h>

#include <gsl/gsl_linalg.h>

struct TreeHolder {
  gsl_matrix *dm;
  struct TreeAdaptor *best;
  double bestscore;
  int totalCount;
  int failedCount;
  int treeindex;
};

struct TreeHolder *clTreehClone(const struct TreeHolder *th)
{
  assert(th);
  assert(th->best && "th->best is NULL");
  struct TreeHolder *result;
  result =  clCalloc(sizeof(*th), 1);
  result->best = clTreeaClone(th->best);
  result->dm = clGslmatrixClone(th->dm);
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
  ts = clInitTreeScore(ta);
  result = clScoreTree(ts, th->dm);
  clFreeTreeScore(ts);
  th->totalCount += 1;
  return result;
}

struct TreeHolder *clTreehNew(const gsl_matrix *distmat, struct TreeAdaptor *tra)
{
  struct TreeHolder *th = clCalloc(sizeof(*th), 1);
  struct LabelPerm *lp;
  assert(tra);
  assert(distmat->size1 >= 4);
  assert(distmat->size1 == distmat->size2);
  th->best = clTreeaClone(tra);
  lp = clTreeaLabelPerm(th->best);
  assert(clLabelpermSize(lp) == distmat->size1);
  assert(th->best);
  assert(th->best->ptr);
  th->dm = clGslmatrixClone(distmat);
  th->bestscore = calculateScore(th, th->best);
  th->treeindex = -1;
  clLabelpermFree(lp);
  return th;
}

void clTreehScramble(struct TreeHolder *th)
{
  int i, mutnum = 10;
  assert(th->best);
  for (i = 0; i < mutnum; i += 1)
    clTreeaMutate(th->best);
  th->bestscore = calculateScore(th, th->best);
}

double clTreehScore(const struct TreeHolder *th)
{
  return th->bestscore;
}

void clTreehSetTreeIndex(struct TreeHolder *th, int treeind)
{
  th->treeindex = treeind;
}

int clTreehGetTreeIndex(struct TreeHolder *th)
{
  return th->treeindex;
}

struct TreeAdaptor *clTreehTreeAdaptor(const struct TreeHolder *th)
{
  return clTreeaClone(th->best);
}

/* returns 1 if it did improve, 0 otherwise */
int clTreehImprove(struct TreeHolder *th)
{
  int itWorked;
  double candscore;
  struct TreeAdaptor *cand = clTreeaClone(th->best);
  clTreeaMutate(cand);
  candscore = calculateScore(th, cand);
//  if (candscore == 1 || candscore > th->bestscore) {
  if (candscore > th->bestscore) {
    th->failedCount = 0;
    itWorked = 1;
    clTreeaFree(th->best);
    th->best = cand;
    th->bestscore = candscore;
  } else {
    th->failedCount += 1;
//    printf("failedcount: %d\n",th->failedCount);
    itWorked = 0;
    clTreeaFree(cand);
  }
  return itWorked;
}

int clTreehTreeCount(const struct TreeHolder *th)
{
  return th->totalCount;
}

int clTreehFailCount(const struct TreeHolder *th)
{
  return th->failedCount;
}

gsl_matrix *clTreehDistMatrix(const struct TreeHolder *th)
{
  return th->dm;
}

void clTreehFree(struct TreeHolder *th)
{
  if (th->best) {
    clTreeaFree(th->best);
    th->best = NULL;
  }
  gsl_matrix_free(th->dm);
  th->dm = NULL;
  clFreeandclear(th);
}

