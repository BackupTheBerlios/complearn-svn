#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>

#include <complearn/complearn.h>
#include <complearn/nccbi.h>

static int fHaveInitted;

void clinitZLib(void);
void clinitBZ2(void);
void clinitBlockSort(void);
void clinitReal(void);
void clinitVirtual(void);
void clinitGoogle(void);
void clinitBZip2X(void);
void clinitPPMDX(void);
void clinitLZMAX(void);

static void clDoBestScan(void);
static void checkInitted(void);
static void initBuiltinCompressors(void);


#define DELIMS ":"

struct CLCompressionInfo {
  struct CompressionBaseAdaptor cba;
  struct CLCompressionInfo *next;
  char *staticErrorMsg;
};

static struct CLCompressionInfo *clciHead = NULL;
static struct CLCompressionInfo **clciHeadPtr = &clciHead;
int clCompressorCount(void)
{
  struct CLCompressionInfo *p = clciHead;
  int c = 0;
  while (p) {
    c += 1;
    p = p->next;
  }
  return c;
}

const char *clCompressorName(int whichOne)
{
  struct CLCompressionInfo *p = clciHead;
  int c = 0;
  if (whichOne >= clCompressorCount())
    return NULL;
  while (c < whichOne) {
    c += 1;
    p = p->next;
  }
  return p->cba.shortNameCB();
}

static struct CLCompressionInfo *findCompressorInfo(const char *name);
static struct CLCompressionInfo **findPointerTo(struct CLCompressionInfo *t);
static void staticErrorExitIfBad(int retval, struct CompressionBase *cb);
static void instanceErrorExitIfBad(int retval, struct CompressionBase *cb);

struct StringStack *clListBuiltinsCB(int fWithDisabled)
{
  struct StringStack *ss = clStringstackNew();
  struct CLCompressionInfo *c;
  for (c = clciHead; c; c = c->next) {
    const char *sn = c->cba.shortNameCB();
    if (fWithDisabled || clIsEnabledCB(sn))
      clStringstackPush(ss, sn);
  }
  return ss;
}

const char *clLastStaticErrorCB(const char *shortName)
{
  struct CLCompressionInfo *ci = findCompressorInfo(shortName);
  clAssert(ci != NULL);
  return ci->staticErrorMsg;
}

void clSetLastErrorCB(struct CompressionBase *cb, const char *msg)
{
  clAssert(msg && "NULL error message error." != NULL);
  cb->cbi->errorMessage = strdup(msg);
}

void clSetLastStaticErrorCB(const char *shortName, const char *msg)
{
  struct CLCompressionInfo *ci = findCompressorInfo(shortName);
  clAssert(ci != NULL);
  ci->staticErrorMsg = strdup(msg);
}

void clFreeCB(struct CompressionBase *cb)
{
  VF(cb, freeCB)(cb);
  if (cb->cbi->madeupParamString != NULL) {
    free(cb->cbi->madeupParamString);
    cb->cbi->madeupParamString = NULL;
  }
  if (cb->cbi->madeupToString != NULL) {
    free(cb->cbi->madeupToString);
    cb->cbi->madeupToString = NULL;
  }
  if (cb->cbi->madeupLongname != NULL) {
    free(cb->cbi->madeupLongname);
    cb->cbi->madeupLongname = NULL;
  }
  free(cb->cbi);
  cb->cbi = NULL;
  free(cb);
}

void clDeregisterCompressor(const char *shortName)
{
  struct CLCompressionInfo *ci = findCompressorInfo(shortName);
  clAssert(ci != NULL);
  struct CLCompressionInfo **pc = findPointerTo(ci);
  *pc = ci->next;
  free(ci); // shortName must be owned by Compressor instance
}

static void addToCLCIList(struct CLCompressionInfo *t)
{
  t->next = clciHead;
  clciHeadPtr = &t->next;
  clciHead = t;
}

static void checkPrepared(struct CompressionBase *cb)
{
  if (cb->cbi->fHavePrepared == 0) {
    int retval;
    retval = VF(cb, prepareToCompressCB)(cb);
    if (retval != 0)
      cb->cbi->fHaveFailed = 1;
    cb->cbi->fHavePrepared = 1;
  }
  instanceErrorExitIfBad(cb->cbi->fHaveFailed, cb);
}

