#ifndef __LABELPERM_H
#define __LABELPERM_H

/*! \file labelperm.h */

struct LabelPerm;

struct LabelPerm *newLabelPerm(struct DoubleA *labelledNodes);
void freeLabelPerm(struct LabelPerm *lph);
void mutateLabelPerm(struct LabelPerm *lph);
int getSizeLP(struct LabelPerm *lph);
struct LabelPerm *cloneLabelPerm(struct LabelPerm *lph);

int getNodeIDForColumnIndexLP(struct LabelPerm *lph, int which);
int getColumnIndexForNodeIDLP(struct LabelPerm *lph, int which);
int isLabelPermIdentical(struct LabelPerm *lpa, struct LabelPerm *lpb);

void verifyLabelPerm(struct LabelPerm *lp);

#endif
