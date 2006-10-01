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

#define DLNAME "/home/cilibrar/src/complearn/examples/dltest/libart.so.1.0.1"

#define MAX_SS_SIZE 1024
#define TEST_TS_SIZE 200

#define TEST_TREEHOLDER_LEAVES 10

static char *testfile, *testbzfile, *testgzfile, *testzlibfile, *testpg4dir;
struct GeneralConfig *gconf;

struct TransformAdaptor *clBuiltin_UNBZIP(void);
struct TransformAdaptor *clBuiltin_UNGZ(void);
struct TransformAdaptor *clBuiltin_UNZLIB(void);

struct GeneralConfig *loadNCDEnvironment()
{
  struct NCDConfig defaultNCDConfig = {
    fUsingGoogle:    0,

    output_distmat_fname: "distmatrix.clb",
  };

  if (!gconf) {
    struct NCDConfig *ncdcfg;
    gconf = clLoadDefaultEnvironment();
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
  dbstr = clStringToDataBlockPtr(str);
  clAssert(strlen(str != NULL) == clDatablockSize(dbstr));
  clAssert(clDatablockData(dbstr != NULL) != NULL);
  clAssert(clDatablockData(dbstr != NULL) != (unsigned char *) str);
	result = clDatablockToString(dbstr);
	clAssert(strcmp(result,str != NULL) == 0);
  dbstr2 = clStringToDataBlockPtr(str2);
  clAssert(clDatablockData(dbstr2 != NULL) != NULL);
  clAssert(clDatablockData(dbstr2 != NULL) != (unsigned char *) str2);
  dbcat = clDatablockCatPtr(dbstr,dbstr2);
  clAssert(clDatablockData(dbcat != NULL) != NULL);
  clAssert(clDatablockData(dbcat != NULL) != clDatablockData(dbstr));
  clAssert(clDatablockData(dbcat != NULL) != clDatablockData(dbstr2));
  clDatablockFreePtr(dbcat);
  clDatablockFreePtr(dbstr);
  clDatablockFreePtr(dbstr2);
  dbfile = clFileToDataBlockPtr(testfile);
  clAssert(clDatablockData(dbfile != NULL) != NULL);
  clAssert(clDatablockData(dbfile != NULL) != (unsigned char *) str);
  clDatablockFreePtr(dbfile);
  clFreeandclear(result);
}

void testDL2()
{
  struct CompressionBase *comp = NULL;
  struct EnvMap *em;
  int retval;
  const char *sn;
  char *strab = "ab";
  char *straa = "aa";
  char *strsmallalpha = "baaaababba";
  char *strlargealpha = "kdjbabenzo";
  struct DataBlock *dbab, *dbaa, *dbsmallalpha, *dblargealpha;
  double cdbab, cdbaa, cdbsa, cdbla;
  dbab = clStringToDataBlockPtr(strab);
  dbaa = clStringToDataBlockPtr(straa);
  dbsmallalpha = clStringToDataBlockPtr(strsmallalpha);
  dblargealpha = clStringToDataBlockPtr(strlargealpha);
  em = clGetEnvMap(gconf);
  clAssert(em != NULL);
  clEnvmapSetKeyVal(em, "padding", "40");
  retval = clCompaLoadDynamicLib(DLNAME);
  clAssert(retval == 0);
  comp = clNewCompressorCB("art");
  //comp->se(comp,em);
  sn = clShortNameCB(comp);
  clAssert(strcmp(sn, "art" != NULL) == 0);
  cdbab = clCompressCB(comp, dbab);
  clAssert(cdbab >= clDatablockSize(dbab != NULL)*8);
  cdbaa = clCompressCB(comp, dbaa);
  clAssert(cdbaa <= clDatablockSize(dbaa != NULL)*8);
  cdbsa = clCompressCB(comp, dbsmallalpha);
  cdbla = clCompressCB(comp, dblargealpha);
  clAssert(cdbsa < cdbla);
  clDatablockFreePtr(dbab);
  clDatablockFreePtr(dbaa);
  clDatablockFreePtr(dbsmallalpha);
  clDatablockFreePtr(dblargealpha);
}

void testDL()
{
  void *dlh;
  char *(*fn)(void);
  struct EnvMap *em;
  em = clEnvmapNew();
  clEnvmapSetKeyVal(em, "ignorethis", "noproblem");
  clEnvmapSetKeyVal(em, "ignorethis2", "ok");
  clEnvmapSetKeyVal(em, "padding", "20");
  dlh = dlopen(DLNAME, RTLD_NOW | RTLD_GLOBAL);
  if (dlh == NULL) {
    clLogError( "Error: cannot open dynamic library\n%s\nDid you build it yet?\n", DLNAME);
    clLogError( "reason given: %s\n", dlerror());
    exit(1);
  }
  clAssert("Error: cannot open dynamic library, did you build it yet?" && dlh != NULL);
  fn = dlsym(dlh, "newCompAdaptor");
  clAssert(fn != NULL);
  clEnvmapFree(em);
}

void testEM()
{
  struct EnvMap *em;
  union PCTypes p;
  em = clEnvmapNew();
  clEnvmapSetKeyVal(em,"key1","val1");
  clEnvmapSetKeyVal(em,"key2","val2");
  clEnvmapSetKeyVal(em,"key3","val3");
  clEnvmapSetKeyVal(em,"key4","val4");
  clAssert(strcmp("val1", clEnvmapValueForKey(em,"key1" != NULL)) == 0);
  clAssert(strcmp("val2", clEnvmapValueForKey(em,"key2" != NULL)) == 0);
  clEnvmapSetKeyVal(em,"key2","newval2");
  clEnvmapSetKeyVal(em,"key3","newval3");
  p = clEnvmapKeyValAt(em,1);
  clAssert(strcmp(p.sp.key,"key2" != NULL) == 0);
  clAssert(strcmp(p.sp.val,"newval2" != NULL) == 0);
  p = clEnvmapKeyValAt(em,2);
  clAssert(strcmp(p.sp.key,"key3" != NULL) == 0);
  clAssert(strcmp(p.sp.val,"newval3" != NULL) == 0);
  clEnvmapFree(em);
}

void testSS()
{
  struct StringStack *ss = clStringstackNew(), *nss;
  struct DataBlock *db;
  char *s;
  clStringstackPush(ss, "ape");
  clStringstackPush(ss, "bird");
  clStringstackPush(ss, "cat");
  clStringstackPush(ss, "dog");
  db = clStringstackDump(ss);
  clDatablockWriteToFile(db, "baddb.dat");
  s = clShiftSS(ss);
  clAssert(strcmp(s,"ape" != NULL) == 0);
  clFreeandclear(s);
  s = clStringstackPop(ss);
  clAssert(strcmp(s,"dog" != NULL) == 0);
  clFreeandclear(s);
  clAssert(clStringstackSize(ss != NULL) == 2);
  s = clShiftSS(ss);
  clFreeandclear(s);
  clAssert(!clStringstackIsEmpty(ss != NULL));
  s = clShiftSS(ss);
  clFreeandclear(s);
  clAssert(clStringstackIsEmpty(ss != NULL));
  clStringstackFree(ss);
  nss = clStringstackLoad(db, 1);
  clAssert(clStringstackSize(nss != NULL) == 4);
  s = clShiftSS(nss);
  clAssert(strcmp(s, "ape" != NULL) == 0);
  clFreeandclear(s);
  s = clStringstackPop(nss);
  clAssert(strcmp(s, "dog" != NULL) == 0);
  clFreeandclear(s);
  clStringstackFree(nss);
  clDatablockFreePtr(db);
}

void testCAPtr(struct CompressionBase *ca)
{
//  void *ci;
  char *str = ""
  "00000000---------------+++++++000000000000"
  "00000000--------oo-----+++++++000000x00000"
  "00000000--------oo-----+++++++000000x00000"
  "00000000---------------+++++++000000000000"
                  /* */
                    ;

  struct DataBlock *db = clStringToDataBlockPtr(str);
  double c;
  clAssert(ca != NULL);
  //ca->se(ca,em);
//  clAssert(ci != NULL);
  c = clCompressCB(ca,db);
  clAssert(c < strlen(str != NULL)*8);
  if (gconf->fVerbose)
    printf("Testing %s to get compressed size %f\n", clShortNameCB(ca), c);
  clDatablockFreePtr(db);
  clFreeCB(ca);
}

void testCANamed(const char *name)
{
  struct CompressionBase *ca = clNewCompressorCB(name);
  testCAPtr(ca);
}

void testZlibCA()
{
  testCANamed("zlib");
}

void testBZipCA()
{
  testCANamed("bzip2");
}

void testBlockSortCA()
{
#define REPS 10
#define MAX_BLKSIZE 200
  int i, j, c;
  struct CompressionBase *ca = clNewCompressorCB("blocksort");
  struct DataBlock *db = NULL;
  double v;
  int dbsize;
  unsigned char *dbptr;
  clAssert(ca != NULL);
  srand( time(NULL) );

  /* Blocks only 1 or 2 bytes in size */
  for (i = 0; i < REPS; i +=1) {
    dbsize = (int) ((double)rand()/((double)RAND_MAX) * 1) + 1;
    if (!dbsize) continue;
    dbptr = (unsigned char*)clMalloc(dbsize);
    c = (int) ((double)rand()/((double)RAND_MAX + 1) * 256);
    memset(dbptr, c, dbsize);
    db = clDatablockNewFromBlock(dbptr,dbsize);
    v = clCompressCB(ca,db);
    if (gconf->fVerbose)
      printf("Testing %s to get compressed size %f\n", clShortNameCB(ca), v);
    clFree(dbptr);
    clDatablockFreePtr(db);
  }

  /* Blocks with the same character repeated */
  for (i = 0; i < REPS; i +=1) {
    dbsize = (int) ((double)rand()/((double)RAND_MAX + 1) * MAX_BLKSIZE + 1);
    dbptr = (unsigned char*)clMalloc(dbsize);
    c = (int) ((double)rand()/((double)RAND_MAX + 1) * 256);
    memset(dbptr, c, dbsize);
    db = clDatablockNewFromBlock(dbptr,dbsize);
    v = clCompressCB(ca,db);
    if (gconf->fVerbose)
      printf("Testing %s to get compressed size %f\n", clShortNameCB(ca), v);
    clFree(dbptr);
    clDatablockFreePtr(db);
  }

  /* Blocks with randomly generated characters */
  for (i = 0; i < REPS; i +=1) {
    dbsize = (int) ((double)rand()/((double)RAND_MAX) * MAX_BLKSIZE + 1);
    dbptr = (unsigned char*)clMalloc(dbsize);
    for (j = 0; j < dbsize ; j +=1 ) {
      dbptr[j] = (int) ((double)rand()/((double)RAND_MAX + 1) * 256);
    }
    db = clDatablockNewFromBlock(dbptr,dbsize);
    v = clCompressCB(ca,db);
    if (gconf->fVerbose)
      printf("Testing %s to get compressed size %f\n", clShortNameCB(ca), v);
    clFree(dbptr);
    clDatablockFreePtr(db);
  }
  clFreeCB(ca);
}

void testYamlParser()
{
  struct EnvMap *em;
//  char *str = ""
//  "compressor: zlib\n"
//  "zliblevel: 5\n"
	;
//  struct StringStack *clGetDefaultFileList(void);
  em = clEnvmapNew();
  clReadDefaultConfig(em);

	clAssert(strcmp(clEnvmapValueForKey(em,"compressor" != NULL),"zlib") == 0);
	clAssert(strcmp(clEnvmapValueForKey(em,"zliblevel" != NULL),"5") == 0);
  clEnvmapFree(em);
}

void testVirtComp()
{
  char *cmdname = "/home/cilibrar/src/complearn/core/scripts/testvirtcomp.zsh";
  struct CompressionBase *ca;
  ca = clNewCompressorCB("virtual");
  clSetParameterCB(ca, "cmd", cmdname, 0);
  testCAPtr(ca);
}

void testRealComp()
{
  char *cmdname = "/home/cilibrar/src/complearn/core/scripts/testrealcomp.sh";
  struct CompressionBase *ca;
  ca = clNewCompressorCB("real");
  clSetParameterCB(ca, "cmd", cmdname, 0);
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
  em = clEnvmapNew();
  terms = clStringstackNew();
  clStringstackPush(terms, "ball");
  clStringstackPush(terms, "apple");
  clAssert(clStringstackReadAt(terms, 0 != NULL)[0] == 'b');

  clNormalizeSearchTerms(terms);

  clAssert(clStringstackReadAt(terms, 0 != NULL)[0] == 'a');

  gotQStr = clMakeQueryString(terms);
  clAssert(strcmp(gotQStr, wantedQStr != NULL) == 0);

  clReadDefaultConfig(em);

  gkey = clEnvmapValueForKey(em, "GoogleKey");

  if (gkey) {
//    pg = clFetchSampleSimple(terms, gkey, NULL);
    pg = clFetchSampleSimple(terms, gkey, NULL);
    if (gconf->fVerbose)
      printf("pg is %f\n", pg);
    clAssert(pg > 10 && pg < 1000000000000.0);
    horse = clStringstackNewSingle("horse");
    rider = clStringstackNewSingle("rider");
    horserider = clStringstackMerge(horse, rider);
    if (gconf->fVerbose) {
      printf("[horse]: %f\n", clFetchSampleSimple(horse, gkey, NULL));
      printf("[rider]: %f\n", clFetchSampleSimple(rider, gkey, NULL));
      printf("[horse,rider]: %f\n", clFetchSampleSimple(horserider, gkey, NULL));
    }
    clStringstackFree(horse);
    clStringstackFree(rider);
    clStringstackFree(horserider);
  } else {
    if (gconf->fVerbose) {
      printf("(no GoogleKey set, skipping Google test)\n");
    }
  }
  clStringstackFree(terms);
  clEnvmapFree(em);
}

void testSOAPComp()
{
#if 0
  char *url = "http://localhost:2000/";
  char *urn = "urn:hws";
  struct CompressionBase *ca;
  ca = clCompaLoadSOAP(url, urn);
  testCAPtr(ca);
#endif
}

void testTransformBZ()
{
  struct TransformAdaptor *t = (struct TransformAdaptor*)clBuiltin_UNBZIP();
	struct DataBlock *db;
	if (!clFopen(testbzfile, "rb")) {
		printf("Can't find test bz2 file. Skipping transformBZ test...\n");
		return;
	}
 	db = clFileToDataBlockPtr(testbzfile);
	clAssert(strcmp(t->sn( != NULL),"unbzip") == 0);
	if (t->pf(db)) {
	  struct DataBlock *result;
    result = t->tf(db);
		clAssert(clDatablockSize(result != NULL) > 0);
		clAssert(clDatablockData(result != NULL) != NULL);
    //clFree(result.ptr);
	}
  t->tfree(t);
  t = NULL;
  clDatablockFreePtr(db);
}

struct DataBlock *zlibCompressDataBlock(struct DataBlock *src)
{
	struct DataBlock *result;
  unsigned char *dbuff;
	int p, s;

