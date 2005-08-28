#include <string.h>
#include <getopt.h>
#include <complearn/complearn.h>
#include <time.h>
#include "complearn/maketreeapp.h"

struct StringStack *labels;
//struct CLNodeSet *dotflips;
struct TreeHolder *dotth;
struct TreeMaster *globtm;
struct GeneralConfig *cur;
gsl_matrix *dm;

static void freedotth (struct TreeHolder *dotth)
{
  if (dotth) {
    treehFree(dotth);
    dotth = NULL;
  }
}

static void maketree_freeappconfig(struct GeneralConfig *cur) {
  struct MakeTreeConfig *maketreecfg = (struct MakeTreeConfig *) cur->ptr;
  gfreeifpresent(maketreecfg->output_tree_fname);
  gfreeandclear(maketreecfg);
}
static void maketree_printapphelp(struct GeneralConfig *cur) {
  char *s;
  s =
"Usage: maketree [OPTION] ... FILE \n\n"

"MAKETREE OPTIONS:\n"
"  -o, --outfile=treename      set the default tree output name\n"
"  -R, --rooted                create rooted tree\n"
"  -O, --ordered               create ordered tree\n"
"  -T, --text-input            format of distance matrix is text\n"
"  -F                          disable self agreement termination and enable\n"
"                              max fail count\n"
"\n";
  printf("%s",s);
}

static void writeDotFile(struct TreeAdaptor *ta, double score, struct CLNodeSet *dotflips)
{
  struct MakeTreeConfig *maketreecfg = (struct MakeTreeConfig *) cur->ptr;
  struct DataBlock *dotdb;
  dotdb = convertTreeToDot(ta, score, labels, dotflips, cur, globtm, dm);
  datablockWriteToFile(dotdb, maketreecfg->output_tree_fname);
  datablockFreePtr(dotdb);
}

void handleBetterTree(struct TreeObserver *tob, struct TreeHolder *th)
{
  printf("Just got new tree with score %f\n", treehScore(th));
  writeDotFile(treehTreeAdaptor(th), treehScore(th), NULL);
  freedotth(dotth);
  dotth = treehClone(th);
}

struct TreeObserver tob = {
    ptr:NULL,
    treeimproved:handleBetterTree,
    treedone:handleBetterTree,
    treesearchstarted:NULL,
    treerejected:NULL,
};

void funcstart(struct TreeOrderObserver *tob)
{
  printf("Started order search.\n");
}
void funcordimproved(struct TreeOrderObserver *tob, struct TreeMolder *th, struct CLNodeSet *flips)
{
  printf("order improvement Or(T) = %f\n", getScoreScaledTM(th));
//  printf("With flips set:\n");
//  clnodesetPrint(flips);
  writeDotFile(treehTreeAdaptorTM(th), getScoreTM(th), flips);
}

void funcorddone(struct TreeOrderObserver *tob, struct TreeMolder *tm, struct CLNodeSet *flips)
{
  printf("Score done to Or(T) = %f\n", getScoreScaledTM(tm));
//  printf("With flips set:\n");
//  clnodesetPrint(flips);
  assert(dotth);
  writeDotFile(treehTreeAdaptorTM(tm), treehScore(dotth), flips);
  //writeDotFile(dotth, flips);
}

struct TreeOrderObserver too = { NULL, funcstart, funcordimproved, funcorddone };

struct GeneralConfig *loadMakeTreeEnvironment()
{
  struct MakeTreeConfig defaultMakeTreeConfig = {
    output_tree_fname: NULL,
  };

  if (!cur) {
    struct MakeTreeConfig *maketreecfg;
    cur = loadDefaultEnvironment();
    cur->ptr = gcalloc(sizeof(struct MakeTreeConfig),1);
    maketreecfg = (struct MakeTreeConfig *) cur->ptr;
    *maketreecfg = defaultMakeTreeConfig;
    maketreecfg->output_tree_fname = gstrdup("treefile.dot");
    cur->freeappcfg = maketree_freeappconfig;
    cur->printapphelp = maketree_printapphelp;
  }
  return cur;
}

