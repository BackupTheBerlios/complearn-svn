#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <dlfcn.h>
#include <zlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <complearn/complearn.h>
#include <complearn/pathkeeper.h>
#include "complearn/ncdapp.h"

/******** testDL struct ********************/

#define DLNAME "/home/cilibrar/src/shared/complearn/examples/dltest/libart.so.1.0.1"

#define MAX_SS_SIZE 1024
#define TEST_TS_SIZE 200

#define TEST_TREEHOLDER_LEAVES 10

static char *testfile, *testbzfile, *testgzfile, *testzlibfile, *testpg4dir;
struct GeneralConfig *gconf;

struct TransformAdaptor *builtin_UNBZIP(void);
struct TransformAdaptor *builtin_UNGZ(void);
struct TransformAdaptor *builtin_UNZLIB(void);

struct GeneralConfig *loadNCDEnvironment()
{
  struct NCDConfig defaultNCDConfig = {
    fUsingGoogle:    0,

    output_distmat_fname: "distmatrix.clb",
  };

  if (!gconf) {
    struct NCDConfig *ncdcfg;
    gconf = loadDefaultEnvironment();
    gconf->ptr = clCalloc(sizeof(struct NCDConfig),1);
    ncdcfg = (struct NCDConfig *) gconf->ptr;
    *ncdcfg = defaultNCDConfig;
  }
  return gconf;
}

void testDataBlock()
{
  struct DataBlock *dbstr, *dbstr2, *dbfile, *dbcat;
  char *str = "hello, world\n";
	char *str2 = "welcome to the jungle\n";
	char *result;
  dbstr = stringToDataBlockPtr(str);
  assert(strlen(str) == datablockSize(dbstr));
  assert(datablockData(dbstr) != NULL);
  assert(datablockData(dbstr) != (unsigned char *) str);
	result = datablockToString(dbstr);
	assert(strcmp(result,str) == 0);
  dbstr2 = stringToDataBlockPtr(str2);
  assert(datablockData(dbstr2) != NULL);
  assert(datablockData(dbstr2) != (unsigned char *) str2);
  dbcat = datablockCatPtr(dbstr,dbstr2);
  assert(datablockData(dbcat) != NULL);
  assert(datablockData(dbcat) != datablockData(dbstr));
  assert(datablockData(dbcat) != datablockData(dbstr2));
  datablockFreePtr(dbcat);
  datablockFreePtr(dbstr);
  datablockFreePtr(dbstr2);
  dbfile = fileToDataBlockPtr(testfile);
  assert(datablockData(dbfile) != NULL);
  assert(datablockData(dbfile) != (unsigned char *) str);
  datablockFreePtr(dbfile);
  clFreeandclear(result);
}

void testDL2()
{
  struct CompAdaptor *comp = NULL;
  struct EnvMap *em;
  char *sn;
  char *strab = "ab";
  char *straa = "aa";
  char *strsmallalpha = "baaaababba";
  char *strlargealpha = "kdjbabenzo";
  struct DataBlock *dbab, *dbaa, *dbsmallalpha, *dblargealpha;
  double cdbab, cdbaa, cdbsa, cdbla;
  dbab = stringToDataBlockPtr(strab);
  dbaa = stringToDataBlockPtr(straa);
  dbsmallalpha = stringToDataBlockPtr(strsmallalpha);
  dblargealpha = stringToDataBlockPtr(strlargealpha);
  em = getEnvMap(gconf);
  assert(em != NULL);
  envmapSetKeyVal(em, "padding", "40");
  comp = compaLoadDynamicLib(DLNAME);
  assert(comp->cf != NULL);
  //comp->se(comp,em);
  sn = compaShortName(comp);
  assert(strcmp(sn, "art") == 0);
  cdbab = compaCompress(comp, dbab);
  assert(cdbab >= datablockSize(dbab)*8);
  cdbaa = compaCompress(comp, dbaa);
  assert(cdbaa <= datablockSize(dbaa)*8);
  cdbsa = compaCompress(comp, dbsmallalpha);
  cdbla = compaCompress(comp, dblargealpha);
  assert(cdbsa < cdbla);
  datablockFreePtr(dbab);
  datablockFreePtr(dbaa);
  datablockFreePtr(dbsmallalpha);
  datablockFreePtr(dblargealpha);
}

void testDL()
{
  void *dlh;
  char *(*fn)(void);
  struct EnvMap *em;
  em = envmapNew();
  envmapSetKeyVal(em, "ignorethis", "noproblem");
  envmapSetKeyVal(em, "ignorethis2", "ok");
  envmapSetKeyVal(em, "padding", "20");
  dlh = dlopen(DLNAME, RTLD_NOW | RTLD_GLOBAL);
  if (dlh == NULL) {
    clogError( "Error: cannot open dynamic library\n%s\nDid you build it yet?\n", DLNAME);
    clogError( "reason given: %s\n", dlerror());
    exit(1);
  }
  assert("Error: cannot open dynamic library, did you build it yet?" && dlh);
  fn = dlsym(dlh, "newCompAdaptor");
  assert(fn);
  envmapFree(em);
}

void testEM()
{
  struct EnvMap *em;
  union PCTypes p;
  em = envmapNew();
  envmapSetKeyVal(em,"key1","val1");
  envmapSetKeyVal(em,"key2","val2");
  envmapSetKeyVal(em,"key3","val3");
  envmapSetKeyVal(em,"key4","val4");
  assert(strcmp("val1", envmapValueForKey(em,"key1")) == 0);
  assert(strcmp("val2", envmapValueForKey(em,"key2")) == 0);
  envmapSetKeyVal(em,"key2","newval2");
  envmapSetKeyVal(em,"key3","newval3");
  p = envmapKeyValAt(em,1);
  assert(strcmp(p.sp.key,"key2") == 0);
  assert(strcmp(p.sp.val,"newval2") == 0);
  p = envmapKeyValAt(em,2);
  assert(strcmp(p.sp.key,"key3") == 0);
  assert(strcmp(p.sp.val,"newval3") == 0);
  envmapFree(em);
}

