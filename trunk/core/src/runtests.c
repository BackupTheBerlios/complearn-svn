#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <dlfcn.h>
#include <zlib.h>
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
    gconf->ptr = gcalloc(sizeof(struct NCDConfig),1);
    ncdcfg = (struct NCDConfig *) gconf->ptr;
    *ncdcfg = defaultNCDConfig;
  }
  return gconf;
}

void testDataBlock()
{
  struct DataBlock dbstr, dbstr2, dbfile, dbcat;
  char *str = "hello, world\n";
	char *str2 = "welcome to the jungle\n";
	char *result;
  dbstr = convertStringToDataBlock(str);
  assert(strlen(str) == dbstr.size);
  assert(dbstr.ptr != NULL);
  assert(dbstr.ptr != (unsigned char *) str);
	result = convertDataBlockToString(dbstr);
	assert(strcmp(result,str) == 0);
  dbstr2 = convertStringToDataBlock(str2);
  assert(dbstr2.ptr != NULL);
  assert(dbstr2.ptr != (unsigned char *) str2);
  dbcat = catDataBlock(dbstr,dbstr2);
  assert(dbcat.ptr != NULL);
  assert(dbcat.ptr != dbstr.ptr);
  assert(dbcat.ptr != dbstr2.ptr);
  freeDataBlock(dbcat);
  freeDataBlock(dbstr);
  freeDataBlock(dbstr2);
  dbfile = convertFileToDataBlock(testfile);
  assert(dbfile.ptr != NULL);
  assert(dbfile.ptr != (unsigned char *) str);
  freeDataBlock(dbfile);
  gfreeandclear(result);
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
  struct DataBlock dbab, dbaa, dbsmallalpha, dblargealpha;
  double cdbab, cdbaa, cdbsa, cdbla;
  dbab = convertStringToDataBlock(strab);
  dbaa = convertStringToDataBlock(straa);
  dbsmallalpha = convertStringToDataBlock(strsmallalpha);
  dblargealpha = convertStringToDataBlock(strlargealpha);
  em = getEnvMap(gconf);
  assert(em != NULL);
  setKeyValEM(em, "padding", "40");
  comp = loadDLCompAdaptor(DLNAME);
  assert(comp->cf != NULL);
  //comp->se(comp,em);
  sn = shortNameCA(comp);
  assert(strcmp(sn, "art") == 0);
  cdbab = compfuncCA(comp, dbab);
  assert(cdbab >= dbab.size*8);
  cdbaa = compfuncCA(comp, dbaa);
  assert(cdbaa <= dbaa.size*8);
  cdbsa = compfuncCA(comp, dbsmallalpha);
  cdbla = compfuncCA(comp, dblargealpha);
  assert(cdbsa < cdbla);
  freeDataBlock(dbab);
  freeDataBlock(dbaa);
  freeDataBlock(dbsmallalpha);
  freeDataBlock(dblargealpha);
}

void testDL()
{
  void *dlh;
  char *(*fn)(void);
  struct EnvMap *em;
  em = newEnvMap();
  setKeyValEM(em, "ignorethis", "noproblem");
  setKeyValEM(em, "ignorethis2", "ok");
  setKeyValEM(em, "padding", "20");
  dlh = dlopen(DLNAME, RTLD_NOW | RTLD_GLOBAL);
  if (dlh == NULL) {
    fprintf(stderr, "Error: cannot open dynamic library\n%s\nDid you build it yet?\n", DLNAME);
    fprintf(stderr, "reason given: %s\n", dlerror());
    exit(1);
  }
  assert("Error: cannot open dynamic library, did you build it yet?" && dlh);
  fn = dlsym(dlh, "newCompAdaptor");
  assert(fn);
  freeEM(em);
}

void testEM()
{
  struct EnvMap *em;
  union PCTypes p;
  em = newEnvMap();
  setKeyValEM(em,"key1","val1");
  setKeyValEM(em,"key2","val2");
  setKeyValEM(em,"key3","val3");
  setKeyValEM(em,"key4","val4");
  assert(strcmp("val1", readValForEM(em,"key1")) == 0);
  assert(strcmp("val2", readValForEM(em,"key2")) == 0);
  setKeyValEM(em,"key2","newval2");
  setKeyValEM(em,"key3","newval3");
  p = getKeyValAt(em,1);
  assert(strcmp(p.sp.key,"key2") == 0);
  assert(strcmp(p.sp.val,"newval2") == 0);
  p = getKeyValAt(em,2);
  assert(strcmp(p.sp.key,"key3") == 0);
  assert(strcmp(p.sp.val,"newval3") == 0);
  freeEM(em);
}

