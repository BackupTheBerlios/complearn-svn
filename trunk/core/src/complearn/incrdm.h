#ifndef __INCRDM_H
#define __INCRDM_H

#include <gsl/gsl_matrix.h>

struct IncrementalDistMatrix;

struct IncrementalDistMatrix *newIDM(struct CompAdaptor *ca);
void addDataBlock(struct IncrementalDistMatrix *idm, struct DataBlock *db);
void freeIncrementalDistMatrix(struct IncrementalDistMatrix *idm);
gsl_matrix *getDistMatrixIDM(struct IncrementalDistMatrix *idm);
int getSizeIDM(struct IncrementalDistMatrix *idm);

#endif