void testSS()
{
  struct StringStack *ss = stringstackNew(), *nss;
  struct DataBlock *db;
  char *s;
  stringstackPush(ss, "ape");
  stringstackPush(ss, "bird");
  stringstackPush(ss, "cat");
  stringstackPush(ss, "dog");
  db = stringstackDump(ss);
  datablockWriteToFile(db, "baddb.dat");
  s = shiftSS(ss);
  assert(strcmp(s,"ape") == 0);
  clFreeandclear(s);
  s = stringstackPop(ss);
  assert(strcmp(s,"dog") == 0);
  clFreeandclear(s);
  assert(stringstackSize(ss) == 2);
  s = shiftSS(ss);
  clFreeandclear(s);
  assert(!stringstackIsEmpty(ss));
  s = shiftSS(ss);
  clFreeandclear(s);
  assert(stringstackIsEmpty(ss));
  stringstackFree(ss);
  nss = stringstackLoad(db, 1);
  assert(stringstackSize(nss) == 4);
  s = shiftSS(nss);
  assert(strcmp(s, "ape") == 0);
  clFreeandclear(s);
  s = stringstackPop(nss);
  assert(strcmp(s, "dog") == 0);
  clFreeandclear(s);
  stringstackFree(nss);
  datablockFreePtr(db);
}

void testCAPtr(struct CompAdaptor *ca)
{
//  void *ci;
  char *str = ""
  "00000000---------------+++++++000000000000"
  "00000000--------oo-----+++++++000000x00000"
  "00000000--------oo-----+++++++000000x00000"
  "00000000---------------+++++++000000000000"
                  /* */
                    ;

  struct DataBlock *db = stringToDataBlockPtr(str);
  double c;
  assert(ca != NULL);
  //ca->se(ca,em);
//  assert(ci != NULL);
  assert(ca->cf != NULL);
  c = compaCompress(ca,db);
  assert(c < strlen(str)*8);
  if (gconf->fVerbose)
    printf("Testing %s to get compressed size %f\n", compaShortName(ca), c);
  datablockFreePtr(db);
  compaFree(ca);
}

void testCANamed(const char *name)
{
  struct CompAdaptor *ca = compaLoadBuiltin(name);
  testCAPtr(ca);
}

void testZlibCA()
{
  testCANamed("zlib");
}

void testBZipCA()
{
  testCANamed("bzip");
}

void testBlockSortCA()
{
#define REPS 10
#define MAX_BLKSIZE 200
  int i, j, c;
  struct CompAdaptor *ca = compaLoadBuiltin("blocksort");
  struct DataBlock *db = NULL;
  double v;
  int dbsize;
  unsigned char *dbptr;
  assert(ca != NULL);
  srand( time(NULL) );
    assert(ca->cf != NULL);

  /* Blocks only 1 or 2 bytes in size */
  for (i = 0; i < REPS; i +=1) {
    dbsize = (int) ((double)rand()/((double)RAND_MAX) * 1) + 1;
    if (!dbsize) continue;
    dbptr = (unsigned char*)clMalloc(dbsize);
    c = (int) ((double)rand()/((double)RAND_MAX + 1) * 256);
    memset(dbptr, c, dbsize);
    db = datablockNewFromBlock(dbptr,dbsize);
    v = compaCompress(ca,db);
    if (gconf->fVerbose)
      printf("Testing %s to get compressed size %f\n", compaShortName(ca), v);
    clFree(dbptr);
    datablockFreePtr(db);
  }

  /* Blocks with the same character repeated */
  for (i = 0; i < REPS; i +=1) {
    dbsize = (int) ((double)rand()/((double)RAND_MAX + 1) * MAX_BLKSIZE + 1);
    dbptr = (unsigned char*)clMalloc(dbsize);
    c = (int) ((double)rand()/((double)RAND_MAX + 1) * 256);
    memset(dbptr, c, dbsize);
    db = datablockNewFromBlock(dbptr,dbsize);
    v = compaCompress(ca,db);
    if (gconf->fVerbose)
      printf("Testing %s to get compressed size %f\n", compaShortName(ca), v);
    clFree(dbptr);
    datablockFreePtr(db);
  }

  /* Blocks with randomly generated characters */
  for (i = 0; i < REPS; i +=1) {
    dbsize = (int) ((double)rand()/((double)RAND_MAX) * MAX_BLKSIZE + 1);
    dbptr = (unsigned char*)clMalloc(dbsize);
    for (j = 0; j < dbsize ; j +=1 ) {
      dbptr[j] = (int) ((double)rand()/((double)RAND_MAX + 1) * 256);
    }
    db = datablockNewFromBlock(dbptr,dbsize);
    v = compaCompress(ca,db);
    if (gconf->fVerbose)
      printf("Testing %s to get compressed size %f\n", compaShortName(ca), v);
    clFree(dbptr);
    datablockFreePtr(db);
  }
  compaFree(ca);
}

void testYamlParser()
{
  struct EnvMap *em;
//  char *str = ""
//  "compressor: zlib\n"
//  "zliblevel: 5\n"
	;
//  struct StringStack *getDefaultFileList(void);
  em = envmapNew();
  readDefaultConfig(em);

	assert(strcmp(envmapValueForKey(em,"compressor"),"zlib") == 0);
	assert(strcmp(envmapValueForKey(em,"zliblevel"),"5") == 0);
  envmapFree(em);
}

void testVirtComp()
{
  char *cmdname = "/home/cilibrar/src/shared/complearn/scripts/testvirtcomp.zsh";
  struct CompAdaptor *ca;
  ca = compaLoadVirtual(cmdname);
  testCAPtr(ca);
}

void testRealComp()
{
  char *cmdname = "/home/cilibrar/src/shared/complearn/scripts/testrealcomp.sh";
  struct CompAdaptor *ca;
  ca = compaLoadReal(cmdname);
  testCAPtr(ca);
}

void testGoogle()
{
  struct StringStack *terms, *horse, *rider, *horserider;
  struct EnvMap *em;
  char *gkey;
  char *wantedQStr="+\"apple\" +\"ball\"";
  const char *gotQStr;
  double pg;
  wantedQStr = wantedQStr; /* warning stopper */
  em = envmapNew();
  terms = stringstackNew();
  stringstackPush(terms, "ball");
  stringstackPush(terms, "apple");
  assert(stringstackReadAt(terms, 0)[0] == 'b');

  normalizeSearchTerms(terms);

  assert(stringstackReadAt(terms, 0)[0] == 'a');

  gotQStr = makeQueryString(terms);
  assert(strcmp(gotQStr, wantedQStr) == 0);

  readDefaultConfig(em);

  gkey = envmapValueForKey(em, "GoogleKey");

  if (gkey) {
//    pg = fetchSampleSimple(terms, gkey, NULL);
    pg = fetchSampleSimple(terms, gkey, NULL);
    if (gconf->fVerbose)
      printf("pg is %f\n", pg);
    assert(pg > 10 && pg < 1000000000000.0);
    horse = stringstackNewSingle("horse");
    rider = stringstackNewSingle("rider");
    horserider = stringstackMerge(horse, rider);
    if (gconf->fVerbose) {
      printf("[horse]: %f\n", fetchSampleSimple(horse, gkey, NULL));
      printf("[rider]: %f\n", fetchSampleSimple(rider, gkey, NULL));
      printf("[horse,rider]: %f\n", fetchSampleSimple(horserider, gkey, NULL));
    }
    stringstackFree(horse);
    stringstackFree(rider);
    stringstackFree(horserider);
  } else {
    if (gconf->fVerbose) {
      printf("(no GoogleKey set, skipping Google test)\n");
    }
  }
  stringstackFree(terms);
  envmapFree(em);
}

