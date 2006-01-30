#ifndef __TREESCORE_H
#define __TREESCORE_H

#include <complearn/quartet.h>

/*! \file treescore.h */

struct TreeScore;
struct TreeAdaptor;
struct AdjAdaptor;

struct TreeScore *initTreeScore(struct TreeAdaptor *ta);

double scoreTree(struct TreeScore *ts, gsl_matrix *dm);
int isConsistent(struct AdjAdaptor *ad, struct Quartet q);
void freeTreeScore(struct TreeScore *ts);

#endif