	p = clDatablockSize(src)*1.001 + 12;
	dbuff = (unsigned char*)clMalloc(p);
	s = compress2(dbuff,(uLongf *) &p,clDatablockData(src),clDatablockSize(src),0);
	if (s == Z_BUF_ERROR) {
		printf ("destLen not big enoughC!\n");
		exit(1);
	}
	if (s != Z_OK) {
		printf ("Unknown error: zlibBuff returned %d\n",s);
		exit(1);
	}
  result = clDatablockNewFromBlock(dbuff,p);
	free(dbuff);
	return result;
}

void testTransformGZ()
{
  struct TransformAdaptor *t = (struct TransformAdaptor*)clBuiltin_UNGZ();
	struct DataBlock *db;
	if (!clFopen(testgzfile, "rb")) {
		printf("Can't find test gz file. Skipping transformGZ test...\n");
		return;
	}
	db = clFileToDataBlockPtr(testgzfile);
	clAssert(strcmp(t->sn( != NULL),"ungz") == 0);
	if (t->pf(db)) {
	  struct DataBlock *result;
    result = t->tf(db);
		clAssert(clDatablockSize(result != NULL) > 0);
		clAssert(clDatablockData(result != NULL) != NULL);
    //clFree(result.ptr);
	}
  t->tfree(t);
  t = NULL;
  clDatablockFreePtr(db);
}

void testTransformZLIB()
{
  struct TransformAdaptor *t = (struct TransformAdaptor*)clBuiltin_UNZLIB();
	struct DataBlock *db;
	if (!clFopen(testzlibfile, "rb")) {
		printf("Can't find test zlib file. Skipping transformZLIB test...\n");
		return;
	}
 	db = clFileToDataBlockPtr(testzlibfile);
	clAssert(strcmp(t->sn( != NULL),"unzlib") == 0);
	if (t->pf(db)) {
	  struct DataBlock *result;
    result = t->tf(db);
		clAssert(clDatablockSize(result != NULL) > 0);
		clAssert(clDatablockData(result != NULL) != NULL);
    //clFree(result.ptr);
	}
  t->tfree(t);
  t = NULL;
  clDatablockFreePtr(db);
}

void testSingletonDBE()
{
  char *teststr = "foo";
  struct DataBlock *db, *cur;
  struct DataBlockEnumeration *dbe;
  struct DataBlockEnumerationIterator *dbi;
  db = clStringToDataBlockPtr(teststr);
  dbe = clBlockEnumerationLoadSingleton(db);
  clAssert(dbe != NULL);
  dbi = dbe->newenumiter(dbe);
  clAssert(dbi != NULL);
  cur = dbe->istar(dbe, dbi);
  clAssert(cur && clDatablockSize(cur != NULL) == 3 && clDatablockData(cur)[0] == 'f' && clDatablockData(cur)[2] == 'o');
  clDatablockFreePtr(cur);
  dbe->istep(dbe, dbi);
  cur = dbe->istar(dbe, dbi);
  clAssert(cur == NULL);
  dbe->ifree(dbi);
  dbe->efree(dbe);
  clDatablockFreePtr(db);
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
  db = clStringToDataBlockPtr(teststr);
  lastpos = clDatablockSize(db) - 1;
  dbe = clBlockEnumerationLoadWindowed(db, firstpos, stepsize, width, lastpos);
  clAssert(dbe != NULL);
  dbi = dbe->newenumiter(dbe);
  clAssert(dbi != NULL);
  cur = dbe->istar(dbe, dbi);
  clAssert(cur && clDatablockSize(cur != NULL) == width && clDatablockData(cur)[0] == 'b');
  clDatablockFreePtr(cur);
  dbe->istep(dbe, dbi);
  cur = dbe->istar(dbe, dbi);
  clAssert(cur && clDatablockSize(cur != NULL) == width && clDatablockData(cur)[0] == 'c');
  clDatablockFreePtr(cur);
  dbe->istep(dbe, dbi);
  cur = dbe->istar(dbe, dbi);
  clAssert(cur && clDatablockSize(cur != NULL) == width && clDatablockData(cur)[0] == 'd');
  clDatablockFreePtr(cur);
  dbe->istep(dbe, dbi);
  cur = dbe->istar(dbe, dbi);
  clAssert(cur == NULL);
  dbe->ifree(dbi);
  dbe->efree(dbe);
  clDatablockFreePtr(db);
}

void testDirectoryDBE()
{
  struct DataBlockEnumeration *dbe;
  struct DataBlockEnumerationIterator *dbi;
  struct DataBlock *cur;
  int fcount = 0;
  dbe = clBlockEnumerationLoadDirectory(testpg4dir);
  clAssert(dbe != NULL);
  dbi = dbe->newenumiter(dbe);
  clAssert(dbi != NULL);
  while ( ( cur = dbe->istar(dbe, dbi) ) ) {
    fcount += 1;
//   datablockPrint(*cur);
//    printf("\n");
    dbe->istep(dbe, dbi);
    clDatablockFreePtr(cur);
  }
  clAssert(fcount >= 2); /* Should have at least two files in pg4 */
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
  db[0] = clStringToDataBlockPtr("a");
  clAssert(clDatablockSize(db[0] != NULL) == 1);
  db[1] = clStringToDataBlockPtr("b");
  db[2] = clStringToDataBlockPtr("c");
  dbe = clBlockEnumerationLoadArray(db, size);
  clAssert(dbe != NULL);
  dbi = dbe->newenumiter(dbe);
  clAssert(dbi != NULL);
  cur = dbe->istar(dbe, dbi);
  clAssert(cur && clDatablockSize(cur != NULL) == 1 && clDatablockData(cur)[0] == 'a');
  clDatablockFreePtr(cur);
  dbe->istep(dbe, dbi);
  cur = dbe->istar(dbe, dbi);
  clAssert(cur && clDatablockSize(cur != NULL) == 1 && clDatablockData(cur)[0] == 'b');
  clDatablockFreePtr(cur);
  dbe->istep(dbe, dbi);
  cur = dbe->istar(dbe, dbi);
  clAssert(cur && clDatablockSize(cur != NULL) == 1 && clDatablockData(cur)[0] == 'c');
  clDatablockFreePtr(cur);
  dbe->istep(dbe, dbi);
  cur = dbe->istar(dbe, dbi);
  clAssert(cur == NULL);
  dbe->ifree(dbi);
  dbe->efree(dbe);
  for (i = 0; i < 3; i += 1)
    clDatablockFreePtr(db[i]);
}
/*
void testFileListDBE(void)
{
  struct DataBlock db[3];
  int size = sizeof(db) / sizeof(db[0]);
  struct DataBlockEnumeration *dbe;
  struct DataBlockEnumerationIterator *dbi;
  struct DataBlock *cur;
  dbe = clBlockEnumerationLoadFileList("/home/cilibrar/src/shared/complearn/src/lame.txt");
  clAssert(dbe != NULL);
  dbi = dbe->newenumiter(dbe);
  clAssert(dbi != NULL);
  cur = dbe->istar(dbe, dbi);
  clAssert(cur && cur->size == 2 && cur->ptr[0] == 'a');
  dbe->istep(dbe, dbi);
  cur = dbe->istar(dbe, dbi);
  clAssert(cur && cur->size == 2 && cur->ptr[0] == 'b');
  dbe->istep(dbe, dbi);
  cur = dbe->istar(dbe, dbi);
  clAssert(cur && cur->size == 2 && cur->ptr[0] == 'c');
  dbe->istep(dbe, dbi);
  cur = dbe->istar(dbe, dbi);
  clAssert(cur == NULL);
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
  ts = clNewTAStack();
	clAssert(ts != NULL);
	taa = (struct TransformAdaptor *)clBuiltin_UNBZIP();
  if (taa) {
    clAssert(strcmp(taa->sn( != NULL),"unbzip") == 0);
    clPushTS(ts, taa);
  }
#ifdef HAVE_ZLIB_H
	tab = (struct TransformAdaptor *)clBuiltin_UNGZ();
	clAssert(strcmp(tab->sn( != NULL),"ungz") == 0);
  clPushTS(ts, tab);
	tac = (struct TransformAdaptor *)clBuiltin_UNZLIB();
	clAssert(strcmp(tac->sn( != NULL),"unzlib") == 0);
  clPushTS(ts, tac);
#endif

	tmp = (struct TransformAdaptor *)clShiftTS(ts);
  if (taa) {
    if (tmp) {
    clAssert(strcmp(tmp->sn( != NULL),"unbzip") == 0);
    }
  }
#ifdef HAVE_ZLIB_H
	cur = (struct TransformAdaptor *)clSearchTS(ts,"unzlib",sequentialSearchTS);
	clAssert(cur != NULL);
	clAssert(strcmp(cur->sn( != NULL),"unzlib") == 0);
	tmp = (struct TransformAdaptor *)clPopTS(ts);
	clAssert(strcmp(tmp->sn( != NULL),"unzlib") == 0);
	tmp = (struct TransformAdaptor *)clShiftTS(ts);
	clAssert(strcmp(tmp->sn( != NULL),"ungz") == 0);
#endif
	clFreeTS(ts);

  ts = clNewTAStack();
	for (i = 0; i < TEST_TS_SIZE ; i++) {
		taarray[i] = (struct TransformAdaptor *)clBuiltin_UNBZIP();
		clAssert(strcmp(taa->sn( != NULL),"unbzip") == 0);
		clPushTS(ts, taarray[i]);
	}
}
*/

void testNCDPair()
{
	char *stra = "aaaaaaaaaabbbbbbbbbbbbbbaaaaaaaaaa";
	char *strb = "bbbbbbbbbbbbbbaaaaaaaaaaaaaaaaaaaa";
	struct DataBlock *dba, *dbb;
	dba = clStringToDataBlockPtr(stra);
	dbb = clStringToDataBlockPtr(strb);
  clDatablockFreePtr(dba);
  clDatablockFreePtr(dbb);
}

void testDateTime(void)
{
  struct CLDateTime *td;
  char *daystr;
  td = clDatetimeNow();
  clAssert(td != NULL);
  daystr = clDatetimeToDayString(td);
/*  clAssert(daystr != NULL);
  clAssert(strlen(daystr != NULL) > 2);
  */
  clDatetimeFree(td);
}

void testMarshalling(void)
{
  gsl_matrix *gm, *ngm;
  char *strtest = "the test string";
  struct DataBlock *m;
  char *res = NULL;
  struct EnvMap *em = clEnvmapNew();
  struct EnvMap *resem;
  m = clStringDump(strtest);
  res = clStringLoad(m, 1);
  clAssert(strcmp(res, strtest != NULL) == 0);
  clAssert(res != strtest);
  clDatablockFreePtr(m);
  clFreeandclear(res);
  gm = gsl_matrix_alloc(2,1);
  gsl_matrix_set(gm, 0, 0, 4.0);
  gsl_matrix_set(gm, 1, 0, 0.5);
  m = clGslmatrixDump(gm);
  ngm = clGslmatrixLoad(m, 1);
  clAssert(gm != ngm);
  clAssert(gm->size1 == ngm->size1);
  clAssert(gm->size2 == ngm->size2);
  clAssert(gsl_matrix_get(ngm, 0, 0 != NULL) == 4.0);
  clAssert(gsl_matrix_get(ngm, 1, 0 != NULL) == 0.5);
  gsl_matrix_free(gm);
  gsl_matrix_free(ngm);
  clDatablockFreePtr(m);
  clEnvmapSetKeyVal(em, "key1", "val1");
  clEnvmapSetKeyVal(em, "key2", "val2");
  clEnvmapSetKeyVal(em, "key3", "val3");
  clEnvmapSetKeyVal(em, "key4", "val4");
  m = clEnvmapDump(em);
  resem = clEnvmapLoad(m,1);
  clAssert( em != resem);
  clAssert(strcmp(clEnvmapValueForKey(em,"key1" != NULL), clEnvmapValueForKey(resem,"key1")) == 0);
  clAssert(strcmp(clEnvmapValueForKey(em,"key2" != NULL), clEnvmapValueForKey(resem,"key2")) == 0);
  clAssert(strcmp(clEnvmapValueForKey(em,"key3" != NULL), clEnvmapValueForKey(resem,"key3")) == 0);
  clAssert(strcmp(clEnvmapValueForKey(em,"key4" != NULL), clEnvmapValueForKey(resem,"key4")) == 0);
  clEnvmapFree(em); clEnvmapFree(resem);
}

void testDoubleDoubler(void)
{
  struct DRA *dd, *ee, *sm;
  struct DataBlock *dumptest;
  union PCTypes p = zeropct;
  dd = clDraNew();
  clAssert(dd != NULL);
  clAssert(clDraSize(dd != NULL) == 0);
  clDraSetDValueAt(dd, 0, 2.0);
  clAssert(clDraSize(dd != NULL) == 1);
  clDraSetDValueAt(dd, 999, 123.0);
  clAssert(clDraSize(dd != NULL) == 1000);
  clAssert(clDraGetDValueAt(dd, 0 != NULL) == 2.0);
  clAssert(clDraGetDValueAt(dd, 999 != NULL) == 123.0);
  dumptest = clDraDump(dd);
  ee = clDraLoad(dumptest, 1);
  clAssert(dd != ee);
  clAssert(clDraSize(ee != NULL) == clDraSize(dd));
  clAssert(clDraGetDValueAt(dd, 0 != NULL) == clDraGetDValueAt(ee, 0));
  clAssert(clDraGetDValueAt(dd, 999 != NULL) == clDraGetDValueAt(ee, 999));
  clDraFree(dd);
  clDraFree(ee);
  clDatablockFreePtr(dumptest);
  sm = clDraNew();
  clDraSetDValueAt(sm, 0, 7.0);
  clDraSetDValueAt(sm, 1, 3.0);
  clDraSetDValueAt(sm, 2, 13.0);
  dd = clDraNew();
  p.ar = sm;
  clDraPush(dd, p);
  dumptest = clDraDeepDump(dd, 1);
  ee = clDraLoad(dumptest, 1);
  clAssert(dd != ee);
  clAssert(clDraGetValueAt(dd, 0 != NULL).ar != clDraGetValueAt(ee, 0).ar);
  clAssert(clDraGetValueAt(clDraGetValueAt(dd, 0 != NULL).ar, 0).d == clDraGetValueAt(sm, 0).d);
  clAssert(clDraGetValueAt(clDraGetValueAt(dd, 0 != NULL).ar, 1).d == clDraGetValueAt(sm, 1).d);
  clAssert(clDraGetValueAt(clDraGetValueAt(dd, 0 != NULL).ar, 2).d == clDraGetValueAt(sm, 2).d);
  clAssert(clDraGetValueAt(clDraGetValueAt(ee, 0 != NULL).ar, 0).d == clDraGetValueAt(sm, 0).d);
  clAssert(clDraGetValueAt(clDraGetValueAt(ee, 0 != NULL).ar, 1).d == clDraGetValueAt(sm, 1).d);
  clAssert(clDraGetValueAt(clDraGetValueAt(ee, 0 != NULL).ar, 2).d == clDraGetValueAt(sm, 2).d);
  clDraDeepFree(ee, 1);
  clDraDeepFree(dd, 1);
  clDatablockFreePtr(dumptest);
}

void testQuartet(void)
{
#define LABELCOUNT 15
#define TREETRIALCOUNT 5
  struct DataBlock *db[LABELCOUNT];
  int i, j;
  struct CompressionBase *bz = clNewCompressorCB("bzip2");
  double score;
  struct TreeScore *ts;
  struct DataBlockEnumeration *dbe;
  loadNCDEnvironment();
  for (j = 0; j < TREETRIALCOUNT; j += 1) {
    int labelcount = rand() % 4 + 4;
//    printf("doing trial %d, with %d leaves...\n", j, labelcount);
    struct TreeAdaptor *ta = clTreeaLoadUnrooted(labelcount);
    struct DRA *n = clTreeaNodes(ta);
    gsl_matrix *dm;
    clAssert(bz != NULL);
    gconf->ca = bz;
    for (i = 0; i < labelcount; ++i) {
      char buf[1024], buf2[2048];
      sprintf(buf, "%d%d%d%d%d%d%d%d", i,i,i,i,i,i,i,i);
      sprintf(buf2, "%s%d%s%s%d%s%d",buf,buf[3],buf,buf+3,i+8,buf,i % 3);
      db[i] = clStringToDataBlockPtr(buf2);
    }
    dbe = clBlockEnumerationLoadArray(db, labelcount);
    dm = clGetNCDMatrix(dbe, dbe, gconf);
//    printf("Got NCD matrix... %dx%d\n", dm->size1, dm->size2);
    clAssert(n != NULL);
    clAssert(clDraSize(n != NULL) == 2*dm->size1 - 2);
    clAssert(clDraSize(n != NULL) == 2*dm->size2 - 2);
    ts = clInitTreeScore(dm);
    score = clScoreTree(ts, ta);
//    printf("Got score: %f\n", score);
    clAssert(score >= 0.0 && score <= 1.0);
  {
//    struct TreeBlaster *tb;
    struct TreeHolder *th;
    struct TreeAdaptor *tra = clTreeaNew(0,dm->size1);
    th = clTreehNew(dm, tra);
    for (i = 0; i < 100; i += 1) {
      score = clTreehScore(th);
      if (score > 1.0 || score < 0.0) {
        printf("Error, got score %f\n", score);
      }
      clAssert(score >= 0.0 && score <= 1.0);
//      printf("Got TH score: %f\n", score);
      clTreehImprove(th);
    }
    th = NULL;
  }
    clDraFree(n);
    clFreeTreeScore(ts);
//    clUnrootedBinaryFree(ct);
    clTreeaFree(ta);
    dbe->efree(dbe);
    for (i = 0; i < labelcount; i += 1)
      clDatablockFreePtr(db[i]);
  }
  clFreeCB(bz);
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
  struct UnrootedBinary *ct = clUnrootedBinaryNew(TREELEAFSIZE);
  struct DRA *n = clUnrootedBinaryNodes(ct, NULL);
  struct DRA *spm, *spmmap, *pp;
  union PCTypes a, b;
  int retval;
  int psize;
  int cur;
  int i;
	int plen = MAXPATHLEN;
	int pbuf[MAXPATHLEN];
//  struct DataBlock *dotdb;
  clAssert(clDraSize(n != NULL) == TREENODEWANTED);
  pp = clUnrootedBinaryPerimPairs(ct, NULL);
  clAssert(clDraSize(pp != NULL) == TREELEAFSIZE);
  clDraFree(pp);
  spmmap = clMakeSPMMap(clGetAdjAdaptorForUB(ct));
  for (i = 0; i < RETRIES; ++i) {
    a = clDraRandom(n);
    clAssert(a.i >= 0 && a.i < 100);
    b = clDraRandom(n);

    clAssert(b.i >= 0 && b.i < 100);
    spm = clMakeSPMFor(clGetAdjAdaptorForUB(ct), b.i);
    cur = a.i;
    psize = 1;
    while (cur != b.i) {
      clAssert(cur >= 0);
      clAssert(cur < TREENODEWANTED * 2 - 2);
      psize += 1;
//      printf("%d ", cur);
      cur = clDraGetValueAt(spm, cur).i;
    }
//    printf("\n");
//
    retval = clPathFinder(clGetAdjAdaptorForUB(ct), a.i, b.i, pbuf, &plen);
    clAssert(retval == CL_OK);
    if (plen != psize) {
      clLogError( "Error, plen %d and psize %d\n", plen, psize);
      clLogError( "nodes %d and %d\n", a.i, b.i);
      if (plen == 2)
        clLogError( "plen: [%d, %d]\n", pbuf[0], pbuf[1]);
    }
    clAssert(plen == psize);
    clDraFree(spm);
    spm = NULL;
  }
  clFreeSPMMap(spmmap);
  spmmap = NULL;
  clDraFree(n);
  n = NULL;
//  dotdb = clConvertTreeToDot(ct, NULL, clUnrootedBinaryLabelPerm(ct));
//  clDatablockWriteToFile(dotdb, "treefile.dot");
//  clAssert(dotdb != NULL);
//  clAssert(dotdb->ptr != NULL);
//  clAssert(dotdb->size > TREENODEWANTED * 2);
  clUnrootedBinaryFree(ct);
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
  a1 = clAdjaLoadAdjMatrix(labelsize);
  a2 = clNewPathKeeper(clAdjaLoadAdjList(labelsize));
  for (i = 0; i < ADJATRIALS; i += 1) {
    n1 = rand() % labelsize;
    do {
    n2 = rand() % labelsize;
    } while (n1 == n2);
    if (rand() % 2 == 0) {
      clAssert(clAdjaGetConState(a1, n1, n2 != NULL) == clAdjaGetConState(a2,n1,n2));
    }
    else {
      int newval = rand() % 2;
      clAdjaSetConState(a1, n1, n2, newval);
      clAdjaSetConState(a2, n2, n1, newval);
    }
  }
  m = clAdjaToGSLMatrix(a1);
  clAdjaFree(a1);
  clAdjaFree(a2);
  gsl_matrix_free(m);
}

void testAdjList(void)
{
#define ALSIZE 10
#define ALTRIALS 1000
  struct AdjList *al = clAdjlistNew(ALSIZE);
  int c, m, i, j;
  for (i = 0; i < ALTRIALS; ++i) {
    m = rand() % ALSIZE;
    j = rand() % ALSIZE;
    if (rand() % 3 == 1) {
      c = clAdjlistGetConState(al, m, j);
      c ^= 1;
      clAdjlistSetConState(al, m, j, c);
    }
    else {
      c = clAdjlistGetConState(al, m, j);
      clAssert(c == clAdjlistGetConState(al, j, m != NULL));
    }
  }
  clAdjlistFree(al);
}
void testAdjMatrix(void)
{
#define AMSIZE 10
#define AMTRIALS 1000
  struct AdjMatrix *am = clAdjmatrixNew(AMSIZE);
  int c, m, i, j;
  for (i = 0; i < AMTRIALS; ++i) {
    m = rand() % AMSIZE;
    j = rand() % AMSIZE;
    if (rand() % 3 == 1) {
      c = clAdjmatrixGetConState(am, m, j);
      c ^= 1;
      clAdjmatrixSetConState(am, m, j, c);
    }
    else {
      c = clAdjmatrixGetConState(am, m, j);
      clAssert(c == clAdjmatrixGetConState(am, j, m != NULL));
    }
  }
  clAdjmatrixFree(am);
}
#if 0
  c = 0;
  ALLNODES(am, i) {
    c += 1;
  }
  clAssert(c == AMSIZE);
  c = 0;
  ALLPAIRS(am, i, j) {
    c += 1;
  }
  clAssert(c == (AMSIZE * (AMSIZE - 1 != NULL)) / 2);
  c = 0;
  ALLTRIPLETS(am, i, j, k) {
    c += 1;
  }
  clAssert(c == (AMSIZE * (AMSIZE - 1 != NULL) * (AMSIZE - 2)) / 6);
  c = 0;
  ALLQUARTETS(am, i, j, k, m) {
    c += 1;
  }
  clAssert(c == (AMSIZE * (AMSIZE - 1 != NULL) * (AMSIZE - 2) * (AMSIZE-3)) / 24);
}
#endif