void testSOAPComp()
{
  char *url = "http://localhost:2000/";
  char *urn = "urn:hws";
  struct CompAdaptor *ca;
  ca = compaLoadSOAP(url, urn);
  testCAPtr(ca);
}

void testTransformBZ()
{
  struct TransformAdaptor *t = (struct TransformAdaptor*)builtin_UNBZIP();
	struct DataBlock *db;
	if (!fopen(testbzfile, "rb")) {
		printf("Can't find test bz2 file. Skipping transformBZ test...\n");
		return;
	}
 	db = fileToDataBlockPtr(testbzfile);
	assert(strcmp(t->sn(),"unbzip") == 0);
	if (t->pf(db)) {
	  struct DataBlock *result;
    result = t->tf(db);
		assert(datablockSize(result) > 0);
		assert(datablockData(result) != NULL);
    //clFree(result.ptr);
	}
  t->tfree(t);
  t = NULL;
  datablockFreePtr(db);
}

struct DataBlock *zlibCompressDataBlock(struct DataBlock *src)
{
	struct DataBlock *result;
  unsigned char *dbuff;
	int p, s;

	p = datablockSize(src)*1.001 + 12;
	dbuff = (unsigned char*)clMalloc(p);
	s = compress2(dbuff,(uLongf *) &p,datablockData(src),datablockSize(src),0);
	if (s == Z_BUF_ERROR) {
		printf ("destLen not big enoughC!\n");
		exit(1);
	}
	if (s != Z_OK) {
		printf ("Unknown error: zlibBuff returned %d\n",s);
		exit(1);
	}
  result = datablockNewFromBlock(dbuff,p);
	free(dbuff);
	return result;
}

void testTransformGZ()
{
  struct TransformAdaptor *t = (struct TransformAdaptor*)builtin_UNGZ();
	struct DataBlock *db;
	if (!fopen(testgzfile, "rb")) {
		printf("Can't find test gz file. Skipping transformGZ test...\n");
		return;
	}
	db = fileToDataBlockPtr(testgzfile);
	assert(strcmp(t->sn(),"ungz") == 0);
	if (t->pf(db)) {
	  struct DataBlock *result;
    result = t->tf(db);
		assert(datablockSize(result) > 0);
		assert(datablockData(result) != NULL);
    //clFree(result.ptr);
	}
  t->tfree(t);
  t = NULL;
  datablockFreePtr(db);
}

void testTransformZLIB()
{
  struct TransformAdaptor *t = (struct TransformAdaptor*)builtin_UNZLIB();
	struct DataBlock *db;
	if (!fopen(testzlibfile, "rb")) {
		printf("Can't find test zlib file. Skipping transformZLIB test...\n");
		return;
	}
 	db = fileToDataBlockPtr(testzlibfile);
	assert(strcmp(t->sn(),"unzlib") == 0);
	if (t->pf(db)) {
	  struct DataBlock *result;
    result = t->tf(db);
		assert(datablockSize(result) > 0);
		assert(datablockData(result) != NULL);
    //clFree(result.ptr);
	}
  t->tfree(t);
  t = NULL;
  datablockFreePtr(db);
}

void testSingletonDBE()
{
  char *teststr = "foo";
  struct DataBlock *db, *cur;
  struct DataBlockEnumeration *dbe;
  struct DataBlockEnumerationIterator *dbi;
  db = stringToDataBlockPtr(teststr);
  dbe = dbeLoadSingleton(db);
  assert(dbe);
  dbi = dbe->newenumiter(dbe);
  assert(dbi);
  cur = dbe->istar(dbe, dbi);
  assert(cur && datablockSize(cur) == 3 && datablockData(cur)[0] == 'f' && datablockData(cur)[2] == 'o');
  datablockFreePtr(cur);
  dbe->istep(dbe, dbi);
  cur = dbe->istar(dbe, dbi);
  assert(cur == NULL);
  dbe->ifree(dbi);
  dbe->efree(dbe);
  datablockFreePtr(db);
}

void testWindowedDBE()
{
  /*               ....|....|....|   */
  char *teststr = "abcdefghijklmno";
  int firstpos = 1; /* at the b */
  int stepsize = 1;
  int width = 12;
  int lastpos;
  struct DataBlock *db, *cur;
  struct DataBlockEnumeration *dbe;
  struct DataBlockEnumerationIterator *dbi;
  db = stringToDataBlockPtr(teststr);
  lastpos = datablockSize(db) - 1;
  dbe = dbeLoadWindowed(db, firstpos, stepsize, width, lastpos);
  assert(dbe);
  dbi = dbe->newenumiter(dbe);
  assert(dbi);
  cur = dbe->istar(dbe, dbi);
  assert(cur && datablockSize(cur) == width && datablockData(cur)[0] == 'b');
  datablockFreePtr(cur);
  dbe->istep(dbe, dbi);
  cur = dbe->istar(dbe, dbi);
  assert(cur && datablockSize(cur) == width && datablockData(cur)[0] == 'c');
  datablockFreePtr(cur);
  dbe->istep(dbe, dbi);
  cur = dbe->istar(dbe, dbi);
  assert(cur && datablockSize(cur) == width && datablockData(cur)[0] == 'd');
  datablockFreePtr(cur);
  dbe->istep(dbe, dbi);
  cur = dbe->istar(dbe, dbi);
  assert(cur == NULL);
  dbe->ifree(dbi);
  dbe->efree(dbe);
  datablockFreePtr(db);
}

void testDirectoryDBE()
{
  struct DataBlockEnumeration *dbe;
  struct DataBlockEnumerationIterator *dbi;
  struct DataBlock *cur;
  int fcount = 0;
  dbe = dbeLoadDirectory(testpg4dir);
  assert(dbe);
  dbi = dbe->newenumiter(dbe);
  assert(dbi);
  while ( ( cur = dbe->istar(dbe, dbi) ) ) {
    fcount += 1;
//   datablockPrint(*cur);
//    printf("\n");
    dbe->istep(dbe, dbi);
    datablockFreePtr(cur);
  }
  assert(fcount >= 2); /* Should have at least two files in pg4 */
  dbe->ifree(dbi);
  dbe->efree(dbe);
}

