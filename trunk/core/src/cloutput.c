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
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <complearn/ncdapp.h>

#include <gsl/gsl_linalg.h>

#define PWD_RDY 1
#define UTS_RDY 1

#if PWD_RDY
#include <pwd.h>
#endif

#if UTS_RDY
#include <sys/utsname.h>
#endif

static void customPrintProduct(struct DataBlockEnumeration *a, struct DataBlockEnumeration *b, const char *rowBegin, const char *rowEnd, const char *elemBegin, const char *elemEnd, struct GeneralConfig *cur);

double clXpremap(double inp, struct GeneralConfig *cur)
{
  if (cur->fDoExponentiate)
    return cur->multiplier * pow(2.0, -inp);
  else
    return inp;
}

gsl_matrix *clSvdProject(gsl_matrix *a)
{
  int retval;
  gsl_matrix *res;
  gsl_matrix *u, *v;
  gsl_vector *s;
  u = clGslmatrixClone(a);
  v = gsl_matrix_alloc(a->size2, a->size2);
  s = gsl_vector_alloc(a->size1);
  retval = gsl_linalg_SV_decomp_jacobi(u, v, s);
//  assert(retval == GSL_OK);
  res = gsl_matrix_alloc(a->size1, a->size2);
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, a, u, 0.0, res);
  return res;
}

void clPrintProduct(struct DataBlockEnumeration *a, struct DataBlockEnumeration *b, struct GeneralConfig *cur)
{
  if (cur->fHTML) {
    printf("<table>\n");
    customPrintProduct(a, b, "<tr>\n", "\n</tr>\n", "<td>", "</td>", cur);
    printf("</table>\n");
  }
  else
    customPrintProduct(a, b, "", "\n", "", "  ", cur);
}

struct DataBlock *clCreateCloneWithNLFree(struct DataBlock *db)
{
  struct DataBlock *result;
  result = clDatablockNewFromBlock(clDatablockData(db),clDatablockSize(db)+1);
  clDatablockData(result)[clDatablockSize(db)] = '\n';
  clDatablockFreePtr(db);
  return result;
}

gsl_matrix *clGetNCDMatrix(struct DataBlockEnumeration *a, struct DataBlockEnumeration *b, struct GeneralConfig *cur)
{
  gsl_matrix *gres;
//  const char *fmtString = "%03.3f ";
  int n1Counter = 0, n1Set = 0;
  int n2Counter = 0, n2Set = 0;
  struct DRA *da = clDraNew();
  struct DataBlockEnumerationIterator *ia, *ib;
  struct DataBlock *dba, *dbb;

  for ( ia = a->newenumiter(a); (dba = a->istar(a, ia)) ; a->istep(a, ia) ) {
    struct DataBlock *obj;
    n1Counter += 1;
    if (cur->fAddNLAtString)
      dba = clCreateCloneWithNLFree(dba);
    for ( ib = b->newenumiter(b); ( obj = b->istar(b,ib) ) ; b->istep(b,ib) ) {
      int madeNewOne = 0;
      double ncd;
      double outnum;
      dbb = obj;
      if (!n2Set) {
        n2Counter += 1;
      }
      if (cur->fAddNLAtString) {
        dbb = clCreateCloneWithNLFree(dbb);
        madeNewOne = 1;
      }
      ncd = clNcdclFunc(dba, dbb, cur);
      outnum = clXpremap(ncd, cur);
//      printf(fmtString, outnum);
      clDraSetDValueAt(da, clDraSize(da), outnum);
      if (madeNewOne)
        clDatablockFreePtr(dbb);
    }
    n2Set = 1;
    b->ifree(ib);
//    printf("\n");
    clDatablockFreePtr(dba);
  }
  n1Set = 1;
  a->ifree(ia);
  gres = gsl_matrix_alloc(n1Counter, n2Counter);
  {
    int x1, x2, i = 0;
    for (x1 = 0; x1 < n1Counter; x1++)
      for (x2 = 0; x2 < n2Counter; x2++) {
        gsl_matrix_set(gres, x1, x2, clDraGetDValueAt(da, i++)); /**  ^-^ **/
      }
  }
  clDraFree(da);
  return gres;
}

