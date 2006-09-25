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

#include <getopt.h>
#include <assert.h>
#include <string.h>

#include <complearn/complearn.h>

static struct GeneralConfig *curEnv;

struct EnvMap *clGetEnvMap(struct GeneralConfig *g)
{
  return g->em;
}

void clPrintActiveEnvironment(void)
{
  struct EnvMap *em = clLoadDefaultEnvironment()->em;
  printf("The environment:\n");
  clEnvmapPrint(em);
}

void clFreeDefaultEnvironment(struct GeneralConfig *g)
{
  assert(g == curEnv);
  if (curEnv->ptr && curEnv->freeappcfg)
    curEnv->freeappcfg(curEnv);
  if (curEnv->em) {
    clEnvmapFree(curEnv->em);
    curEnv->em = NULL;
  }
  if (curEnv->ca) {
    clFreeCB(curEnv->ca);
    curEnv->ca = NULL;
  }
  clFreeandclear(curEnv);
}

struct GeneralConfig *clLoadDefaultEnvironment()
{
  struct GeneralConfig defaultConfig = {
    fVerbose:        0,
    fAscii:          0,
    fBinary:         0,
    fDoExponentiate: 0,
    fJustSize:       0,
    fSVD:            0,
    fHTML:           0,
    fAddNL:          0,
    fAddNLAtString:  0,
    fUsingFilenames: 0,
    fSuppressVisibleDetails: 0,
    fShowLabels:     1,
    fTextToStdout:   1,

    M:             1.0,
    multiplier:    1.0,

    output_tree_fname: "tree",
    config_filename: NULL,
    em: NULL,
    cmdKeeper: NULL,
    compressor_name: NULL,
    ca: NULL,

    ptr:      NULL,
    freeappcfg:   NULL,
    upappcfg:     NULL,
    upappem:      NULL,
    printapphelp: NULL,
    printappenv:  NULL,
  };

  if (!curEnv) {
    curEnv = clMalloc(sizeof(*curEnv));
    *curEnv = defaultConfig;
    curEnv->em = clEnvmapNew();
    curEnv->cmdKeeper = clStringstackNew();
    clReadDefaultConfig(curEnv->em);
    if (curEnv->compressor_name == NULL)
      curEnv->compressor_name = clEnvmapValueForKey(curEnv->em,"compressor");
    if (curEnv->compressor_name == NULL)
      curEnv->compressor_name = "blocksort";
  }
  return curEnv;
}

void clPrintOptionHelp(void)
{
  char *s;
  s =
"OPTIONS:\n"
"  -c, --config-file=FILE      in YAML format\n"
"  -S, --size                  compressed size 1 FILE, STRING or DIR\n"
"  -x, --exp                   print out 2^val instead of val\n"
"  -B, --binary                enable binary output mode\n"
"  -P, --property=name=value   set propery / environment setting name=value\n"
"  -s, --suppress              suppress ASCII output\n"
"  -b, --both                  enable both binary and text output mode\n"
"  -H, --html                  output in HTML format\n"
"  -r, --suppressdetails       do not print details to dot file\n"
"  -V, --version               print version information and exit\n"
"  -v, --verbose               extra diagnostic information\n"
"  -h, --help                  print this help and exit\n";

  if (curEnv && curEnv->printapphelp)
    curEnv->printapphelp(curEnv);

  printf("%s",s);
}

char *clAddNL(const char *inp)
{
  char *out;
  out = clCalloc(strlen(inp) + 2, 1);
  strcpy(out, inp);
  strcat(out, "\n");
  return out;
}

static void cleanupBeforeExit()
{
//  clBlockEnumerationFactoryFree(curEnv->da.dbf);
  clFreeDefaultEnvironment(curEnv);
  curEnv = NULL;
}

void clSaveCmd(struct GeneralConfig *ev, int argc, char **argv)
{
  static char sbuf[16384];
  struct CLDateTime *cdt;
  char *ptr = sbuf;
  cdt = clDatetimeNow();
  while (*argv) {
    ptr += sprintf(ptr, " %s", *argv);
    argv += 1;
  }
  ptr += sprintf(ptr, " # at %s", clDatetimeToHumString(cdt));
  clDatetimeFree(cdt);
  *ptr = '\0';
  clStringstackPush(ev->cmdKeeper, sbuf+1);
}

