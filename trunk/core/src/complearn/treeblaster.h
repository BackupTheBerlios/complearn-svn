#ifndef __TREEBLASTER_H
#define __TREEBLASTER_H

#include <complearn/cltypes.h>

#if GSL_RDY
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>
#endif

struct TreeBlaster;

#if GSL_RDY
struct TreeBlaster *newTreeBlaster(gsl_matrix *gsl, struct TreeAdaptor *ta);
#endif

struct CLNodeSet *findTreeOrder(struct TreeBlaster *tbl, double *s);

void freeTreeBlaster(struct TreeBlaster *tbl);

struct TreeOrderObserver {
  void *ptr;
  t_treeordersearchstarted treeordersearchstarted;
  t_treeorderimproved treeorderimproved;
  t_treeorderdone treeorderdone;
};

void setTreeOrderObserver(struct TreeBlaster *tbl, struct TreeOrderObserver *tob);
int getKTB(struct TreeBlaster *tbl);
int getLabelCountTB(struct TreeBlaster *tbl);

#endif