struct DataBlock *clMakeCLBDistMatrix(gsl_matrix *gres, struct StringStack *labels, struct StringStack *cmds, struct EnvMap *em)
{
  struct EnvMap *myenv = NULL;
  struct StringStack *mycmds = NULL, *mylabels = NULL;
  struct DataBlock *dbdmtagged, *dblabelstagged, *dbcommandstagged, *dbenvmap=NULL, *db;
  if (em == NULL) {
    em = clEnvmapNew();
    clEnvmapSetKeyVal(em, "defaults", "1");
    myenv = em;
  }
  if (cmds == NULL) {
    cmds = clStringstackNew();
    clStringstackPush(cmds, "# (no commands given)");
    mycmds = cmds;
  }
  if (labels == NULL) {
    int i;
    labels = clStringstackNew();
    for (i = 0; i < gres->size1; i += 1) {
      char buf[16];
      sprintf(buf, "%d", i+1);
      clStringstackPush(labels, buf);
    }
    mylabels = labels;
  }
  int i;
  for (i = 0; i < clEnvmapSize(em); i += 1) {
    union PCTypes p;
    p = clEnvmapKeyValAt(em, i);
    if (clEnvmapIsMarkedAt(em, i) && !clEnvmapIsPrivateAt(em, i) )
      clEnvmapSetKeyVal(em, p.sp.key, p.sp.val);
  }
  if (clEnvmapSize(em) > 0)
    dbenvmap = clEnvmapDump(em);
  dbdmtagged = clDistmatrixDump(gres);
  assert(labels);
  dblabelstagged = clLabelsDump(labels);
  dbcommandstagged = clCommandsDump(cmds);
  db = clPackage_DataBlocks(TAGNUM_TAGMASTER, dbdmtagged, dblabelstagged, dbcommandstagged, dbenvmap, NULL);
//  clDatablockWriteToFile(db, ncdcfg->output_distmat_fname);
//  clDatablockFreePtr(db);
  clDatablockFreePtr(dblabelstagged);
  clDatablockFreePtr(dbdmtagged);
  if (myenv) {
    clEnvmapFree(myenv);
    myenv = NULL;
    em = NULL;
  }
  if (mylabels) {
    clStringstackFree(mylabels);
    mylabels = NULL;
    labels = NULL;
  }
  if (mycmds) {
    clStringstackFree(mycmds);
    mycmds = NULL;
    cmds = NULL;
  }
  return db;
}

/* TODO: NCD only function; move to a better location */
static void customPrintProduct(struct DataBlockEnumeration *a, struct DataBlockEnumeration *b, const char *rowBegin, const char *rowEnd, const char *elemBegin, const char *elemEnd, struct GeneralConfig *cur)
{
  int n1c, n2c;
  struct DataBlockEnumerationIterator *dei = a->newenumiter(a);
  struct StringStack *labels = clStringstackNew();
  struct DataBlock *curdb;
  struct NCDConfig *ncdcfg = (struct NCDConfig *) cur->ptr;
  gsl_matrix *gres;
  while ( ( curdb = a->istar(a, dei) ) ) {
    clStringstackPush(labels, a->ilabel(a, dei));
    a->istep(a, dei);
    clDatablockFreePtr(curdb);
  }
  a->ifree(dei);
  gres = clGetNCDMatrix(a, b, cur);

  if (cur->fSVD) {
    gres = clSvdProject(gres);
  }
  if (cur->fBinary) {
    struct DataBlock *db;
    db = clMakeCLBDistMatrix(gres, labels, cur->cmdKeeper, cur->em);
    clDatablockWriteToFile(db, ncdcfg->output_distmat_fname);
    clDatablockFreePtr(db);
  }
  for (n1c = 0; n1c < gres->size1; n1c++) {
    printf(rowBegin);
    if (cur->fShowLabels) {
      printf(elemBegin);
      printf("%s", clStringstackReadAt(labels, n1c));
      printf(elemEnd);
    }
    for (n2c = 0; n2c < gres->size2; n2c++) {
      printf(elemBegin);
      printf("%09.9f  ", gsl_matrix_get(gres, n1c, n2c));
      printf(elemEnd);
    }
    printf(rowEnd);
  }
  gsl_matrix_free(gres);
	clStringstackFree(labels);
}

#define PARAMLINESIZE 1024
static struct StringStack *convertParamsToStringStack(struct EnvMap *em, char
    *startparamstr, char *endparamstr)
{
  int i;
  struct StringStack *ss = clStringstackNew();
  char param[PARAMLINESIZE];

  for (i = 0; i < clEnvmapSize(em); i += 1) {
    union PCTypes p;
    p = clEnvmapKeyValAt(em, i);
    if (clEnvmapIsMarkedAt(em, i) && !clEnvmapIsPrivateAt(em, i) ) {
      if (strcmp(p.sp.key,"GoogleKey")==0)
        continue;
      sprintf(param, "%s%s: %s%s",startparamstr,p.sp.key,p.sp.val,endparamstr);
      clStringstackPush(ss, param);
    }
  }
  sprintf(param,"%sUsername: %s%s", startparamstr, clGetUsername(), endparamstr);
  clStringstackPush(ss,param);
  return ss;
}