void testSS()
{
  struct StringStack *ss = newStringStack(), *nss;
  struct DataBlock db;
  char *s;
  pushSS(ss, "ape");
  pushSS(ss, "bird");
  pushSS(ss, "cat");
  pushSS(ss, "dog");
  db = dumpStringStack(ss);
  writeDataBlockToFile(&db, "baddb.dat");
  s = shiftSS(ss);
  assert(strcmp(s,"ape") == 0);
  gfreeandclear(s);
  s = popSS(ss);
  assert(strcmp(s,"dog") == 0);
  gfreeandclear(s);
  assert(sizeSS(ss) == 2);
  s = shiftSS(ss);
  gfreeandclear(s);
  assert(!isEmptySS(ss));
  s = shiftSS(ss);
  gfreeandclear(s);
  assert(isEmptySS(ss));
  freeSS(ss);
  nss = loadStringStack(db, 1);
  assert(sizeSS(nss) == 4);
  s = shiftSS(nss);
  assert(strcmp(s, "ape") == 0);
  gfreeandclear(s);
  s = popSS(nss);
  assert(strcmp(s, "dog") == 0);
  gfreeandclear(s);
  freeSS(nss);
  freeDataBlock(db);
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

  struct DataBlock db = convertStringToDataBlock(str);
  double c;
  assert(ca != NULL);
  //ca->se(ca,em);
//  assert(ci != NULL);
  assert(ca->cf != NULL);
  c = compfuncCA(ca,db);
  assert(c < strlen(str)*8);
  if (gconf->fVerbose)
    printf("Testing %s to get compressed size %f\n", shortNameCA(ca), c);
  freeDataBlock(db);
  freeCA(ca);
}

void testCANamed(const char *name)
{
  struct CompAdaptor *ca = loadBuiltinCA(name);
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
  struct CompAdaptor *ca = loadBuiltinCA("blocksort");
  struct DataBlock db;
  double v;
  assert(ca != NULL);
  srand( time(NULL) );
    assert(ca->cf != NULL);

  /* Blocks only 0 or 1 byte in size */
  for (i = 0; i < REPS; i +=1) {
    db.size = (int) ((double)rand()/((double)RAND_MAX + 1) * 1);
    db.ptr = (unsigned char*)gmalloc(db.size);
    c = (int) ((double)rand()/((double)RAND_MAX + 1) * 256);
    memset(db.ptr, c, db.size);
    v = compfuncCA(ca,db);
    if (gconf->fVerbose)
      printf("Testing %s to get compressed size %f\n", shortNameCA(ca), v);
    freeDataBlock(db);
  }

  /* Blocks with the same character repeated */
  for (i = 0; i < REPS; i +=1) {
    db.size = (int) ((double)rand()/((double)RAND_MAX + 1) * MAX_BLKSIZE);
    db.ptr = (unsigned char*)gmalloc(db.size);
    c = (int) ((double)rand()/((double)RAND_MAX + 1) * 256);
    memset(db.ptr, c, db.size);
    v = compfuncCA(ca,db);
    if (gconf->fVerbose)
      printf("Testing %s to get compressed size %f\n", shortNameCA(ca), v);
    freeDataBlock(db);
  }

  /* Blocks with randomly generated characters */
  for (i = 0; i < REPS; i +=1) {
    db.ptr = (unsigned char*)gmalloc(db.size);
    for (j = 0; j < db.size ; j +=1 ) {
      db.ptr[j] = (int) ((double)rand()/((double)RAND_MAX + 1) * 256);
    }
    v = compfuncCA(ca,db);
    if (gconf->fVerbose)
      printf("Testing %s to get compressed size %f\n", shortNameCA(ca), v);
    freeDataBlock(db);
  }
  freeCA(ca);
}

void testYamlParser()
{
  struct EnvMap *em;
//  char *str = ""
//  "compressor: zlib\n"
//  "zliblevel: 5\n"
	;
//  struct StringStack *getDefaultFileList(void);
  em = newEnvMap();
  readDefaultConfig(em);

	assert(strcmp(readValForEM(em,"compressor"),"zlib") == 0);
	assert(strcmp(readValForEM(em,"zliblevel"),"5") == 0);
  freeEM(em);
}

void testVirtComp()
{
  char *cmdname = "/home/cilibrar/src/shared/complearn/scripts/testvirtcomp.zsh";
  struct CompAdaptor *ca;
  ca = loadVirtComp(cmdname);
  testCAPtr(ca);
}

void testRealComp()
{
  char *cmdname = "/home/cilibrar/src/shared/complearn/scripts/testrealcomp.sh";
  struct CompAdaptor *ca;
  ca = loadRealComp(cmdname);
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
  em = newEnvMap();
  terms = newStringStack();
  pushSS(terms, "ball");
  pushSS(terms, "apple");
  assert(readAtSS(terms, 0)[0] == 'b');

  normalizeSearchTerms(terms);

  assert(readAtSS(terms, 0)[0] == 'a');

  gotQStr = makeQueryString(terms);
  assert(strcmp(gotQStr, wantedQStr) == 0);

  readDefaultConfig(em);

  gkey = readValForEM(em, "GoogleKey");

  if (gkey) {
//    pg = fetchSampleSimple(terms, gkey, NULL);
    pg = fetchSampleSimple(terms, gkey, NULL);
    if (gconf->fVerbose)
      printf("pg is %f\n", pg);
    assert(pg > 10 && pg < 1000000000000.0);
    horse = newSingleSS("horse");
    rider = newSingleSS("rider");
    horserider = mergeSS(horse, rider);
    if (gconf->fVerbose) {
      printf("[horse]: %f\n", fetchSampleSimple(horse, gkey, NULL));
      printf("[rider]: %f\n", fetchSampleSimple(rider, gkey, NULL));
      printf("[horse,rider]: %f\n", fetchSampleSimple(horserider, gkey, NULL));
    }
    freeSS(horse);
    freeSS(rider);
    freeSS(horserider);
  } else {
    if (gconf->fVerbose) {
      printf("(no GoogleKey set, skipping Google test)\n");
    }
  }
  freeSS(terms);
  freeEM(em);
}