void testArrayDBE()
{
  struct DataBlock *db[3];
  int size = sizeof(db) / sizeof(db[0]);
  struct DataBlockEnumeration *dbe;
  struct DataBlockEnumerationIterator *dbi;
  struct DataBlock *cur;
  int i;
  db[0] = stringToDataBlockPtr("a");
  assert(datablockSize(db[0]) == 1);
  db[1] = stringToDataBlockPtr("b");
  db[2] = stringToDataBlockPtr("c");
  dbe = dbeLoadArray(db, size);
  assert(dbe);
  dbi = dbe->newenumiter(dbe);
  assert(dbi);
  cur = dbe->istar(dbe, dbi);
  assert(cur && datablockSize(cur) == 1 && datablockData(cur)[0] == 'a');
  datablockFreePtr(cur);
  dbe->istep(dbe, dbi);
  cur = dbe->istar(dbe, dbi);
  assert(cur && datablockSize(cur) == 1 && datablockData(cur)[0] == 'b');
  datablockFreePtr(cur);
  dbe->istep(dbe, dbi);
  cur = dbe->istar(dbe, dbi);
  assert(cur && datablockSize(cur) == 1 && datablockData(cur)[0] == 'c');
  datablockFreePtr(cur);
  dbe->istep(dbe, dbi);
  cur = dbe->istar(dbe, dbi);
  assert(cur == NULL);
  dbe->ifree(dbi);
  dbe->efree(dbe);
  for (i = 0; i < 3; i += 1)
    datablockFreePtr(db[i]);
}
/*
void testFileListDBE(void)
{
  struct DataBlock db[3];
  int size = sizeof(db) / sizeof(db[0]);
  struct DataBlockEnumeration *dbe;
  struct DataBlockEnumerationIterator *dbi;
  struct DataBlock *cur;
  dbe = dbeLoadFileList("/home/cilibrar/src/shared/complearn/src/lame.txt");
  assert(dbe);
  dbi = dbe->newenumiter(dbe);
  assert(dbi);
  cur = dbe->istar(dbe, dbi);
  assert(cur && cur->size == 2 && cur->ptr[0] == 'a');
  dbe->istep(dbe, dbi);
  cur = dbe->istar(dbe, dbi);
  assert(cur && cur->size == 2 && cur->ptr[0] == 'b');
  dbe->istep(dbe, dbi);
  cur = dbe->istar(dbe, dbi);
  assert(cur && cur->size == 2 && cur->ptr[0] == 'c');
  dbe->istep(dbe, dbi);
  cur = dbe->istar(dbe, dbi);
  assert(cur == NULL);
  dbe->ifree(dbi);
  dbe->efree(dbe);
}
*/

/*
void testTAStack()
{
	int i;
  struct TransformAdaptorStack *ts;
	struct TransformAdaptor *taa;
	struct TransformAdaptor *tab;
	struct TransformAdaptor *tac;
	struct TransformAdaptor *tmp = NULL;
	struct TransformAdaptor *cur = NULL;
	struct TransformAdaptor *taarray[TEST_TS_SIZE];
  ts = newTAStack();
	assert(ts != NULL);
#ifdef BZIP2_RDY
	taa = (struct TransformAdaptor *)builtin_UNBZIP();
	assert(strcmp(taa->sn(),"unbzip") == 0);
  pushTS(ts, taa);
#endif
#ifdef ZLIB_RDY
	tab = (struct TransformAdaptor *)builtin_UNGZ();
	assert(strcmp(tab->sn(),"ungz") == 0);
  pushTS(ts, tab);
	tac = (struct TransformAdaptor *)builtin_UNZLIB();
	assert(strcmp(tac->sn(),"unzlib") == 0);
  pushTS(ts, tac);
#endif

#ifdef BZIP2_RDY
	tmp = (struct TransformAdaptor *)shiftTS(ts);
	assert(strcmp(tmp->sn(),"unbzip") == 0);
#endif
#ifdef ZLIB_RDY
	cur = (struct TransformAdaptor *)searchTS(ts,"unzlib",sequentialSearchTS);
	assert(cur);
	assert(strcmp(cur->sn(),"unzlib") == 0);
	tmp = (struct TransformAdaptor *)popTS(ts);
	assert(strcmp(tmp->sn(),"unzlib") == 0);
	tmp = (struct TransformAdaptor *)shiftTS(ts);
	assert(strcmp(tmp->sn(),"ungz") == 0);
#endif
	freeTS(ts);

  ts = newTAStack();
	for (i = 0; i < TEST_TS_SIZE ; i++) {
		taarray[i] = (struct TransformAdaptor *)builtin_UNBZIP();
		assert(strcmp(taa->sn(),"unbzip") == 0);
		pushTS(ts, taarray[i]);
	}
}
*/

void testNCDPair()
{
	char *stra = "aaaaaaaaaabbbbbbbbbbbbbbaaaaaaaaaa";
	char *strb = "bbbbbbbbbbbbbbaaaaaaaaaaaaaaaaaaaa";
	struct DataBlock *dba, *dbb;
	dba = stringToDataBlockPtr(stra);
	dbb = stringToDataBlockPtr(strb);
  datablockFreePtr(dba);
  datablockFreePtr(dbb);
}

void testDateTime(void)
{
  struct CLDateTime *td;
  char *daystr;
  td = cldatetimeNow();
  assert(td);
  daystr = cldatetimeToDayString(td);
/*  assert(daystr);
  assert(strlen(daystr) > 2);
  */
  cldatetimeFree(td);
}

void testMarshalling(void)
{
  gsl_matrix *gm, *ngm;
  char *strtest = "the test string";
  struct DataBlock *m;
  char *res = NULL;
  struct EnvMap *em = envmapNew();
  struct EnvMap *resem;
  m = stringDump(strtest);
  res = stringLoad(m, 1);
  assert(strcmp(res, strtest) == 0);
  assert(res != strtest);
  datablockFreePtr(m);
  clFreeandclear(res);
  gm = gsl_matrix_alloc(2,1);
  gsl_matrix_set(gm, 0, 0, 4.0);
  gsl_matrix_set(gm, 1, 0, 0.5);
  m = gslmatrixDump(gm);
  ngm = gslmatrixLoad(m, 1);
  assert(gm != ngm);
  assert(gm->size1 == ngm->size1);
  assert(gm->size2 == ngm->size2);
  assert(gsl_matrix_get(ngm, 0, 0) == 4.0);
  assert(gsl_matrix_get(ngm, 1, 0) == 0.5);
  gsl_matrix_free(gm);
  gsl_matrix_free(ngm);
  datablockFreePtr(m);
  envmapSetKeyVal(em, "key1", "val1");
  envmapSetKeyVal(em, "key2", "val2");
  envmapSetKeyVal(em, "key3", "val3");
  envmapSetKeyVal(em, "key4", "val4");
  m = envmapDump(em);
  resem = envmapLoad(m,1);
  assert( em != resem);
  assert(strcmp(envmapValueForKey(em,"key1"), envmapValueForKey(resem,"key1")) == 0);
  assert(strcmp(envmapValueForKey(em,"key2"), envmapValueForKey(resem,"key2")) == 0);
  assert(strcmp(envmapValueForKey(em,"key3"), envmapValueForKey(resem,"key3")) == 0);
  assert(strcmp(envmapValueForKey(em,"key4"), envmapValueForKey(resem,"key4")) == 0);
  envmapFree(em); envmapFree(resem);
}

