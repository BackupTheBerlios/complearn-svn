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
#include <string.h>
#include <getopt.h>
#include <complearn/complearn.h>
#include <time.h>
#include "complearn/maketreeapp.h"

struct StringStack *labels;
struct TreeHolder *dotth;
struct TreeMaster *globtm;
struct GeneralConfig *cur;
gsl_matrix *dm;

static void freedotth (void)
{
  if (dotth) {
    clTreehFree(dotth);
    dotth = NULL;
  }
}

static void maketree_freeappconfig(struct GeneralConfig *cur) {
  struct MakeTreeConfig *maketreecfg = (struct MakeTreeConfig *) cur->ptr;
  clFreeifpresent(maketreecfg->output_tree_fname);
  clFreeandclear(maketreecfg);
}
static void maketree_printapphelp(struct GeneralConfig *cur) {
  char *s;
  s =
"Usage: maketree [OPTION] ... FILE \n\n"

"MAKETREE OPTIONS:\n"
"  -d, --suppress-random-seed  do not use a random(time) seed for PRNG\n"
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
  dotdb = clConvertTreeToDot(ta, score, labels, dotflips, cur, globtm, dm);
  clDatablockWriteToFile(dotdb, maketreecfg->output_tree_fname);
  clDatablockFreePtr(dotdb);
}

void handleBetterTree(struct TreeObserver *tob, struct TreeHolder *th)
{
  printf("Just got new tree with score %f\n", clTreehScore(th));
  struct TreeAdaptor *ta = clTreehTreeAdaptor(th);
  double sc = clTreehScore(th);
  writeDotFile(ta, sc, NULL);
  freedotth();
  clTreeaFree(ta);
  dotth = clTreehClone(th);
}

struct TreeObserver tob = {
    ptr:NULL,
    treeimproved:handleBetterTree,
    treedone:handleBetterTree,
    treesearchstarted:NULL,
    treerejected:NULL,
};

void clFuncstart(struct TreeOrderObserver *tob)
{
  printf("Started order search.\n");
}
void clFuncordimproved(struct TreeOrderObserver *tob, struct TreeMolder *th, struct CLNodeSet *flips)
{
//  printf("order improvement Or(T) = %f\n", clTreemolderScoreScaled(th));
//  printf("With flips set:\n");
//  clNodesetPrint(flips);
  writeDotFile(clTreemolderTreeAdaptor(th), clTreemolderScore(th), flips);
}

void clFuncorddone(struct TreeOrderObserver *tob, struct TreeMolder *tm, struct CLNodeSet *flips)
{
  printf("Score done to Or(T) = %f\n", clTreemolderScoreScaled(tm));
//  printf("With flips set:\n");
//  clNodesetPrint(flips);
  assert(dotth);
  writeDotFile(clTreemolderTreeAdaptor(tm), clTreehScore(dotth), flips);
  //writeDotFile(dotth, flips);
}

struct TreeOrderObserver too = { NULL, clFuncstart, clFuncordimproved, clFuncorddone };

struct GeneralConfig *loadMakeTreeEnvironment()
{
  struct MakeTreeConfig defaultMakeTreeConfig = {
    output_tree_fname: NULL,
    suppressRandomSeed: 0,
  };

  if (!cur) {
    struct MakeTreeConfig *maketreecfg;
    cur = clLoadDefaultEnvironment();
    cur->ptr = clCalloc(sizeof(struct MakeTreeConfig),1);
    maketreecfg = (struct MakeTreeConfig *) cur->ptr;
    *maketreecfg = defaultMakeTreeConfig;
    maketreecfg->output_tree_fname = clStrdup("treefile.dot");
    cur->freeappcfg = maketree_freeappconfig;
    cur->printapphelp = maketree_printapphelp;
  }
  return cur;
}

