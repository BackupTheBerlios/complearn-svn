#include <stdio.h>
#include <stdlib.h>

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
 *  and call a single clFunction called compclFunc and pass it one string. The
 *  compression server is expected to return a double precision floating-point
 *  value indicating the compressed size, in bits, of the string.  Custom
 *  compressor module through dynamic library
 *  a custom compression module defines 7 C clFunctions in a dynamic library with
 *  the following names: newcompinst, compclFunc, freecompclFunc, shortname,
 *  longname, compparam, apiver
 *
 */

int isRooted = 0;

int main(int argc, char *argv[])
{
  int i, matched, maxtrials;
  struct DataBlock *db, *dbdm;
  struct DRA *dd;
  FILE *fp;
  char *outfile = "treestats.txt";
  double start, finish;

  gsl_matrix *dm;

  if (argc != 3) {
    printf("Usage: %s <DISTMATRIX> <MAXTRIALS>\n", argv[0]);
    exit(1);
  }

  db = clFileToDataBlockPtr(argv[1]);
  printf("opening %s\n", argv[1]);
  dd = clLoad_DataBlock_package(db);
  dbdm = clScanForTag(dd, TAGNUM_CLDISTMATRIX);
  dm = clDistmatrixLoad(dbdm, 1);

  maxtrials = atoi(argv[2]);
  printf("doing %d trials\n", maxtrials);
  matched = 0;

  start = cldatetimeStaticTimer();
  for ( i = 0; i < maxtrials ; i += 1) {
    struct TreeMaster *tm1, *tm2;
    struct TreeHolder *th1, *th2;
    printf("starting trial #%d of %d\n", i+1, maxtrials);
    gsl_matrix *cdm = clGslmatrixClone(dm);

    tm1 = clTreemasterNew(cdm, isRooted);
    th1 = clTreemasterFindTree(tm1);
    tm2 = clTreemasterNew(cdm, isRooted);
    th2 = clTreemasterFindTree(tm2);

    if (clTreehScore(th1) == clTreehScore(th2))
      matched +=1;
    gsl_matrix_free(cdm);
    clTreemasterFree(tm1);
    clTreemasterFree(tm2);
  }
  finish = cldatetimeStaticTimer();

  fp = clFopen(outfile,"a+");
  fprintf(fp, "%d %d %f %f %f %f %f\n",
      (int) dm->size1, (int) maxtrials, (double) matched / (double) maxtrials,
      finish - start, (finish - start) / (2.0 * maxtrials), log((finish-start)/(2.0*maxtrials))/log(2.0), log(finish-start)/log(2.0));

  printf("matched: %d\n", matched);
  printf("maxtrials: %d\n", maxtrials);
  printf("%.2f%% matching tree pairs for %dx%d matrix\n",
      ( 100.0 * (double) matched / (double) maxtrials), (int) dm->size1, (int) dm->size1);

  clFclose(fp);
  gsl_matrix_free(dm);
  clDatablockFreePtr(db);
  clDatablockFreePtr(dbdm);
  clFree_DataBlock_package(dd);
  exit(0);
}