void testDoubleDoubler(void)
{
  struct DoubleA *dd, *ee, *sm;
  struct DataBlock *dumptest;
  union PCTypes p = zeropct;
  dd = doubleaNew();
  assert(dd);
  assert(doubleaSize(dd) == 0);
  doubleaSetDValueAt(dd, 0, 2.0);
  assert(doubleaSize(dd) == 1);
  doubleaSetDValueAt(dd, 999, 123.0);
  assert(doubleaSize(dd) == 1000);
  assert(doubleaGetDValueAt(dd, 0) == 2.0);
  assert(doubleaGetDValueAt(dd, 999) == 123.0);
  dumptest = doubleaDump(dd);
  ee = doubleaLoad(dumptest, 1);
  assert(dd != ee);
  assert(doubleaSize(ee) == doubleaSize(dd));
  assert(doubleaGetDValueAt(dd, 0) == doubleaGetDValueAt(ee, 0));
  assert(doubleaGetDValueAt(dd, 999) == doubleaGetDValueAt(ee, 999));
  doubleaFree(dd);
  doubleaFree(ee);
  datablockFreePtr(dumptest);
  sm = doubleaNew();
  doubleaSetDValueAt(sm, 0, 7.0);
  doubleaSetDValueAt(sm, 1, 3.0);
  doubleaSetDValueAt(sm, 2, 13.0);
  dd = doubleaNew();
  p.ar = sm;
  doubleaPush(dd, p);
  dumptest = doubleaDeepDump(dd, 1);
  ee = doubleaLoad(dumptest, 1);
  assert(dd != ee);
  assert(doubleaGetValueAt(dd, 0).ar != doubleaGetValueAt(ee, 0).ar);
  assert(doubleaGetValueAt(doubleaGetValueAt(dd, 0).ar, 0).d == doubleaGetValueAt(sm, 0).d);
  assert(doubleaGetValueAt(doubleaGetValueAt(dd, 0).ar, 1).d == doubleaGetValueAt(sm, 1).d);
  assert(doubleaGetValueAt(doubleaGetValueAt(dd, 0).ar, 2).d == doubleaGetValueAt(sm, 2).d);
  assert(doubleaGetValueAt(doubleaGetValueAt(ee, 0).ar, 0).d == doubleaGetValueAt(sm, 0).d);
  assert(doubleaGetValueAt(doubleaGetValueAt(ee, 0).ar, 1).d == doubleaGetValueAt(sm, 1).d);
  assert(doubleaGetValueAt(doubleaGetValueAt(ee, 0).ar, 2).d == doubleaGetValueAt(sm, 2).d);
  doubleaDeepFree(ee, 1);
  doubleaDeepFree(dd, 1);
  datablockFreePtr(dumptest);
}

void testQuartet(void)
{
#define LABELCOUNT 15
#define TREETRIALCOUNT 5
  struct DataBlock *db[LABELCOUNT];
  int i, j;
  struct CompAdaptor *bz = compaLoadBuiltin("bzip");
  double score;
  struct TreeScore *ts;
  struct DataBlockEnumeration *dbe;
  for (j = 0; j < TREETRIALCOUNT; j += 1) {
    int labelcount = rand() % 4 + 4;
//    printf("doing trial %d, with %d leaves...\n", j, labelcount);
    struct TreeAdaptor *ta = treeaLoadUnrooted(labelcount);
    struct DoubleA *n = treeaNodes(ta);
    gsl_matrix *dm;
    assert(bz);
    gconf->ca = bz;
    for (i = 0; i < labelcount; ++i) {
      char buf[1024], buf2[2048];
      sprintf(buf, "%d%d%d%d%d%d%d%d", i,i,i,i,i,i,i,i);
      sprintf(buf2, "%s%d%s%s%d%s%d",buf,buf[3],buf,buf+3,i+8,buf,i % 3);
      db[i] = stringToDataBlockPtr(buf2);
    }
    dbe = dbeLoadArray(db, labelcount);
    dm = getNCDMatrix(dbe, dbe, gconf);
//    printf("Got NCD matrix... %dx%d\n", dm->size1, dm->size2);
    assert(n);
    assert(doubleaSize(n) == 2*dm->size1 - 2);
    assert(doubleaSize(n) == 2*dm->size2 - 2);
    ts = initTreeScore(ta);
    score = scoreTree(ts, dm);
//    printf("Got score: %f\n", score);
    assert(score >= 0.0 && score <= 1.0);
  {
//    struct TreeBlaster *tb;
    struct TreeHolder *th;
    struct TreeAdaptor *tra = treeaNew(0,dm->size1);
    th = treehNew(dm, tra);
    for (i = 0; i < 100; i += 1) {
      score = treehScore(th);
      if (score > 1.0 || score < 0.0) {
        printf("Error, got score %f\n", score);
      }
      assert(score >= 0.0 && score <= 1.0);
//      printf("Got TH score: %f\n", score);
      treehImprove(th);
    }
    th = NULL;
  }
    doubleaFree(n);
    freeTreeScore(ts);
//    unrootedbinaryFree(ct);
    treeaFree(ta);
    dbe->efree(dbe);
    for (i = 0; i < labelcount; i += 1)
      datablockFreePtr(db[i]);
  }
  compaFree(bz);
  gconf->ca = NULL;
  bz = NULL;
}

