#ifndef __TREEPARSER_H
#define __TREEPARSER_H

#include <complearn/quartet.h>


/*! \file treeparser.h */

struct TreeParser;
struct TreeAdaptor;
struct AdjAdaptor;

struct TreeParser *initTreeParser(struct TreeAdaptor *ta);

double parserTree(struct TreeParser *ts, gsl_matrix *dm);

#endif
