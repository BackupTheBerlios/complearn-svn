#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <complearn/complearn.h>
#include <mpi/mpi.h>

#define PROTOTAG 50

#define MSG_NONE 0
#define MSG_LOADCLB 1
#define MSG_EXIT 2

struct MasterSlaveModel {
  int isFree;
  double lastScore;
};

struct MasterState {
  int nodecount;
  struct MasterSlaveModel *workers;
  double bestscore;
  gsl_matrix *dm;
  struct DataBlock *clbdb;
  struct DataBlock *bestTree;
  struct TreeAdaptor *ta;
};


struct SlaveState {
  struct DataBlock *dbdm;
  gsl_matrix *dm;
  struct DataBlock *best;
};

void doMasterLoop(void);
void doSlaveLoop(void);
int receiveMessage(struct DataBlock **ptr, double *score);
struct DataBlock *wrapWithTag(struct DataBlock *dbinp, int tag, double score);
struct DataBlock *unwrapForTag(struct DataBlock *dbbig,int *tag,double *score);

int my_rank;
int p;

void setMPIGlobals(void) {
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &p);
}

void sendBlock(int dest, struct DataBlock *idb, int tag, double d)
{
  struct DataBlock *wdb;
  wdb = wrapWithTag(idb, tag, d);
  MPI_Send(datablockData(wdb), datablockSize(wdb), MPI_CHAR, dest,
     PROTOTAG, MPI_COMM_WORLD);
}
void doMasterLoop(void) {
  struct MasterState ms;
  int dest;
  const char *fname = "distmatrix.clb";
  ms.workers = clCalloc(sizeof(struct MasterSlaveModel), p);
  ms.nodecount = p;
  ms.bestscore = 0;
  struct DataBlock *db, *wdb;
  ms.clbdb = fileToDataBlockPtr(fname);
  printf("Read file: %s\n", fname);
  ms.dm = clbDBDistMatrix(ms.clbdb);
  printf("Loaded distmatrix with %d entries.\n", ms.dm->size1);
  ms.ta = treeaNew(0, ms.dm->size1);
//  printf("Master: got DataBlockDistMatrix, size %d\n", datablockSize(wdb));
//  printf("Master: sending...\n");
  for (dest = 1; dest < p ; dest += 1) {
    printf("sending MSG_LOADCLB to %d\n", dest);
    sendBlock(dest, ms.clbdb, MSG_LOADCLB, 3.3333);
  }
  for (dest = 1; dest < p ; dest += 1) {
    printf("sending MSG_EXIT to %d\n", dest);
    sendBlock(dest, NULL, MSG_EXIT, 3.3333);
  }
}

void doSlaveLoop(void) {
  struct DataBlock *db;
  int tag;
  double score;
  struct SlaveState ss;
  for (;;) {
    tag = receiveMessage(&db, &score);
//    printf("Got tag: %d\n", tag);
    switch (tag) {

      case MSG_EXIT:
        return;

      case MSG_LOADCLB:
        ss.dbdm = db;
//        ss.dm = clbDistMatrixLoad(ss.dbdm);
        break;

      default:
        fprintf(stderr, "Unknown CL message code: %d\n" , tag);
        exit(1);
    }
  }
}

int receiveMessage(struct DataBlock **ptr, double *score) {
  int source = 0;
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
  MPI_Get_count(&status, MPI_CHAR, &size);
//  printf("got this length from the probe: %d\n",size);
  message = clCalloc(size,1);
  MPI_Recv(message, size, MPI_CHAR, source, PROTOTAG, MPI_COMM_WORLD, &status);
  rec_db = datablockNewFromBlock(message,size);
  db = unwrapForTag(rec_db, &tag, score);

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

  MPI_Finalize();
  return 0;
}