void testCLTree(void)
{
#define RETRIES 1
#define TREELEAFSIZE 4
#define TREENODEWANTED (2*TREELEAFSIZE-2)
#define MAXPATHTESTS 128
#define MAXPATHLEN 16
  struct UnrootedBinary *ct = unrootedbinaryNew(TREELEAFSIZE);
  struct DoubleA *n = unrootedbinaryNodes(ct, NULL);
  struct DoubleA *spm, *spmmap, *pp;
  union PCTypes a, b;
  int retval;
  int psize;
  int cur;
  int i;
	int plen = MAXPATHLEN;
	int pbuf[MAXPATHLEN];
//  struct DataBlock *dotdb;
  assert(doubleaSize(n) == TREENODEWANTED);
  pp = unrootedbinaryPerimPairs(ct, NULL);
  assert(doubleaSize(pp) == TREELEAFSIZE);
  doubleaFree(pp);
  spmmap = makeSPMMap(getAdjAdaptorForUB(ct));
  for (i = 0; i < RETRIES; ++i) {
    a = doubleaRandom(n);
    assert(a.i >= 0 && a.i < 100);
    b = doubleaRandom(n);

    assert(b.i >= 0 && b.i < 100);
    spm = makeSPMFor(getAdjAdaptorForUB(ct), b.i);
    cur = a.i;
    psize = 1;
    while (cur != b.i) {
      assert(cur >= 0);
      assert(cur < TREENODEWANTED * 2 - 2);
      psize += 1;
//      printf("%d ", cur);
      cur = doubleaGetValueAt(spm, cur).i;
    }
//    printf("\n");
//
    retval = pathFinder(getAdjAdaptorForUB(ct), a.i, b.i, pbuf, &plen);
    assert(retval == CL_OK);
    if (plen != psize) {
      clogError( "Error, plen %d and psize %d\n", plen, psize);
      clogError( "nodes %d and %d\n", a.i, b.i);
      if (plen == 2)
        clogError( "plen: [%d, %d]\n", pbuf[0], pbuf[1]);
    }
    assert(plen == psize);
    doubleaFree(spm);
    spm = NULL;
  }
  freeSPMMap(spmmap);
  spmmap = NULL;
  doubleaFree(n);
  n = NULL;
//  dotdb = convertTreeToDot(ct, NULL, unrootedbinaryLabelPerm(ct));
//  datablockWriteToFile(dotdb, "treefile.dot");
//  assert(dotdb);
//  assert(dotdb->ptr);
//  assert(dotdb->size > TREENODEWANTED * 2);
  unrootedbinaryFree(ct);
  ct = NULL;
}

#define ADJATRIALS 1000
void testAdjAdaptor(void)
{
  struct AdjAdaptor *a1, *a2;
  gsl_matrix *m;
  int labelsize;
  int i, n1, n2;
  labelsize = rand() % 10 + 4;
  a1 = adjaLoadAdjMatrix(labelsize);
  a2 = newPathKeeper(adjaLoadAdjList(labelsize));
  for (i = 0; i < ADJATRIALS; i += 1) {
    n1 = rand() % labelsize;
    do {
    n2 = rand() % labelsize;
    } while (n1 == n2);
    if (rand() % 2 == 0) {
      assert(adjaGetConState(a1, n1, n2) == adjaGetConState(a2,n1,n2));
    }
    else {
      int newval = rand() % 2;
      adjaSetConState(a1, n1, n2, newval);
      adjaSetConState(a2, n2, n1, newval);
    }
  }
  m = adjaToGSLMatrix(a1);
  adjaFree(a1);
  adjaFree(a2);
  gsl_matrix_free(m);
}

void testAdjList(void)
{
#define ALSIZE 10
#define ALTRIALS 1000
  struct AdjList *al = adjlistNew(ALSIZE);
  int c, m, i, j;
  for (i = 0; i < ALTRIALS; ++i) {
    m = rand() % ALSIZE;
    j = rand() % ALSIZE;
    if (rand() % 3 == 1) {
      c = adjlistGetConState(al, m, j);
      c ^= 1;
      adjlistSetConState(al, m, j, c);
    }
    else {
      c = adjlistGetConState(al, m, j);
      assert(c == adjlistGetConState(al, j, m));
    }
  }
  adjlistFree(al);
}
void testAdjMatrix(void)
{
#define AMSIZE 10
#define AMTRIALS 1000
  struct AdjMatrix *am = adjmatrixNew(AMSIZE);
  int c, m, i, j;
  for (i = 0; i < AMTRIALS; ++i) {
    m = rand() % AMSIZE;
    j = rand() % AMSIZE;
    if (rand() % 3 == 1) {
      c = adjmatrixGetConState(am, m, j);
      c ^= 1;
      adjmatrixSetConState(am, m, j, c);
    }
    else {
      c = adjmatrixGetConState(am, m, j);
      assert(c == adjmatrixGetConState(am, j, m));
    }
  }
  adjmatrixFree(am);
}
#if 0
  c = 0;
  ALLNODES(am, i) {
    c += 1;
  }
  assert(c == AMSIZE);
  c = 0;
  ALLPAIRS(am, i, j) {
    c += 1;
  }
  assert(c == (AMSIZE * (AMSIZE - 1)) / 2);
  c = 0;
  ALLTRIPLETS(am, i, j, k) {
    c += 1;
  }
  assert(c == (AMSIZE * (AMSIZE - 1) * (AMSIZE - 2)) / 6);
  c = 0;
  ALLQUARTETS(am, i, j, k, m) {
    c += 1;
  }
  assert(c == (AMSIZE * (AMSIZE - 1) * (AMSIZE - 2) * (AMSIZE-3)) / 24);
}
#endif

void doSBS3Test(void);

void testSpringBall(void)
{
  doSBS3Test();
}