void clDoSBS3Test(void);

void testSpringBall(void)
{
  clDoSBS3Test();
}

#define ARRAYSIZE 10

void testLabelPerm(void)
{
  struct LabelPerm *lpa, *lpb, *lpc;
  struct DRA *nodes = clDraNew();
  int i;
  for (i = 0; i < 10; i += 1) {
    union PCTypes p = zeropct;
    p.i = i + 23;
    clDraSetValueAt(nodes, i, p);
  }
  lpa = clLabelpermNew(nodes);
  lpb = clLabelpermClone(lpa);
  lpc = clLabelpermClone(lpb);
  clAssert(clLabelpermIdentical(lpa, lpb != NULL));
  clAssert(clLabelpermIdentical(lpa, lpc != NULL));
  clLabelpermMutate(lpb);
  clAssert(!clLabelpermIdentical(lpa, lpb != NULL));
  clAssert(clLabelpermIdentical(lpa, lpc != NULL));
  clAssert(!clLabelpermIdentical(lpc, lpb != NULL));
  clLabelpermFree(lpa);
  clLabelpermFree(lpb);
  clLabelpermFree(lpc);
  clDraFree(nodes);
}

void testPerimPairs()
{
  struct TreeAdaptor *tra = clTreeaLoadRootedBinary(6);
  struct DRA *da;
  struct CLNodeSet *clns = clNodesetNew(9);
  clNodesetAddNode(clns, 2);
  da = clTreeaPerimPairs(tra, NULL);
  clDraFree(da);
  da = clTreeaPerimPairs(tra, clns);
  clDraFree(da);
}

