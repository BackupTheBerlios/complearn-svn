#ifndef __TREEMASTER_H
#define __TREEMASTER_H

#include <complearn/cltypes.h>

#if GSL_RDY
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>
#endif

struct TreeMaster;

#if GSL_RDY
struct TreeMaster *newTreeMaster(gsl_matrix *gsl, int isRooted);
struct TreeMaster *newTreeMasterEx(gsl_matrix *gsl, int isRooted, struct EnvMap *em);
struct TreeMaster *newTreeMasterEz(gsl_matrix *gsl);
#endif

struct TreeHolder *getStarterTree(struct TreeMaster *tm);
struct TreeHolder *findTree(struct TreeMaster *tm);
struct TreeHolder *getTreeAtIndex(struct TreeMaster *tm, int i);
int totalTreesExamined(struct TreeMaster *tm);
int getNodeCountTM(struct TreeMaster *tm);
int getKTM(struct TreeMaster *tm);
void setUserDataTM(struct TreeMaster *tm, void *udata);
void *getUserDataTM(struct TreeMaster *tm);
int getLabelCountTM(struct TreeMaster *tm);

void freeTreeMaster(struct TreeMaster *tm);

struct TreeObserver {
  void *ptr;
  t_treesearchstarted treesearchstarted;
  t_treeimproved treeimproved;
  t_treerejected treerejected;
  t_treedone treedone;
};

void setTreeObserver(struct TreeMaster *tm, struct TreeObserver *tob);
struct TreeObserver *getTreeObserver(struct TreeMaster *tm);

struct CLDateTime *getEndTimeTM(struct TreeMaster *tm);
struct CLDateTime *getStartTimeTM(struct TreeMaster *tm);
void abortTreeSearch(struct TreeMaster *tm);

#endif