static struct CLCompressionInfo **findPointerTo(struct CLCompressionInfo *t)
{
  struct CLCompressionInfo *c = clciHead;
  struct CLCompressionInfo **pc = clciHeadPtr;
  while (c != NULL) {
    if (c->next == t)
      return pc;
    pc = &(c->next);
    c = c->next;
  }
  return NULL;
}
// Starting point of CompressionBase initialization
static struct CLCompressionInfo *findCompressorInfo(const char *name)
{
  struct CLCompressionInfo *c;
  for (c = clciHead; c; c = c->next) {
    if (strcmp(name, c->cba.shortNameCB()) == 0)
      return c;
  }
  return NULL;
}
struct CompressionBase *clCompressorNewEM(const char *shortName, struct EnvMap *em)
{
  struct CompressionBase *cb = clNewCompressorCB(shortName);
  clEnvmapMerge(cb->cbi->em, em);
  return cb;
}

struct CompressionBase *clNewCompressorCB(const char *shortName)
{
  struct CLCompressionInfo *ci;
  checkInitted();
  ci = findCompressorInfo(shortName);
  if (ci == NULL) {
  	char buf[1024];
	sprintf(buf, "Cannot find compressor %s", shortName);
	fprintf(stderr, "ERROR: %s\n", buf);
	clPrintCompressors();
  	clLogError(buf);
	exit(1);
  }
  clAssert(ci != NULL);
  if (!clIsEnabledCB(shortName))
    return NULL;
  struct CompressionBase *cb = calloc(ci->cba.allocSizeCB(), 1);
  struct CompressionBaseInternal *cbi = calloc(sizeof(struct CompressionBaseInternal), 1);
  cbi->cb = cb;
  cbi->vptr = &ci->cba;
  cb->cbi = cbi;
  cbi->em = clEnvmapNew();
  int retval;
  retval = VF(cb, specificInitCB)(cb);
  staticErrorExitIfBad(retval, cb);
  return cb;
}
static void instanceErrorExitIfBad(int retval, struct CompressionBase *cb)
{
  if (retval != 0) {
    fprintf(stderr, "Instance error in %s: %s\n", VF(cb,shortNameCB)(), clLastErrorCB(cb));
    exit(1);
  }
}

static void staticErrorExitIfBad(int retval, struct CompressionBase *cb)
{
  if (retval != 0) {
    fprintf(stderr, "Static error in %s: %s\n", VF(cb,shortNameCB)(), clLastStaticErrorCB(VF(cb,shortNameCB)()));
    exit(1);
  }
}

int clSetParameterCB(struct CompressionBase *cb, const char *key, const char *val, int isPrivate)
{
  clAssert(cb->cbi->fHavePrepared == 0 && "Parameters already locked." != NULL);
  clEnvmapSetKeyVal(cb->cbi->em, key, val);
  if (isPrivate)
    clEnvmapSetKeyPrivate(cb->cbi->em, key);
  return 0;
}

void clRegisterCB(struct CompressionBaseAdaptor *vptr)
{
  void **dvptr = (void **) ((void *) &cbsuper);
  struct CLCompressionInfo *ci;
  int cbas = sizeof(struct CompressionBaseAdaptor);
  int ps = sizeof(void *);
  const char *shortName;
  void **vpn;
  void **svptr;
  int i;
  checkInitted();
  ci = calloc(sizeof(struct CLCompressionInfo), 1);
  clAssert(vptr != NULL);
  clAssert(vptr->shortNameCB != NULL);
  shortName = vptr->shortNameCB();
  clAssert(shortName != NULL);
  clAssert(strlen(shortName) > 0);
  if (findCompressorInfo(shortName) != NULL) {
    fprintf(stderr, "Warning: already loaded compressor %s, ignoring subsequent registrations by the same name.\n", shortName);
    return;
  }
  vpn = (void **) (void*) &ci->cba;
  svptr = (void **) vptr;
  for (i = 0; i < (cbas/ps); i += 1) {
    void *f = svptr[i];
    if (f == NULL)
      f = dvptr[i];
    if (f == NULL) {
      fprintf(stderr, "Function %d undefined.\n", i);
      exit(1);
    }
    vpn[i] = f;
  }
  addToCLCIList(ci);
}

