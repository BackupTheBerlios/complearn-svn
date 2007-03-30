#ifndef __CNEX_H
#define __CNEX_H

#include <complearn/complearn.h>

char *nodeToNewick(struct TreeAdaptor *ta, struct LabelPerm *lp, struct StringStack *labels, int me, int fromWhere);
struct DataBlock *matToNexus(gsl_matrix *gm, struct StringStack *labels, struct TreeAdaptor *ta);
int isNexusFile(struct DataBlock *db);
struct DataBlock *getNexusTaxaBlock(struct DataBlock *db);
struct DataBlock *getNexusDistancesBlock(struct DataBlock *db);
struct DataBlock *getNexusTreeBlock(struct DataBlock *db);
char *findSubstringCaseInsensitive(struct DataBlock *db, const char *tstr);
struct StringStack *getNexusLabels(struct DataBlock *db);
gsl_matrix *getNexusDistanceMatrix(struct DataBlock *db);
char *getNexusTreeString(struct DataBlock *db, char **ttype);

#endif
