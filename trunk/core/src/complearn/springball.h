#ifndef __SPRINGBALL_H
#define __SPRINGBALL_H

#if GSL_RDY
#include <gsl/gsl_odeiv.h>
#include <gsl/gsl_vector.h>

struct SpringBallSystem;
struct SBS3;

gsl_matrix *convertAdjAToGSLMatrix(struct AdjA *aa);
void stepTowardsTree(gsl_matrix *smooth, struct TreeAdaptor *ta, double dt);

struct SpringBallSystem *newSBS(struct TreeAdaptor *ta);
void freeSBS(struct SpringBallSystem *sbs);

int stepForward(struct SpringBallSystem *sbs, double tstep);
int isStable(struct SpringBallSystem *sbs);
void set2DForce(struct SpringBallSystem *sbs, int newval);
double maxBallDisplacement(struct SpringBallSystem *sbs);
double getMaxVal(const struct SpringBallSystem *sbs);
void jiggleSBS(struct SpringBallSystem *sbs);
gsl_vector *getBallPosition(struct SpringBallSystem *sbs, int whichBall);
void setModelSpeedSBS(struct SpringBallSystem *sbs, double modelSpeed);
void changeTargetTreeSBS(struct SpringBallSystem *sbs, struct TreeAdaptor *ta);
double getSpringSmoothSBS(struct SpringBallSystem *sbs, int i, int j);
void evolveForward(struct SpringBallSystem *sbs);
int getNodeCountSBS(struct SpringBallSystem *sbs);

#endif

#endif
