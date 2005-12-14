#include "clmalloc.h"
#include <getopt.h>
#include <assert.h>
#include <string.h>

#include <complearn/complearn.h>

static struct GeneralConfig *curEnv;

struct EnvMap *getEnvMap(struct GeneralConfig *g)
{
  return g->em;
}

void printActiveEnvironment(void)
{
  struct EnvMap *em = loadDefaultEnvironment()->em;
  printf("The environment:\n");
  envmapPrint(em);
}

void freeDefaultEnvironment(struct GeneralConfig *g)
{
  assert(g == curEnv);
  if (curEnv->ptr && curEnv->freeappcfg)
    curEnv->freeappcfg(curEnv);
  clFreeifpresent(curEnv->compressor_name);
  if (curEnv->em) {
    envmapFree(curEnv->em);
    curEnv->em = NULL;
  }
  if (curEnv->ca) {
    compaFree(curEnv->ca);
    curEnv->ca = NULL;
  }
  clFreeandclear(curEnv);
}

void updateEMToConfig(struct GeneralConfig *env)
{
  if (curEnv && curEnv->upappcfg)
    curEnv->upappcfg(curEnv);
  if (envmapValueForKey(env->em, "compressor"))
      env->compressor_name = strdup(envmapValueForKey(env->em, "compressor"));
}

void updateConfigToEM(struct GeneralConfig *env)
{
  if (curEnv && curEnv->upappem)
    curEnv->upappem(curEnv);
  if (env->compressor_name) {
    envmapSetKeyVal(env->em, "compressor", env->compressor_name);
    envmapSetKeyMarked(env->em, "compressor");
  }
  if (env->config_filename) {
    envmapSetKeyVal(env->em, "config-file", env->config_filename);
  }
}

struct GeneralConfig *loadDefaultEnvironment()
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
    curEnv->em = envmapNew();
    curEnv->cmdKeeper = stringstackNew();
    curEnv->compressor_name = clStrdup("blocksort");
    readDefaultConfig(curEnv->em);
    updateEMToConfig(curEnv);
  }
  return curEnv;
}

void printOptionHelp(void)
{
  char *s;
  s =
"OPTIONS:\n"
"  -c, --config-file=FILE      in YAML format\n"
"  -S, --size                  compressed size 1 FILE, STRING or DIR\n"
"  -x, --exp                   print out 2^val instead of val\n"
"  -B, --binary                enable binary output mode\n"
"  -P, --svd-project           output a singular value decomposition matrix\n"
"  -s, --suppress              suppress ASCII output\n"
"  -b, --both                  enable both binary and text output mode\n"
"  -H, --html                  output in HTML format\n"
"  -P, --svd-project           activate SVD projection mode\n"
"  -r, --suppressdetails       do not print details to dot file\n"
"  -V, --version\n"
"  -v, --verbose\n"
"  -h, --help\n";

  if (curEnv && curEnv->printapphelp)
    curEnv->printapphelp(curEnv);

  printf("%s",s);
}

char *addNL(const char *inp)
{
  char *out;
  out = clCalloc(strlen(inp) + 2, 1);
  strcpy(out, inp);
  strcat(out, "\n");
  return out;
}

static void cleanupBeforeExit()
{
//  dbefactoryFree(curEnv->da.dbf);
  freeDefaultEnvironment(curEnv);
  curEnv = NULL;
}

void saveCmd(struct GeneralConfig *ev, int argc, char **argv)
{
  static char sbuf[16384];
  struct CLDateTime *cdt;
  char *ptr = sbuf;
  cdt = cldatetimeNow();
  while (*argv) {
    ptr += sprintf(ptr, " %s", *argv);
    argv += 1;
  }
  ptr += sprintf(ptr, " # at %s", cldatetimeToHumString(cdt));
  cldatetimeFree(cdt);
  *ptr = '\0';
  stringstackPush(ev->cmdKeeper, sbuf+1);
}

int complearn_getopt_long(int argc,  char * const argv[], const char *optstring,
                const struct option *longopts, int *longindex, struct GeneralConfig *cfg)
{
  static int fCmdSaved;
  int oldargc = argc;
  int oldoptind;
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
    saveCmd(cfg, oldargc, oldargv);
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
/*  if (longindex) {
    printf("Longindex is first %d\n", *longindex);
  }
  */
  result = getopt_long(argc, argv, optstring, longopts, longindex);
//  printf("Got first result %c with longindex %d\n", result, *longindex);
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
    const char *const def_short_options="C:c:T:BhVvjsbPHrU:Sx";
    static struct option def_long_options[] = {
      { "config-file", 1, NULL, 'c' },
      { "version", 0, NULL, 'V' },
      { "verbose", 0, NULL, 'v' },
      { "suppress", 0, NULL, 's' }, /* suppress ASCII output */
      { "binary", 0, NULL, 'B' },   /* binary output mode */
      { "both", 0, NULL, 'b' },   /* both output mode */
      { "svd-project", 0, NULL, 'P' },
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
        printOptionHelp();
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
        clFreeifpresent(curEnv->compressor_name);
        curEnv->compressor_name = clStrdup(optarg);
        break;
      case 's':
        curEnv->fAscii = 0;
        break;
      case 'c':
        curEnv->config_filename = clStrdup(optarg);
        readSpecificFile(curEnv->em, curEnv->config_filename);
        updateEMToConfig(curEnv);
        break;
      case 'r':
        curEnv->fSuppressVisibleDetails = 1;
        break;
      case 'U':
        handleLine(curEnv->em,optarg);
        updateEMToConfig(curEnv);
        break;
      case 'P':
        curEnv->fSVD = 1;
        break;
      case 'H':
        curEnv->fHTML = 1;
        break;

      default:
        fDidOurOption = 0;
        if (oldopterr)
          clogError("Unrecognized command-line option phase %d with long index %d\n", result, *longindex);
        break;
    }
  }
  } while (fDidOurOption);
  memcpy(cfg, curEnv, sizeof(*curEnv));
  curEnv = NULL;
  return result;
}

