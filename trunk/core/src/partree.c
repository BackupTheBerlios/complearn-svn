#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <complearn/complearn.h>
#include <mpi/mpi.h>

#define PROTOTAG 50

#define MSG_NONE 0
#define MSG_INIT 1
#define MSG_EXIT 2

int my_rank;
int p;

void setMPIGlobals(void) {
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &p);
}

gsl_matrix *clbDistMatrix(char *fname);

struct DataBlock *wrapWithTag(struct DataBlock *dbinp, int tag, double score);
struct DataBlock *unwrapForTag(struct DataBlock *dbbig,int *tag,double *score);

void doMasterLoop(void) {
  struct DataBlock *db;
  int dest;
  db = fileToDataBlockPtr("distmatrix.clb");
  fprintf(stderr,"Master: read file DataBlockDistMatrix, size %d\n", datablockSize(db));
  fprintf(stderr,"Master: sending...\n");
  for (dest = 1; dest < p ; dest += 1) {
    fprintf(stderr,"sending to %d\n", dest);
    MPI_Send(datablockData(db), datablockSize(db), MPI_CHAR, dest,
       PROTOTAG, MPI_COMM_WORLD);
    fprintf(stderr,"sent to %d\n", dest);
  }
  for (;;)
    ;
}
void doSlaveLoop(void) {
  fprintf(stderr,"in slave loop, my_rank=%d\n", my_rank);
  for (;;) {
    struct DataBlock *db;
    int tag;
    tag = receiveMessage(&db);
    if (tag == MSG_NONE) {
      fprintf(stderr,"Got no messages...\n");
      sleep(1);
    } else {
      int dbsize;
      dbsize = db ? datablockSize(db) : 0;
      fprintf(stderr,"Got tag %d with size %d\n", tag, dbsize);
    }
  }
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
  unsigned int len;
  unsigned char *smallblock = NULL;

  len = datablockSize(dbbig)-4-sizeof(double);
  assert(len >= 0);
  if (len) {
    clCalloc(len,1);
    memcpy(smallblock, ((char *)datablockData(dbbig))+4+sizeof(double), len);
    memcpy(tag, datablockData(dbbig), 4);
    if (score)
      memcpy(score, datablockData(dbbig)+4, sizeof(double));
    result = datablockNewFromBlock(smallblock, len);
    clFree(smallblock);
  }
  return result;
}

int receiveMessage(struct DataBlock **ptr)
{
  int flag=0;
  int i;
  unsigned char *message;
  int tag;
  MPI_Status status;
  fprintf(stderr,"About to probe...\n");
  //MPI_Iprobe(MPI_ANY_SOURCE, PROTOTAG, MPI_COMM_WORLD, &flag, &status);
  MPI_Probe(0, PROTOTAG, MPI_COMM_WORLD, &status);
  fprintf(stderr,"done probing with flag %d\n", flag);
//  if (flag == 0 || status.MPI_ERROR != MPI_SUCCESS) {
//    *ptr = NULL;
//    return MSG_NONE;
//  }
/*  if (status.MPI_ERROR != MPI_SUCCESS) {
    sleep(1);
  } else {
    break;
  }
  */
  MPI_Get_count(&status, MPI_CHAR, &i);
  fprintf(stderr,"got this length from the probe: %d\n",i);
  message = clCalloc(i,1);
  MPI_Recv(message, i, MPI_CHAR, status.MPI_SOURCE, PROTOTAG, MPI_COMM_WORLD, &status);

  return 0;
}

int main(int argc, char **argv)
{
  int source;
  int silen = 128;
  int gherr;
  char hname[128];
  char *message;
  MPI_Status status;

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


