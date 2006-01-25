#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <complearn/complearn.h>
#include <mpi/mpi.h>

#define PROTOTAG 50

#define MSG_NONE 0
#define MSG_INIT 1
#define MSG_EXIT 2

void doMasterLoop(void);
void doSlaveLoop(void);
int receiveMessage(struct DataBlock **ptr);
struct DataBlock *wrapWithTag(struct DataBlock *dbinp, int tag, double score);
struct DataBlock *unwrapForTag(struct DataBlock *dbbig,int *tag,double *score);

int my_rank;
int p;

void setMPIGlobals(void) {
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &p);
}

void doMasterLoop(void) {
  int dest;
  struct DataBlock *db, *wdb;
  db = fileToDataBlockPtr("distmatrix.clb");
  wdb = wrapWithTag(db, 2, 3.3333);
  printf("Master: got DataBlockDistMatrix, size %d\n", datablockSize(wdb));
  printf("Master: sending...\n");
  for (dest = 1; dest < p ; dest += 1) {
    MPI_Send(datablockData(db), datablockSize(wdb), MPI_CHAR, dest,
       PROTOTAG, MPI_COMM_WORLD);
  }
}

void doSlaveLoop(void) {
  struct DataBlock *db;
  int tag;
  tag = receiveMessage(&db);
}

int receiveMessage(struct DataBlock **ptr) {
  int source = 0;
  int size;
  char *message;
  int tag;
  double score;
  struct DataBlock *db;
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
  printf("got this length from the probe: %d\n",size);
  message = clCalloc(size,1);
  MPI_Recv(message, size, MPI_CHAR, source, PROTOTAG, MPI_COMM_WORLD, &status);
  printf("before unwrapForTag call\n");
  db = unwrapForTag( (struct DataBlock *)message, &tag, &score);
  printf("got tag: %d, score: %f\n", tag, score);

  return 0;
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

  printf("in unwrapForTag point A\n");
  len = datablockSize(dbbig)-4-sizeof(double);
  assert(len >= 0);
  printf("in unwrapForTag point B; here's len: %d\n", len);
  if (len) {
    smallblock = clCalloc(len,1);
    printf("in unwrapForTag point C\n");
    memcpy(smallblock, ((char *)datablockData(dbbig))+4+sizeof(double), len);
    printf("in unwrapForTag point D\n");
    memcpy(tag, datablockData(dbbig), 4);
    printf("in unwrapForTag point E\n");
    if (score)
      memcpy(score, datablockData(dbbig)+4, sizeof(double));
    result = datablockNewFromBlock(smallblock, len);
    clFree(smallblock);
  }
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