struct DataBlock *clConvertTreeToDot(struct TreeAdaptor *ta, double score, struct StringStack *labels, struct CLNodeSet *flips, struct GeneralConfig *cur, struct TreeMaster *tm, gsl_matrix *dm)
{
  int i, j;
  struct LabelPerm *labelperm = clTreeaLabelPerm(ta);
  struct AdjAdaptor *ad = clTreeaAdjAdaptor(ta);
  struct DRA *nodes;
  static char labbuf[128];
  static char lab[1024];
  struct DataBlock *result;
  struct StringStack *dotacc;
  struct StringStack *params = NULL;
  char con1[1024];
  int dasize, tmpsize;
  char *startparamstr, *endparamstr;
  unsigned char *dbuff;

  assert(ad);
  nodes = clSimpleWalkTree(ta, flips);
  dasize = clAdjaSize(ad);
  assert(dasize > 0);
  assert(clDraSize(nodes) == dasize);
  dotacc = clStringstackNew();
  assert(dotacc);
  assert(labelperm);

  clStringstackPush(dotacc, "graph \"tree\" {");
  if (cur && cur->fSuppressVisibleDetails) {
    startparamstr = "/* "; endparamstr = " */";
  } else {
    clStringstackPush(dotacc, "label=\"\\");
    startparamstr = ""; endparamstr = "\\n\\";
  }

  sprintf(con1, "%s%s version %s%s", startparamstr, PACKAGE_NAME,
      PACKAGE_VERSION, endparamstr);
  clStringstackPush(dotacc, con1);
  if (score != 0.0) {
    sprintf(con1, "%stree score S(T) = %f%s", startparamstr, score,
        endparamstr);
    clStringstackPush(dotacc, con1);
  }
  if (cur) {
    params = convertParamsToStringStack(cur->em, startparamstr, endparamstr);
    for (i = 0; i < clStringstackSize(params); i += 1)
      clStringstackPush(dotacc, clStringstackReadAt(params,i));
  }

  if (!(cur && cur->fSuppressVisibleDetails))
    clStringstackPush(dotacc, "\";");

  if (cur) {
    for (i = 0; i < clStringstackSize(cur->cmdKeeper); i += 1) {
      char *cmd = clStringstackReadAt(cur->cmdKeeper, i);
      sprintf(con1, "/* Step %d: %s */", i+1, cmd);
      clStringstackPush(dotacc, con1);
    }
  }
  if (tm) {
    int t0, tf, dt;
    if (clTreemasterStartTime(tm)) {
      sprintf(con1, "/* start time %s */", clDatetimeToHumString(clTreemasterStartTime(tm)));
      clStringstackPush(dotacc, con1);
    }
    if (clTreemasterEndTime(tm)) {
      sprintf(con1, "/* end   time %s */", clDatetimeToHumString(clTreemasterEndTime(tm)));
      clStringstackPush(dotacc, con1);
    }
    sprintf(con1, "/* total trees: %d */", clTreemasterTreeCount(tm));
    clStringstackPush(dotacc, con1);
    if (clTreemasterEndTime(tm) && clTreemasterStartTime(tm)) {
      t0 = clDatetimeToInt(clTreemasterStartTime(tm));
      tf = clDatetimeToInt(clTreemasterEndTime(tm));
      dt = tf - t0;
      if (dt > 0) {
        sprintf(con1, "/* trees / sec : %f */", clTreemasterTreeCount(tm) /
            (double) dt);
        clStringstackPush(dotacc, con1);
      }
    }
  }
  sprintf(con1,"/* Hostname: %s */", clGetHostname());
  clStringstackPush(dotacc,con1);
  sprintf(con1,"/* Machine desc: %s */", clGetUTSName());
  clStringstackPush(dotacc,con1);
  sprintf(con1,"/* PID: %d */", clGetPID());
  clStringstackPush(dotacc,con1);

