#ifndef __TREEPARSER_H
#define __TREEPARSER_H

#include <complearn/quartet.h>

#if GSL_RDY

#endif

/*! \file treeparser.h */

struct TreeParser;
struct TreeAdaptor;
struct AdjAdaptor;

struct TreeParser *initTreeParser(struct TreeAdaptor *ta);

#if GSL_RDY
double parserTree(struct TreeParser *ts, gsl_matrix *dm);
#else
double parserTreeDD(struct TreeParser *ts, struct DoubleA *dm);
#endif
//int isConsistent(struct AdjAdaptor *ad, struct Quartet q);
//void freeTreeParser(struct TreeParser *ts);

#endif
