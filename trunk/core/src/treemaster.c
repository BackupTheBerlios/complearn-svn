#include <complearn/complearn.h>
#include <string.h>
#include <assert.h>

#if GSL_RDY

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

int getKTM(struct TreeMaster *tm)
{
  assert(tm);
  return tm->k;
}

int getNodeCountTM(struct TreeMaster *tm)
{
  assert(tm);
  return tm->nodecount;
}

int getLabelCountTM(struct TreeMaster *tm)
{
  assert(tm);
  return tm->dm->size1;
}

static void callImprovedFunctionMaybe(struct TreeMaster *tm)
{
  if (tm->tob && tm->tob->treeimproved) {
    struct TreeHolder *th = cloneTreeHolder(tm->best);
    tm->tob->treeimproved(tm->tob, th);
    freeTreeHolder(th);
  }
}

static void setBestPtr(struct TreeMaster *tm)
{
  int i;
  for (i = 0; i < tm->k; i += 1)
    if (i == 0 || getCurScore(tm->best) < getCurScore(tm->th[i])) {
      if (tm->best) {
        freeTreeHolder(tm->best);
        tm->best = NULL;
        }
      tm->best = cloneTreeHolder(tm->th[i]);
    }
  if (getCurScore(tm->best) >= tm->printedScore) {
    tm->printedScore = getCurScore(tm->best);
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
  val = readValForEM(srcenv,keyname);
  if (val)
    *placeToSet = atoi(val);
}

struct TreeMaster *newTreeMasterEx(gsl_matrix *gsl, int isRooted, struct EnvMap *em)
{
  struct TreeMaster *result;
  struct TreeMasterConfig tmc = getTreeMasterDefaultConfig();
  assert(em);

  tmc.fIsRooted = isRooted;

  tm_setIntValueMaybe(em, "isRooted", &tmc.fIsRooted);
  tm_setIntValueMaybe(em, "selfAgreementTermination", &tmc.fSelfAgreementTermination);
  if (!tmc.fSelfAgreementTermination)
    tm_setIntValueMaybe(em, "maxFailCount", &tmc.maxFailCount);
  else
    tmc.maxFailCount = -1;

  result = newTreeMaster(gsl, tmc.fIsRooted);
  result->activeConfig = tmc;
  if (!result->activeConfig.fSelfAgreementTermination)
    result->k = 1;
  return result;
}

struct TreeMaster *newTreeMasterEz(gsl_matrix *gsl)
{
  return newTreeMaster(gsl, 0);
}

struct TreeMaster *newTreeMaster(gsl_matrix *gsl, int isRooted)
{
  int i, howbig;
  struct TreeMasterConfig tmc = getTreeMasterDefaultConfig();
  struct TreeAdaptor *tra;
  struct TreeMaster *tm = gcalloc(sizeof(struct TreeMaster), 1);
  struct TreeFactory *tf = newTreeFactory(isRooted);
  struct AdjA *aa;
  assert(gsl);
  assert(gsl->size1 == gsl->size2);
  assert(gsl->size1 > 3);
  howbig = gsl->size1;
  tm->dm = cloneGSLMatrix(gsl);
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
  tra = newTreeTF(tf, howbig);
  aa = treegetadjaTRA(tra);
  tm->nodecount = adjaSize(aa);
  for (i = 0; i < tm->k; i += 1) {
    tm->th[i] = newTreeHolder(tm->dm, tra);
    setTreeIndexTH(tm->th[i], i);
    scrambleTreeHolder(tm->th[i]);
  }
  tm->activeConfig = tmc;
  freeTreeFactory(tf);
  treefreeTRA(tra);
  setBestPtr(tm);
  return tm;
}

int totalTreesExamined(struct TreeMaster *tm)
{
  if (tm->activeConfig.fSelfAgreementTermination) {
    int i;
    int sum = 0;
    for (i = 0; i < tm->k; i += 1)
      sum += getTotalTreeCount(tm->th[i]);
    return sum;
  } else {
    return getTotalTreeCount(tm->best);
  }
}

static int doStep(struct TreeMaster *tm)
{
  //int whoseTurn = rand() % 2;
  int choseTree;
  int result;
  if (!tm->activeConfig.fSelfAgreementTermination) {
    result = tryToImprove(tm->best);
    if (result)
      callImprovedFunctionMaybe(tm);
    return result;
  }
  choseTree = rand() % tm->k;
  result = tryToImprove(tm->th[choseTree]);
  if (result) {
//    printf("Tree %d improved to %f after %d tries (%d mutation stepsize)\n", choseTree, getCurScore(tm->th[choseTree]), getTotalTreeCount(tm->th[choseTree]), treemutecountTRA(getCurTree(tm->th[choseTree])));
    tm->lastChanged = choseTree;
    setBestPtr(tm);
  } else {
//    printf("no improvement, trying another tree... (%d)\n", getSuccessiveFailCount(tm->th[choseTree]));
    if (!tm->fAbortNow && tm->tob && tm->tob->treerejected)
      tm->tob->treerejected(tm->tob);
  }
  return result;
}

static int isIdenticalTreeTRA(struct TreeAdaptor *ta1, struct TreeAdaptor *ta2)
{
  return isIdenticalTree(treegetadjaTRA(ta1), treegetlabelpermTRA(ta1),
                         treegetadjaTRA(ta2), treegetlabelpermTRA(ta2));
}

static int checkDone(struct TreeMaster *tm)
{
  int i;
  if (tm->activeConfig.fSelfAgreementTermination) {
    for (i = 1; i < tm->k; ++i) {
      if (getCurScore(tm->th[i-1]) != getCurScore(tm->th[i]))
        return 0;
      if (!isIdenticalTreeTRA(getCurTree(tm->th[i-1]), getCurTree(tm->th[i])))
        return 0;
    }
    return 1;
  } else {
//    printf("failcount: %d  maxcount: %d\n", getSuccessiveFailCount(tm->best), tm->activeConfig.maxFailCount);
    return !(getSuccessiveFailCount(tm->best) < tm->activeConfig.maxFailCount);
  }
}

struct CLDateTime *getEndTimeTM(struct TreeMaster *tm)
{
  return tm->endTime;
}

struct CLDateTime *getStartTimeTM(struct TreeMaster *tm)
{
  return tm->startTime;
}

struct TreeHolder *findTree(struct TreeMaster *tm)
{
  int retval;
  tm->startTime = cldtNow();
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
  tm->endTime = cldtNow();
#if 1
  if (!tm->fAbortNow && tm->tob && tm->tob->treedone) {
    struct TreeHolder *th = cloneTreeHolder(tm->th[0]);
    tm->tob->treedone(tm->tob, th);
    freeTreeHolder(th);
  }
#endif
  tm->fAbortNow = 0;
  return tm->th[0];
}

void freeTreeMaster(struct TreeMaster *tm)
{
  int i;
//  printf("Freeing treemaster at %p\n", tm);
  for (i = 0; i < tm->k; i += 1) {
    freeTreeHolder(tm->th[i]);
    tm->th[i]=NULL;
  }
  if (tm->best) {
    freeTreeHolder(tm->best);
    tm->best=NULL;
  }
  gsl_matrix_free(tm->dm);
  tm->dm = NULL;
  memset(tm, 0, sizeof(*tm));
  gfreeandclear(tm);
}

struct TreeObserver *getTreeObserver(struct TreeMaster *tm)
{
  return tm->tob;
}

void setTreeObserver(struct TreeMaster *tm, struct TreeObserver *tob)
{
  tm->tob = tob;
}

void setUserDataTM(struct TreeMaster *tm, void *val)
{
  tm->udata = val;
}

void *getUserDataTM(struct TreeMaster *tm)
{
  return tm->udata;
}

void abortTreeSearch(struct TreeMaster *tm)
{
  tm->fAbortNow = 1;
}

struct TreeHolder *getStarterTree(struct TreeMaster *tm)
{
  assert(tm->th[0]);
  return cloneTreeHolder(tm->th[0]);
}

struct TreeHolder *getTreeAtIndex(struct TreeMaster *tm, int i)
{
  assert(i >= 0);
  assert(i < tm->k);
  return cloneTreeHolder(tm->th[i]);
}

#else

#error "GSL not installed. Download from http://ftp.gnu.org/gnu/gsl/gsl-1.6.tar.gz or apt-get install libgsl0-dev . Already installed?: specify prefix with --with-gsl=PREFIX"

#endif
