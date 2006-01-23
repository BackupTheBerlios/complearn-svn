#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <complearn/complearn.h>
#include <mpi/mpi.h>

gsl_matrix *clbDistMatrix(char *fname);

int main(int argc, char **argv)
{
  int my_rank;
  int p;
  int source;
  int dest;
  int silen = 128;
  int gherr;
  char hname[128];
  char *message;
  MPI_Status status;
  int tag = 50;
  struct DataBlock *db;
  int i;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &p);

  if (my_rank == 0) {
    gsl_matrix *dm;
    dm = clbDistMatrix("distmatrix.clb");
    db = gslmatrixDump(dm);
    printf("Master: got DataBlockDistMatrix, size %d\n", datablockSize(db));
    printf("Master: sending...\n");
    for (dest = 1; dest < p ; dest += 1) {
      MPI_Send(datablockData(db), datablockSize(db), MPI_CHAR, dest,
         tag, MPI_COMM_WORLD);
    }
  } else {
    source = 0;
    for(;;) {
      MPI_Probe(source, tag, MPI_COMM_WORLD, &status);
      if (status.MPI_ERROR != MPI_SUCCESS) {
        sleep(1);
      } else {
        break;
      }
    }
    MPI_Get_count(&status, MPI_CHAR, &i);
    printf("got this length from the probe: %d\n",i);
    message = malloc(i);
    MPI_Recv(message, i, MPI_CHAR, source, tag, MPI_COMM_WORLD, &status);
  }

  MPI_Finalize();
  return 0;
}


