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
#include <complearn/complearn.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#define _GNU_SOURCE
#include <getopt.h>

#include <gsl/gsl_blas.h>

#include <complearn/ncdapp.h>

static struct GeneralConfig *cur;

static void ncd_freeappconfig(struct GeneralConfig *cur) {
  struct NCDConfig *ncdcfg = (struct NCDConfig *) cur->ptr;
  clFreeifpresent(ncdcfg->output_distmat_fname);
  clFreeandclear(ncdcfg);
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
"  -m, --module=mycomp.so      Load custom dynamic compression module\n"
"  -D, --delcache              clear the Google cache\n"
"  -o, --outfile=distmatname   set the default distance matrix output name\n"
"  -r, --realcomp=pathname     use real compressor, passing in pathname of compressor\n"
"\n";
  printf("%s",s);
}

void loadCompressor(struct GeneralConfig *cur)
{
  int i;
  union PCTypes p;
  struct NCDConfig *ncdcfg = (struct NCDConfig *) cur->ptr;
  if (cur->ca == NULL) {
    if (cur->fVerbose)
      printf("About to load %s..\n", cur->compressor_name);
    cur->ca = clNewCompressorCB(cur->compressor_name);
    if (cur->ca == NULL) {
      //clogError( "Error, cannot load builtin compressor %s\n", cur->compressor_name);
      clPrintCompressors();
      clogError("Error loading %s: %s\n", cur->compressor_name, clLastStaticErrorCB(cur->compressor_name));
    }
    if (cur->fVerbose)
      printf("Done loading %p.\n", cur->ca);
  }
  assert(cur);
  assert(cur->em);
  assert(cur->ca);
  if (cur->fVerbose)
      printf("New compressor instance %s:%p initialized\n", cur->compressor_name, cur->ca);
  for (i = 0; i < clEnvmapSize(cur->em); i += 1) {
    p = clEnvmapKeyValAt(cur->em,i);
    clSetParameterCB(cur->ca, p.sp.key, p.sp.val, clEnvmapIsPrivateAt(cur->em,i));
  }
  if (ncdcfg->fUsingGoogle) {
    struct DataBlock *db;
    db = clStringToDataBlockPtr("m\n");
    cur->M = pow(2.0, clCompressCB(cur->ca, db));
    cur->multiplier = cur->M;
    clDatablockFreePtr(db);
  }
}
void printCounts(struct DataBlockEnumeration *a)
{
  const char *fmtString = "%06.1f ";
  struct DataBlockEnumerationIterator *ia;
  struct DataBlock *dba;
  struct NCDConfig *ncdcfg = (struct NCDConfig *) cur->ptr;
  for ( ia = a->newenumiter(a); (dba = a->istar(a, ia)) ; a->istep(a, ia) ) {
    double pg;
    if (cur->fDoExponentiate && ncdcfg->fUsingGoogle && clDatablockSize(dba) == 1 && (clDatablockData(dba)[0] == 'm' || clDatablockData(dba)[0] == 'M')) {
      pg = 0.0;
    }
    else {
//      ncdcfg->M = 1.0;
      pg = clCompressCB(cur->ca, dba);
    }
    printf(fmtString, clXpremap(pg, cur));
    //double m = clCalculateM();
    clDatablockFreePtr(dba);
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
    cur = clLoadDefaultEnvironment();
    cur->ptr = clCalloc(sizeof(struct NCDConfig),1);
    ncdcfg = (struct NCDConfig *) cur->ptr;
    *ncdcfg = defaultNCDConfig;
    ncdcfg->output_distmat_fname = clStrdup("distmatrix.clb");
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
  const char *const ncd_short_options="f:l:Lp:t:d:w:m:gDo:r:";
  struct option ncd_long_options[] = {
      { "file-mode", 1, NULL, 'f' },
      { "literal-mode", 1, NULL, 'l' },  /* also can be called "quoted mode" */ { "list", 0, NULL, 'L' },         /* list compressors */
      { "plainlist-mode", 1, NULL, 'p' },
      { "termlist-mode", 1, NULL, 't' },
      { "directory-mode", 1, NULL, 'd' },
      { "windowed-mode", 1, NULL, 'w' },
      { "module", 1, NULL, 'm' },
      { "google", 0, NULL, 'g' },
      { "delcache", 0, NULL, 'D' }, /* clear the google cache */
      { "outfile", 1, NULL, 'o' },   /* distmatrix output file <filename> */
      { "realcomp", 1, NULL, 'r' },  /* real compressor with command <cmd> */
      { NULL, 0, NULL, 0 },
  };
  cur = loadNCDEnvironment();
  if (argc == 1) {
    clPrintOptionHelp();
    clPrintCompressors();
    exit(0);
  }
  ncdcfg = (struct NCDConfig *) cur->ptr;
#if HAVE_LIBCSOAP_SOAP_CLIENT_H
//  testGSoapReq();
#endif
  whichLongOpt = 1;
  if (ncdcfg->da.dbf == NULL) /* must init */ {
    ncdcfg->da.desize = 0;
    ncdcfg->da.dbf = clDbefactoryNew();
  }
  while (1) {
    int retval;
    next_option = clComplearn_getopt_long(argc, argv, ncd_short_options, ncd_long_options, &whichLongOpt, cur);
    if (next_option == -1) /* options done */
      break;

    switch (next_option) {
      case 'o':
        clFreeifpresent(ncdcfg->output_distmat_fname);
        ncdcfg->output_distmat_fname = clStrdup(optarg);
        break;
      case 'f':
        clDbefactorySetMode(ncdcfg->da.dbf, DBF_MODE_FILE);
        ncdcfg->da.de[ncdcfg->da.desize++] =  clDbefactoryNewDBE(ncdcfg->da.dbf, optarg);
        break;
      case 't':
        clDbefactorySetMode(ncdcfg->da.dbf, DBF_MODE_STRINGLIST);
        ncdcfg->da.de[ncdcfg->da.desize++] =  clDbefactoryNewDBE(ncdcfg->da.dbf, optarg);
        cur->fUsingFilenames = 1;
        break;
      case 'p':
        clDbefactorySetMode(ncdcfg->da.dbf, DBF_MODE_FILELIST);
        ncdcfg->da.de[ncdcfg->da.desize++] =  clDbefactoryNewDBE(ncdcfg->da.dbf, optarg);
        break;
      case 'l':
        clDbefactorySetMode(ncdcfg->da.dbf, DBF_MODE_QUOTED);
        ncdcfg->da.de[ncdcfg->da.desize++] =  clDbefactoryNewDBE(ncdcfg->da.dbf, optarg);
        break;
      case 'L':
        clPrintCompressors();
//        cleanupBeforeExit();
        exit(0);
        break;
      case 'd':
        clDbefactorySetMode(ncdcfg->da.dbf, DBF_MODE_DIRECTORY);
        ncdcfg->da.de[ncdcfg->da.desize++] =  clDbefactoryNewDBE(ncdcfg->da.dbf, optarg);
        break;
      case 'w':
        clDbefactorySetMode(ncdcfg->da.dbf, DBF_MODE_WINDOWED);
        ncdcfg->da.de[ncdcfg->da.desize++] =  clDbefactoryNewDBE(ncdcfg->da.dbf, optarg);
        break;
      case 'r':
        cur->ca = clNewCompressorCB("real");
        clSetParameterCB(cur->ca, "cmd", optarg, 0);
        cur->compressor_name = clStrdup(optarg);
        break;
      case 'm':
        retval = clCompaLoadDynamicLib(optarg);
        if (retval != 0) {
          fprintf(stderr, "Error loading module %s\n", optarg);
          exit(1);
        }
        break;
      case 'g':
        clDbefactorySetMode(ncdcfg->da.dbf, DBF_MODE_QUOTED);
        cur->compressor_name = clStrdup("google");
        break;
      case 'D':
        clDeleteSavedGC();
        printf("Cache deleted.\n");
//        cleanupBeforeExit();
        exit(0);
        break;

      default:
        printf("Unrecognized option, in case %d\n", next_option);
    }
  }
  if (strcmp(cur->compressor_name, "google") == 0 ||
      (cur->ca != NULL && strcmp(clShortNameCB(cur->ca),"google") == 0)) {
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
        goodop = clAddNL(op);
      }
      else
        goodop = clStrdup(op);
      ncdcfg->da.de[ncdcfg->da.desize++] =  clDbefactoryNewDBE(ncdcfg->da.dbf, goodop);
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
    clPrintProduct(ncdcfg->da.de[0], ncdcfg->da.de[1], cur);
  }
#endif
//  cleanupBeforeExit();
  return 0;
}