  int rootnode = -1;
  for (i = 0; i < dasize; i += 1) {
    int nodenum = clDraGetValueAt(nodes, i).i;
    assert(nodenum >= 0 && nodenum <= 3 * dasize);
    char *str;
    char *extrastr = "";
    if (clTreeaIsRoot(ta, nodenum)) {
      extrastr=",style=bold";
      rootnode = nodenum;
    }
    if (clTreeaIsQuartettable(ta, nodenum) && labels)
      str = clStringstackReadAt(labels, clLabelpermColIndexForNodeID(labelperm,nodenum));
    else {
      sprintf(labbuf, "%c%d", clTreeaIsQuartettable(ta, nodenum) ? 'L' : 'k', nodenum);
      str = labbuf;
    }
    sprintf(lab,"%d [label=\"%s\"%s];", nodenum, str, extrastr);
    clStringstackPush(dotacc, lab);
  }
  for (i = 0; i < dasize; i += 1) {
    int n1 = clDraGetValueAt(nodes, i).i;
    assert(n1 >= 0 && n1 <= 3 * dasize);
    sprintf(con1, "%d", n1);
    for (j = 0; j < dasize; j += 1) {
      int n2 = clDraGetValueAt(nodes, j).i;
//      printf("For %d, got %d on labelperm %p\n", j, n2, labelperm);
      assert(n2 >= 0 && n2 <= 3 * dasize);
      char con2[1024];
      sprintf(con2, "%d", n2);
      if (n1 > n2 && clAdjaGetConState(ad, n1, n2)) {
        char buf[4096];
        sprintf(buf, "%s -- %s [weight=\"2\"];", con1, con2);
        clStringstackPush(dotacc, buf);
      }
    }
  }
  if (rootnode != -1) {
    sprintf(lab,"%d [label=\"%s\",color=white,fontcolor=black];", 9999, "root");
    clStringstackPush(dotacc, lab);
    sprintf(lab,"%d -- %d;", 9999, rootnode);
    clStringstackPush(dotacc, lab);
  }
  /* Perimeter walker */
  if (flips)
  {
    int i;
    struct DRA *dapairs;
    dapairs = clTreeaPerimPairs(ta, flips);
    for (i = 0; i < clDraSize(dapairs); i += 1) {
      int dmx = clLabelpermColIndexForNodeID(labelperm,clDraGetValueAt(dapairs,i).ip.x);
      int dmy = clLabelpermColIndexForNodeID(labelperm,clDraGetValueAt(dapairs,i).ip.y);
      double disthere = gsl_matrix_get(dm, dmx, dmy);
      sprintf(lab, "i%d [label=\"%03.3f\",color=\"white\"];", i, disthere);
      clStringstackPush(dotacc, lab);
      sprintf(lab, "i%d -- %d [style=\"dotted\"];",  i, clDraGetValueAt(dapairs, i).ip.x);
      clStringstackPush(dotacc, lab);
      sprintf(lab, "i%d -- %d [style=\"dotted\"];",  i, clDraGetValueAt(dapairs, i).ip.y);
      clStringstackPush(dotacc, lab);
    }
    clDraFree(dapairs);
  }
  clStringstackPush(dotacc, "}");
  tmpsize = 0;
  for (i = 0; i < clStringstackSize(dotacc); i += 1)
    tmpsize += strlen(clStringstackReadAt(dotacc, i)) + 1; /* for the \n */
  dbuff = clCalloc(tmpsize+1, 1); /* extra byte for temporary \0 */
  j = 0;
  for (i = 0; i < clStringstackSize(dotacc); i += 1)
    j += sprintf((char *) (dbuff + j), "%s\n", clStringstackReadAt(dotacc, i));
  result = clDatablockNewFromBlock(dbuff,tmpsize);
  clFree(dbuff);
  clStringstackFree(dotacc);
  if (cur && params)
    clStringstackFree(params);
  clDraFree(nodes);
  clLabelpermFree(labelperm);
  labelperm = NULL;
  return result;
}

const char *clGetUsername(void)
{
  static char username[128];
#if PWD_RDY
  struct passwd *p;
  uid_t whoami;
  whoami = getuid();
  p = getpwuid(whoami);
  strcpy(username, p->pw_name);
#else
  strcpy(username, "unknown");
#endif
  return username;
}

int clGetPID(void)
{
  return getpid();
}

const char *clGetHostname(void)
{
  static char hostname[1024];
  gethostname(hostname, 1024);
  return hostname;
}

const char *clGetUTSName(void)
{
  static char utsname[1024];
#if UTS_RDY
  struct utsname uts;
  uname(&uts);
  sprintf(utsname, "[%s (%s), %s]", uts.sysname, uts.release, uts.machine);
#else
  strcpy(utsname, "[unknown]");
#endif
  return utsname;
}

