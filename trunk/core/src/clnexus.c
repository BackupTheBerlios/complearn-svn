#include <complearn/complearn.h>
#include <ctype.h>
#include <string.h>

static const char *nexHeaderString = ""
"#nexus\n"
"\n"
"BEGIN Taxa;\n"
"DIMENSIONS ntax=%d;\n"
"TAXLABELS\n";

static const char *nexMidString = ""
";\n"
"END; [Taxa]\n"
"\n";

static const char *nexPreDistString = ""
"BEGIN Distances;\n"
"DIMENSIONS ntax=%d;\n"
"FORMAT labels=left diagonal triangle=both;\n"
"MATRIX\n";


static const char *makePreDistString(gsl_matrix *gm)
{
  static char result[1024];
  sprintf(result, nexPreDistString, gm->size1);
  return result;
}

static const char *makeHeaderString(int s)
{
  static char result[1024];
  sprintf(result, nexHeaderString, s);
  return result;
}

char *nodeToNewick(struct TreeAdaptor *ta, struct LabelPerm *lp, struct StringStack *labels, int me, int fromWhere) {
  //int s = clStringstackSize(labels);
  int a[3];
  int todo=0;
  int i;
  int rs=1;
  char *result[3];
  char *bigr;
  int done = 0;
  if (clTreeaIsQuartettable(ta, me))
    return clStrdup(clStringstackReadAt(labels, clLabelpermColIndexForNodeID(lp, me)));
  todo = sizeof(a)/sizeof(a[0]);
  clAdjaNeighbors(clTreeaAdjAdaptor(ta), me, a, &todo);
  for (i = 0; i < todo; i += 1)
    if (a[i] != fromWhere) {
      result[done++] = nodeToNewick(ta, lp, labels, a[i], me);
      rs += strlen(result[done-1])+1;
    }
  assert(done == 2 || done == 3);
  bigr = clCalloc(rs,1);
  if (done == 2)
    sprintf(bigr, "(%s,%s)", result[0], result[1]);
  else
    sprintf(bigr, "(%s,%s,%s)",result[0],result[1],result[2]);
  return bigr;
}

struct DataBlock *clMatToNexus(gsl_matrix *gm, struct StringStack *labels, struct TreeAdaptor *ta)
{
  int s = clStringstackSize(labels);
  int ms = 50 * s * s + 8192;
  char *dbm = clCalloc(ms,1);
  char *curSpot = dbm;
  struct DataBlock *res;
  int i, j;
  printf("Matrix is size %d\n", s);
  curSpot += sprintf(curSpot, "%s", makeHeaderString(s));
  for (i = 0; i < s; i += 1)
    curSpot += sprintf(curSpot,"[%d] '%s'\n",i+1,clStringstackReadAt(labels,i));
  curSpot += sprintf(curSpot, "%s", nexMidString);
  if (gm) {
    curSpot += sprintf(curSpot, "%s", makePreDistString(gm));
    for (i = 0; i < s; i += 1) {
      curSpot += sprintf(curSpot,"[%d] '%s'",i+1,clStringstackReadAt(labels,i));
      for (j = 0; j < s; j += 1)
        curSpot += sprintf(curSpot, " %f", gsl_matrix_get(gm,j,i));
      curSpot += sprintf(curSpot, "\n");
    }
    curSpot += sprintf(curSpot, ";\nEND; [Distances]\n");
  }
  if (ta) {
    char *res = nodeToNewick(ta, clTreeaLabelPerm(ta), labels, 0, -1);
    curSpot += sprintf(curSpot, "\nBEGIN Trees;\n[1] tree 'CLQCS'= %s;\nEND; [Trees]\n", res);
    clFree(res);
  }
  res = clStringToDataBlockPtr(dbm);
  clFree(dbm);
  return res;
}

int clIsNexusFile(struct DataBlock *db)
{
  const char *targetStr = "#nexus";
  char s[7];
  int i;
  if (clDatablockSize(db) < sizeof(s))
    return 0;
  memcpy(s, clDatablockData(db), sizeof(s));
  for (i = 0; i < sizeof(s); i += 1)
    s[i] = tolower(s[i]);
  if (!isspace(s[strlen(targetStr)]))
    return 0;
  s[strlen(targetStr)] = '\0';
  return strcmp(targetStr, s) == 0;
}