int main(int argc, char **argv)
{
  int next_option;
  const char *const short_options = "ROTFo:";
  static struct option long_options[] = {
      { "rooted", 0, NULL, 'R' },
      { "unordered", 0, NULL, 'u' },
      { "text", 0, NULL, 'T' },
      { "outfile", 1, NULL, 'o' },
      { NULL, 0, NULL, 0 },
  };
#if GSL_RDY
  double score;
  struct DoubleA *res;
  struct TreeBlaster *tb;
  struct TreeHolder *th;
  struct TreeMaster *tm;
  struct CLNodeSet *clns = NULL;
  struct TreeAdaptor *ub;
  int j;
  int isRooted = 0;
  int isOrdered = 1;
  int isEnablingMaxFailCount = 0;
  int isText = 0;
  double s;
  char *fname = NULL;
  struct MakeTreeConfig *maketreecfg;
  cur = loadMakeTreeEnvironment();
  maketreecfg = (struct MakeTreeConfig *) cur->ptr;

  while (1) {
    next_option = complearn_getopt_long(argc, argv, short_options, long_options, NULL, cur);
    if (next_option == -1) /* options done */
      break;
    switch (next_option) {
      case 'R':
        isRooted = 1;
        envmapSetKeyVal(cur->em, "isRooted", "1");
        break;
      case 'F':
        isEnablingMaxFailCount = 1;
        envmapSetKeyVal(cur->em, "selfAgreementTermination", "0");
        break;
      case 'u':
        isOrdered = 0;
        envmapSetKeyVal(cur->em, "isOrdered", "0");
        break;
      case 'T':
        isText = 1;
        break;
      case 'o':
        gfreeifpresent(maketreecfg->output_tree_fname);
        maketreecfg->output_tree_fname = gstrdup(optarg);
        break;
    }
  }
  if (optind < argc) {
    while (optind < argc)
       fname = argv[optind++];
  }
  srand(time(NULL));
  if (!fname) {
    printf("Usage: %s <distmatrix.clb>\n", argv[0]);
    exit(1);
  }

  if (isText) {
    dm = cltxtDistMatrix(fname);
    labels = cltxtLabels(fname);
  } else {
    struct EnvMap *em = clbEnvMap(fname);
    struct StringStack *ss = clbCommands(fname);
    char *cmd = NULL;
    assert(ss);
    if (cur->cmdKeeper) {
      cmd = shiftSS(cur->cmdKeeper);
      stringstackFree(cur->cmdKeeper);
      cur->cmdKeeper = NULL;
    }
    cur->cmdKeeper = ss;
    if (cmd)
      stringstackPush(cur->cmdKeeper, cmd);
    envmapMerge(cur->em, em);
    dm = clbDistMatrix(fname);
    labels = clbLabels(fname);
    envmapFree(em);
    gfreeandclear(cmd);
  }

  printf("The matrix is %d by %d\n", dm->size1, dm->size2);

  tm = newTreeMasterEx(dm, isRooted, cur->em);
  globtm = tm;
  setTreeObserverTM(tm, &tob);
  th = findTree(tm);
  ub = treehTreeAdaptor(th);
  s = treehScore(th);
  j = totalTreesExamined(tm);
  printf("Examined %d trees total\n", j);
  if (isOrdered) {
    printf("Proceeding with ordering phase...\n");
    tb = treebNew(dm, ub);
    printf("tb is %p\n", tb);
    treebSetTreeOrderObserver(tb, &too);
    clns = treebFindTreeOrder(tb, &score);
    printf("Got score %f\n", score);
    printf("And flipped node set:\n ");
    clnodesetPrint(clns);
  }
  printf("\nNode ordering:\n");
  res = simpleWalkTree(ub, clns);
  doubleaPrintIntList(res);
  printf("\nLeaf ordering:\n");
  for (j = 0; j < doubleaSize(res); j += 1) {
    int nodenum = doubleaGetValueAt(res, j).i;
    if (treeaIsQuartettable(ub, nodenum))
      printf("%d ", nodenum);
  }
  printf("\nLeaf labels, in %s order:\n", isOrdered ? "best" : "arbitrary");
  for (j = 0; j < doubleaSize(res); j += 1) {
    int nodenum = doubleaGetValueAt(res, j).i;
    struct LabelPerm *lph = treeaLabelPerm(ub);
    if (!treeaIsQuartettable(ub, nodenum))
      continue;
    int indnum = labelpermColIndexForNodeID(lph, nodenum);
    printf("%s\n", stringstackReadAt(labels, indnum));
  }
  printf("Done.\n");
  doubleaFree(res);
  freedotth(dotth);
  freeTreeMaster(tm);
  gslmatrixFree(dm);
  if (labels) {
    stringstackFree(labels);
  }
  return 0;
#else
	printf("no gsl support; maketree impossible\n");
	exit(1);
	return 1;
#endif
}