void testSOAPComp()
{
  char *url = "http://localhost:2000/";
  char *urn = "urn:hws";
  struct CompAdaptor *ca;
  ca = loadSOAPAdaptor(url, urn);
  testCAPtr(ca);
}

void testTransformBZ()
{
  struct TransformAdaptor *t = (struct TransformAdaptor*)builtin_UNBZIP();
	struct DataBlock db;
	if (!fopen(testbzfile, "rb")) {
		printf("Can't find test bz2 file. Skipping transformBZ test...\n");
		return;
	}
 	db = convertFileToDataBlock(testbzfile);
	assert(strcmp(t->sn(),"unbzip") == 0);
	if (t->pf(db)) {
	  struct DataBlock result;
    result = t->tf(db);
		assert(result.size > 0);
		assert(result.ptr != NULL);
    freeDataBlock(result);
	}
  t->tfree(t);
  t = NULL;
  freeDataBlock(db);
}

struct DataBlock zlibCompressDataBlock(struct DataBlock src)
{
	struct DataBlock result;
  unsigned char *dbuff;
	int p, s;

	p = src.size*1.001 + 12; 
	dbuff = (unsigned char*)gmalloc(p);
	s = compress2(dbuff,(uLongf *) &p,src.ptr,src.size,0);
	if (s == Z_BUF_ERROR) {
		printf ("destLen not big enough!\n");
		exit(1);
	}
	if (s != Z_OK) {
		printf ("Unknown error: zlibBuff returned %d\n",s);
		exit(1);
	}
	result.size = p;
	result.ptr = (unsigned char *) gmalloc(result.size);
	memcpy(result.ptr,dbuff,result.size);
	free(dbuff);
	return result;
}

void testTransformGZ()
{
  struct TransformAdaptor *t = (struct TransformAdaptor*)builtin_UNGZ();
	struct DataBlock db;
	if (!fopen(testgzfile, "rb")) {
		printf("Can't find test gz file. Skipping transformGZ test...\n");
		return;
	}
	db = convertFileToDataBlock(testgzfile);
	assert(strcmp(t->sn(),"ungz") == 0);
	if (t->pf(db)) {
	  struct DataBlock result;
    result = t->tf(db);
		assert(result.size > 0);
		assert(result.ptr != NULL);
    freeDataBlock(result);
	}
  t->tfree(t);
  t = NULL;
  freeDataBlock(db);
}

void testTransformZLIB()
{
  struct TransformAdaptor *t = (struct TransformAdaptor*)builtin_UNZLIB();
	struct DataBlock db;
	if (!fopen(testzlibfile, "rb")) {
		printf("Can't find test zlib file. Skipping transformZLIB test...\n");
		return;
	}
 	db = convertFileToDataBlock(testzlibfile);
	assert(strcmp(t->sn(),"unzlib") == 0);
	if (t->pf(db)) {
	  struct DataBlock result;
    result = t->tf(db);
		assert(result.size > 0);
		assert(result.ptr != NULL);
    freeDataBlock(result);
	}
  t->tfree(t);
  t = NULL;
  freeDataBlock(db);
}

void testSingletonDBE()
{
  char *teststr = "foo";
  struct DataBlock db, *cur;
  struct DataBlockEnumeration *dbe;
  struct DataBlockEnumerationIterator *dbi;
  db = convertStringToDataBlock(teststr);
  dbe = loadSingletonDBE(&db);
  assert(dbe);
  dbi = dbe->newenumiter(dbe);
  assert(dbi);
  cur = dbe->istar(dbe, dbi);
  assert(cur && cur->size == 3 && cur->ptr[0] == 'f' && cur->ptr[2] == 'o');
  freeDataBlockPtr(cur);
  dbe->istep(dbe, dbi);
  cur = dbe->istar(dbe, dbi);
  assert(cur == NULL);
  dbe->ifree(dbi);
  dbe->efree(dbe);
  freeDataBlock(db);
}