static int findBlockLength(struct DataBlock *db)
{
  int state = 0;
  int s;
  unsigned char *t = clDatablockData(db);
  assert(isNexusFile(db));
  for (s = 0; s < clDatablockSize(db); s += 1) {
    char c = t[s];
    if (isspace(c)) {
      state = 1;
      continue;
    }
    switch (tolower(c)) {
      case 'e': if (state == 1) state = 2; break;
      case 'n': if (state == 2) state = 3; break;
      case 'd': if (state == 3) state = 4; break;
      case ';': if (state == 4) state = 5; break;
      default:
        state = 0;
    }
    if (state == 5)
      break;
  }
  if (state == 5) {
    unsigned char *t2 = (unsigned char *) index((char *) (t+s), '\n');
    if (t2 == NULL)
      return -1;
    return (t2-t)+1;
  }
  return -1;
}

struct DataBlock *grabNexusBlock(struct DataBlock *db, const char *blockName)
{
  int blen = strlen(blockName);
  int bend;
  char *fs1, *s1 = clCalloc(blen+20,1);
  char *s2 = clCalloc(blen+20,1);
  struct DataBlock *result = NULL, *db2 = NULL;
  sprintf(s1, "begin %s;", blockName);
  fs1 = findSubstringCaseInsensitive(db, s1);
  if (fs1 == NULL)
    goto done;
  db2 = clStringToDataBlockPtr(fs1);
  bend = findBlockLength(db2);
  clDatablockData(db2)[bend] = '\0';
  result = clStringToDataBlockPtr((char *) clDatablockData(db2));
  clDatablockFreePtr(db2);
  done:
    clFree(s1);
    clFree(s2);
    return result;
}

char *findSubstringCaseInsensitive(struct DataBlock *db, const char *tstr)
{
  char *targetStr = clStrdup(tstr);
  char *testStr = clDatablockToString(db);
  char *sub;
  char *result = NULL;
  int i;
  for (i = 0; targetStr[i]; i += 1)
    targetStr[i] = tolower(targetStr[i]);
  for (i = 0; testStr[i]; i += 1)
    testStr[i] = tolower(testStr[i]);
  sub = strstr(testStr, targetStr);
  if (sub != NULL)
    result = (char *) (clDatablockData(db) + (sub - testStr));
  clFree(targetStr);
  clFree(testStr);
  return result;
}

struct DataBlock *getNexusTaxaBlock(struct DataBlock *db)
{
  return grabNexusBlock(db, "Taxa");
}

struct DataBlock *getNexusDistancesBlock(struct DataBlock *db)
{
  return grabNexusBlock(db, "Distances");
}

gsl_matrix *getNexusDistanceMatrix(struct DataBlock *db)
{
  char *labelbuf = clCalloc(clDatablockSize(db)+1, 1);
  int lptr = 0;
  struct DataBlock *tdb = getNexusDistancesBlock(db);
  struct StringStack *ss = clGetNexusLabels(db);
  char *d = (char *) clDatablockData(tdb);
  int i, s = clDatablockSize(tdb);
  int dc = 0;
  int ms = clStringstackSize(ss);
  gsl_matrix *g = gsl_matrix_calloc(ms, ms);
  for (i = 0; i < s; i += 1) {
    char c = d[i];
    if (isspace(c)) {
      if (lptr > 0) {
        labelbuf[lptr++] = '\0';
        if (strspn(labelbuf, "0123456789.-+eE") == lptr-1) {
          if (dc >= ms*ms)
            printf("Warning, discarding extra value %s in matrix\n", labelbuf);
          else {
            gsl_matrix_set(g, dc%ms, dc/ms, atof(labelbuf));
            dc += 1;
          }
        }
      }
      lptr = 0;
      continue;
    }
    labelbuf[lptr++] = c;
  }
  clFree(labelbuf);
  clDatablockFreePtr(tdb);
  clStringstackFree(ss);
  if (dc != ms*ms)
    printf("Warning, not enough values in matrix, filling with 0.\n");
  return g;
}

struct DataBlock *getNexusTreeBlock(struct DataBlock *db)
{
  return grabNexusBlock(db, "Trees");
}

struct StringStack *clGetNexusLabels(struct DataBlock *db)
{
  char *labelbuf = clCalloc(clDatablockSize(db)+1, 1);
  int lptr = 0;
  struct DataBlock *tdb = getNexusTaxaBlock(db);
  struct StringStack *ss = clStringstackNew();
  char *d = (char *) clDatablockData(tdb);
  int i, s = clDatablockSize(tdb);
  for (i = 0; i < s; i += 1) {
    char c = d[i];
    if (isspace(c)) {
      lptr = 0;
      continue;
    }
    if (c == '\'') {
      if (lptr > 0) {
        labelbuf[lptr++] = '\0';
        clStringstackPush(ss, labelbuf);
        lptr = 0;
      }
      continue;
    }
    labelbuf[lptr++] = c;
  }
  clFree(labelbuf);
  clDatablockFreePtr(tdb);
  return ss;
}

