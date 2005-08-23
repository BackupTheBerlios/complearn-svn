#include <complearn/complearn.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#define _GNU_SOURCE
#include <getopt.h>

#include <malloc.h>

#if GSL_RDY
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>
#endif

#include "complearn/ncdapp.h"

static struct GeneralConfig *cur;

static void ncd_freeappconfig(struct GeneralConfig *cur) {
  struct NCDConfig *ncdcfg = (struct NCDConfig *) cur->ptr;
  gfreeifpresent(ncdcfg->output_distmat_fname);
  gfreeandclear(ncdcfg);
}
static void ncd_printapphelp(struct GeneralConfig *cur) {
  char *s;
  s = 
"Usage: ncd [OPTION] ... [FILE | STRING | DIR ] [FILE | STRING | DIR] \n\n"

"ENUMERATION MODES:\n"
"  -f, --file-mode=FILE        file; default mode\n"
"  -l, --literal-mode=STRING   string literal\n"
"  -p, --plainlist-mode=FILE   list of file names by linebreaks\n"
"  -t, --termlist-mode=FILE    list of string literals separated by linebreaks\n"
"  -d, --directory-mode=DIR    directory of files\n"
"  -w, --windowed-mode=FILE,firstpos,stepsize,width,lastpos\n"
"                              file be separated into windows\n"
"\n"

"NCD OPTIONS:\n"
"  -C, --compressor=STRING     use builtin compressor\n"
"  -L, --list                  list of available builtin compressors\n"
"  -g, --google                use Google compression (NGD)\n"
"  -D, --delcache              clear the Google cache\n"
"  -o, --outfile=distmatname   set the default distance matrix output name\n"
"\n";
  printf("%s",s);
}

void loadCompressor(struct GeneralConfig *cur)
{
  struct NCDConfig *ncdcfg = (struct NCDConfig *) cur->ptr;
  if (cur->ca == NULL) {
    if (cur->fVerbose)
      printf("About to load %s..\n", cur->compressor_name);
      cur->ca = loadBuiltinCA(cur->compressor_name);
    if (cur->ca == NULL) {
      fprintf(stderr, "Error, cannot load builtin compressor %s\n", cur->compressor_name);
      printBuiltinCompressors();
      exit(1);
    }
    if (cur->fVerbose)
      printf("Done loading %p.\n", cur->ca);
  }
    if (cur->fVerbose)
      printf("Now have instance %p\n", cur->ca->cptr);
  if (ncdcfg->fUsingGoogle) {
    struct DataBlock db;
    db = convertStringToDataBlock("m\n");
    cur->M = pow(2.0, compfuncCA(cur->ca, db));
    cur->multiplier = cur->M;
    freeDataBlock(db);
  }
  updateConfigToEM(cur);
}
void printCounts(struct DataBlockEnumeration *a)
{
  const char *fmtString = "%06.1f ";
  struct DataBlockEnumerationIterator *ia;
  struct DataBlock *dba;
  struct NCDConfig *ncdcfg = (struct NCDConfig *) cur->ptr;
  for ( ia = a->newenumiter(a); (dba = a->istar(a, ia)) ; a->istep(a, ia) ) {
    double pg;
    if (cur->fDoExponentiate && ncdcfg->fUsingGoogle && dba->size == 1 && (dba->ptr[0] == 'm' || dba->ptr[0] == 'M')) {
      pg = 0.0;
    }
    else {
//      ncdcfg->M = 1.0;
      pg = compfuncCA(cur->ca, *dba);
    }
    printf(fmtString, xpremap(pg, cur));
    //double m = calculateM();
    freeDataBlockPtr(dba);
  }
  printf("\n");
}

struct GeneralConfig *loadNCDEnvironment()
{
  struct NCDConfig defaultNCDConfig = {
    fUsingGoogle:    0,
    output_distmat_fname: NULL,
  };

  if (!cur) {
    struct NCDConfig *ncdcfg;
    cur = loadDefaultEnvironment();
    cur->ptr = gcalloc(sizeof(struct NCDConfig),1);
    ncdcfg = (struct NCDConfig *) cur->ptr;
    *ncdcfg = defaultNCDConfig;
    ncdcfg->output_distmat_fname = gstrdup("distmatrix.clb");
    cur->freeappcfg = ncd_freeappconfig;
    cur->printapphelp = ncd_printapphelp;
  }
  return cur;
}

