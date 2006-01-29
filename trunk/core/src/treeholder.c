#include <complearn/complearn.h>
#include <complearn/treeholder.h>
#include <assert.h>
#include <string.h>

#if GSL_RDY

#include <gsl/gsl_linalg.h>

struct TreeHolder {
  gsl_matrix *dm;
  struct TreeAdaptor *best;
  double bestscore;
  int totalCount;
  int failedCount;
  int treeindex;
};

struct TreeHolder *treehClone(const struct TreeHolder *th)
{
  struct TreeHolder *result;
  if (th == NULL) {
    clogError("NULL ptr in treehClone()\n");
  }
  assert(th);
  assert(th->best && "th->best is NULL");
  result =  clCalloc(sizeof(*th), 1);
  result->best = treeaClone(th->best);
  result->dm = gslmatrixClone(th->dm);
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
  if (th == NULL || ta == NULL) {
    clogError("NULL ptr in calculateScore()\n");
  }
  ts = initTreeScore(ta);
  result = scoreTree(ts, th->dm);
  freeTreeScore(ts);
  th->totalCount += 1;
  return result;
}

struct TreeHolder *treehNew(const gsl_matrix *distmat, struct TreeAdaptor *tra)
{
  struct TreeHolder *th = clCalloc(sizeof(*th), 1);
  struct LabelPerm *lp;
  if (distmat == NULL || tra == NULL) {
    clogError("NULL ptr in treehNew()\n");
  }
  assert(tra);
  assert(distmat->size1 >= 4);
  assert(distmat->size1 == distmat->size2);
  th->best = treeaClone(tra);
  lp = treeaLabelPerm(th->best);
  assert(labelpermSize(lp) == distmat->size1);
  assert(th->best);
  assert(th->best->ptr);
  th->dm = gslmatrixClone(distmat);
  th->bestscore = calculateScore(th, th->best);
  th->treeindex = -1;
  labelpermFree(lp);
  return th;
}

void treehScramble(struct TreeHolder *th)
{
  int i, mutnum = 10;
  if (th == NULL) {
    clogError("NULL ptr in treehScramble()\n");
  }
  assert(th->best);
  for (i = 0; i < mutnum; i += 1)
    treeaMutate(th->best);
  th->bestscore = calculateScore(th, th->best);
}

double treehScore(const struct TreeHolder *th)
{
  if (th == NULL) {
    clogError("NULL ptr in treehScore()\n");
  }
  return th->bestscore;
}

void treehSetTreeIndex(struct TreeHolder *th, int treeind)
{
  if (th == NULL) {
    clogError("NULL ptr in treehSetTreeIndex()\n");
  }
  th->treeindex = treeind;
}

int treehGetTreeIndex(struct TreeHolder *th)
{
  if (th == NULL) {
    clogError("NULL ptr in treehGetTreeIndex()\n");
  }
  return th->treeindex;
}

struct TreeAdaptor *treehTreeAdaptor(const struct TreeHolder *th)
{
  if (th == NULL) {
    clogError("NULL ptr in treehTreeAdaptor()\n");
  }
  return treeaClone(th->best);
}

/* returns 1 if it did improve, 0 otherwise */
int treehImprove(struct TreeHolder *th)
{
  int itWorked;
  double candscore;
  struct TreeAdaptor *cand;
  if (th == NULL) {
    clogError("NULL ptr in treehImprove()\n");
  }
  cand = treeaClone(th->best);
  treeaMutate(cand);
  candscore = calculateScore(th, cand);
//  if (candscore == 1 || candscore > th->bestscore) {
  if (candscore > th->bestscore) {
    th->failedCount = 0;
    itWorked = 1;
    treeaFree(th->best);
    th->best = cand;
    th->bestscore = candscore;
  } else {
    th->failedCount += 1;
//    printf("failedcount: %d\n",th->failedCount);
    itWorked = 0;
    treeaFree(cand);
  }
  return itWorked;
}

int treehTreeCount(const struct TreeHolder *th)
{
  if (th == NULL) {
    clogError("NULL ptr in treehTreeCount()\n");
  }
  return th->totalCount;
}

int treehFailCount(const struct TreeHolder *th)
{
  if (th == NULL) {
    clogError("NULL ptr in treehFailCount()\n");
  }
  return th->failedCount;
}

gsl_matrix *treehDistMatrix(const struct TreeHolder *th)
{
  if (th == NULL) {
    clogError("NULL ptr in treehDistMatrix()\n");
  }
  return th->dm;
}

void treehFree(struct TreeHolder *th)
{
  if (th == NULL) {
    clogError("NULL ptr in treehFree()\n");
  }
  if (th->best) {
    treeaFree(th->best);
    th->best = NULL;
  }
  gsl_matrix_free(th->dm);
  th->dm = NULL;
  clFreeandclear(th);
}
#endif