double clCompressCB(struct CompressionBase *cb, struct DataBlock *db)
{
  checkPrepared(cb);
  return VF(cb, compressCB)(cb, db);
}

struct DataBlock *clConcatCB(struct CompressionBase *cb, struct DataBlock *db1,
                            struct DataBlock *db2)
{
  checkPrepared(cb);
  return VF(cb, concatCB)(cb, db1, db2);
}

struct EnvMap *clGetParametersCB(struct CompressionBase *cb)
{
  return cb->cbi->em;
}

void clPrintParametersCB(struct CompressionBase *cb)
{
  struct EnvMap *em = clGetParametersCB(cb);
  clEnvmapPrint(em);
}

const char *clLastErrorCB(struct CompressionBase *cb)
{
  return cb->cbi->errorMessage;
}

int clIsEnabledCB(const char *shortName)
{
  struct CLCompressionInfo *ci;
  ci = findCompressorInfo(shortName);
  if (ci == NULL)
    return 0;
  if (ci->cba.isDisabledCB())
    return 0;
  if (ci->cba.isCompileProblemCB())
    return 0;
  if (ci->cba.isRuntimeProblemCB())
    return 0;
  return 1;
}

void clPrintCompressors(void)
{
  checkInitted();
  int cc, i;
  const char *fmtstrtitle = "%-12s:%7s:%8s%9s:%30s:%s%s\n";
  const char *fmtstrdata  = "%-12s:%1s:%5s:%8d%9s:%30s:%s%s\n";
  printf("Compressor options\n");
  printf("+ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +\n");
  printf(fmtstrtitle,  "Name", "Auto En","Window", "Rounding", "Description", "Errors", "");
  printf(fmtstrtitle, "-----------", "-------", "------", "--------", "-----------------------", "------", "");
  cc = clCompressorCount();
  for (i = 0; i < cc; i += 1) {
    const char *sn = clCompressorName(i);
    struct CLCompressionInfo *ci = findCompressorInfo(sn);
    const char *erm = NULL;
    if (!clIsEnabledCB(sn))
      erm = clLastStaticErrorCB(sn);
    else
      if (ci == NULL)
        clLogError("NULL compressor returned error.");
    printf(fmtstrdata,
      sn,
      ci->cba.isAutoEnabledCB() ? "A" : "_",
      clIsEnabledCB(sn)?"(yes)" : "(no)",
      ci->cba.getWindowSizeCB(),
      ci->cba.doesRoundWholeBytesCB() ? "(int)" : "(double)",
      ci->cba.longNameCB(),
      erm ? "\n         disabled because " : "",
      erm ? erm : ""
    );
  }
}

const char *clGetParamStringCB(struct CompressionBase *cb)
{
  checkPrepared(cb);
  return VF(cb, paramStringCB)(cb);
  return "(no parmstring yet)";
}

void clZombieReaperCB(int q)
{
  int stat;
  while(waitpid(-1, &stat, WNOHANG) > 0) ;
}
/** \brief Returns an fd to read the output of given command with DataBlock
 * fed in through stdin.
 * \param inp pointer to DataBlock to feed in to external program
 * \param cmd string indicating pathname for external program
 * \return fd that may be read to get data from external program stdout
 */
int clForkPipeExecAndFeedCB(struct DataBlock *inp, const char *cmd, struct StringStack *afterCmdArgs)
{
  int i;
	int pout[2], pin[2];
  int retval;
  int childid;
  char const *arglist[128];
  int argnum = 0;
  arglist[argnum++] = cmd;
  if (afterCmdArgs) {
    for (i = 0;i < clStringstackSize(afterCmdArgs); i += 1)
      arglist[argnum++] = clStringstackReadAt(afterCmdArgs, i);
  }
  arglist[argnum++] = NULL;
  retval = pipe(pout);
  if (retval)
    clLogError("pipe");
  retval = pipe(pin);
  if (retval)
    clLogError("pipe");
  signal(SIGCHLD, (void(*)(int))clZombieReaperCB);
  childid = fork();
  if (childid < 0) { // An error
        clLogError("fork");
      }
  if (childid) { // parent
    int wlen, wtot = 0, wleft;
    wleft = clDatablockSize(inp);
    while (wleft > 0) {
      wlen = write(pout[1], clDatablockData(inp)+wtot, wleft);
      if (wlen < 0) {
        clLogError("write");
        continue;
      }
      wtot += wlen;
      wleft -= wlen;
    }
    close(pout[1]);
    close(pout[0]);
    close(pin[1]);
  } else       { // child
    close(pin[0]);
    close(pout[1]);
    retval = dup2(pout[0],0);
    if (retval < 0)
      clLogError("dup2");
    retval = dup2(pin[1],1);
    if (retval < 0)
      clLogError("dup2");
    execv(cmd, (char * const*)arglist);
    printf("Shouldn't be here, wound up returning from exec!!\n");
    exit(1);
  }
  return pin[0];
}