extern struct DataBlock da;
int main(int argc, char **argv)
{
  struct NCDConfig *ncdcfg;
  int next_option, whichLongOpt;
  void testGSoapReq(void);
  const char *const ncd_short_options="f:l:Lp:t:d:w:gDo:";
  struct option ncd_long_options[] = {
      { "file-mode", 1, NULL, 'f' },
      { "literal-mode", 1, NULL, 'l' },  /* also can be called "quoted mode" */ { "list", 0, NULL, 'L' },         /* list compressors */
      { "plainlist-mode", 1, NULL, 'p' },
      { "termlist-mode", 1, NULL, 't' },
      { "directory-mode", 1, NULL, 'd' },
      { "windowed-mode", 1, NULL, 'w' },
      { "google", 0, NULL, 'g' },
      { "delcache", 0, NULL, 'D' }, /* clear the google cache */
      { "outfile", 1, NULL, 'o' },   /* distmatrix output file <filename> */
      { NULL, 0, NULL, 0 },
  };
  cur = loadNCDEnvironment();
  ncdcfg = (struct NCDConfig *) cur->ptr;
#if CSOAP_RDY
//  testGSoapReq();
#endif
  whichLongOpt = 1;
  if (ncdcfg->da.dbf == NULL) /* must init */ {
    ncdcfg->da.desize = 0;
    ncdcfg->da.dbf = newDBEFactory();
  }
  while (1) {
    next_option = complearn_getopt_long(argc, argv, ncd_short_options, ncd_long_options, &whichLongOpt, cur);
    if (next_option == -1) /* options done */
      break;

    switch (next_option) {
      case 'o':
        gfreeifpresent(ncdcfg->output_distmat_fname);
        ncdcfg->output_distmat_fname = gstrdup(optarg);
        break;
      case 'f':
        dbef_setMode(ncdcfg->da.dbf, DBF_MODE_FILE);
        ncdcfg->da.de[ncdcfg->da.desize++] =  dbef_convertStr(ncdcfg->da.dbf, optarg);
        break;
      case 't':
        dbef_setMode(ncdcfg->da.dbf, DBF_MODE_STRINGLIST);
        ncdcfg->da.de[ncdcfg->da.desize++] =  dbef_convertStr(ncdcfg->da.dbf, optarg);
        cur->fUsingFilenames = 1;
        break;
      case 'p':
        dbef_setMode(ncdcfg->da.dbf, DBF_MODE_FILELIST);
        ncdcfg->da.de[ncdcfg->da.desize++] =  dbef_convertStr(ncdcfg->da.dbf, optarg);
        break;
      case 'l':
        dbef_setMode(ncdcfg->da.dbf, DBF_MODE_QUOTED);
        ncdcfg->da.de[ncdcfg->da.desize++] =  dbef_convertStr(ncdcfg->da.dbf, optarg);
        break;
      case 'L':
        printBuiltinCompressors();
//        cleanupBeforeExit();
        exit(0);
        break;
      case 'd':
        dbef_setMode(ncdcfg->da.dbf, DBF_MODE_DIRECTORY);
        ncdcfg->da.de[ncdcfg->da.desize++] =  dbef_convertStr(ncdcfg->da.dbf, optarg);
        break;
      case 'w':
        dbef_setMode(ncdcfg->da.dbf, DBF_MODE_WINDOWED);
        ncdcfg->da.de[ncdcfg->da.desize++] =  dbef_convertStr(ncdcfg->da.dbf, optarg);
        break;
      case 'g':
        dbef_setMode(ncdcfg->da.dbf, DBF_MODE_QUOTED);
        cur->compressor_name = gstrdup("google");
        break;
      case 'D':
        deleteSavedGC();
        printf("Cache deleted.\n");
//        cleanupBeforeExit();
        exit(0);
        break;

      default:
        printf("Unrecognized option, in case %d\n", next_option);
    }
  }
  if (strcmp(cur->compressor_name, "google") == 0 ||
      (cur->ca != NULL && strcmp(shortNameCA(cur->ca),"google") == 0)) {
    ncdcfg->fUsingGoogle = 1;
  }

  //cur->fAddNL = cur->fUsingGoogle && !cur->fUsingFilenames;
 // cur->fAddNLAtString = cur->fUsingGoogle && cur->fUsingFilenames;
  cur->fAddNLAtString = ncdcfg->fUsingGoogle;

  if (optind < argc) {
    while (optind < argc) {
      char *op, *goodop;
      op = argv[optind++];
      if (cur->fAddNL) {
        goodop = addNL(op);
      }
      else
        goodop = gstrdup(op);
      ncdcfg->da.de[ncdcfg->da.desize++] =  dbef_convertStr(ncdcfg->da.dbf, goodop);
    }
  }
#if 1
//  printf("Got %d DataBlock enumerations.\n", da.desize);
  if (cur->fJustSize) {
    if (ncdcfg->da.desize == 1) {
      loadCompressor(cur);
      printCounts(ncdcfg->da.de[0]);
      exit(0);
    }
  }
  if (ncdcfg->da.desize == 2) {
    loadCompressor(cur);
    printProduct(ncdcfg->da.de[0], ncdcfg->da.de[1], cur);
  }
#endif
//  cleanupBeforeExit();
  return 0;
}
