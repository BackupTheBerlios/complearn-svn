#ifndef __TREESCORE_H
#define __TREESCORE_H

#include <complearn/quartet.h>

/*! \file treescore.h */

struct TreeScore;
struct TreeAdaptor;
struct AdjAdaptor;

struct TreeScore *clInitTreeScore(struct TreeAdaptor *ta);

double clScoreTree(struct TreeScore *ts, gsl_matrix *dm);
int clIsConsistent(struct AdjAdaptor *ad, struct Quartet q);
void clFreeTreeScore(struct TreeScore *ts);

#endif
