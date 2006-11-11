#include <R.h>
#include <Rdefines.h>
#include <Rinternals.h>
#include <dlfcn.h>
#include <R_ext/Rdynload.h>
#include <complearn/complearn.h>

// looking at http://cran.r-project.org/doc/manuals/R-exts.html
// and        http://www.sfu.ca/~sblay/R-C-interface.txt
//

SEXP ncd(SEXP sexpstr1, SEXP sexpstr2);

R_CallMethodDef callMethods[] = {
    {"ncd", &ncd, 2},
    {NULL, NULL, 0}
};

void R_init_rcomplearn(DllInfo *info)
{
  printf("RCOMPLEARN Initialized\n");
  R_registerRoutines(info, NULL, callMethods, NULL, NULL);
}

double computeNCD(SEXP s1, SEXP s2)
{
  double result;
  struct DataBlock *db1, *db2;
  db1 = clStringToDataBlockPtr( CHAR(STRING_ELT(s1,0)));
  db2 = clStringToDataBlockPtr( CHAR(STRING_ELT(s2,0)));
// TODO: figure out if this is a dynamic-linking bug in Makefile or what
//  result = clNcdFunc(db1, db2, NULL);
  clDatablockFreePtr(db1);
  clDatablockFreePtr(db2);
  result = 0.6;
  return result;
}

SEXP ncd(SEXP sexpstr1, SEXP sexpstr2) {
  SEXP ans;
 // PROTECT(sexpstr1 = asChar(sexpstr1));
 // PROTECT(sexpstr2 = asChar(sexpstr2));
  PROTECT(ans = allocVector(REALSXP, 1));
  REAL(ans)[0] = computeNCD(sexpstr1, sexpstr2);
 // UNPROTECT(3);
  UNPROTECT(1);
  return(ans);
}