void testWindowedDBE()
{
  /*               ....|....|....|   */
  char *teststr = "abcdefghijklmno";
  int firstpos = 1; /* at the b */
  int stepsize = 1;
  int width = 12;
  int lastpos;
  struct DataBlock db, *cur;
  struct DataBlockEnumeration *dbe;
  struct DataBlockEnumerationIterator *dbi;
  db = convertStringToDataBlock(teststr);
  lastpos = db.size - 1;
  dbe = loadWindowedDBE(&db, firstpos, stepsize, width, lastpos);
  assert(dbe);
  dbi = dbe->newenumiter(dbe);
  assert(dbi);
  cur = dbe->istar(dbe, dbi);
  assert(cur && cur->size == width && cur->ptr[0] == 'b');
  freeDataBlockPtr(cur);
  dbe->istep(dbe, dbi);
  cur = dbe->istar(dbe, dbi);
  assert(cur && cur->size == width && cur->ptr[0] == 'c');
  freeDataBlockPtr(cur);
  dbe->istep(dbe, dbi);
  cur = dbe->istar(dbe, dbi);
  assert(cur && cur->size == width && cur->ptr[0] == 'd');
  freeDataBlockPtr(cur);
  dbe->istep(dbe, dbi);
  cur = dbe->istar(dbe, dbi);
  assert(cur == NULL);
  dbe->ifree(dbi);
  dbe->efree(dbe);
  freeDataBlock(db);
}

void testDirectoryDBE()
{
  struct DataBlockEnumeration *dbe;
  struct DataBlockEnumerationIterator *dbi;
  struct DataBlock *cur;
  int fcount = 0;
  dbe = loadDirectoryDBE(testpg4dir);
  assert(dbe);
  dbi = dbe->newenumiter(dbe);
  assert(dbi);
  while ( ( cur = dbe->istar(dbe, dbi) ) ) {
    fcount += 1;
//   printDataBlock(*cur);
//    printf("\n");
    dbe->istep(dbe, dbi);
    freeDataBlockPtr(cur);
  }
  assert(fcount >= 2); /* Should have at least two files in pg4 */
  dbe->ifree(dbi);
  dbe->efree(dbe);
}

void testArrayDBE()
{
  struct DataBlock db[3];
  int size = sizeof(db) / sizeof(db[0]);
  struct DataBlockEnumeration *dbe;
  struct DataBlockEnumerationIterator *dbi;
  struct DataBlock *cur;
  int i;
  db[0] = convertStringToDataBlock("a");
  assert(db[0].size == 1);
  db[1] = convertStringToDataBlock("b");
  db[2] = convertStringToDataBlock("c");
  dbe = loadArrayDBE(db, size);
  assert(dbe);
  dbi = dbe->newenumiter(dbe);
  assert(dbi);
  cur = dbe->istar(dbe, dbi);
  assert(cur && cur->size == 1 && cur->ptr[0] == 'a');
  freeDataBlockPtr(cur);
  dbe->istep(dbe, dbi);
  cur = dbe->istar(dbe, dbi);
  assert(cur && cur->size == 1 && cur->ptr[0] == 'b');
  freeDataBlockPtr(cur);
  dbe->istep(dbe, dbi);
  cur = dbe->istar(dbe, dbi);
  assert(cur && cur->size == 1 && cur->ptr[0] == 'c');
  freeDataBlockPtr(cur);
  dbe->istep(dbe, dbi);
  cur = dbe->istar(dbe, dbi);
  assert(cur == NULL);
  dbe->ifree(dbi);
  dbe->efree(dbe);
  for (i = 0; i < 3; i += 1)
    freeDataBlock(db[i]);
}
/*
void testFileListDBE(void)
{
  struct DataBlock db[3];
  int size = sizeof(db) / sizeof(db[0]);
  struct DataBlockEnumeration *dbe;
  struct DataBlockEnumerationIterator *dbi;
  struct DataBlock *cur;
  dbe = loadFileListDBE("/home/cilibrar/src/shared/complearn/src/lame.txt");
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
	struct DataBlock dba, dbb;
	dba = convertStringToDataBlock(stra);
	dbb = convertStringToDataBlock(strb);
  freeDataBlock(dba);
  freeDataBlock(dbb);
}

void testDateTime(void)
{
  struct CLDateTime *td;
  char *daystr;
  td = cldtNow();
  assert(td);
  daystr = cldt_daystring(td);
/*  assert(daystr);
  assert(strlen(daystr) > 2);
  */
  cldtfree(td);
}

