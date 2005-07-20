#ifndef __TREEADAPTOR_H
#define __TREEADAPTOR_H

#include <complearn/cltypes.h>

struct TreeAdaptor {
  void *ptr;
  t_treemutate treemutate;
  t_treefree treefree;
  t_treeclone treeclone;
  t_treegetlabelperm treegetlabelperm;
  t_treegetadja treegetadja;
  t_treenodepred treeisquartetable;
  t_treenodepred treeisflippable;
  t_treenodepred treeisroot;
  t_treemutecount treemutecount;
  t_treeperimpairs treeperimpairs;
};

struct TreeFactory;
struct TreeFactory *newTreeFactory(int isRooted);
struct TreeAdaptor *newTreeTF(struct TreeFactory *tf, int howManyNodes);
void freeTreeFactory(struct TreeFactory *tf);

void treemutateTRA(struct TreeAdaptor *tra);
void treefreeTRA(struct TreeAdaptor *tra);
struct TreeAdaptor *treecloneTRA(struct TreeAdaptor *tra);
struct LabelPerm *treegetlabelpermTRA(struct TreeAdaptor *tra);
struct AdjA *treegetadjaTRA(struct TreeAdaptor *tra);
int treeIsQuartettable(struct TreeAdaptor *tra, int which);
int treeIsFlippable(struct TreeAdaptor *tra, int which);
int treeIsRoot(struct TreeAdaptor *tra, int which);
int treemutecountTRA(struct TreeAdaptor *tra);
int treeGetNodeCountTRA(struct TreeAdaptor *tra);
double getTreeDifferenceScore(struct TreeAdaptor *tra1, struct TreeAdaptor *tra2);
struct DoubleA *treeperimpairsTRA(struct TreeAdaptor *tra, struct CLNodeSet *flips);

#endif
