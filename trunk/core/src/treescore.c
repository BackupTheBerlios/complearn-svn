#include <assert.h>
#include <malloc.h>
#include <complearn/complearn.h>

#if GSL_RDY
#include <gsl/gsl_matrix.h>
#endif

struct TreeScore {
  struct TreeAdaptor *ta;
//  struct UnrootedBinary *tree;
  double penalty;
};

struct TreeScore *initTreeScore(struct TreeAdaptor *ta)
{
  struct TreeScore *ts = gcalloc(sizeof(struct TreeScore), 1);
  ts->ta = ta;
  return ts;
}

static int pathsIntersect(int *path1, int len1, int *path2, int len2)
{
#define MAXPATHPI 128
  static int pathWatcher[MAXPATHPI];
  int i;
  int retval = 0;
  for (i = 0; i < len1; ++i)
    pathWatcher[ path1[i] ] = 0;
  for (i = 0; i < len2; ++i)
    pathWatcher[ path2[i] ] = 1;
  for (i = 0; i < len1; ++i)
    if (pathWatcher[ path1[i] ] == 1) {
      retval = 1;
      break;
    }
  return retval;
}

int isConsistent(struct AdjAdaptor *ad, struct Quartet q)
{
#define MAXPATHTS 128
  static int nbuf1[MAXPATHTS], nbuf2[MAXPATHTS];
  int p1length = MAXPATHTS, p2length = MAXPATHTS;
  int pathretval;
  pathretval = pathFinder(ad, q.q[0], q.q[1], nbuf1, &p1length);
  assert(pathretval == CL_OK);
  pathretval = pathFinder(ad, q.q[2], q.q[3], nbuf2, &p2length);
  assert(pathretval == CL_OK);
  return !pathsIntersect(nbuf1, p1length, nbuf2, p2length);
}

#if GSL_RDY
double scoreTree(struct TreeScore *ts, gsl_matrix *dm)
{
  int i, j, k, m, p, x;
  double sum = 0;
  double maxtot = 0, mintot = 0;
  struct LabelPerm *lp = treegetlabelpermTRA(ts->ta);
  assert(dm->size1 == dm->size2);
  assert(dm->size1 == labelpermSize(lp));
  ALLQUARTETS(dm->size1, i, j, k, m) {
    double mincur=0, maxcur=0;
    for (p = 0; p < 3; p += 1) {
      struct Quartet q = permuteLabelsDirect(i, j, k, m, p);
      double curcost = gsl_matrix_get(dm, q.q[0], q.q[1]) + gsl_matrix_get(dm, q.q[2], q.q[3]);
      for (x = 0; x < 4; x += 1)
        q.q[x] = labelpermNodeIDForColIndex(lp, q.q[x]);
      if (p == 0 || (curcost < mincur))
        mincur = curcost;
      if (p == 0 || (curcost > maxcur))
        maxcur = curcost;
      if (isConsistent(treegetadjaTRA(ts->ta), q))
        sum += curcost;
    }
    maxtot += maxcur;
    mintot += mincur;
  }
  return 1.0 - ((sum - mintot) / (maxtot - mintot));
}
#endif

void freeTreeScore(struct TreeScore *ts)
{
  gfreeandclear(ts);
}

