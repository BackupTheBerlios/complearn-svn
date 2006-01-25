#include <string.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <complearn/complearn.h>
#include <mpi/mpi.h>

#define PROTOTAG 50

#define MSG_NONE 0
#define MSG_LOADCLB 1
#define MSG_EXIT 2
#define MSG_NEWASSIGNMENT 3
#define MSG_LASTASSIGNMENT 4
#define MSG_BETTER 5
#define MSG_NOBETTER 6

struct MasterSlaveModel {
  int isFree;
  double lastScore;
};

struct MasterState {
  int nodecount;
  struct MasterSlaveModel *workers;
  struct GeneralConfig *cfg;
  double bestscore;
  gsl_matrix *dm;
  struct StringStack *labels;
  struct DataBlock *clbdb;
  struct DataBlock *bestTree;
  struct TreeAdaptor *ta;
};


struct SlaveState {
  struct DataBlock *dbdm;
  gsl_matrix *dm;
  struct DataBlock *bestdb;
  struct StringStack *labels;
  struct GeneralConfig *cfg;
  struct TreeAdaptor *ta;
  double myLastScore;
  double shouldBeScore;
};

void doMasterLoop(void);
void doSlaveLoop(void);
void sendExitEveryWhere(void);
int receiveMessage(struct DataBlock **ptr, double *score, int *fw);
struct DataBlock *wrapWithTag(struct DataBlock *dbinp, int tag, double score);
struct DataBlock *unwrapForTag(struct DataBlock *dbbig,int *tag,double *score);

int my_rank;
int p;

void ignorer(int lameness)
{
  fprintf(stderr, "ignoring signal...\n");
  signal(SIGINT, ignorer);
}

void bailer(int lameness)
{
  fprintf(stderr, "Closing down all slaves...\n");
  sendExitEveryWhere();
  MPI_Finalize();
  exit(0);
}

void setMPIGlobals(void) {
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &p);
  if (my_rank == 0)
    signal(SIGINT, bailer);
  else
    signal(SIGINT, ignorer);
  nice(19);
}

void sendBlock(int dest, struct DataBlock *idb, int tag, double d)
{
  struct DataBlock *wdb;
  wdb = wrapWithTag(idb, tag, d);
  MPI_Send(datablockData(wdb), datablockSize(wdb), MPI_CHAR, dest,
     PROTOTAG, MPI_COMM_WORLD);
  datablockFreePtr(wdb);
}

int findFree(struct MasterState *ms)
{
  int i;
  for (i = 0; i < ms->nodecount; i += 1)
    if (ms->workers[i].isFree)
      return i;
  return -1;
}

double tsScore(struct TreeAdaptor *ta, gsl_matrix *gm)
{
  struct TreeScore *ts;
  double s;
  ts = initTreeScore(ta);
  s = scoreTree(ts, gm);
  freeTreeScore(ts);
  return s;
}

void writeBestToFile(struct MasterState *ms)
{
  const char *outfname = "treefile.dot";
  struct DataBlock *db;
  printf("score %f\n", ms->bestscore);
  db = convertTreeToDot(ms->ta, ms->bestscore, ms->labels, NULL, ms->cfg, NULL, ms->dm);
  datablockWriteToFile(db, outfname);
  datablockFreePtr(db);
}

void sendExitEveryWhere(void)
{
  int dest;
    for (dest = 1; dest < p; dest += 1) {
      printf("sending MSG_EXIT to %d\n", dest);
      sendBlock(dest, NULL, MSG_EXIT, 3.3333);
    }
}

