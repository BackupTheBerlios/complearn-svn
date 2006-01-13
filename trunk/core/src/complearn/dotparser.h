
#ifndef __DOTPARSER_H
#define __DOTPARSER_H

#include <complearn/cltypes.h>

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>

/*! \file dotparser.h */

struct DotParseTree {
  struct StringStack *labels;
  struct TreeAdaptor *ta;
};

void doParseTest(struct DataBlock *db);

#endif
