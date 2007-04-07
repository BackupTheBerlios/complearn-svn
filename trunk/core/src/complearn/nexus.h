#ifndef __CLNEX_H
#define __CLNEX_H

struct TreeAdaptor;
struct LabelPerm;
struct DataBlock;
struct StringStack;

char *nodeToNewick(struct TreeAdaptor *ta, struct LabelPerm *lp, struct StringStack *labels, int me, int fromWhere);
struct DataBlock *clMatToNexus(gsl_matrix *gm, struct StringStack *labels, struct TreeAdaptor *ta);
int clIsNexusFile(struct DataBlock *db);
struct DataBlock *getNexusTaxaBlock(struct DataBlock *db);
struct DataBlock *getNexusDistancesBlock(struct DataBlock *db);
struct DataBlock *getNexusTreeBlock(struct DataBlock *db);
char *findSubstringCaseInsensitive(struct DataBlock *db, const char *tstr);
struct StringStack *clGetNexusLabels(struct DataBlock *db);
gsl_matrix *getNexusDistanceMatrix(struct DataBlock *db);
char *getNexusTreeString(struct DataBlock *db, char **ttype);

#endif
