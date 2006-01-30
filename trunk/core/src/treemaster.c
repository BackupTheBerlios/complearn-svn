#include <complearn/complearn.h>
#include <string.h>
#include <assert.h>

struct TreeMasterConfig {
  int fSelfAgreementTermination;
  int fIsRooted;
  int maxFailCount;
};

struct TreeMaster {
  gsl_matrix *dm;
  int k;
  int nodecount;
  int lastChanged;
  int fAbortNow;
  void *udata;
  struct TreeHolder *best;
  double printedScore;
  struct TreeHolder *th[16];
  struct TreeObserver *tob;
  struct TreeMasterConfig activeConfig;
  struct CLDateTime *startTime, *endTime;
};

int treemasterK(struct TreeMaster *tm)
{
  if (tm == NULL) {
    clogError("NULL ptr in treemasterK()\n");
  }
  assert(tm);
  return tm->k;
}

static void callImprovedFunctionMaybe(struct TreeMaster *tm)
{
  if (tm == NULL) {
    clogError("NULL ptr in callImprovedFunctionMaybe()\n");
  }
  if (tm->tob && tm->tob->treeimproved) {
    struct TreeHolder *th = treehClone(tm->best);
    tm->tob->treeimproved(tm->tob, th);
    treehFree(th);
  }
}

static void setBestPtr(struct TreeMaster *tm)
{
  int i;
  if (tm == NULL) {
    clogError("NULL ptr in setBestPtr()\n");
  }
  for (i = 0; i < tm->k; i += 1)
    if (i == 0 || treehScore(tm->best) < treehScore(tm->th[i])) {
      if (tm->best) {
        treehFree(tm->best);
        tm->best = NULL;
        }
      tm->best = treehClone(tm->th[i]);
    }
  if (treehScore(tm->best) > tm->printedScore) {
    tm->printedScore = treehScore(tm->best);
    callImprovedFunctionMaybe(tm);
  }
}

static struct TreeMasterConfig getTreeMasterDefaultConfig(void)
{
  struct TreeMasterConfig tmc_default = { 1, 0, 100000};
  return tmc_default;
}

static void tm_setIntValueMaybe(struct EnvMap *srcenv, const char *keyname, int *placeToSet) {
  char *val;
  if (srcenv == NULL || keyname == NULL) {
    clogError("NULL ptr in tm_setIntValueMaybe()\n");
  }
  val = envmapValueForKey(srcenv,keyname);
  if (val)
    *placeToSet = atoi(val);
}

static void validateMatrixForTree(gsl_matrix *gsl)
{
  int i, j;
  if (gsl == NULL) {
    clogError("NULL ptr in validateMatrixForTree()\n");
  }
  if (gsl->size1 != gsl->size2) {
    clogError( "Matrix must be square, but this one is %d by %d\n", gsl->size1, gsl->size2);
    exit(1);
  }
  for (i = 0; i < gsl->size1; i += 1) {
    for (j = 0; j < gsl->size2; j += 1) {
      if (gsl_matrix_get(gsl, i, j) != gsl_matrix_get(gsl, 0, 0))
        return;
    }
  }
  clogError( "Sorry, this is a constant matrix so no information is available.\n");
  exit(1);
}

struct TreeMaster *treemasterNewEx(gsl_matrix *gsl, int isRooted, struct EnvMap *em)
{
  struct TreeMaster *result;
  struct TreeMasterConfig tmc = getTreeMasterDefaultConfig();
  if (em == NULL || gsl == NULL) {
    clogError("NULL ptr in treemasterNewEx()\n");
  }
  assert(em);

  validateMatrixForTree(gsl);
  tmc.fIsRooted = isRooted;

  tm_setIntValueMaybe(em, "isRooted", &tmc.fIsRooted);
  tm_setIntValueMaybe(em, "selfAgreementTermination", &tmc.fSelfAgreementTermination);
  if (!tmc.fSelfAgreementTermination)
    tm_setIntValueMaybe(em, "maxFailCount", &tmc.maxFailCount);
  else
    tmc.maxFailCount = -1;

  result = treemasterNew(gsl, tmc.fIsRooted);
  result->activeConfig = tmc;
  if (!result->activeConfig.fSelfAgreementTermination)
    result->k = 1;
  return result;
}

struct TreeMaster *treemasterNewEz(gsl_matrix *gsl)
{
  if (gsl == NULL) {
    clogError("NULL ptr in treemasterNewEz()\n");
  }
  return treemasterNew(gsl, 0);
}

