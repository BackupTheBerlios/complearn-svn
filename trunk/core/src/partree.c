/*
* Copyright (c) 2006 Rudi Cilibrasi, Rulers of the RHouse
* All rights reserved.     cilibrar@cilibrar.com
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the RHouse nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE RULERS AND CONTRIBUTORS "AS IS" AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE RULERS AND CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <string.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <gsl/gsl_histogram.h>
#include <complearn/complearn.h>
#include <mpi.h>

#define PROTOTAG 50

#define MSG_NONE 0
#define MSG_LOADCLB 1
#define MSG_EXIT 2
#define MSG_NEWASSIGNMENT 3
#define MSG_LASTASSIGNMENT 4
#define MSG_BETTER 5
#define MSG_NOBETTER 6
#define MSG_ROGUE 7
#define MSG_ALERT 8
#define MSG_HISTOP 9

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
  struct TreeHolder *th;
  double myLastScore;
  double shouldBeScore;
};

#define HISTOLABEL_MUTGOOD
#define HISTOLABEL_MUTBAD

struct HistOpCommand {
  int label;
  int index;
  double x, weight;
};

void doMasterLoop(void);
void doSlaveLoop(void);
void addToHistogram(int lab, int index, double x, double weight);
void sendExitEveryWhere(void);
void sendBlock(int dest, struct DataBlock *idb, int tag, double d);
int receiveMessage(struct DataBlock **ptr, double *score, int *fw);
struct DataBlock *wrapWithTag(struct DataBlock *dbinp, int tag, double score);
struct DataBlock *unwrapForTag(struct DataBlock *dbbig,int *tag,double *score);
void clogSendAlert( const char *fmt, ...);

int my_rank;
int p;
int mustQuit;
time_t quitTime;

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

static gsl_histogram *gh[128];
#define MAXHISTONUM 64

static gsl_histogram *getHistoFor(int ind)
{
  if (gh[ind] == NULL) {
    gh[ind] = gsl_histogram_alloc(MAXHISTONUM);
  }
  return gh[ind];
}

void handleHistogramMsg(int fromWho, int histolab, int index, double x, double weight)
{

}

void addToHistogram(int histolab, int index, double x, double weight)
{
  struct HistOpCommand hoc;
  struct DataBlock *db;
  memset(&hoc, 0, sizeof(hoc));
  hoc.label = histolab;
  hoc.index = index;
  hoc.x = x;
  hoc.weight = weight;
  db = clDatablockNewFromBlock(&hoc,sizeof(hoc));
  sendBlock(0, db, MSG_HISTOP, 0.0);
  clDatablockFreePtr(db);
}

static void sendAlertForEmit(char *str)
{
  if (my_rank == 0)
    fprintf(stderr, "%s", str);
  else
    clogSendAlert("%s", str);
}

void setMPIGlobals(void) {
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &p);
  if (my_rank == 0)
    signal(SIGINT, bailer);
  else
    signal(SIGINT, ignorer);
  nice(19);
  clogSetEmitFunction(sendAlertForEmit);
}

void sendBlock(int dest, struct DataBlock *idb, int tag, double d)
{
  struct DataBlock *wdb;
  wdb = wrapWithTag(idb, tag, d);
  MPI_Send(clDatablockData(wdb), clDatablockSize(wdb), MPI_CHAR, dest,
     PROTOTAG, MPI_COMM_WORLD);
  clDatablockFreePtr(wdb);
}

int findFree(struct MasterState *ms)
{
  int i;
  for (i = 0; i < ms->nodecount; i += 1)
    if (ms->workers[i].isFree)
      return i;
  return -1;
}

void clogSendAlert( const char *fmt, ...)
{
  va_list args;
  static char buf[16384];
  struct DataBlock *db;
  int len;
  va_start(args, fmt);
  vsprintf(buf, fmt, args);
  va_end( args );
  len = strlen(buf);
  db = clDatablockNewFromBlock(buf,len+1);
  sendBlock(0, db, MSG_ALERT, 0.0);
  clDatablockFreePtr(db);
}

double tsScore(struct TreeAdaptor *ta, gsl_matrix *gm)
{
  struct TreeScore *ts;
  double s;
  ts = clInitTreeScore(gm);
  s = clScoreTree(ts, ta);
  clFreeTreeScore(ts);
  return s;
}

void writeBestToFile(struct MasterState *ms)
{
  const char *outfname = "treefile.dot";
  struct DataBlock *db;
  printf("score %f\n", ms->bestscore);
  db = clConvertTreeToDot(ms->ta, ms->bestscore, ms->labels, NULL, ms->cfg, NULL, ms->dm);
  clDatablockWriteToFile(db, outfname);
  clDatablockFreePtr(db);
}

void sendExitEveryWhere(void)
{
  int dest;
  for (dest = 1; dest < p; dest += 1) {
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
  ms.cfg = clLoadDefaultEnvironment();
  ms.nodecount = p;
  for (i = 0; i < ms.nodecount; i += 1) {
    ms.workers[i].isFree = 1;
    ms.workers[i].lastScore = 0;
  }
  ms.workers[0].isFree = 0; // Master is never free

  ms.clbdb = clFileToDataBlockPtr(fname);
  printf("Read file: %s\n", fname);
  ms.dm = clbDBDistMatrix(ms.clbdb);
  ms.labels = clbDBLabels(ms.clbdb);
  printf("Loaded distmatrix with %d entries.\n", (int) ms.dm->size1);
  ms.ta = clTreeaNew(0, ms.dm->size1);
  ms.bestscore = tsScore(ms.ta, ms.dm);
  ms.bestTree = clConvertTreeToDot(ms.ta, ms.bestscore, ms.labels, NULL, ms.cfg, NULL, ms.dm);
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
        time_t curt;
        time(&curt);
        if (mustQuit && (curt > quitTime))
          bailer(0);
        tag = receiveMessage(&db, &score, &who);
        if (tag == MSG_ALERT) {
          char *alc = NULL;
          if (db)
            alc = (char *) clDatablockData(db);
          if (!alc)
            alc = "NULL";
          fprintf(stderr, "ALERT %03d: %s\n", who, alc);
          clDatablockFreePtr(db);
          continue;
        }
        ms.workers[who].isFree = 1;
        if (tag == MSG_ROGUE) {
          ms.workers[who].lastScore = 0.0;
        }
        if (tag == MSG_BETTER) {
          if (score > ms.bestscore) {
            dpt = clParseDotDB(db, ms.clbdb);
            clDatablockFreePtr(ms.bestTree);
            ms.bestTree = db;
            if (ms.ta) {
              clTreeaFree(ms.ta);
              ms.ta = NULL;
            }
            ms.ta = dpt->tree;
            clStringstackFree(dpt->labels);
            dpt->labels = NULL;
            ms.bestscore = score;
            writeBestToFile(&ms);
            clFree(dpt);
          } else {
            assert(db);
            clDatablockFreePtr(db);
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

int maxTrialFunc(int leaves)
{
  int minTrials = 4;
  int result = (leaves*leaves*leaves*leaves)/2000;
  if (result < minTrials)
    result = minTrials;
  return result;
}

void calculateTree(struct SlaveState *ss)
{
  int result;
  int failCount = 0;
  int MAXTRIES;
  assert(ss->dm->size1 >= 4);
  assert(ss->dm->size2 >= 4);
  assert(ss->dm->size1 == ss->dm->size2);
  MAXTRIES = maxTrialFunc(ss->dm->size1);
  if (clTreehScore(ss->th) != ss->shouldBeScore) {
    fprintf(stderr, "Early Rogue master... should be %9.9f but got %9.9f\n", ss->shouldBeScore, clTreehScore(ss->th));
    fprintf(stderr, "Resending for new from %d\n", my_rank);
    sendBlock(0, NULL, MSG_ROGUE, 0);
    goto bail;
  }
  else {

//    fprintf(stderr, "verified for %d at  %9.9f\n", my_rank, ss->shouldBeScore);
    ;
    }
  while (failCount < MAXTRIES) {
    result = clTreehImprove(ss->th);
    if (result) {
      struct TreeAdaptor *ta = NULL;
      ta = clTreehTreeAdaptor(ss->th);
      struct DataBlock *db;
      double newScore =  clTreehScore(ss->th);
      db = clConvertTreeToDot(ta, newScore, ss->labels, NULL, ss->cfg, NULL, ss->dm);
      sendBlock(0, db, MSG_BETTER, newScore);
      clDatablockFreePtr(db);
      clTreeaFree(ta);
      goto bail;
    }
    failCount += 1;
  }
//  assert(clTreehScore(th) == ss->shouldBeScore);
  if (clTreehScore(ss->th) != ss->shouldBeScore) {
    fprintf(stderr, "Rogue master... should be %9.9f but got %9.9f\n", ss->shouldBeScore, clTreehScore(ss->th));
    fprintf(stderr, "Resending for new from %d\n", my_rank);
    sendBlock(0, NULL, MSG_ROGUE, 0);
  }
  else
    sendBlock(0, NULL, MSG_NOBETTER, ss->myLastScore);
bail:
  return;
}

void doSlaveLoop(void) {
  struct DataBlock *db;
  int tag, dum;
  double score;
  struct SlaveState ss;
  struct DotParseTree *dpt;
  ss.cfg = clLoadDefaultEnvironment();
  ss.myLastScore = 0;
  ss.dbdm = NULL;
  ss.bestdb = NULL;
  ss.ta = NULL;
  ss.th = NULL;
  for (;;) {
    tag = receiveMessage(&db, &score, &dum);
    clogSendAlert("got tag %d with db %08x", tag, (unsigned int) db);
    switch (tag) {

      case MSG_EXIT:
        fprintf(stderr, "Slave %d exitting...\n", my_rank);
        MPI_Finalize();
        exit(0);
        return;

      case MSG_LOADCLB:
        ss.dbdm = db;
        srand(time(NULL) + my_rank * 107);
//        printf("SLAVE: db ptr %p\n",db);
//        printf("SLAVE: db size %d\n",clDatablockSize(db));
        ss.dm = clbDBDistMatrix(ss.dbdm);
        ss.labels = clbDBLabels(ss.dbdm);
//        printf("SLAVE:dist matrix size %d\n",ss.dm->size1);
        break;

      case MSG_NEWASSIGNMENT:
//        assert(score != ss.myLastScore);
        ss.shouldBeScore = score;
        ss.myLastScore = score;
        if (ss.bestdb) {
          clDatablockFreePtr(ss.bestdb);
          ss.bestdb = NULL;
        }
        ss.bestdb = db;
        dpt = clParseDotDB(db, ss.dbdm);
        if (dpt->labels) {
          clStringstackFree(dpt->labels);
          dpt->labels = NULL;
        }
        ss.ta = dpt->tree;
        if (ss.th) {
          clTreehFree(ss.th);
          ss.th = NULL;
        }
        ss.th = clTreehNew(ss.dm, ss.ta);
        clFree(dpt);
//        fprintf(stderr, "SLAVE %d got new assignment with score %f\n", my_rank, ss.myLastScore);
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
  rec_db = clDatablockNewFromBlock(message,size);
  clFree(message);
  db = unwrapForTag(rec_db, &tag, score);
  *ptr = db;
  clDatablockFreePtr(rec_db);

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
    dbsize = clDatablockSize(dbinp);
  len = dbsize+4+sizeof(double);
  bigblock = clCalloc(len,1);
  memcpy(bigblock, &tag, 4);
  memcpy(bigblock+4, &score, sizeof(double));
  if (dbsize)
    memcpy(bigblock+4+sizeof(double), clDatablockData(dbinp), dbsize);
  result = clDatablockNewFromBlock(bigblock, len);
  clFree(bigblock);
  return result;
}

struct DataBlock *unwrapForTag(struct DataBlock *dbbig, int *tag, double *score)
{
  struct DataBlock *result = NULL;
  int len;
  unsigned char *smallblock = NULL;

  len = clDatablockSize(dbbig)-4-sizeof(double);
  assert(len >= 0);
  if (len) {
    smallblock = clCalloc(len,1);
    memcpy(smallblock, ((char *)clDatablockData(dbbig))+4+sizeof(double), len);
    result = clDatablockNewFromBlock(smallblock, len);
    clFree(smallblock);
  }
  memcpy(tag, clDatablockData(dbbig), 4);
  if (score)
    memcpy(score, clDatablockData(dbbig)+4, sizeof(double));
  return result;
}

int main(int argc, char **argv)
{
  int i;

  for (i = 0; i < argc; i += 1)
    if (i < argc-1 && strcmp(argv[i], "-t") == 0) {
      mustQuit = 1;
      quitTime = time(NULL) + atoi(argv[i+1]);
      break;
    }
  MPI_Init(&argc, &argv);
    setMPIGlobals();

  if (my_rank == 0) {
    doMasterLoop();
  } else {
    doSlaveLoop();
  }

  return 0;
}