void doMasterLoop(void) {
  struct MasterState ms;
  int i, dest;
  struct DataBlock *db;
  double score;
  const char *fname = "distmatrix.clb";
  ms.workers = clCalloc(sizeof(struct MasterSlaveModel), p);
  ms.cfg = loadDefaultEnvironment();
  ms.nodecount = p;
  for (i = 0; i < ms.nodecount; i += 1) {
    ms.workers[i].isFree = 1;
    ms.workers[i].lastScore = 0;
  }
  ms.workers[0].isFree = 0; // Master is never free

  ms.clbdb = fileToDataBlockPtr(fname);
  printf("Read file: %s\n", fname);
  ms.dm = clbDBDistMatrix(ms.clbdb);
  ms.labels = clbDBLabels(ms.clbdb);
  printf("Loaded distmatrix with %d entries.\n", ms.dm->size1);
  ms.ta = treeaNew(0, ms.dm->size1);
  ms.bestscore = tsScore(ms.ta, ms.dm);
  ms.bestTree = convertTreeToDot(ms.ta, ms.bestscore, ms.labels, NULL, ms.cfg, NULL, ms.dm);
  writeBestToFile(&ms);
  for (dest = 1; dest < ms.nodecount ; dest += 1) {
//    printf("sending MSG_LOADCLB to %d\n", dest);
    sendBlock(dest, ms.clbdb, MSG_LOADCLB, 3.3333);
  }
  for (;;) {
      struct DotParseTree *dpt;
      int freeguy, who;
      freeguy = findFree(&ms);
      if (freeguy == -1) { /* no free guys */
        int tag;
        tag = receiveMessage(&db, &score, &who);
        ms.workers[who].isFree = 1;
        if (tag == MSG_BETTER) {
          if (score > ms.bestscore) {
            dpt = parseDotDB(db, ms.clbdb);
            datablockFreePtr(ms.bestTree);
            ms.bestTree = db;
            if (ms.ta)
              treeaFree(ms.ta);
            ms.ta = dpt->tree;
            stringstackFree(dpt->labels);
            dpt->labels = NULL;
            ms.bestscore = score;
            writeBestToFile(&ms);
            clFree(dpt);
          } else {
            assert(db);
            datablockFreePtr(db);
            db = NULL;
          }
        }
      } else {
          
        if (ms.workers[freeguy].lastScore != ms.bestscore) {
//          printf("Sending tree with score %f to worker %d\n", ms.bestscore, freeguy);
          sendBlock(freeguy, ms.bestTree, MSG_NEWASSIGNMENT, ms.bestscore);
          ms.workers[freeguy].lastScore = ms.bestscore;
        } else {
          sendBlock(freeguy, NULL, MSG_LASTASSIGNMENT, ms.bestscore);
        }
        ms.workers[freeguy].isFree =  0;
      }
  }
  //sendExitEverywhere(&ms);
}

void calculateTree(struct SlaveState *ss)
{
  int result;
  struct TreeHolder *th;
  int failCount = 0;
  int MAXTRIES = 10;
  assert(ss->dm->size1 >= 4);
  assert(ss->dm->size2 >= 4);
  assert(ss->dm->size1 == ss->dm->size2);
  th = treehNew(ss->dm, ss->ta);
  while (failCount < MAXTRIES) {
    result = treehImprove(th);
    if (result) {
      struct TreeAdaptor *ta = NULL;
      ta = treehTreeAdaptor(th);
      struct DataBlock *db;
      double newScore =  treehScore(th);
      db = convertTreeToDot(ss->ta, newScore, ss->labels, NULL, ss->cfg, NULL, ss->dm);
      sendBlock(0, db, MSG_BETTER, newScore);
      datablockFreePtr(db);
      treeaFree(ta);
      goto bail;
    }
    failCount += 1;
  }
  sendBlock(0, NULL, MSG_NOBETTER, ss->myLastScore);
//  assert(treehScore(th) == ss->shouldBeScore);
  if (treehScore(th) != ss->shouldBeScore) {
    printf("Rogue master... should be %9.9f but got %9.9f\n", ss->shouldBeScore, treehScore(th));
    exit(1);
  }
bail:
  treehFree(th);
}