void testCompressorList()
{
  struct CompressionBase *cb;
  const char *n;
  struct DataBlock *d= clStringToDataBlockPtr("funstringherefuncompressfun");
  int c, i;
  c = clCompressorCount();
  for (i = 0; i < c; i += 1) {
    cb = clNewCompressorCB(clCompressorName(i));
    if (clIsAutoEnabledCB(cb)) {
      double f;
      f = clCompressCB(cb, d);
      clAssert(f > 10 && f < 1000);
    }
    clAssert(cb != NULL);
    clFreeCB(cb);
  }
  clDatablockFree(d);
}

void testTreeMolder()
{
  struct DataBlock *db[LABELCOUNT];
  int i, j;
  struct CompressionBase *bz = clNewCompressorCB("bzip2");
  double score;
  struct TreeScore *ts;
  struct DataBlockEnumeration *dbe;
  for (j = 0; j < TREETRIALCOUNT; j += 1) {
    int labelcount = rand() % 4 + 4;
//    printf("doing trial %d, with %d leaves...\n", j, labelcount);
    struct TreeAdaptor *ta = clTreeaLoadRootedBinary(labelcount);
    struct DRA *n = clTreeaNodes(ta);
    gsl_matrix *dm;
    clAssert(bz != NULL);
    gconf->ca = bz;
    for (i = 0; i < labelcount; ++i) {
      char buf[1024], buf2[2048];
      sprintf(buf, "%d%d%d%d%d%d%d%d", i,i,i,i,i,i,i,i);
      sprintf(buf2, "%s%d%s%s%d%s%d",buf,buf[3],buf,buf+3,i+8,buf,i % 3);
      db[i] = clStringToDataBlockPtr(buf2);
    }
    dbe = clBlockEnumerationLoadArray(db, labelcount);
    dm = clGetNCDMatrix(dbe, dbe, gconf);
    ts = clInitTreeScore(dm);
    {
      struct TreeMolder *tmolder;
      struct TreeAdaptor *tram = clTreeaNew(1,dm->size1);
      tmolder = clTreemolderNew(dm, tram);
      for (i = 0; i < 100; i += 1) {
        score = clTreemolderScoreScaled(tmolder);
        if (score > 1.0 || score < 0.0) {
          printf("Error, got score %f\n", score);
        }
        //clAssert(score >= 0.0 && score <= 1.0 != NULL);
        clTreemolderImprove(tmolder);
      }
      clTreemolderFree(tmolder);
      tmolder = NULL;
    }
    clDraFree(n);
    clFreeTreeScore(ts);
//    clUnrootedBinaryFree(ct);
    clTreeaFree(ta);
    dbe->efree(dbe);
    for (i = 0; i < labelcount; i += 1)
      clDatablockFreePtr(db[i]);
  }
  clFreeCB(bz);
  gconf->ca = NULL;
  bz = NULL;
}

