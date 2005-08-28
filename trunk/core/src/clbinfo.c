#include <stdio.h>
#include <stdlib.h>
#include <gsl/gsl_matrix.h>
#include <complearn/complearn.h>

/*! \mainpage CompLearn API Documentation
 *
 *  Click links at top to explore docs.
 *
 *  \section sect_comp Five Different Ways to Interface External Compressors with CompLearn
 *
 *  \subsection sub_builtin Builtin Compressors: bzip or zlib
 *  requires installation of bzip2 and/or zlib libraries
 *  For bzip, the parameters blocksize, workfactor, and bzverbosity are honored.
 *  For zlib, the parameter zliblevel is used.
 *  For bzip as well as zlib, all parameters are optional and defaults can be
 *  used instead.
 *
 *  \subsection sub_real Real Compressor
 *  The real compressor interface requires a parameter cmd in the envmap for
 *  newcompinst. cmd specifies the filename of a command to execute to compress
 *  data. This executable is expected to read the input data from STDIN and
 *  output compressed data to STDOUT. The compressed bytes will be counted and
 *  converted to bits.
 *
 *  \subsection sub_virt Virtual Compressor
 *  The virtual compressor interface requires a parameter cmd in the envmap
 *  for newcompinst. cmd specifies the filename of a command to execute to
 *  compress data. This executable is expected to read the input data from
 *  STDIN and output the floating point number representing the compressed
 *  size, in bits, to STDOUT. This number should be printed in standard
 *  ASCII-format as a decimal.
 *
 *  \subsection sub_soap SOAP Compressor Server
 *  requires a url of the form http://localhost:2000/ and urn of the form
 *  urn:hws. The complearn system will connect to the specified SOAP server
 *  and call a single function called compfunc and pass it one string. The
 *  compression server is expected to return a double precision floating-point
 *  value indicating the compressed size, in bits, of the string.  Custom
 *  compressor module through dynamic library
 *  a custom compression module defines 7 C functions in a dynamic library with
 *  the following names: newcompinst, compfunc, freecompfunc, shortname,
 *  longname, compparam, apiver
 *
 */

int isRooted = 0;

int main(int argc, char *argv[])
{
  int i, matched, maxtrials;
  struct DataBlock db, dbdm;
  struct DoubleA *dd;
  FILE *fp;
  char *outfile = "treestats.txt";
  double start, finish;

  gsl_matrix *dm;

  if (argc != 3) {
    printf("Usage: %s <DISTMATRIX> <MAXTRIALS>\n", argv[0]);
    exit(1);
  }

  db = fileToDataBlock(argv[1]);
  printf("opening %s\n", argv[1]);
  dd = load_DataBlock_package(db);
  dbdm = scanForTag(dd, TAGNUM_CLDISTMATRIX);
  dm = distmatrixLoad(dbdm, 1);

  maxtrials = atoi(argv[2]);
  printf("doing %d trials\n", maxtrials);
  matched = 0;

  start = cldatetimeStaticTimer();
  for ( i = 0; i < maxtrials ; i += 1) {
    struct TreeMaster *tm1, *tm2;
    struct TreeHolder *th1, *th2;
    printf("starting trial #%d of %d\n", i+1, maxtrials);
    gsl_matrix *cdm = gslmatrixClone(dm);

    tm1 = treemasterNew(cdm, isRooted);
    th1 = treemasterFindTree(tm1);
    tm2 = treemasterNew(cdm, isRooted);
    th2 = treemasterFindTree(tm2);

    if (treehScore(th1) == treehScore(th2))
      matched +=1;
    gsl_matrix_free(cdm);
    treemasterFree(tm1);
    treemasterFree(tm2);
  }
  finish = cldatetimeStaticTimer();

  fp = clfopen(outfile,"a+");
  fprintf(fp, "%d %d %f %f %f %f %f\n",
      dm->size1, maxtrials, (double) matched / (double) maxtrials,
      finish - start, (finish - start) / (2.0 * maxtrials), log((finish-start)/(2.0*maxtrials))/log(2.0), log(finish-start)/log(2.0));

  printf("matched: %d\n", matched);
  printf("maxtrials: %d\n", maxtrials);
  printf("%.2f%% matching tree pairs for %dx%d matrix\n",
      ( 100.0 * (double) matched / (double) maxtrials), dm->size1, dm->size1);

  clfclose(fp);
  gsl_matrix_free(dm);
  datablockFree(db);
  datablockFree(dbdm);
  doubleaFree(dd);
  exit(0);
}