#if GSL_RDY
void testMarshalling(void)
{
  gsl_matrix *gm, *ngm;
  char *strtest = "the test string";
  struct DataBlock m;
  char *res = NULL;
  struct EnvMap *em = newEnvMap();
  struct EnvMap *resem;
  m = dumpString(strtest);
  res = loadString(m, 1);
  assert(strcmp(res, strtest) == 0);
  assert(res != strtest);
  freeDataBlock(m);
  gfreeandclear(res);
  gm = gsl_matrix_alloc(2,1);
  gsl_matrix_set(gm, 0, 0, 4.0);
  gsl_matrix_set(gm, 1, 0, 0.5);
  m = dumpGSLMatrix(gm);
  ngm = loadGSLMatrix(m, 1);
  assert(gm != ngm);
  assert(gm->size1 == ngm->size1);
  assert(gm->size2 == ngm->size2);
  assert(gsl_matrix_get(ngm, 0, 0) == 4.0);
  assert(gsl_matrix_get(ngm, 1, 0) == 0.5);
  gsl_matrix_free(gm);
  gsl_matrix_free(ngm);
  freeDataBlock(m);
  setKeyValEM(em, "key1", "val1");
  setKeyValEM(em, "key2", "val2");
  setKeyValEM(em, "key3", "val3");
  setKeyValEM(em, "key4", "val4");
  m = dumpEnvMap(em);
  resem = loadEnvMap(m,1);
  assert( em != resem);
  assert(strcmp(readValForEM(em,"key1"), readValForEM(resem,"key1")) == 0);
  assert(strcmp(readValForEM(em,"key2"), readValForEM(resem,"key2")) == 0);
  assert(strcmp(readValForEM(em,"key3"), readValForEM(resem,"key3")) == 0);
  assert(strcmp(readValForEM(em,"key4"), readValForEM(resem,"key4")) == 0);
  freeEM(em); freeEM(resem);
}
#endif

void testDoubleDoubler(void)
{
  struct DoubleA *dd, *ee, *sm;
  struct DataBlock dumptest;
  union PCTypes p = zeropct;
  dd = newDoubleDoubler();
  assert(dd);
  assert(getSize(dd) == 0);
  setDValueAt(dd, 0, 2.0);
  assert(getSize(dd) == 1);
  setDValueAt(dd, 999, 123.0);
  assert(getSize(dd) == 1000);
  assert(getDValueAt(dd, 0) == 2.0);
  assert(getDValueAt(dd, 999) == 123.0);
  dumptest = dumpDoubleDoubler(dd);
  ee = loadDoubleDoubler(dumptest, 1);
  assert(dd != ee);
  assert(getSize(ee) == getSize(dd));
  assert(getDValueAt(dd, 0) == getDValueAt(ee, 0));
  assert(getDValueAt(dd, 999) == getDValueAt(ee, 999));
  freeDoubleDoubler(dd);
  freeDoubleDoubler(ee);
  freeDataBlock(dumptest);
  sm = newDoubleDoubler();
  setDValueAt(sm, 0, 7.0);
  setDValueAt(sm, 1, 3.0);
  setDValueAt(sm, 2, 13.0);
  dd = newDoubleDoubler();
  p.ar = sm;
  pushValue(dd, p);
  dumptest = dumpDeepDoubleDoubler(dd, 1);
  ee = loadDoubleDoubler(dumptest, 1);
  assert(dd != ee);
  assert(getValueAt(dd, 0).ar != getValueAt(ee, 0).ar);
  assert(getValueAt(getValueAt(dd, 0).ar, 0).d == getValueAt(sm, 0).d);
  assert(getValueAt(getValueAt(dd, 0).ar, 1).d == getValueAt(sm, 1).d);
  assert(getValueAt(getValueAt(dd, 0).ar, 2).d == getValueAt(sm, 2).d);
  assert(getValueAt(getValueAt(ee, 0).ar, 0).d == getValueAt(sm, 0).d);
  assert(getValueAt(getValueAt(ee, 0).ar, 1).d == getValueAt(sm, 1).d);
  assert(getValueAt(getValueAt(ee, 0).ar, 2).d == getValueAt(sm, 2).d);
  freeDeepDoubleDoubler(ee, 1);
  freeDeepDoubleDoubler(dd, 1);
  freeDataBlock(dumptest);
}

#if GSL_RDY
void testQuartet(void)
{
#define LABELCOUNT 15
#define TREETRIALCOUNT 5
  struct DataBlock db[LABELCOUNT];
  int i, j;
  struct CompAdaptor *bz = loadBuiltinCA("bzip");
  double score;
  struct TreeScore *ts;
  struct DataBlockEnumeration *dbe;
  for (j = 0; j < TREETRIALCOUNT; j += 1) {
    int labelcount = rand() % 4 + 4;
//    printf("doing trial %d, with %d leaves...\n", j, labelcount);
    struct UnrootedBinary *ct = newUnrootedBinary(labelcount);
    struct TreeAdaptor *ta = loadUBTRA(ct);
    struct DoubleA *n = getTreeNodes(ct, NULL);
    gsl_matrix *dm;
    assert(bz);
    gconf->ca = bz;
    for (i = 0; i < labelcount; ++i) {
      char buf[1024], buf2[2048];
      sprintf(buf, "%d%d%d%d%d%d%d%d", i,i,i,i,i,i,i,i);
      sprintf(buf2, "%s%d%s%s%d%s%d",buf,buf[3],buf,buf+3,i+8,buf,i % 3);
      db[i] = convertStringToDataBlock(buf2);
    }
    dbe = loadArrayDBE(db, labelcount);
    dm = getNCDMatrix(dbe, dbe, gconf);
//    printf("Got NCD matrix... %dx%d\n", dm->size1, dm->size2);
    assert(n);
    assert(getSize(n) == 2*dm->size1 - 2);
    assert(getSize(n) == 2*dm->size2 - 2);
    ts = initTreeScore(ta);
    score = scoreTree(ts, dm);
//    printf("Got score: %f\n", score);
    assert(score >= 0.0 && score <= 1.0);
#if 1
  {
//    struct TreeBlaster *tb;
    struct TreeHolder *th;
    struct TreeAdaptor *tra = newTreeTRA(0,dm->size1);
    th = newTreeHolder(dm, tra);
    for (i = 0; i < 100; i += 1) {
      score = getCurScore(th);
      if (score > 1.0 || score < 0.0) {
        printf("Error, got score %f\n", score);
      }
      assert(score >= 0.0 && score <= 1.0);
//      printf("Got TH score: %f\n", score);
      tryToImprove(th);
    }
#if 0
    tb = newTreeBlaster(dm, getCurTree(th));
    printf("tb is %p\n", tb);
    struct CLNodeSet *clns;
    setTreeOrderObserver(tb, &too);
    clns = findTreeOrder(tb, &score);
    printf("Got score %f\n", score);
    printf("And flipped node set:\n ");
    printCLNodeSet(clns);
    printf("Done.\n");
    
//    printf("score is %f\n", getScoreTB(tb));
    freeTreeHolder(th);
    freeTreeBlaster(tb);
#endif
    th = NULL;
  }
#endif
    freeDoubleDoubler(n);
    freeTreeScore(ts);
//    freeUnrootedBinary(ct);
    treefreeTRA(ta);
    dbe->efree(dbe);
    for (i = 0; i < labelcount; i += 1)
      freeDataBlock(db[i]);
  }
  freeCA(bz);
  gconf->ca = NULL;
  bz = NULL;
}
#endif

