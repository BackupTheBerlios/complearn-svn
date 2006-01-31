#include <complearn/complearn.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include "ncdapp.h"

#include <gsl/gsl_linalg.h>

#if PWD_RDY
#include <pwd.h>
#endif

#if UTS_RDY
#include <sys/utsname.h>
#endif

static void customPrintProduct(struct DataBlockEnumeration *a, struct DataBlockEnumeration *b, const char *rowBegin, const char *rowEnd, const char *elemBegin, const char *elemEnd, struct GeneralConfig *cur);

double xpremap(double inp, struct GeneralConfig *cur)
{
  if (cur->fDoExponentiate)
    return cur->multiplier * pow(2.0, -inp);
  else
    return inp;
}

gsl_matrix *svdProject(gsl_matrix *a)
{
  int retval;
  gsl_matrix *res;
  gsl_matrix *u, *v;
  gsl_vector *s;
  u = gslmatrixClone(a);
  v = gsl_matrix_alloc(a->size2, a->size2);
  s = gsl_vector_alloc(a->size1);
  retval = gsl_linalg_SV_decomp_jacobi(u, v, s);
//  assert(retval == GSL_OK);
  res = gsl_matrix_alloc(a->size1, a->size2);
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, a, u, 0.0, res);
  return res;
}

void printProduct(struct DataBlockEnumeration *a, struct DataBlockEnumeration *b, struct GeneralConfig *cur)
{
  if (cur->fHTML) {
    printf("<table>\n");
    customPrintProduct(a, b, "<tr>\n", "\n</tr>\n", "<td>", "</td>", cur);
    printf("</table>\n");
  }
  else
    customPrintProduct(a, b, "", "\n", "", "  ", cur);
}

struct DataBlock *createCloneWithNLFree(struct DataBlock *db)
{
  struct DataBlock *result;
  result = datablockNewFromBlock(datablockData(db),datablockSize(db)+1);
  datablockData(result)[datablockSize(db)] = '\n';
  datablockFreePtr(db);
  return result;
}

gsl_matrix *getNCDMatrix(struct DataBlockEnumeration *a, struct DataBlockEnumeration *b, struct GeneralConfig *cur)
{
  gsl_matrix *gres;
//  const char *fmtString = "%03.3f ";
  int n1Counter = 0, n1Set = 0;
  int n2Counter = 0, n2Set = 0;
  struct DRA *da = draNew();
  struct DataBlockEnumerationIterator *ia, *ib;
  struct DataBlock *dba, *dbb;

  for ( ia = a->newenumiter(a); (dba = a->istar(a, ia)) ; a->istep(a, ia) ) {
    struct DataBlock *obj;
    n1Counter += 1;
    if (cur->fAddNLAtString)
      dba = createCloneWithNLFree(dba);
    for ( ib = b->newenumiter(b); ( obj = b->istar(b,ib) ) ; b->istep(b,ib) ) {
      int madeNewOne = 0;
      double ncd;
      double outnum;
      dbb = obj;
      if (!n2Set) {
        n2Counter += 1;
      }
      if (cur->fAddNLAtString) {
        dbb = createCloneWithNLFree(dbb);
        madeNewOne = 1;
      }
      ncd = ncdfunc(dba, dbb, cur);
      outnum = xpremap(ncd, cur);
//      printf(fmtString, outnum);
      draSetDValueAt(da, draSize(da), outnum);
      if (madeNewOne)
        datablockFreePtr(dbb);
    }
    n2Set = 1;
    b->ifree(ib);
//    printf("\n");
    datablockFreePtr(dba);
  }
  n1Set = 1;
  a->ifree(ia);
  gres = gsl_matrix_alloc(n1Counter, n2Counter);
  {
    int x1, x2, i = 0;
    for (x1 = 0; x1 < n1Counter; x1++)
      for (x2 = 0; x2 < n2Counter; x2++) {
        gsl_matrix_set(gres, x1, x2, draGetDValueAt(da, i++)); /**  ^-^ **/
      }
  }
  draFree(da);
  return gres;
}