int main(int argc, char **argv)
{
  int next_option;
  const char *const short_options = "ROTFo:d";
  static struct option long_options[] = {
      { "rooted", 0, NULL, 'R' },
      { "unordered", 0, NULL, 'u' },
      { "text", 0, NULL, 'T' },
      { "outfile", 1, NULL, 'o' },
      { "suppress-random-seed", 0, NULL, 'd' },
      { NULL, 0, NULL, 0 },
  };
  double score;
  struct DRA *res;
  struct TreeBlaster *tb = NULL;
  struct DataBlock *dmdb;
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
    next_option = clComplearn_getopt_long(argc, argv, short_options, long_options, NULL, cur);
    if (next_option == -1) /* options done */
      break;
    switch (next_option) {
      case 'R':
        isRooted = 1;
        clEnvmapSetKeyVal(cur->em, "isRooted", "1");
        break;
      case 'F':
        isEnablingMaxFailCount = 1;
        clEnvmapSetKeyVal(cur->em, "selfAgreementTermination", "0");
        break;
      case 'd':
        maketreecfg->suppressRandomSeed = 1;
        break;
      case 'u':
        isOrdered = 0;
        clEnvmapSetKeyVal(cur->em, "isOrdered", "0");
        break;
      case 'T':
        isText = 1;
        break;
      case 'o':
        clFreeifpresent(maketreecfg->output_tree_fname);
        maketreecfg->output_tree_fname = clStrdup(optarg);
        break;
    }
  }
  if (optind < argc) {
    while (optind < argc)
       fname = argv[optind++];
  }
  if (!maketreecfg->suppressRandomSeed)
    srand(time(NULL));
  if (!fname) {
    printf("Usage: %s <distmatrix.clb>\n", argv[0]);
    exit(1);
  }
  dmdb = clFileToDataBlockPtr(fname);
  printf("Read file: %s\n", fname);
  dm = clReadAnyDistMatrix(dmdb);
  labels = clReadAnyDistMatrixLabels(dmdb);
  if (labels && clStringstackSize(labels) != dm->size1) {
    printf("Error, labels is size %d but matrix is size %d\n", clStringstackSize(labels), dm->size1);
  }
  if (!clbIsCLBFile(dmdb)) {
    ;
  } else {
/*
    struct EnvMap *em = clbEnvMap(fname);
    struct StringStack *ss = clbCommands(fname);
    char *cmd = NULL;
    assert(ss);
    if (cur->cmdKeeper) {
      cmd = clShiftSS(cur->cmdKeeper);
      clStringstackFree(cur->cmdKeeper);
      cur->cmdKeeper = NULL;
    }
    cur->cmdKeeper = ss;
    if (cmd)
      clStringstackPush(cur->cmdKeeper, cmd);
    clEnvmapMerge(cur->em, em);
    //dm = clbDistMatrix(fname);
    //labels = clbLabels(fname);
    clEnvmapFree(em);
    clFreeandclear(cmd);
*/ /*TODO : add command here */
  }

  printf("The matrix is %d by %d\n", (int) dm->size1, (int) dm->size2);

  tm = clTreemasterNewEx(dm, isRooted, cur->em);
  globtm = tm;
  clTreemasterSetTreeObserver(tm, &tob);
  th = clTreemasterFindTree(tm);
  ub = clTreehTreeAdaptor(th);
  s = clTreehScore(th);
  j = clTreemasterTreeCount(tm);
  printf("Examined %d trees total\n", j);
  if (isOrdered) {
    tb = clTreebNew(dm, ub);
    clTreebSetTreeOrderObserver(tb, &too);
    clns = clTreebFindTreeOrder(tb, &score);
    clTreebFree(tb);
  }
  printf("\nNode ordering:\n");
  res = clSimpleWalkTree(ub, clns);
  clDraPrintIntList(res);
  printf("\nLeaf ordering:\n");
  for (j = 0; j < clDraSize(res); j += 1) {
    int nodenum = clDraGetValueAt(res, j).i;
    if (clTreeaIsQuartettable(ub, nodenum))
      printf("%d ", nodenum);
  }
  printf("\nLeaf labels, in %s order:\n", isOrdered ? "best" : "arbitrary");
  struct LabelPerm *lph = clTreeaLabelPerm(ub);
  for (j = 0; j < clDraSize(res); j += 1) {
    int nodenum = clDraGetValueAt(res, j).i;
    if (!clTreeaIsQuartettable(ub, nodenum))
      continue;
    int indnum = clLabelpermColIndexForNodeID(lph, nodenum);
    printf("%s\n", clStringstackReadAt(labels, indnum));
  }
  clLabelpermFree(lph);
  printf("Done.\n");
  clDraFree(res);
  freedotth();
  clTreemasterFree(tm);
  clTreeaFree(ub);
  clGslmatrixFree(dm);
  if (labels) {
    clStringstackFree(labels);
  }
  if (cur && cur->ptr) {
    clFree(cur->ptr);
    cur->ptr = NULL;
  }
  clFreeDefaultEnvironment(cur);
  return 0;
}