int findInSS(struct StringStack *ss, const char *searchstr)
{
  int i;
  for (i = 0; i < clStringstackSize(ss); i += 1)
    if (strcmp(clStringstackReadAt(ss, i), searchstr) == 0)
      return i;
  return -1;
}

static int parsenextree(struct StringStack *labels, struct AdjAdaptor *aa,
struct LabelPerm *lp, char *treeString, int knumused)
{
  int c, i;
  int plev = 0;
  char *labbuf = clCalloc(strlen(treeString)+2, 1);
  int lptr = 0;
  int nbuf[20];
  int nc = 0;
  int mynum = knumused + clStringstackSize(labels);
  knumused += 1;
  for (;;) {
    c = *treeString;
    if (c == '\0')
      break;
    treeString += 1;
    labbuf[lptr++] = c;
    if (c == '(') { plev += 1; if (plev == 1) lptr = 0; continue; }
    if (c == ')') plev -= 1;
    if (plev == 0 || (plev == 1 && c == ',')) {
      int nn;
      assert(lptr > 0);
      assert(nc < 4);
      labbuf[lptr-1] = '\0';
      strtok(labbuf, ":");
      nn = findInSS(labels, labbuf);
      if (nn == -1) {
        if (labbuf[0] == '(')
          nn = parsenextree(labels, aa, lp, labbuf, knumused);
        if (nn == -1) {
          printf("Got bad string: <%s>\n", labbuf);
          exit(0);
        }
      }
      assert(nn != -1);
      nbuf[nc++] = nn;
      lptr = 0;
    }
  }
  free(labbuf);
  for (i = 0 ; i < nc; i += 1)
    clAdjaSetConState(aa, mynum, nbuf[i], 1);
  return mynum;
}

struct TreeAdaptor *getNexusTree(struct DataBlock *db, struct StringStack *labels)
{
  struct TreeAdaptor *ta = NULL;
  struct LabelPerm *lp = NULL;
  struct AdjAdaptor *aa;
  char *tt, *ts;
  int i, j, s, bs;
  s = clStringstackSize(labels);
  bs = 2*s-2;
  ts = getNexusTreeString(db, &tt);
  ta = clTreeaNew(0, s);
  aa = clTreeaAdjAdaptor(ta);
  for (i = 0; i < s; i += 1)
    clTreeaclLabelpermSetColumnIndexToNodeNumber(ta, i, i);
  lp = clTreeaLabelPerm(ta);
  for (i = 0; i < bs; i += 1)
    for (j = 0; j < bs; j += 1)
      if (i != j)
        clAdjaSetConState(aa, i, j, 0);
  parsenextree(labels, aa, lp, ts, 0);
  return ta;
}

char *getNexusTreeString(struct DataBlock *db, char **ttype)
{
  struct DataBlock *tdb = getNexusTreeBlock(db);
  char *p, *pequ, *psemi, *psquot, *psq2;
  char *s = (char *) clDatablockData(tdb);
  char *res;
  if (ttype != NULL)
    *ttype = NULL;
  p = index(s, '\n');
  if (p == NULL)
    return NULL;
  while (*p == '\r' || *p == '\n')
    p += 1;
  pequ = index(p, '=');
  if (pequ == NULL)
    return NULL;
  pequ += 1;
  while ((*pequ != 0) && isspace(*pequ) )
    pequ += 1;
  psemi = index(p, ';');
  if (psemi == NULL)
    return NULL;
  psquot = index(p, '\'');
  if (psquot == NULL)
    return NULL;
  psq2 = index(psquot+1, '\'');
  if (psq2 == NULL)
    return NULL;
  *ttype = clCalloc( psq2 - psquot + 1 , 1 );
  strncpy(*ttype, psquot+1, psq2 - psquot-1);
  res = clCalloc(psemi - pequ + 1, 1);
  strncpy(res, pequ, psemi - pequ);
  return res;
}

int isNexusFile(struct DataBlock *db)
{
  const char *targetStr = "#nexus";
  char s[7];
  int i;
  if (clDatablockSize(db) < sizeof(s))
    return 0;
  memcpy(s, clDatablockData(db), sizeof(s));
  for (i = 0; i < sizeof(s); i += 1)
    s[i] = tolower(s[i]);
  if (!isspace(s[strlen(targetStr)]))
    return 0;
  s[strlen(targetStr)] = '\0';
  return strcmp(targetStr, s) == 0;
}