/* TODO: NCD only function; move to a better location */
static void customPrintProduct(struct DataBlockEnumeration *a, struct DataBlockEnumeration *b, const char *rowBegin, const char *rowEnd, const char *elemBegin, const char *elemEnd, struct GeneralConfig *cur)
{
  int n1c, n2c;
  struct DataBlockEnumerationIterator *dei = a->newenumiter(a);
  struct StringStack *labels = stringstackNew();
  struct DataBlock *curdb;
  struct NCDConfig *ncdcfg = (struct NCDConfig *) cur->ptr;
  gsl_matrix *gres;
  while ( ( curdb = a->istar(a, dei) ) ) {
    stringstackPush(labels, a->ilabel(a, dei));
    a->istep(a, dei);
    datablockFreePtr(curdb);
  }
  a->ifree(dei);
  gres = getNCDMatrix(a, b, cur);

  if (cur->fSVD) {
    gres = svdProject(gres);
  }
  if (cur->fBinary) {
    struct DataBlock *dbdmtagged, *dblabelstagged, *dbcommandstagged, *dbenvmap=NULL, *db;
    struct EnvMap *em = envmapNew();
    int i;
    for (i = 0; i < envmapSize(cur->em); i += 1) {
      union PCTypes p;
      p = envmapKeyValAt(cur->em, i);
      if (envmapIsMarkedAt(cur->em, i) && !envmapIsPrivateAt(cur->em, i) )
        envmapSetKeyVal(em, p.sp.key, p.sp.val);
    }
    if (envmapSize(em) > 0)
      dbenvmap = envmapDump(em);
    dbdmtagged = distmatrixDump(gres);
    assert(labels);
    dblabelstagged = labelsDump(labels);
    dbcommandstagged = commandsDump(cur->cmdKeeper);
    db = package_DataBlocks(TAGNUM_TAGMASTER, dbdmtagged, dblabelstagged, dbcommandstagged, dbenvmap, NULL);
    datablockWriteToFile(db, ncdcfg->output_distmat_fname);
    datablockFreePtr(db);
    datablockFreePtr(dblabelstagged);
    datablockFreePtr(dbdmtagged);
  }
  for (n1c = 0; n1c < gres->size1; n1c++) {
    printf(rowBegin);
    if (cur->fShowLabels) {
      printf(elemBegin);
      printf("%s", stringstackReadAt(labels, n1c));
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
	stringstackFree(labels);
}

#define PARAMLINESIZE 1024
static struct StringStack *convertParamsToStringStack(struct EnvMap *em, char
    *startparamstr, char *endparamstr)
{
  int i;
  struct StringStack *ss = stringstackNew();
  char param[PARAMLINESIZE];

  for (i = 0; i < envmapSize(em); i += 1) {
    union PCTypes p;
    p = envmapKeyValAt(em, i);
    if (envmapIsMarkedAt(em, i) && !envmapIsPrivateAt(em, i) ) {
      sprintf(param, "%s%s: %s%s",startparamstr,p.sp.key,p.sp.val,endparamstr);
      stringstackPush(ss, param);
    }
  }
  sprintf(param,"%sUsername: %s%s", startparamstr, getUsername(), endparamstr);
  stringstackPush(ss,param);
  return ss;
}

struct DataBlock *convertTreeToDot(struct TreeAdaptor *ta, double score, struct StringStack *labels, struct CLNodeSet *flips, struct GeneralConfig *cur, struct TreeMaster *tm, gsl_matrix *dm)
{
  int i, j;
  struct LabelPerm *labelperm = treeaLabelPerm(ta);
  struct AdjAdaptor *ad = treeaAdjAdaptor(ta);
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
  nodes = simpleWalkTree(ta, flips);
  dasize = adjaSize(ad);
  assert(dasize > 0);
  assert(draSize(nodes) == dasize);
  dotacc = stringstackNew();
  assert(dotacc);
  assert(labelperm);

  stringstackPush(dotacc, "graph \"tree\" {");
  if (cur && cur->fSuppressVisibleDetails) {
    startparamstr = "/* "; endparamstr = " */";
  } else {
    stringstackPush(dotacc, "label=\"\\");
    startparamstr = ""; endparamstr = "\\n\\";
  }

  sprintf(con1, "%s%s version %s%s", startparamstr, PACKAGE_NAME,
      PACKAGE_VERSION, endparamstr);
  stringstackPush(dotacc, con1);
  if (score != 0.0) {
    sprintf(con1, "%stree score S(T) = %f%s", startparamstr, score,
        endparamstr);
    stringstackPush(dotacc, con1);
  }
  if (cur) {
    params = convertParamsToStringStack(cur->em, startparamstr, endparamstr);
    for (i = 0; i < stringstackSize(params); i += 1)
      stringstackPush(dotacc, stringstackReadAt(params,i));
  }

  if (!(cur && cur->fSuppressVisibleDetails))
    stringstackPush(dotacc, "\";");

  if (cur) {
    for (i = 0; i < stringstackSize(cur->cmdKeeper); i += 1) {
      char *cmd = stringstackReadAt(cur->cmdKeeper, i);
      sprintf(con1, "/* Step %d: %s */", i+1, cmd);
      stringstackPush(dotacc, con1);
    }
  }
  if (tm) {
    int t0, tf, dt;
    if (treemasterStartTime(tm)) {
      sprintf(con1, "/* start time %s */", cldatetimeToHumString(treemasterStartTime(tm)));
      stringstackPush(dotacc, con1);
    }
    if (treemasterEndTime(tm)) {
      sprintf(con1, "/* end   time %s */", cldatetimeToHumString(treemasterEndTime(tm)));
      stringstackPush(dotacc, con1);
    }
    sprintf(con1, "/* total trees: %d */", treemasterTreeCount(tm));
    stringstackPush(dotacc, con1);
    if (treemasterEndTime(tm) && treemasterStartTime(tm)) {
      t0 = cldatetimeToInt(treemasterStartTime(tm));
      tf = cldatetimeToInt(treemasterEndTime(tm));
      dt = tf - t0;
      if (dt > 0) {
        sprintf(con1, "/* trees / sec : %f */", treemasterTreeCount(tm) /
            (double) dt);
        stringstackPush(dotacc, con1);
      }
    }
  }
  sprintf(con1,"/* Hostname: %s */", getHostname());
  stringstackPush(dotacc,con1);
  sprintf(con1,"/* Machine desc: %s */", getUTSName());
  stringstackPush(dotacc,con1);
  sprintf(con1,"/* PID: %d */", getPID());
  stringstackPush(dotacc,con1);

  int rootnode = -1;
  for (i = 0; i < dasize; i += 1) {
    int nodenum = draGetValueAt(nodes, i).i;
    assert(nodenum >= 0 && nodenum <= 3 * dasize);
    char *str;
    char *extrastr = "";
    if (treeaIsRoot(ta, nodenum)) {
      extrastr=",style=bold";
      rootnode = nodenum;
    }
    if (treeaIsQuartettable(ta, nodenum) && labels)
      str = stringstackReadAt(labels, labelpermColIndexForNodeID(labelperm,nodenum));
    else {
      sprintf(labbuf, "%c%d", treeaIsQuartettable(ta, nodenum) ? 'L' : 'k', nodenum);
      str = labbuf;
    }
    sprintf(lab,"%d [label=\"%s\"%s];", nodenum, str, extrastr);
    stringstackPush(dotacc, lab);
  }
  for (i = 0; i < dasize; i += 1) {
    int n1 = draGetValueAt(nodes, i).i;
    assert(n1 >= 0 && n1 <= 3 * dasize);
    sprintf(con1, "%d", n1);
    for (j = 0; j < dasize; j += 1) {
      int n2 = draGetValueAt(nodes, j).i;
//      printf("For %d, got %d on labelperm %p\n", j, n2, labelperm);
      assert(n2 >= 0 && n2 <= 3 * dasize);
      char con2[1024];
      sprintf(con2, "%d", n2);
      if (n1 > n2 && adjaGetConState(ad, n1, n2)) {
        char buf[4096];
        sprintf(buf, "%s -- %s [weight=\"2\"];", con1, con2);
        stringstackPush(dotacc, buf);
      }
    }
  }
  if (rootnode != -1) {
    sprintf(lab,"%d [label=\"%s\",color=white,fontcolor=black];", 9999, "root");
    stringstackPush(dotacc, lab);
    sprintf(lab,"%d -- %d;", 9999, rootnode);
    stringstackPush(dotacc, lab);
  }
  /* Perimeter walker */
  if (flips)
  {
    int i;
    struct DRA *dapairs;
    dapairs = treeaPerimPairs(ta, flips);
    for (i = 0; i < draSize(dapairs); i += 1) {
      int dmx = labelpermColIndexForNodeID(labelperm,draGetValueAt(dapairs,i).ip.x);
      int dmy = labelpermColIndexForNodeID(labelperm,draGetValueAt(dapairs,i).ip.y);
      double disthere = gsl_matrix_get(dm, dmx, dmy);
      sprintf(lab, "i%d [label=\"%03.3f\",color=\"white\"];", i, disthere);
      stringstackPush(dotacc, lab);
      sprintf(lab, "i%d -- %d [style=\"dotted\"];",  i, draGetValueAt(dapairs, i).ip.x);
      stringstackPush(dotacc, lab);
      sprintf(lab, "i%d -- %d [style=\"dotted\"];",  i, draGetValueAt(dapairs, i).ip.y);
      stringstackPush(dotacc, lab);
    }
    draFree(dapairs);
  }
  stringstackPush(dotacc, "}");
  tmpsize = 0;
  for (i = 0; i < stringstackSize(dotacc); i += 1)
    tmpsize += strlen(stringstackReadAt(dotacc, i)) + 1; /* for the \n */
  dbuff = clCalloc(tmpsize+1, 1); /* extra byte for temporary \0 */
  j = 0;
  for (i = 0; i < stringstackSize(dotacc); i += 1)
    j += sprintf((char *) (dbuff + j), "%s\n", stringstackReadAt(dotacc, i));
  result = datablockNewFromBlock(dbuff,tmpsize);
  stringstackFree(dotacc);
  if (cur && params)
    stringstackFree(params);
  draFree(nodes);
  labelpermFree(labelperm);
  labelperm = NULL;
  return result;
}

const char *getUsername(void)
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

int getPID(void)
{
  return getpid();
}

const char *getHostname(void)
{
  static char hostname[1024];
  gethostname(hostname, 1024);
  return hostname;
}

const char *getUTSName(void)
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

