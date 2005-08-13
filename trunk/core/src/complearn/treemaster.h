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

/** \brief Searches for the next best tree
 *
 *  findTree() will search for a new tree, and return a pointer to a TreeHolder
 *  if the tree it is holding has a better tree score than the tree previously
 *  found in the last call of findTree().
 *
 *  If needed, abortTreeSearch(), can be called in a separate thread to
 *  prematurely interrupt findTree().
 *
 *  \param tm pointer to TreeMaster
 */
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

/** \brief Aborts current search for better tree
 *
 *  Interrupts search for better tree. Used for multi-threaded processing.
 *  Assumes findTree() has already been called in another thread for the same
 *  TreeMaster.
 *  \param tm pointer to TreeMaster
 */
void abortTreeSearch(struct TreeMaster *tm);

#endif