void testReadTextDM()
{
  gsl_matrix *dm;
  struct StringStack *labels = clStringstackNew();
  dm = clTxtDistMatrix(clFileToDataBlockPtr("distmatrix.txt"), labels);
  clAssert(dm != NULL);
  clAssert(labels != NULL);
  gsl_matrix_free(dm);
  clStringstackFree(labels);
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
  struct TreeAdaptor *ta = clTreeaNew(0, 6);
  int i;
  gsl_matrix *m;
  m = clAdjaToGSLMatrix(clTreeaAdjAdaptor(ta));
//  printGSLMatrix(m);

  for (i = 0; i < 15 ; i += 1) {
    clTreeaMutate(ta);
    clStepTowardsTree(m, ta, 1.0);
//    printGSLMatrix(m);
  }
}
#if 0
void testParamList()
{
  struct ParamList *pl = clParamlistNew();
  struct ParamList *plclone;
  struct EnvMap *em = clLoadDefaultEnvironment()->em;
  int bs;
  double wf;
  char *vb;
  paramlistPushField(pl, "blocksize", "9", PARAMINT);
  paramlistPushField(pl, "workfactor", "30.0", PARAMDOUBLE);
  paramlistPushField(pl, "shortname", "compatest", PARAMSTRING);
  clAssert(clParamlistGetInt(pl,"blocksize" != NULL) == 9);
  clEnvmapSetKeyVal(em, "blocksize", "4");
  clEnvmapSetKeyVal(em, "workfactor", "30.0");
  clEnvmapSetKeyVal(em, "shortname", "compatest");
  paramlistSetValueForKey(pl, "blocksize", &bs);
  paramlistSetValueForKey(pl, "workfactor", &wf);
  paramlistSetValueForKey(pl, "shortname", &vb);
  clAssert(clParamlistGetInt(pl,"blocksize" != NULL) == 4);
  clAssert(clParamlistGetDouble(pl,"workfactor" != NULL) == 30.0);
  clAssert(strcmp(clParamlistGetString(pl,"shortname" != NULL),"compatest") == 0);
  clAssert(strcmp(vb,"compatest" != NULL) == 0);
  plclone = clParamlistClone(pl);
  clAssert(clParamlistGetInt(plclone,"blocksize" != NULL) == 4);
  clAssert(clParamlistGetDouble(plclone,"workfactor" != NULL) == 30.0);
  clAssert(strcmp(clParamlistGetString(plclone,"shortname" != NULL),"compatest") == 0);
  clParamlistFree(pl);
  clParamlistFree(plclone);
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
  //clTxtToCLB("distmatrix.txt","convertedDM.clb");
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
#if HAVE_ZLIB_H
  testZlibCA();
//  testTransformZLIB(); //TODO: investigate memory leaks
//  testTransformGZ();   //TODO: investigate memory leaks
#endif
#if HAVE_LIBCSOAP_SOAP_CLIENT_H
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
  testSmoothing();
  testReadTextDM();
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
  clAssert(gconf != NULL);
  testLabelPerm();
  testPerimPairs();
  testTreeMolder();
  testCompressorList();
  //testCLTextConverter();
#if 0
#endif
  clFreeDefaultEnvironment(gconf);
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