struct TreeMaster *treemasterNew(gsl_matrix *gsl, int isRooted)
{
  int i, howbig;
  struct TreeMasterConfig tmc = getTreeMasterDefaultConfig();
  struct TreeAdaptor *tra;
  struct TreeMaster *tm = clCalloc(sizeof(struct TreeMaster), 1);
  struct AdjAdaptor *aa;
  if (gsl == NULL) {
    clogError("NULL ptr in treemasterNew()\n");
  }
  assert(gsl);
  assert(gsl->size1 == gsl->size2);
  assert(gsl->size1 > 3);
  howbig = gsl->size1;
  tm->dm = gslmatrixClone(gsl);
  tm->printedScore = -1;
  tm->fAbortNow = 0;
  assert(tm->dm);
  if (gsl->size1 < 16)
    tm->k = 3;
  else
    tm->k = 2;
  if (gsl->size1 < 18) /* to push accuracy up a bit */
    tm->k += 1;
  if (gsl->size1 < 10) /* small trees are very quick yet very uncertain */
    tm->k += 1;
  if (gsl->size1 <= 5)
    tm->k += 1;
  tra = treeaNew(isRooted, howbig);
  aa = treeaAdjAdaptor(tra);
  tm->nodecount = adjaSize(aa);
  for (i = 0; i < tm->k; i += 1) {
    tm->th[i] = treehNew(tm->dm, tra);
    treehSetTreeIndex(tm->th[i], i);
    treehScramble(tm->th[i]);
  }
  tm->activeConfig = tmc;
  treeaFree(tra);
  setBestPtr(tm);
  return tm;
}

int treemasterTreeCount(struct TreeMaster *tm)
{
  if (tm == NULL) {
    clogError("NULL ptr in treemasterTreeCount()\n");
  }
  if (tm->activeConfig.fSelfAgreementTermination) {
    int i;
    int sum = 0;
    for (i = 0; i < tm->k; i += 1)
      sum += treehTreeCount(tm->th[i]);
    return sum;
  } else {
    return treehTreeCount(tm->best);
  }
}

static int doStep(struct TreeMaster *tm)
{
  //int whoseTurn = rand() % 2;
  int choseTree;
  int result;
  if (tm == NULL) {
    clogError("NULL ptr in doStep()\n");
  }
  if (!tm->activeConfig.fSelfAgreementTermination) {
    result = treehImprove(tm->best);
    if (result)
      callImprovedFunctionMaybe(tm);
    return result;
  }
  choseTree = rand() % tm->k;
  result = treehImprove(tm->th[choseTree]);
  if (result) {
//    printf("Tree %d improved to %f after %d tries (%d mutation stepsize)\n", choseTree, treehScore(tm->th[choseTree]), treehTreeCount(tm->th[choseTree]), treeaMutationCount(treehTreeAdaptor(tm->th[choseTree])));
    tm->lastChanged = choseTree;
    setBestPtr(tm);
  } else {
//    printf("no improvement, trying another tree... (%d)\n", treehFailCount(tm->th[choseTree]));
    if (!tm->fAbortNow && tm->tob && tm->tob->treerejected)
      tm->tob->treerejected(tm->tob);
  }
  return result;
}

static int isIdenticalTreeTRA(struct TreeAdaptor *ta1, struct TreeAdaptor *ta2)
{
  struct LabelPerm *lab1, *lab2;
  int result;
  if (ta1 == NULL || ta2 == NULL) {
    clogError("NULL ptr in isIdenticalTreeTRA()\n");
  }
  lab1 = treeaLabelPerm(ta1);
  lab2 = treeaLabelPerm(ta2);
  result = isIdenticalTree(treeaAdjAdaptor(ta1), lab1,
                         treeaAdjAdaptor(ta2), lab2);
  labelpermFree(lab1);
  labelpermFree(lab2);
  return result;
}

static int checkDone(struct TreeMaster *tm)
{
  int i;
  if (tm == NULL) {
    clogError("NULL ptr in checkDone()\n");
  }
  if (tm->activeConfig.fSelfAgreementTermination) {
    for (i = 1; i < tm->k; ++i) {
      if (treehScore(tm->th[i-1]) != treehScore(tm->th[i]))
        return 0;
      if (!isIdenticalTreeTRA(treehTreeAdaptor(tm->th[i-1]), treehTreeAdaptor(tm->th[i])))
        return 0;
    }
    return 1;
  } else {
//    printf("failcount: %d  maxcount: %d\n", treehFailCount(tm->best), tm->activeConfig.maxFailCount);
    return !(treehFailCount(tm->best) < tm->activeConfig.maxFailCount);
  }
}

