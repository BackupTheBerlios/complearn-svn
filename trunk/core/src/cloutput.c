#include <complearn/complearn.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include "complearn/ncdapp.h"

#if GSL_RDY
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>
#endif

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

#if GSL_RDY
gsl_matrix *svdProject(gsl_matrix *a)
{
  int retval;
  gsl_matrix *res;
  gsl_matrix *u, *v;
  gsl_vector *s;
  u = cloneGSLMatrix(a);
  v = gsl_matrix_alloc(a->size2, a->size2);
  s = gsl_vector_alloc(a->size1);
  retval = gsl_linalg_SV_decomp_jacobi(u, v, s);
//  assert(retval == GSL_OK);
  res = gsl_matrix_alloc(a->size1, a->size2);
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, a, u, 0.0, res);
  return res;
}
#endif

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
  struct DataBlock *result = gcalloc(sizeof(*result),1);
  result->size = db->size + 1;
  result->ptr = gmalloc(result->size);
  memcpy(result->ptr, db->ptr, db->size);
  result->ptr[db->size] = '\n';
  freeDataBlockPtr(db);
  return result;
}

#if GSL_RDY
gsl_matrix *getNCDMatrix(struct DataBlockEnumeration *a, struct DataBlockEnumeration *b, struct GeneralConfig *cur)
{
  gsl_matrix *gres;
//  const char *fmtString = "%03.3f ";
  int n1Counter = 0, n1Set = 0;
  int n2Counter = 0, n2Set = 0;
  struct DoubleA *da = newDoubleDoubler();
  struct DataBlockEnumerationIterator *ia, *ib;
  struct DataBlock *dba;
  for ( ia = a->newenumiter(a); (dba = a->istar(a, ia)) ; a->istep(a, ia) ) {
    struct DataBlock *obj;
    n1Counter += 1;
    if (cur->fAddNLAtString)
      dba = createCloneWithNLFree(dba);
    for ( ib = b->newenumiter(b); ( obj = b->istar(b,ib) ) ; b->istep(b,ib) ) {
      struct DataBlock *dbb = obj;
      double ncd;
      double outnum;
      if (!n2Set) {
        n2Counter += 1;
      }
      if (cur->fAddNLAtString)
        dbb = createCloneWithNLFree(dbb);
      ncd = ncdfunc(dba, dbb, cur);
      outnum = xpremap(ncd, cur);
//      printf(fmtString, outnum);
      setDValueAt(da, getSize(da), outnum);
      freeDataBlockPtr(dbb);
    }
    n2Set = 1;
    b->ifree(ib);
//    printf("\n");
    freeDataBlockPtr(dba);
  }
  n1Set = 1;
  a->ifree(ia);
  gres = gsl_matrix_alloc(n1Counter, n2Counter);
  {
    int x1, x2, i = 0;
    for (x1 = 0; x1 < n1Counter; x1++)
      for (x2 = 0; x2 < n2Counter; x2++) {
        gsl_matrix_set(gres, x1, x2, getDValueAt(da, i++)); /**  ^-^ **/
      }
  }
  freeDoubleDoubler(da);
  return gres;
}
#endif

