#ifndef __TREESCORE_H
#define __TREESCORE_H

#include <complearn/quartet.h>

#if GSL_RDY
#include <gsl/gsl_matrix.h>
#endif

/*! \file treescore.h */

struct TreeScore;
struct TreeAdaptor;
struct AdjA;

struct TreeScore *initTreeScore(struct TreeAdaptor *ta);

#if GSL_RDY
double scoreTree(struct TreeScore *ts, gsl_matrix *dm);
#else
double scoreTreeDD(struct TreeScore *ts, struct DoubleA *dm);
#endif
int isConsistent(struct AdjA *ad, struct Quartet q);
void freeTreeScore(struct TreeScore *ts);

#endif
