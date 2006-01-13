#ifndef __TREESCORE_H
#define __TREESCORE_H

#include <complearn/quartet.h>

#if GSL_RDY

#endif

/*! \file treescore.h */

struct TreeScore;
struct TreeAdaptor;
struct AdjAdaptor;

struct TreeScore *initTreeScore(struct TreeAdaptor *ta);

#if GSL_RDY
double scoreTree(struct TreeScore *ts, gsl_matrix *dm);
#else
double scoreTreeDD(struct TreeScore *ts, struct DoubleA *dm);
#endif
int isConsistent(struct AdjAdaptor *ad, struct Quartet q);
void freeTreeScore(struct TreeScore *ts);

#endif