/* TODO: NCD only function; move to a better location */
static void customPrintProduct(struct DataBlockEnumeration *a, struct DataBlockEnumeration *b, const char *rowBegin, const char *rowEnd, const char *elemBegin, const char *elemEnd, struct GeneralConfig *cur)
{
#if GSL_RDY
  int n1c, n2c;
  struct DataBlockEnumerationIterator *dei = a->newenumiter(a);
  struct StringStack *labels = newStringStack();
  struct DataBlock *curdb;
  struct NCDConfig *ncdcfg = (struct NCDConfig *) cur->ptr;
  gsl_matrix *gres;
  while ( ( curdb = a->istar(a, dei) ) ) {
    pushSS(labels, a->ilabel(a, dei));
    a->istep(a, dei);
    freeDataBlockPtr(curdb);
  }
  a->ifree(dei);
  gres = getNCDMatrix(a, b, cur);

  if (cur->fSVD) {
    gres = svdProject(gres);
  }
  if (cur->fBinary) {
    struct DataBlock dbdmtagged, dblabelstagged, dbcommandstagged, dbenvmap, db;
    struct EnvMap *em = newEnvMap();
    int i;
    for (i = 0; i < sizeEM(cur->em); i += 1) {
      union PCTypes p;
      p = getKeyValAt(cur->em, i);
      if (isMarkedAtEM(cur->em, i) && !isPrivateAtEM(cur->em, i) )
        setKeyValEM(em, p.sp.key, p.sp.val);
    }
    if (sizeEM(em) > 0)
      dbenvmap = dumpEnvMap(em);
    dbdmtagged = dumpCLDistMatrix(gres);
    assert(labels);
    dblabelstagged = dumpDMLabels(labels);
    dbcommandstagged = dumpCommands(cur->cmdKeeper);
    db = package_DataBlocks(TAGNUM_TAGMASTER, &dbdmtagged, &dblabelstagged, &dbcommandstagged, &dbenvmap, NULL);
    writeDataBlockToFile(&db, ncdcfg->output_distmat_fname);
    freeDataBlock(db);
    freeDataBlock(dblabelstagged);
    freeDataBlock(dbdmtagged);
  }
  for (n1c = 0; n1c < gres->size1; n1c++) {
    printf(rowBegin);
    if (cur->fShowLabels) {
      printf(elemBegin);
      printf("%s", readAtSS(labels, n1c));
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
	freeSS(labels);
#else
	assert ( 0 && "GSL not supported");
	exit(1);
#endif
}

#define PARAMLINESIZE 1024
static struct StringStack *convertParamsToStringStack(struct EnvMap *em, char
    *startparamstr, char *endparamstr)
{
  int i;
  struct StringStack *ss = newStringStack();
  char param[PARAMLINESIZE];

  for (i = 0; i < sizeEM(em); i += 1) {
    union PCTypes p;
    p = getKeyValAt(em, i);
    if (isMarkedAtEM(em, i) && !isPrivateAtEM(em, i) ) {
      sprintf(param, "%s%s: %s%s",startparamstr,p.sp.key,p.sp.val,endparamstr);
      pushSS(ss, param);
    }
  }
  sprintf(param,"%sUsername: %s%s", startparamstr, getUsername(), endparamstr);
  pushSS(ss,param);
  return ss;
}

struct DataBlock *convertTreeToDot(struct TreeAdaptor *ta, double score, struct StringStack *labels, struct CLNodeSet *flips, struct GeneralConfig *cur, struct TreeMaster *tm, gsl_matrix *dm)
{
  int i, j;
  struct LabelPerm *labelperm = treegetlabelpermTRA(ta);
  struct AdjAdaptor *ad = treegetadjaTRA(ta);
  struct DoubleA *nodes;
  static char labbuf[128];
  static char lab[1024];
  struct DataBlock *result;
  struct StringStack *dotacc;
  struct StringStack *params = NULL;
  char con1[1024];
  int dasize;
  char *startparamstr, *endparamstr;

  assert(ad);
  nodes = simpleWalkTree(ta, flips);
  dasize = adjaSize(ad);
  assert(dasize > 0);
  assert(getSize(nodes) == dasize);
  dotacc = newStringStack();
  assert(dotacc);
  assert(labelperm);

  pushSS(dotacc, "graph \"tree\" {");
  if (cur && cur->fSuppressVisibleDetails) {
    startparamstr = "/* "; endparamstr = " */";
  } else {
    pushSS(dotacc, "label=\"\\");
    startparamstr = ""; endparamstr = "\\n\\";
  }

  sprintf(con1, "%s%s version %s%s", startparamstr, PACKAGE_NAME,
      PACKAGE_VERSION, endparamstr);
  pushSS(dotacc, con1);
  if (score != 0.0) {
    sprintf(con1, "%stree score S(T) = %f%s", startparamstr, score,
        endparamstr);
    pushSS(dotacc, con1);
  }
  if (cur) {
    params = convertParamsToStringStack(cur->em, startparamstr, endparamstr);
    for (i = 0; i < sizeSS(params); i += 1)
      pushSS(dotacc, readAtSS(params,i));
  }

  if (!(cur && cur->fSuppressVisibleDetails))
    pushSS(dotacc, "\";");

  if (cur) {
    for (i = 0; i < sizeSS(cur->cmdKeeper); i += 1) {
      char *cmd = readAtSS(cur->cmdKeeper, i);
      sprintf(con1, "/* Step %d: %s */", i+1, cmd);
      pushSS(dotacc, con1);
    }
  }
  if (tm) {
    int t0, tf, dt;
    if (getStartTimeTM(tm)) {
      sprintf(con1, "/* start time %s */", cldt_to_humstr(getStartTimeTM(tm)));
      pushSS(dotacc, con1);
    }
    if (getEndTimeTM(tm)) {
      sprintf(con1, "/* end   time %s */", cldt_to_humstr(getEndTimeTM(tm)));
      pushSS(dotacc, con1);
    }
    sprintf(con1, "/* total trees: %d */", totalTreesExamined(tm));
    pushSS(dotacc, con1);
    if (getEndTimeTM(tm) && getStartTimeTM(tm)) {
      t0 = cldt_to_i(getStartTimeTM(tm));
      tf = cldt_to_i(getEndTimeTM(tm));
      dt = tf - t0;
      if (dt > 0) {
        sprintf(con1, "/* trees / sec : %f */", totalTreesExamined(tm) /
            (double) dt);
        pushSS(dotacc, con1);
      }
    }
  }
  sprintf(con1,"/* Hostname: %s */", getHostname());
  pushSS(dotacc,con1);
  sprintf(con1,"/* Machine desc: %s */", getUTSName());
  pushSS(dotacc,con1);
  sprintf(con1,"/* PID: %d */", getPID());
  pushSS(dotacc,con1);

  int rootnode = -1;
  for (i = 0; i < dasize; i += 1) {
    int nodenum = getValueAt(nodes, i).i;
    assert(nodenum >= 0 && nodenum <= 3 * dasize);
    char *str;
    char *extrastr = "";
    if (treeIsRoot(ta, nodenum)) {
      extrastr=",style=bold";
      rootnode = nodenum;
    }
    if (treeIsQuartettable(ta, nodenum) && labels)
      str = readAtSS(labels, getColumnIndexForNodeIDLP(labelperm,nodenum));
    else {
      sprintf(labbuf, "%c%d", treeIsQuartettable(ta, nodenum) ? 'L' : 'k', nodenum);
      str = labbuf;
    }
    sprintf(lab,"%d [label=\"%s\"%s];", nodenum, str, extrastr);
    pushSS(dotacc, lab);
  }
  for (i = 0; i < dasize; i += 1) {
    int n1 = getValueAt(nodes, i).i;
    assert(n1 >= 0 && n1 <= 3 * dasize);
    sprintf(con1, "%d", n1);
    for (j = 0; j < dasize; j += 1) {
      int n2 = getValueAt(nodes, j).i;
//      printf("For %d, got %d on labelperm %p\n", j, n2, labelperm);
      assert(n2 >= 0 && n2 <= 3 * dasize);
      char con2[1024];
      sprintf(con2, "%d", n2);
      if (n1 > n2 && adjaGetConState(ad, n1, n2)) {
        char buf[4096];
        sprintf(buf, "%s -- %s [weight=\"2\"];", con1, con2);
        pushSS(dotacc, buf);
      }
    }
  }
  if (rootnode != -1) {
    sprintf(lab,"%d [label=\"%s\",color=white,fontcolor=black];", 9999, "root");
    pushSS(dotacc, lab);
    sprintf(lab,"%d -- %d;", 9999, rootnode);
    pushSS(dotacc, lab);
  }
  /* Perimeter walker */
  if (flips)
  {
    int i;
    struct DoubleA *dapairs;
    dapairs = treeperimpairsTRA(ta, flips);
    for (i = 0; i < getSize(dapairs); i += 1) {
      int dmx = getColumnIndexForNodeIDLP(labelperm,getValueAt(dapairs,i).ip.x);
      int dmy = getColumnIndexForNodeIDLP(labelperm,getValueAt(dapairs,i).ip.y);
      double disthere = gsl_matrix_get(dm, dmx, dmy);
      sprintf(lab, "i%d [label=\"%03.3f\",color=\"white\"];", i, disthere);
      pushSS(dotacc, lab);
      sprintf(lab, "i%d -- %d [style=\"dotted\"];",  i, getValueAt(dapairs, i).ip.x);
      pushSS(dotacc, lab);
      sprintf(lab, "i%d -- %d [style=\"dotted\"];",  i, getValueAt(dapairs, i).ip.y);
      pushSS(dotacc, lab);
    }
    freeDoubleDoubler(dapairs);
  }
  pushSS(dotacc, "}");
  result = gcalloc(sizeof(struct DataBlock), 1);
  result->size = 0;
  for (i = 0; i < sizeSS(dotacc); i += 1)
    result->size += strlen(readAtSS(dotacc, i)) + 1; /* for the \n */
  result->ptr = gcalloc(result->size+1, 1); /* extra byte for temporary \0 */
  j = 0;
  for (i = 0; i < sizeSS(dotacc); i += 1)
    j += sprintf((char *) (result->ptr + j), "%s\n", readAtSS(dotacc, i));
  freeSS(dotacc);
  if (cur && params)
    freeSS(params);
  freeDoubleDoubler(nodes);
  return result;
}

FILE *makeTmpCopyStdin(void)
{
  static char tmpfile[128];
  int fd;
  FILE *fp;
  if (tmpfile[0] == 0) {
    struct DataBlock db = convertFileToDataBlockFP(stdin);
    strcpy(tmpfile, "/tmp/clstdintmp-XXXXXX");
    fd = mkstemp(tmpfile);
    close(fd);
    writeDataBlockToFile(&db, tmpfile);
    freeDataBlock(db);
  }
  fp = fopen(tmpfile,"r");
  return fp;
}

FILE *clfopen(const char *fname, char *mode)
{
  if (strcmp(fname,"-") == 0) {
    return (mode[0] == 'r' ? makeTmpCopyStdin() : stdout);
  }
  else
    return fopen(fname, mode);
}

void clfclose(FILE *fp)
{
  if ( (fp == stdin) || (fp == stdout) )
    return;
  else {
    fclose(fp);
    return;
  }
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