void doSlaveLoop(void) {
  struct DataBlock *db;
  int tag, dum;
  double score;
  struct SlaveState ss;
  struct DotParseTree *dpt;
  ss.cfg = loadDefaultEnvironment();
  ss.myLastScore = 0;
  ss.dbdm = NULL;
  ss.bestdb = NULL;
  for (;;) {
    tag = receiveMessage(&db, &score, &dum);
    switch (tag) {

      case MSG_EXIT:
        fprintf(stderr, "Slave %d exitting...\n", my_rank);
        MPI_Finalize();
        exit(0);
        return;

      case MSG_LOADCLB:
        ss.dbdm = db;
        srand(time(NULL) + my_rank * 107);
        printf("SLAVE: db ptr %p\n",db);
        printf("SLAVE: db size %d\n",datablockSize(db));
        ss.dm = clbDBDistMatrix(ss.dbdm);
        ss.labels = clbDBLabels(ss.dbdm);
        printf("SLAVE:dist matrix size %d\n",ss.dm->size1);
        break;

      case MSG_NEWASSIGNMENT:
        assert(score != ss.myLastScore);
        ss.shouldBeScore = score;
        ss.myLastScore = score;
        if (ss.bestdb)
          datablockFreePtr(ss.bestdb);
        ss.bestdb = db;
        dpt = parseDotDB(db, ss.dbdm);
        if (dpt->labels) {
          stringstackFree(dpt->labels);
          dpt->labels = NULL;
        }
        ss.ta = dpt->tree;
        clFree(dpt);
        printf("SLAVE %d got new assignment with score %f\n", my_rank, ss.myLastScore);
        calculateTree(&ss);
        break;

      case MSG_LASTASSIGNMENT:
        assert(ss.ta);
        calculateTree(&ss);
        break;

      default:
        fprintf(stderr, "Unknown CL message code: %d\n" , tag);
        exit(1);
    }
  }
}

int receiveMessage(struct DataBlock **ptr, double *score, int *fromWhom) {
  int source = MPI_ANY_SOURCE;
  int size;
  char *message;
  int tag;
  struct DataBlock *rec_db, *db;
  MPI_Status status;
  for (;;) {
    MPI_Probe(source, PROTOTAG, MPI_COMM_WORLD, &status);
    if (status.MPI_ERROR != MPI_SUCCESS) {
      sleep(1);
    } else {
      break;
    }
  }
  source = status.MPI_SOURCE;
  *fromWhom = source;
  MPI_Get_count(&status, MPI_CHAR, &size);
//  printf("got this length from the probe: %d\n",size);
  message = clCalloc(size,1);
  MPI_Recv(message, size, MPI_CHAR, source, PROTOTAG, MPI_COMM_WORLD, &status);
  rec_db = datablockNewFromBlock(message,size);
  clFree(message);
  db = unwrapForTag(rec_db, &tag, score);
  *ptr = db;
  datablockFreePtr(rec_db);

  //printf("UNWRAPPED got tag: %d, score: %f\n", tag, *score);

  return tag;
}

struct DataBlock *wrapWithTag(struct DataBlock *dbinp, int tag, double score)
{
  int dbsize = 0;
  struct DataBlock *result;
  unsigned int len;
  unsigned char *bigblock;
  if (dbinp)
    dbsize = datablockSize(dbinp);
  len = dbsize+4+sizeof(double);
  bigblock = clCalloc(len,1);
  memcpy(bigblock, &tag, 4);
  memcpy(bigblock+4, &score, sizeof(double));
  if (dbsize)
    memcpy(bigblock+4+sizeof(double), datablockData(dbinp), dbsize);
  result = datablockNewFromBlock(bigblock, len);
  clFree(bigblock);
  return result;
}

struct DataBlock *unwrapForTag(struct DataBlock *dbbig, int *tag, double *score)
{
  struct DataBlock *result = NULL;
  int len;
  unsigned char *smallblock = NULL;

  len = datablockSize(dbbig)-4-sizeof(double);
  assert(len >= 0);
  if (len) {
    smallblock = clCalloc(len,1);
    memcpy(smallblock, ((char *)datablockData(dbbig))+4+sizeof(double), len);
    result = datablockNewFromBlock(smallblock, len);
    clFree(smallblock);
  }
  memcpy(tag, datablockData(dbbig), 4);
  if (score)
    memcpy(score, datablockData(dbbig)+4, sizeof(double));
  return result;
}

int main(int argc, char **argv)
{
  MPI_Init(&argc, &argv);
    setMPIGlobals();

  if (my_rank == 0) {
    doMasterLoop();
  } else {
    doSlaveLoop();
  }

  return 0;
}
