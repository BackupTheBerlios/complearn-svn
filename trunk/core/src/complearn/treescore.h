#ifndef __TREESCORE_H
#define __TREESCORE_H

#include <complearn/quartet.h>

/*! \file treescore.h */

struct TreeScore;
struct TreeAdaptor;
struct AdjAdaptor;

struct TreeScore *clInitTreeScore(gsl_matrix *dm);

double clScoreTree(struct TreeScore *ts, struct TreeAdaptor *ta);
int clIsConsistent(struct AdjAdaptor *ad, struct Quartet q);
void clFreeTreeScore(struct TreeScore *ts);

#endif
