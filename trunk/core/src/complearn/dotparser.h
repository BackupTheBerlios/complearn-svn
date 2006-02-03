
#ifndef __DOTPARSER_H
#define __DOTPARSER_H

#include <complearn/cltypes.h>


#include <gsl/gsl_blas.h>

/*! \file dotparser.h */

struct DotParseTree {
  struct StringStack *labels;
  struct TreeAdaptor *tree;
  gsl_matrix *dm;
  char *parseErrorString;
};

struct DotParseTree *clParseDotDB(struct DataBlock *db, struct DataBlock *matdb);

#endif