void testCLTree(void)
{
#define RETRIES 1
#define TREELEAFSIZE 4 
#define TREENODEWANTED (2*TREELEAFSIZE-2)
#define MAXPATHTESTS 128
  struct UnrootedBinary *ct = newUnrootedBinary(TREELEAFSIZE);
  struct DoubleA *n = getTreeNodes(ct, NULL);
  struct DoubleA *spm, *spmmap, *pp;
  union PCTypes a, b;
  int retval;
  int psize;
  int cur;
  int i;
	int plen = MAXPATHTESTS;
	int pbuf[MAXPATHTESTS];
//  struct DataBlock *dotdb;
  assert(getSize(n) == TREENODEWANTED);
  pp = getPerimeterPairs(ct, NULL);
  assert(getSize(pp) == TREELEAFSIZE);
  freeDoubleDoubler(pp);
  spmmap = makeSPMMap(getAdjAdaptorForUB(ct));
  for (i = 0; i < RETRIES; ++i) {
    a = getRandomElement(n);
    assert(a.i >= 0 && a.i < 100);
    b = getRandomElement(n);
    assert(b.i >= 0 && b.i < 100);
    spm = makeSPMFor(getAdjAdaptorForUB(ct), b.i);
    cur = a.i;
    psize = 1;
    do {
      assert(cur >= 0);
      assert(cur < TREENODEWANTED * 2 - 2);
      psize += 1;
//      printf("%d ", cur);
      cur = getValueAt(spm, cur).i;
    }
    while (cur != b.i);
//    printf("\n");
    retval = pathFinder(getAdjAdaptorForUB(ct), a.i, b.i, pbuf, &plen);
    assert(retval == CL_OK);
    assert(plen == psize);
    freeDoubleDoubler(spm);
    spm = NULL;
  }
  freeSPMMap(spmmap);
  spmmap = NULL;
  freeDoubleDoubler(n);
  n = NULL;
//  dotdb = convertTreeToDot(ct, NULL, getLabelPerm(ct));
//  writeDataBlockToFile(dotdb, "treefile.dot");
//  assert(dotdb);
//  assert(dotdb->ptr);
//  assert(dotdb->size > TREENODEWANTED * 2);
  freeUnrootedBinary(ct);
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
  m = convertAdjAdaptorToGSLMatrix(a1);
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

#if GSL_RDY
void doSBS3Test(void);

void testSpringBall(void)
{
  doSBS3Test();
}
#endif

#if GSL_RDY
void testALTagFile(void)
{
  char *s = "hello world";
  gsl_matrix *gm, *ngm;
  char *result;
  struct StringStack *ss, *nes, *nlabels;
  struct DataBlock dbstr, dbss, dbgslm, dbdm, dblabels, dbpkg, dbpkg_read;
  struct DoubleA *dd;
  int i;
  t_tagtype curtnum;

  dbstr = dumpString(s);
  result = loadString(dbstr, 1);
  assert(strcmp(s,result) == 0);

  ss = newStringStack();
  pushSS(ss, "Liesl");
  pushSS(ss, "Frederick");
  pushSS(ss, "Louisa");

  dbss = dumpStringStack(ss);
  nes = loadStringStack(dbss, 1);
  assert(strcmp(readAtSS(nes,0), readAtSS(ss,0)) == 0);
  assert(strcmp(readAtSS(nes,1), readAtSS(ss,1)) == 0);
  assert(strcmp(readAtSS(nes,2), readAtSS(ss,2)) == 0);

  dblabels = dumpDMLabels(ss);
  nlabels = loadDMLabels(dblabels,1);
  assert(strcmp(readAtSS(nlabels,0), readAtSS(ss,0)) == 0);
  assert(strcmp(readAtSS(nlabels,1), readAtSS(ss,1)) == 0);
  assert(strcmp(readAtSS(nlabels,2), readAtSS(ss,2)) == 0);

  freeSS(nes);
  gfreeandclear(result);

#define TAGFILENAME "tagtest.clb"
  gm = gsl_matrix_alloc(2,1);
  gsl_matrix_set(gm, 0, 0, 4.0);
  gsl_matrix_set(gm, 1, 0, 0.5);
  dbgslm = dumpGSLMatrix(gm);
  dbdm = dumpCLDistMatrix(gm);
  ngm = loadGSLMatrix(dbgslm, 1);
  assert(gm != ngm);
  assert(gm->size1 == ngm->size1);
  assert(gm->size2 == ngm->size2);
  assert(gsl_matrix_get(ngm, 0, 0) == 4.0);
  assert(gsl_matrix_get(ngm, 1, 0) == 0.5);
  gsl_matrix_free(ngm);
  ngm = loadCLDistMatrix(dbdm,1);
  assert(gm != ngm);
  assert(gm->size1 == ngm->size1);
  assert(gm->size2 == ngm->size2);
  assert(gsl_matrix_get(ngm, 0, 0) == 4.0);
  assert(gsl_matrix_get(ngm, 1, 0) == 0.5);
  gsl_matrix_free(ngm);

  dbpkg = package_DataBlocks(TAGNUM_TAGMASTER, &dbstr, &dbgslm, &dbss, NULL);
  freeDataBlock(dbss);
  freeDataBlock(dbstr);
  freeDataBlock(dbgslm);

  writeDataBlockToFile(&dbpkg,TAGFILENAME);
  freeDataBlock(dbpkg);
  dbpkg_read = convertFileToDataBlock(TAGFILENAME);
  unlink(TAGFILENAME);
  dd = load_DataBlock_package(dbpkg_read);
  for (i = 0; i < getSize(dd); i += 1) {
    curtnum = getValueAt(dd,i).idbp.tnum;
    switch (curtnum) {
      case TAGNUM_STRING:
        result = loadString(*getValueAt(dd,i).idbp.db, 1);
        assert(strcmp(s,result) == 0);
        gfreeandclear(result);
        break;
      case TAGNUM_GSLMATRIX:
        ngm = loadGSLMatrix(*getValueAt(dd,i).idbp.db, 1);
        assert(gm != ngm);
        assert(gm->size1 == ngm->size1);
        assert(gm->size2 == ngm->size2);
        assert(gsl_matrix_get(ngm, 0, 0) == 4.0);
        assert(gsl_matrix_get(ngm, 1, 0) == 0.5);
        gsl_matrix_free(ngm);
        break;
      case TAGNUM_STRINGSTACK:
        nes = loadStringStack(*getValueAt(dd,i).idbp.db, 1);
        assert(strcmp(readAtSS(nes,0), readAtSS(ss,0)) == 0);
        assert(strcmp(readAtSS(nes,1), readAtSS(ss,1)) == 0);
        assert(strcmp(readAtSS(nes,2), readAtSS(ss,2)) == 0);
        freeSS(nes);
        break;
      default:
        break;
    }
  }
  freeDoubleDoubler(dd);
  freeSS(ss);
  gsl_matrix_free(gm);
  freeDataBlock(dbpkg_read);
}
#endif

#define ARRAYSIZE 10

void testLabelPerm(void)
{
  struct LabelPerm *lpa, *lpb, *lpc;
  struct DoubleA *nodes = newDoubleDoubler();
  int i;
  for (i = 0; i < 10; i += 1) {
    union PCTypes p = zeropct;
    p.i = i + 23;
    setValueAt(nodes, i, p);
  }
  lpa = newLabelPerm(nodes);
  lpb = cloneLabelPerm(lpa);
  lpc = cloneLabelPerm(lpb);
  assert(isLabelPermIdentical(lpa, lpb));
  assert(isLabelPermIdentical(lpa, lpc));
  mutateLabelPerm(lpb);
  assert(!isLabelPermIdentical(lpa, lpb));
  assert(isLabelPermIdentical(lpa, lpc));
  assert(!isLabelPermIdentical(lpc, lpb));
  freeLabelPerm(lpa);
  freeLabelPerm(lpb);
  freeLabelPerm(lpc);
  freeDoubleDoubler(nodes);
}

void testPerimPairs()
{
  struct TreeAdaptor *tra = loadNewRootedTRA(6);
  struct DoubleA *da;
  struct CLNodeSet *clns = newCLNodeSet(9);
  addNodeToSet(clns, 2);
  da = treeperimpairsTRA(tra, NULL);
  freeDoubleDoubler(da);
  da = treeperimpairsTRA(tra, clns);
  freeDoubleDoubler(da);
}

void testTreeMolder()
{
  struct DataBlock db[LABELCOUNT];
  int i, j;
  struct CompAdaptor *bz = loadBuiltinCA("bzip");
  double score;
  struct TreeScore *ts;
  struct DataBlockEnumeration *dbe;
  for (j = 0; j < TREETRIALCOUNT; j += 1) {
    int labelcount = rand() % 4 + 4;
//    printf("doing trial %d, with %d leaves...\n", j, labelcount);
    struct UnrootedBinary *ct = newUnrootedBinary(labelcount);
    struct TreeAdaptor *ta = loadUBTRA(ct);
    struct DoubleA *n = getTreeNodes(ct, NULL);
    gsl_matrix *dm;
    assert(bz);
    gconf->ca = bz;
    for (i = 0; i < labelcount; ++i) {
      char buf[1024], buf2[2048];
      sprintf(buf, "%d%d%d%d%d%d%d%d", i,i,i,i,i,i,i,i);
      sprintf(buf2, "%s%d%s%s%d%s%d",buf,buf[3],buf,buf+3,i+8,buf,i % 3);
      db[i] = convertStringToDataBlock(buf2);
    }
    dbe = loadArrayDBE(db, labelcount);
    dm = getNCDMatrix(dbe, dbe, gconf);
    ts = initTreeScore(ta);
    {
      struct TreeMolder *tmolder;
      struct TreeAdaptor *tram = newTreeTRA(1,dm->size1);
      tmolder = newTreeMolder(dm, tram);
      for (i = 0; i < 100; i += 1) {
        score = getScoreScaledTM(tmolder);
        if (score > 1.0 || score < 0.0) {
          printf("Error, got score %f\n", score);
        }
        //assert(score >= 0.0 && score <= 1.0);
        tryToImproveTM(tmolder);
      }
      freeTreeMolder(tmolder);
      tmolder = NULL;
    }
    freeDoubleDoubler(n);
    freeTreeScore(ts);
//    freeUnrootedBinary(ct);
    treefreeTRA(ta);
    dbe->efree(dbe);
    for (i = 0; i < labelcount; i += 1)
      freeDataBlock(db[i]);
  }
  freeCA(bz);
  gconf->ca = NULL;
  bz = NULL;
}

void testReadTextDM()
{
  gsl_matrix *dm;
  struct StringStack *labels;
  dm = get_dm_from_txt("distmatrix.txt");
  labels = get_labels_from_txt("distmatrix.txt");
  assert(dm);
  assert(labels);
  gsl_matrix_free(dm);
  freeSS(labels);
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
  struct TreeAdaptor *ta = newTreeTRA(0, 6);
  int i;
  gsl_matrix *m;
  m = convertAdjAdaptorToGSLMatrix(treegetadjaTRA(ta));
//  printGSLMatrix(m);

  for (i = 0; i < 15 ; i += 1) {
    treemutateTRA(ta);
    stepTowardsTree(m, ta, 1.0);
//    printGSLMatrix(m);
  }
}

char *findDir(const char *dir)
{
  const char *locations[] = { ".", "..", "../.."};
  int i;
  struct stat sbuf;
  char *pbuf = gmalloc(1024);

  for(i = 0; locations[i]; i += 1) {
    sprintf(pbuf,"%s/%s",locations[i],dir);
     if (stat(pbuf, &sbuf) == 0)
       return pbuf;
  }
  printf("Error finding test directory; runtests Aborted\n");
  exit(1);
}

int main(int argc, char **argv)
{
  char *testpath;
  char *srcpath;
  testfile = gmalloc(1024);
  if (argc > 1)
    testpath = argv[1];
  else
    testpath = findDir("test");
  srcpath = findDir("src");
  sprintf(testfile, "%s/runtests.c", srcpath);
  testbzfile = (char *)gmalloc(strlen(testpath)+20);
  testgzfile = (char *)gmalloc(strlen(testpath)+20);
  testzlibfile = (char *)gmalloc(strlen(testpath)+20);
  testpg4dir = (char *)gmalloc(strlen(testpath)+20);
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
//  testGoogle();        //TODO: more memory leaks left here
#endif
//  testYamlParser();    //FIXME: convert to new parser or fix Syck
  testRealComp();
  testVirtComp();
//  testDL();   //TODO: investigate mem leaks which may be caused by dlopen
//  testDL2();  //TODO: investigate mem leaks which may be caused by dlopen
  testArrayDBE();
  testDirectoryDBE();
#if GSL_RDY
  testMarshalling();
  testSpringBall();
  testCLTree();
  testQuartet();
  testALTagFile();
  testSmoothing();
//  testReadTextDM();
#endif
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
#if 0
#endif
  freeDefaultEnvironment(gconf);
  gconf = NULL;

  if (testpath) {
    gfreeandclear(testpath);
    gfreeandclear(testbzfile);
    gfreeandclear(testgzfile);
    gfreeandclear(testzlibfile);
  }
  if (srcpath) {
    gfreeandclear(srcpath);
  }
  return 0;
}

