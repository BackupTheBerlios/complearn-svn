#ifndef __TREEMOLDER_H
#define __TREEMOLDER_H

#include <complearn/cltypes.h>

/*! \file treemolder.h */

struct TreeMolder;

#if GSL_RDY
struct TreeMolder *newTreeMolder(gsl_matrix *gm, struct TreeAdaptor *ta);
void freeTreeMolder(struct TreeMolder *tm);
double getScoreTM(struct TreeMolder *tm);
double getScoreScaledTM(struct TreeMolder *tm);
struct CLNodeSet *getFlips(struct TreeMolder *tm);
void scrambleTreeMolder(struct TreeMolder *tm);
int tryToImproveTM(struct TreeMolder *tm);
struct TreeAdaptor *getCurTreeTM(const struct TreeMolder *tmo);
int getNodeCountTMO(const struct TreeMolder *tmo);

#endif

#endif

