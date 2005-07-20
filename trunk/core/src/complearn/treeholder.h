#ifndef __TREEHOLDER_H
#define __TREEHOLDER_H

struct TreeHolder;
struct TreeAdaptor;

#if GSL_RDY
struct TreeHolder *newTreeHolder(const gsl_matrix *distmat, struct TreeAdaptor *tra);
double getCurScore(const struct TreeHolder *th);
struct TreeAdaptor *getCurTree(const struct TreeHolder *th);
/* returns 1 if it did improve, 0 otherwise */
int tryToImprove(struct TreeHolder *th);

struct TreeHolder *cloneTreeHolder(const struct TreeHolder *th);
int getTotalTreeCount(const struct TreeHolder *th);
int getSuccessiveFailCount(const struct TreeHolder *th);
void scrambleTreeHolder(struct TreeHolder *th);
gsl_matrix *getDistMatrixTH(const struct TreeHolder *th);
void setTreeIndexTH(struct TreeHolder *th, int treeind);
int getTreeIndexTH(struct TreeHolder *th);

void freeTreeHolder(struct TreeHolder *th);

#endif

#endif