struct CLDateTime *treemasterEndTime(struct TreeMaster *tm)
{
  if (tm == NULL) {
    clogError("NULL ptr in treemasterEndTime()\n");
  }
  return tm->endTime;
}

struct CLDateTime *treemasterStartTime(struct TreeMaster *tm)
{
  if (tm == NULL) {
    clogError("NULL ptr in treemasterStartTime()\n");
  }
  return tm->startTime;
}

struct TreeHolder *treemasterFindTree(struct TreeMaster *tm)
{
  int retval;
  if (tm == NULL) {
    clogError("NULL ptr in treemasterFindTree()\n");
  }
  tm->startTime = cldatetimeNow();
  if (tm->tob && tm->tob->treesearchstarted)
    tm->tob->treesearchstarted(tm->tob);
  for (;;) {
    retval = doStep(tm);
    if (tm->fAbortNow)
      break;
    if (retval || !tm->activeConfig.fSelfAgreementTermination) {
      if (checkDone(tm))
        break;
    }
  }
  tm->endTime = cldatetimeNow();
#if 1
  if (!tm->fAbortNow && tm->tob && tm->tob->treedone) {
    struct TreeHolder *th = treehClone(tm->th[0]);
    tm->tob->treedone(tm->tob, th);
    treehFree(th);
  }
#endif
  tm->fAbortNow = 0;
  return tm->th[0];
}

void treemasterFree(struct TreeMaster *tm)
{
  int i;
  if (tm == NULL) {
    clogError("NULL ptr in treemasterFree()\n");
  }
//  printf("Freeing treemaster at %p\n", tm);
  for (i = 0; i < tm->k; i += 1) {
    treehFree(tm->th[i]);
    tm->th[i]=NULL;
  }
  if (tm->best) {
    treehFree(tm->best);
    tm->best=NULL;
  }
  gsl_matrix_free(tm->dm);
  tm->dm = NULL;
  memset(tm, 0, sizeof(*tm));
  clFreeandclear(tm);
}

struct TreeObserver *treemasterGetTreeObserver(struct TreeMaster *tm)
{
  if (tm == NULL) {
    clogError("NULL ptr in treemasterGetTreeObserver()\n");
  }
  return tm->tob;
}

void treemasterSetTreeObserver(struct TreeMaster *tm, struct TreeObserver *tob)
{
  if (tm == NULL || tob == NULL) {
    clogError("NULL ptr in treemasterSetTreeObserver()\n");
  }
  if (tm->tob) {
    clFreeandclear(tm->tob);
  }
  tm->tob = clCalloc(sizeof(*tob), 1);
  *(tm->tob) = *tob;
}

void treemasterSetUserData(struct TreeMaster *tm, void *val)
{
  if (tm == NULL) {
    clogError("NULL ptr in treemasterSetUserData()\n");
  }
  tm->udata = val;
}

void *treemasterGetUserData(struct TreeMaster *tm)
{
  if (tm == NULL) {
    clogError("NULL ptr in treemasterGetUserData()\n");
  }
  return tm->udata;
}

void treemasterAbortSearch(struct TreeMaster *tm)
{
  if (tm == NULL) {
    clogError("NULL ptr in treemasterAbortSearch()\n");
  }
  tm->fAbortNow = 1;
}

struct TreeHolder *treemasterStarterTree(struct TreeMaster *tm)
{
  if (tm == NULL || tm->th[0] == NULL) {
    clogError("NULL ptr in treemasterStarterTree()\n");
  }
  assert(tm->th[0]);
  return treehClone(tm->th[0]);
}

struct TreeHolder *treemasterTreeAtIndex(struct TreeMaster *tm, int i)
{
  if (tm == NULL) {
    clogError("NULL ptr in treemasterTreeAtIndex()\n");
  }
  assert(i >= 0);
  assert(i < tm->k);
  return treehClone(tm->th[i]);
}

int treemasterLabelCount(struct TreeMaster *tm)
{
  if (tm == NULL) {
    clogError("NULL ptr in treemasterLabelCount()\n");
  }
  return tm->dm->size1;
}