int clComplearn_getopt_long(int argc,  char * const argv[], const char *optstring,
                const struct option *longopts, int *longindex, struct GeneralConfig *cfg)
{
  static int fCmdSaved;
  int oldargc = argc;
  int oldoptind;
  char *propName = NULL, *propVal = NULL;
  char *oldargv[64];
  int result;
  int oldopterr = opterr;
  int oldlongind = 0;
  int i;
  int fDidOurOption;
  assert(argv);
  assert(argc > 0);
  for (i = 0; i < argc; i += 1)
    oldargv[i] = clStrdup(argv[i]);
  oldargv[i] = NULL;
  if (!fCmdSaved) {
    clSaveCmd(cfg, oldargc, oldargv);
    fCmdSaved = 1;
  }
  assert(cfg);
  curEnv = cfg;
  do {
    oldoptind = optind;
    fDidOurOption = 0;
    if (longindex)
      oldlongind = *longindex;
    opterr = 0; /* suppress error message here */
  result = getopt_long(argc, argv, optstring, longopts, longindex);
  opterr = oldopterr; /* restore old opterr value */
  if (result == ':' && oldopterr) { /* missing parameter must print error */
    if (longindex)
      *longindex = oldlongind;
    optind = oldoptind;
    result = getopt_long(argc, argv, optstring, longopts, longindex);
    if (longindex)
      *longindex = oldlongind;
  }
  if (result == -1) { /* long option successful */
    return result;
  }
  if (result == '?') {
    optind = oldoptind;
    if (longindex)
      *longindex = oldlongind;
    const char *const def_short_options="c:VvsBbP:C:SxHT:rU:h";
    static struct option def_long_options[] = {
      { "config-file", 1, NULL, 'c' },
      { "version", 0, NULL, 'V' },
      { "verbose", 0, NULL, 'v' },
      { "suppress", 0, NULL, 's' }, /* suppress ASCII output */
      { "binary", 0, NULL, 'B' },   /* binary output mode */
      { "both", 0, NULL, 'b' },   /* both output mode */
      { "property", 1, NULL, 'P' },
      { "compressor", 1, NULL, 'C' },
      { "size", 0, NULL, 'S' },     /* Just the compressed size, not NCD */
      { "exp", 0, NULL, 'x' }, /* print out 2^val instead of val */
      { "html", 0, NULL, 'H' },
      { "treeout", 1, NULL, 'T' },   /* tree output file <filename> */
      { "suppressdetails", 0, NULL, 'r' },
      { "user-settings", 1, NULL, 'U' },
      { "help", 0, NULL, 'h' },
      { 0, 0, 0, 0 }
  };
//    printf("Passing in %d for argc, %s for av0, %s for av1, %p for av2\n", oldargc, oldargv[0], oldargv[1], oldargv[2]);
    result = getopt_long(oldargc, oldargv, def_short_options, def_long_options, NULL);
//    printf("Got second result %d with optind %d\n", result, optind);
    if (result == -1) /* options done */
      break;

    fDidOurOption = 1;

    switch (result) {
      case 'h':
        clPrintOptionHelp();
        cleanupBeforeExit();
        exit(0);
      case 'v':
        curEnv->fVerbose = 1;
        break;
      case 'V':
        printf("%s version %s, please send bugs to %s\n", PACKAGE_NAME, PACKAGE_VERSION, PACKAGE_BUGREPORT);
        cleanupBeforeExit();
        exit(0);
      case 'T':
        curEnv->output_tree_fname = clStrdup(optarg);
        break;
      case 'B':
        curEnv->fBinary = 1;
        break;
      case 'S':
        curEnv->fJustSize = 1;
        break;
      case 'x':
        curEnv->fDoExponentiate = 1;
        break;
      case 'b':
        curEnv->fBinary = 1;
        curEnv->fAscii = 1;
        break;
      case 'C':
        curEnv->compressor_name = clStrdup(optarg);
        break;
      case 's':
        curEnv->fAscii = 0;
        break;
      case 'c':
        curEnv->config_filename = clStrdup(optarg);
        clReadSpecificFile(curEnv->em, curEnv->config_filename);
        break;
      case 'r':
        curEnv->fSuppressVisibleDetails = 1;
        break;
      case 'U':
        clHandleLine(curEnv->em,optarg);
        break;
      case 'P':
        propName = strtok(optarg, "=");
	propVal = strtok(NULL, "=");
	if (propName == NULL || propVal == NULL || propName[0] == 0) {
		clLogError("Bad property setting syntax, please see --help.");
	}
	clEnvmapSetKeyVal(curEnv->em, propName, propVal);
        break;
      case 'H':
        curEnv->fHTML = 1;
        break;

      default:
        fDidOurOption = 0;
        if (oldopterr)
          clLogError("Unrecognized command-line option phase %d with long index %d\n", result, *longindex);
        break;
    }
  }
  } while (fDidOurOption);
  memcpy(cfg, curEnv, sizeof(*curEnv));
  curEnv = NULL;
  return result;
}

