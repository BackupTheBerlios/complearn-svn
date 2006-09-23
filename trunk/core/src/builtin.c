#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>


#include "newcomp.h"
#include "ncbase.h"
#include "nccbi.h"
#include "ncazlib.h"
#include "ncblocksort.h"
#include <complearn/complearn.h>

#define DELIMS ":"

struct CLCompressionInfo {
  struct CompressionBaseAdaptor cba;
  struct CLCompressionInfo *next;
  char *staticErrorMsg;
};

static struct CLCompressionInfo *clciHead = NULL;
static struct CLCompressionInfo **clciHeadPtr = &clciHead;

static struct CLCompressionInfo *findCompressorInfo(const char *name);
static struct CLCompressionInfo **findPointerTo(struct CLCompressionInfo *t);
static void staticErrorExitIfBad(int retval, struct CompressionBase *cb);
static void instanceErrorExitIfBad(int retval, struct CompressionBase *cb);

const char *clLastStaticErrorCB(const char *shortName)
{
  struct CLCompressionInfo *ci = findCompressorInfo(shortName);
  assert(ci != NULL);
  return ci->staticErrorMsg;
}

void clSetLastErrorCB(struct CompressionBase *cb, const char *msg)
{
  assert(msg && "NULL error message error.");
  cb->cbi->errorMessage = strdup(msg);
}

void clSetStaticErrorMessage(const char *shortName, const char *msg)
{
  struct CLCompressionInfo *ci = findCompressorInfo(shortName);
  assert(ci != NULL);
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

void deregisterCompressorCL(const char *shortName)
{
  struct CLCompressionInfo *ci = findCompressorInfo(shortName);
  assert(ci != NULL);
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

static struct CLCompressionInfo *findCompressorInfo(const char *name)
{
  struct CLCompressionInfo *c;
  for (c = clciHead; c; c = c->next) {
    if (strcmp(name, c->cba.shortNameCB()) == 0)
      return c;
  }
  return NULL;
}

void **dvptr = (void **) &cbsuper;
struct CompressionBase *clNewCompressorCB(const char *shortName)
{
  struct CLCompressionInfo *ci;
  ci = findCompressorInfo(shortName);
  assert(ci != NULL);
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
  assert(cb->cbi->fHavePrepared == 0 && "Parameters already locked.");
  clEnvmapSetKeyVal(cb->cbi->em, key, val);
  if (isPrivate)
    clEnvmapSetKeyPrivate(cb->cbi->em, key);
  return 0;
}

void clRegisterCB(struct CompressionBaseAdaptor *vptr)
{
  struct CLCompressionInfo *ci = calloc(sizeof(struct CLCompressionInfo), 1);
  int cbas = sizeof(struct CompressionBaseAdaptor);
  int ps = sizeof(void *);
  assert(vptr != NULL);
  assert(vptr->shortNameCB != NULL);
  const char *shortName = vptr->shortNameCB();
  assert(shortName != NULL);
  assert(strlen(shortName) > 0);
  assert(findCompressorInfo(shortName) == NULL);
  void **vpn = (void **) &ci->cba;
  void **svptr = (void **) vptr;
  int i;
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

void printCompressors(void)
{
  struct CLCompressionInfo *c;
  const char *fmtstrtitle = "%-12s:%7s:%8s%9s:%30s:%s%s\n";
  const char *fmtstrdata  = "%-12s:%1s:%5s:%8d%9s:%30s:%s%s\n";
  printf("Compressor options\n");
  printf("+ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +\n");
  printf(fmtstrtitle,  "Name", "Auto En","Window", "Rounding", "Description", "Errors", "");
  printf(fmtstrtitle, "-----------", "-------", "------", "--------", "-----------------------", "------", "");
  for (c = clciHead; c; c = c->next) {
    const char *erm = NULL;
    if (!clIsEnabledCB(c->cba.shortNameCB()))
      erm = clLastStaticErrorCB(c->cba.shortNameCB());
    printf(fmtstrdata,
      c->cba.shortNameCB(),
      c->cba.isAutoEnabledCB() ? "A" : "_",
      clIsEnabledCB(c->cba.shortNameCB())?"(yes)" : "(no)",
      c->cba.getWindowSizeCB(),
      c->cba.doesRoundWholeBytesCB() ? "(int)" : "(double)",
      c->cba.longNameCB(),
      erm ? "\nreason:" : "",
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

void clZombie_reaperCB(int q)
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
int clForkPipeExecAndFeedCB(struct DataBlock *inp, const char *cmd)
{
	int pout[2], pin[2];
  int retval;
  int childid;
  retval = pipe(pout);
  if (retval)
    clogError("pipe");
  retval = pipe(pin);
  if (retval)
    clogError("pipe");
  signal(SIGCHLD, (void(*)(int))clZombie_reaperCB);
  childid = fork();
  if (childid < 0) { // An error
        clogError("fork");
      }
  if (childid) { // parent
    int wlen, wtot = 0, wleft;
    wleft = clDatablockSize(inp);
    while (wleft > 0) {
      wlen = write(pout[1], clDatablockData(inp)+wtot, wleft);
      if (wlen < 0) {
        clogError("write");
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
      clogError("dup2");
    retval = dup2(pin[1],1);
    if (retval < 0)
      clogError("dup2");
    execl(cmd, cmd, NULL);
    printf("Shouldn't be here, wound up returning from exec!!\n");
    exit(1);
  }
  return pin[0];
}

void initZLib(void);
void initBZ2(void);
void initReal(void);
void initVirtual(void);
void initGoogle(void);
void printCompressors(void);
void doBestScan(void);

void doBestScan(void)
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

const char *expandCommand(const char *inpcmd)
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
  buf = calloc(1, inplen + pathlen + 16);
  path = strdup(path);
  for (p = strtok(path, DELIMS); p; p = strtok(NULL, DELIMS)) {
    sprintf(buf, "%s/%s", p, inpcmd);
    if (stat(buf, &st) == 0)
      return buf;
  }
  free(path);
  return NULL;
}

void initBuiltinCompressors(void) {
  initGoogle();
  initReal();
  initVirtual();
  initBZ2();
  initBlockSort();
  initZLib();
}