void testALTagFile(void)
{
  char *s = "hello world";
  gsl_matrix *gm, *ngm;
  char *result;
  struct StringStack *ss, *nes, *nlabels;
  struct DataBlock *dbstr, *dbss, *dbgslm, *dbdm, *dblabels, *dbpkg, *dbpkg_read;
  struct DoubleA *dd;
  int i;
  t_tagtype curtnum;

  dbstr = stringDump(s);
  result = stringLoad(dbstr, 1);
  assert(strcmp(s,result) == 0);

  ss = stringstackNew();
  stringstackPush(ss, "Liesl");
  stringstackPush(ss, "Frederick");
  stringstackPush(ss, "Louisa");

  dbss = stringstackDump(ss);
  nes = stringstackLoad(dbss, 1);
  assert(strcmp(stringstackReadAt(nes,0), stringstackReadAt(ss,0)) == 0);
  assert(strcmp(stringstackReadAt(nes,1), stringstackReadAt(ss,1)) == 0);
  assert(strcmp(stringstackReadAt(nes,2), stringstackReadAt(ss,2)) == 0);

  dblabels = labelsDump(ss);
  nlabels = labelsLoad(dblabels,1);
  assert(strcmp(stringstackReadAt(nlabels,0), stringstackReadAt(ss,0)) == 0);
  assert(strcmp(stringstackReadAt(nlabels,1), stringstackReadAt(ss,1)) == 0);
  assert(strcmp(stringstackReadAt(nlabels,2), stringstackReadAt(ss,2)) == 0);

  stringstackFree(nes);
  clFreeandclear(result);

#define TAGFILENAME "tagtest.clb"
  gm = gsl_matrix_alloc(2,1);
  gsl_matrix_set(gm, 0, 0, 4.0);
  gsl_matrix_set(gm, 1, 0, 0.5);
  dbgslm = gslmatrixDump(gm);
  dbdm = distmatrixDump(gm);
  ngm = gslmatrixLoad(dbgslm, 1);
  assert(gm != ngm);
  assert(gm->size1 == ngm->size1);
  assert(gm->size2 == ngm->size2);
  assert(gsl_matrix_get(ngm, 0, 0) == 4.0);
  assert(gsl_matrix_get(ngm, 1, 0) == 0.5);
  gsl_matrix_free(ngm);
  ngm = distmatrixLoad(dbdm,1);
  assert(gm != ngm);
  assert(gm->size1 == ngm->size1);
  assert(gm->size2 == ngm->size2);
  assert(gsl_matrix_get(ngm, 0, 0) == 4.0);
  assert(gsl_matrix_get(ngm, 1, 0) == 0.5);
  gsl_matrix_free(ngm);

  dbpkg = package_DataBlocks(TAGNUM_TAGMASTER, dbstr, dbgslm, dbss, NULL);
  datablockFreePtr(dbss);
  datablockFreePtr(dbstr);
  datablockFreePtr(dbgslm);

  datablockWriteToFile(dbpkg,TAGFILENAME);
  datablockFreePtr(dbpkg);
  dbpkg_read = fileToDataBlockPtr(TAGFILENAME);
  unlink(TAGFILENAME);
  dd = load_DataBlock_package(dbpkg_read);
  for (i = 0; i < doubleaSize(dd); i += 1) {
    struct DataBlock *dblame;
    curtnum = doubleaGetValueAt(dd,i).idbp.tnum;
    switch (curtnum) {
      case TAGNUM_STRING:
        dblame = doubleaGetValueAt(dd,i).idbp.db;
        result = stringLoad(dblame, 1);
        assert(strcmp(s,result) == 0);
        clFreeandclear(result);
        break;
      case TAGNUM_GSLMATRIX:
        dblame = doubleaGetValueAt(dd,i).idbp.db;
        ngm = gslmatrixLoad(dblame, 1);
        assert(gm != ngm);
        assert(gm->size1 == ngm->size1);
        assert(gm->size2 == ngm->size2);
        assert(gsl_matrix_get(ngm, 0, 0) == 4.0);
        assert(gsl_matrix_get(ngm, 1, 0) == 0.5);
        gsl_matrix_free(ngm);
        break;
      case TAGNUM_STRINGSTACK:
        dblame = doubleaGetValueAt(dd,i).idbp.db;
        nes = stringstackLoad(dblame, 1);
        assert(strcmp(stringstackReadAt(nes,0), stringstackReadAt(ss,0)) == 0);
        assert(strcmp(stringstackReadAt(nes,1), stringstackReadAt(ss,1)) == 0);
        assert(strcmp(stringstackReadAt(nes,2), stringstackReadAt(ss,2)) == 0);
        stringstackFree(nes);
        break;
      default:
        break;
    }
  }
  doubleaFree(dd);
  stringstackFree(ss);
  gsl_matrix_free(gm);
  datablockFreePtr(dbpkg_read);
}

#define ARRAYSIZE 10

void testLabelPerm(void)
{
  struct LabelPerm *lpa, *lpb, *lpc;
  struct DoubleA *nodes = doubleaNew();
  int i;
  for (i = 0; i < 10; i += 1) {
    union PCTypes p = zeropct;
    p.i = i + 23;
    doubleaSetValueAt(nodes, i, p);
  }
  lpa = labelpermNew(nodes);
  lpb = labelpermClone(lpa);
  lpc = labelpermClone(lpb);
  assert(labelpermIdentical(lpa, lpb));
  assert(labelpermIdentical(lpa, lpc));
  labelpermMutate(lpb);
  assert(!labelpermIdentical(lpa, lpb));
  assert(labelpermIdentical(lpa, lpc));
  assert(!labelpermIdentical(lpc, lpb));
  labelpermFree(lpa);
  labelpermFree(lpb);
  labelpermFree(lpc);
  doubleaFree(nodes);
}

void testPerimPairs()
{
  struct TreeAdaptor *tra = treeaLoadRootedBinary(6);
  struct DoubleA *da;
  struct CLNodeSet *clns = clnodesetNew(9);
  clnodesetAddNode(clns, 2);
  da = treeaPerimPairs(tra, NULL);
  doubleaFree(da);
  da = treeaPerimPairs(tra, clns);
  doubleaFree(da);
}

void testTreeMolder()
{
  struct DataBlock *db[LABELCOUNT];
  int i, j;
  struct CompAdaptor *bz = compaLoadBuiltin("bzip");
  double score;
  struct TreeScore *ts;
  struct DataBlockEnumeration *dbe;
  for (j = 0; j < TREETRIALCOUNT; j += 1) {
    int labelcount = rand() % 4 + 4;
//    printf("doing trial %d, with %d leaves...\n", j, labelcount);
    struct TreeAdaptor *ta = treeaLoadRootedBinary(labelcount);
    struct DoubleA *n = treeaNodes(ta);
    gsl_matrix *dm;
    assert(bz);
    gconf->ca = bz;
    for (i = 0; i < labelcount; ++i) {
      char buf[1024], buf2[2048];
      sprintf(buf, "%d%d%d%d%d%d%d%d", i,i,i,i,i,i,i,i);
      sprintf(buf2, "%s%d%s%s%d%s%d",buf,buf[3],buf,buf+3,i+8,buf,i % 3);
      db[i] = stringToDataBlockPtr(buf2);
    }
    dbe = dbeLoadArray(db, labelcount);
    dm = getNCDMatrix(dbe, dbe, gconf);
    ts = initTreeScore(ta);
    {
      struct TreeMolder *tmolder;
      struct TreeAdaptor *tram = treeaNew(1,dm->size1);
      tmolder = treemolderNew(dm, tram);
      for (i = 0; i < 100; i += 1) {
        score = treemolderScoreScaled(tmolder);
        if (score > 1.0 || score < 0.0) {
          printf("Error, got score %f\n", score);
        }
        //assert(score >= 0.0 && score <= 1.0);
        treemolderImprove(tmolder);
      }
      treemolderFree(tmolder);
      tmolder = NULL;
    }
    doubleaFree(n);
    freeTreeScore(ts);
//    unrootedbinaryFree(ct);
    treeaFree(ta);
    dbe->efree(dbe);
    for (i = 0; i < labelcount; i += 1)
      datablockFreePtr(db[i]);
  }
  compaFree(bz);
  gconf->ca = NULL;
  bz = NULL;
}