int clIsAutoEnabledCB(struct CompressionBase *cb)
{
  return VF(cb, isAutoEnabledCB)();
}

static void clDoBestScan(void)
{
  struct CLCompressionInfo *c;
  const char *fname = "nc";
  struct DataBlock *d = clFileToDataBlockPtr(fname);
  printf("Best Scan with datablock %s, size %d\n", fname, clDatablockSize(d)*8);
  for (c = clciHead; c; c = c->next) {
    if (c->cba.isAutoEnabledCB() && clIsEnabledCB(c->cba.shortNameCB())) {
      struct CompressionBase *cb = clNewCompressorCB(c->cba.shortNameCB());
      double sz = clCompressCB(cb, d);
      double ratio = sz/(clDatablockSize(d)*8);
      char pctspot[16];
      char sizespot[32];
      sprintf(pctspot, "%6.6f", ratio*100);
      sprintf(sizespot, "%6.6f", sz);
      pctspot[5] = 0;
      printf("%-12s:%s%% :%18s\n", VF(cb, shortNameCB)(), pctspot, sizespot);
    }
  }
}

const char *clShortNameCB(struct CompressionBase *cb)
{
  return VF(cb, shortNameCB)();
}

const char *clLongNameCB(struct CompressionBase *cb)
{
  return VF(cb, longNameCB)();
}

const char *clExpandCommand(const char *inpcmd)
{
  static char *buf;
  struct stat st;
  char *path, *p;
  int inplen, pathlen;
  if (inpcmd[0] == '/')
    return inpcmd;
  inplen = strlen(inpcmd);
  path = getenv("PATH");
  if (path == NULL)
    path = "/bin:/usr/bin";
  pathlen = strlen(path);
  if (buf)
    free(buf);
  path = strdup(path);
  for (p = strtok(path, DELIMS); p; p = strtok(NULL, DELIMS)) {
    buf = clJoinAsPath(p, inpcmd);
    if (stat(buf, &st) == 0)
      return buf;
  }
  free(path);
  return NULL;
}

static void checkInitted(void) {
  if (!fHaveInitted)
    initBuiltinCompressors();
}

struct ParamList *clGetParameterListCB(struct CompressionBase *cb)
{
  struct ParamList *pl = clParamlistNew();
  struct EnvMap *em = clGetParametersCB(cb);
  int i, j=0;
  for (i = 0; i < pl->size; i += 1) {
    if (!clEnvmapIsMarkedAt(em, i))
      continue;
    union PCTypes p;
    p = clEnvmapKeyValAt(em, i);
    pl->fields[j++] = clFielddescNew(p.sp.key, p.sp.val, 0);
  }
  pl->size = j;
  return pl;
}

static void initBuiltinCompressors(void) {
  char *modPath;
  fHaveInitted = 1;
  if (fHaveInitted == 0)
    clDoBestScan(); // to stop warnings for now
  clinitGoogle();
  clinitReal();
  clinitVirtual();
  clinitBZ2();
  clinitBlockSort();
  clinitZLib();
  clinitBZip2X();
#if STATICLOADEXTRAMODS
  clinitPPMDX();
  clinitLZMAX();
#endif
  modPath = getenv("COMPLEARNMODPATH");
  if (modPath)
    clScanDirForModules(modPath, NULL);
  clScanDirForModules(clGetSystemModuleDir(), NULL);
  clScanDirForModules(clGetHomeModuleDir(), NULL);
}