void testReadTextDM()
{
  gsl_matrix *dm;
  struct StringStack *labels;
  dm = cltxtDistMatrix("distmatrix.txt");
  labels = cltxtLabels("distmatrix.txt");
  assert(dm);
  assert(labels);
  gsl_matrix_free(dm);
  stringstackFree(labels);
}

void printGSLMatrix(gsl_matrix *m){
  int i, j;
  printf("GSLMatrix:\n");
  for (i = 0; i < m->size1 ; i += 1) {
    for (j = 0; j < m->size2 ; j += 1)
      printf("%.1f ", (double) gsl_matrix_get(m,i,j));
    printf("\n");
  }
}
void testSmoothing()
{
  struct TreeAdaptor *ta = treeaNew(0, 6);
  int i;
  gsl_matrix *m;
  m = adjaToGSLMatrix(treeaAdjAdaptor(ta));
//  printGSLMatrix(m);

  for (i = 0; i < 15 ; i += 1) {
    treeaMutate(ta);
    clStepTowardsTree(m, ta, 1.0);
//    printGSLMatrix(m);
  }
}
#if 0
void testParamList()
{
  struct ParamList *pl = paramlistNew();
  struct ParamList *plclone;
  struct EnvMap *em = loadDefaultEnvironment()->em;
  int bs;
  double wf;
  char *vb;
  paramlistPushField(pl, "blocksize", "9", PARAMINT);
  paramlistPushField(pl, "workfactor", "30.0", PARAMDOUBLE);
  paramlistPushField(pl, "shortname", "compatest", PARAMSTRING);
  assert(paramlistGetInt(pl,"blocksize") == 9);
  envmapSetKeyVal(em, "blocksize", "4");
  envmapSetKeyVal(em, "workfactor", "30.0");
  envmapSetKeyVal(em, "shortname", "compatest");
  paramlistSetValueForKey(pl, "blocksize", &bs);
  paramlistSetValueForKey(pl, "workfactor", &wf);
  paramlistSetValueForKey(pl, "shortname", &vb);
  assert(paramlistGetInt(pl,"blocksize") == 4);
  assert(paramlistGetDouble(pl,"workfactor") == 30.0);
  assert(strcmp(paramlistGetString(pl,"shortname"),"compatest") == 0);
  assert(strcmp(vb,"compatest") == 0);
  plclone = paramlistClone(pl);
  assert(paramlistGetInt(plclone,"blocksize") == 4);
  assert(paramlistGetDouble(plclone,"workfactor") == 30.0);
  assert(strcmp(paramlistGetString(plclone,"shortname"),"compatest") == 0);
  paramlistFree(pl);
  paramlistFree(plclone);
}
#endif

char *findDir(const char *dir)
{
  const char *locations[] = { ".", "..", "../.."};
  int i;
  struct stat sbuf;
  char *pbuf = clMalloc(1024);

  for(i = 0; locations[i]; i += 1) {
    sprintf(pbuf,"%s/%s",locations[i],dir);
     if (stat(pbuf, &sbuf) == 0)
       return pbuf;
  }
  printf("Error finding test directory; runtests Aborted\n");
  exit(1);
}
void testCLTextConverter(void)
{
  cltxtToCLB("distmatrix.txt","convertedDM.clb");
}

int main(int argc, char **argv)
{
  char *testpath;
  char *srcpath;
  testfile = clMalloc(1024);
  if (argc > 1)
    testpath = argv[1];
  else
    testpath = findDir("test");
  srcpath = findDir("src");
  sprintf(testfile, "%s/runtests.c", srcpath);
  testbzfile = (char *)clMalloc(strlen(testpath)+20);
  testgzfile = (char *)clMalloc(strlen(testpath)+20);
  testzlibfile = (char *)clMalloc(strlen(testpath)+20);
  testpg4dir = (char *)clMalloc(strlen(testpath)+20);
  sprintf(testbzfile,"%s/pg4/hello.txt.bz2",testpath);
  sprintf(testgzfile,"%s/pg4/hello.txt.gz",testpath);
  sprintf(testzlibfile,"%s/pg4/hello.txt.z",testpath);
  sprintf(testpg4dir,"%s/pg4",testpath);
  gconf = loadNCDEnvironment();
//  if (gconf->fVerbose)
//    printf("Running tests...\n");

  testSS();
  testEM();
  testDataBlock();
  testBlockSortCA();

#if BZIP2_RDY
  testBZipCA();
//  testTransformBZ(); //FIXME: creating memory leak seemingly caused by bzip
#endif
#if ZLIB_RDY
  testZlibCA();
//  testTransformZLIB(); //TODO: investigate memory leaks
//  testTransformGZ();   //TODO: investigate memory leaks
#endif
#if CSOAP_RDY
//  testSOAPComp();      //TODO: investigate massive memory leaks here
  testGoogle();        //TODO: more memory leaks left here
#endif
//  testYamlParser();    //FIXME: convert to new parser or fix Syck
  testRealComp();
  testVirtComp();
//  testDL();   //TODO: investigate mem leaks which may be caused by dlopen
//  testDL2();  //TODO: investigate mem leaks which may be caused by dlopen
  testArrayDBE();
  testDirectoryDBE();
//  testParamList();
  testMarshalling();
  testSpringBall();
  testCLTree();
  testQuartet();
  testALTagFile();
  testSmoothing();
//  testReadTextDM();
  testSingletonDBE();
  testWindowedDBE();
/*  testFileListDBE(); */
/*  testTAStack(); */
  testNCDPair();
  testDateTime();
  testDoubleDoubler();
  testAdjMatrix();
  testAdjList();
  testAdjAdaptor();
  assert(gconf);
  testLabelPerm();
  testPerimPairs();
  testTreeMolder();
  //testCLTextConverter();
#if 0
#endif
  freeDefaultEnvironment(gconf);
  gconf = NULL;

  if (testpath) {
    clFreeandclear(testpath);
    clFreeandclear(testbzfile);
    clFreeandclear(testgzfile);
    clFreeandclear(testzlibfile);
  }
  if (srcpath) {
    clFreeandclear(srcpath);
  }
  return 0;
}

