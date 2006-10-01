/*
* Copyright (c) 2006 Rudi Cilibrasi, Rulers of the RHouse
* All rights reserved.     cilibrar@cilibrar.com
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the RHouse nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE RULERS AND CONTRIBUTORS "AS IS" AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE RULERS AND CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <complearn/complearn.h>
#include <unistd.h>
#include <string.h>

/*! \file springball.c */

#define AJAC_PARITY 0
#define TWODFORCESTR 5.00

#include <gsl/gsl_vector.h>

const double BASECHARGE = 4.0;
const double BASEMASS = 0.1;
double massScale  = 1.0;

struct SBS4;

/** \brief ODE for charged spring ball system evolving over time
 *
 * The physical simulation for the SpringBall system.
 *
 * \sa springball.h
 * \struct SpringBallSystem
 */
struct SpringBallSystem {
  struct SBS4 *sbs4;
  gsl_odeiv_system sys;
  gsl_odeiv_step *s;
  gsl_odeiv_step_type *T;
  gsl_odeiv_evolve *e;
  gsl_odeiv_control *c;
  double h;
  double t;
  double modelSpeed;
};

/** \brief half-force 2-ball system centered on object-ball (ball 1)
 *
 * This structure is the basic building block for all larger systems.
 * Two of these structures (one and its inverse with ball 1 and ball 2
 * exchanged and thus i, j, etc) together make one complete 2 ball system.
 * Several of these superimposed on top of each other yield a larger
 * multibody system.  Each 2-ball system has the following adjustments:
 *
 * it may or may not have a spring
 *
 * a rest length for the spring if it exists
 *
 * each ball may hold a positive or negative charge
 *
 * each ball has its own mass, and position
 *
 * ball 1 has a velocity used for calculating viscous drag
 *
 * there is a drag flag/coefficient used to make sure drag is calculated
 * only once despite "n choose 2" * 2 different SBS3 to calculate each round
 *
 * \struct SBS3
 *
 * \sa springball.h
 */
struct SBS3 {
  int i, j;
  gsl_vector_view p1, p2, v1; /* x1 == position of ball 1 */
                            /* v1 == velocity of ball 1 */
                            /* x2 == position of ball 2 */
  double c1, c2, m1, m2; /* charge 1, charge 2, mass 1, mass 2 */
  double restLen; /* rest length of the spring, if it exists */
  double drag;    /* viscosity of the air */

  gsl_matrix_view kmat; /* 1x1 submatrix holding the smoothed k */
  /* scratch regs */
};

/** \brief maintains additional simulation parameters such a k (spring) and 2D
 * force
 *
 * This is used for simulating an experimental flattening force.  And also
 * smoothing the k values yielded from the evolving TreeAdaptors.
 *
 * \struct SBS4
 *
 * \sa springball.h
 */
struct SBS4 {
  struct DRA *subsys;
  int d;
  int twodforce;
  gsl_matrix *smoothk, *targetk; /* ballnum size squares */
  gsl_vector_view *pos, *vel;
  double *statear, *stateback, *statebackj, *foball, *foballbase;
};

gsl_vector *clSbsBallPosition(struct SpringBallSystem *sbs, int whichBall)
{
  return (gsl_vector *) &sbs->sbs4->pos[whichBall];
}

int clFunc(double t, const double uy[], double f[], void *params);

void clSbsChangeTargetTree(struct SpringBallSystem *sbs, struct TreeAdaptor *ta)
{
  if (sbs->sbs4->targetk) {
//    gsl_matrix_free(sbs->sbs4->targetk);
    sbs->sbs4->targetk = NULL;
  }
  sbs->sbs4->targetk = clAdjaToGSLMatrix(clTreeaAdjAdaptor(ta));
}

struct SBS3 *clSbsNew3(int i, int j, gsl_vector_view p1, gsl_vector_view p2, gsl_vector_view v1)
{
  struct SBS3 *ts;
  ts = clCalloc(sizeof(*ts), 1);
  ts->i = i;
  ts->j = j;
  ts->p1 = p1;
  ts->p2 = p2;
  ts->v1 = v1;
  ts->c1 = ts->c2 = BASECHARGE;
  ts->m1 = ts->m2 = BASEMASS;
  ts->restLen = 0.75;
  ts->drag = 0.1;
  return ts;
}

double clipValInbetween(double val, double minval, double maxval)
{
  assert(minval <= maxval);
  if (val > maxval)
    val = maxval;
  if (val < minval)
    val = minval;
  return val;
}

void clStepTowards(gsl_matrix *smooth, const gsl_matrix *target, double dt, double speed) {
  int i, j;
  for (i = 0; i < smooth->size1 ; i += 1) {
    for (j = 0; j < smooth->size2; j += 1) {
      double oldval = gsl_matrix_get(smooth, i, j);
      double wanted = gsl_matrix_get(target, i, j);
      double stepsize = dt * speed;
      double low = oldval - stepsize;
      double high = oldval + stepsize;
      gsl_matrix_set(smooth, i, j, clipValInbetween(wanted, low, high));
    }
  }
}

void clStepTowardsTree(gsl_matrix *smooth, struct TreeAdaptor *ta, double dt) {
  const double springkspeed = 0.1; /* "newtons" per "meter-second" */
  gsl_matrix *targetks = clAdjaToGSLMatrix(clTreeaAdjAdaptor(ta));
  clStepTowards(smooth, targetks, dt, springkspeed);
  gsl_matrix_free(targetks);
}

gsl_matrix *clAdjaToGSLMatrix(struct AdjAdaptor *aa)
{
  gsl_matrix *m;
  int size, i, j;
  size = clAdjaSize(aa);
  m = gsl_matrix_alloc(size,size);
  for (i = 0; i < size ; i += 1) {
    for (j = 0; j < size; j += 1) {
      gsl_matrix_set(m, i, j, (double) clAdjaGetConState(aa,i,j));
    }
  }
  return m;
}

static struct SBS4 *clSbsNew4(struct TreeAdaptor *ta)
{
  int i, j;
  int howBig = clTreeaNodeCount(ta);
  const double posRadius = 3.0;
  struct SBS4 *sbs4 = clCalloc(sizeof(*sbs4), 1);
  int pairnum = howBig * (howBig-1);
  sbs4->d = 3;
  /* initially, there are no springs */
  sbs4->smoothk = gsl_matrix_calloc(howBig, howBig);
  sbs4->targetk = clAdjaToGSLMatrix(clTreeaAdjAdaptor(ta));

  /* views for position and velocity by ball */
  sbs4->pos = clCalloc(sizeof(*sbs4->pos), howBig);
  sbs4->vel = clCalloc(sizeof(*sbs4->vel), howBig);

  sbs4->statear = clCalloc(sizeof(double), 2*sbs4->d*howBig);
  sbs4->stateback = clCalloc(sizeof(double), 2*sbs4->d*howBig);
  sbs4->statebackj = clCalloc(sizeof(double), 2*sbs4->d*howBig);
  sbs4->foball = clCalloc(sizeof(double), 2*sbs4->d*howBig);
  sbs4->foballbase = clCalloc(sizeof(double), 2*sbs4->d*howBig);

  sbs4->subsys = clDraNew();

  for (i = 0; i < howBig; i += 1) {

    /* position, approximately spherical layout with radius posRadius */
    sbs4->statear[6*i+0] = posRadius * cos(i*2.1);
    sbs4->statear[6*i+1] = posRadius * sin(i*2.1);
    sbs4->statear[6*i+2] = posRadius * sin(i*5.13);

    sbs4->pos[i] = gsl_vector_view_array(sbs4->statear + 6*i, 3);

    /* velocity 0 at first */
    sbs4->statear[6*i+3] = 0.0;
    sbs4->statear[6*i+4] = 0.0;
    sbs4->statear[6*i+5] = 0.0;

    sbs4->vel[i] = gsl_vector_view_array(sbs4->statear + 6*i + 3, 3);

  }

  for (i = 0; i < howBig; i += 1) {

    for (j = 0; j < howBig; j += 1) {
      struct SBS3 *sbs3;
      union PCTypes p = zeropct;
      if (i == j)
        continue;
      sbs3 = clSbsNew3(i, j, sbs4->pos[i], sbs4->pos[j], sbs4->vel[i]);
      p.ptr = sbs3;

      sbs3->kmat = gsl_matrix_submatrix(sbs4->smoothk, i, j, 1, 1);

      clDraPush(sbs4->subsys, p);
    }
  }

  pairnum += 0;
  assert(clDraSize(sbs4->subsys != NULL) == pairnum);

  return sbs4;
}

#define X1 gsl_vector_get((gsl_vector *) &model->p1, 0)
#define X2 gsl_vector_get((gsl_vector *) &model->p2, 0)
#define Y1 gsl_vector_get((gsl_vector *) &model->p1, 1)
#define Y2 gsl_vector_get((gsl_vector *) &model->p2, 1)
#define Z1 gsl_vector_get((gsl_vector *) &model->p1, 2)
#define Z2 gsl_vector_get((gsl_vector *) &model->p2, 2)
#define VX1 gsl_vector_get((gsl_vector *) &model->v1, 0)
#define VY1 gsl_vector_get((gsl_vector *) &model->v1, 1)
#define VZ1 gsl_vector_get((gsl_vector *) &model->v1, 2)
#define D (model->drag)
#define K (gsl_matrix_get((gsl_matrix *) &model->kmat, 0, 0))
#define LR (model->restLen)
#define C1 (model->c1)
#define C2 (model->c2)
#define M1 (model->m1)
#define M2 (model->m2)

static void addValueToElement(gsl_matrix *m, int i, int j, double delta)
{
  gsl_matrix_set(m, i, j, gsl_matrix_get(m,i,j)+delta/massScale);
}

void clCalculateJacobian(struct SBS3 *model, gsl_matrix *m)
{
  int mustDoDrag;
  int mustDoCharge = 0;
  int mustDoSpring = 0;
  int obji = model->i;
  int objj = model->j;
  int ffi = 6*obji + 3; // firstforceind
  int fpi = 6*obji;       // firstposind
  int spi = 6*objj;      // secondposind
  int fvi = 6*obji + 3;   // firstvelind
  assert(model != NULL);
  mustDoDrag = (model->j == 0 || (model->i == 0 && model->j == 1));
  if (C1 != 0 && C2 != 0)
    mustDoCharge =1;
  if (K > 0) {
    mustDoSpring = 1;
  }
  massScale = M1;
  if (mustDoCharge) {
    double denom = pow(((X1-X2)*(X1-X2)+(Y1-Y2)*(Y1-Y2)+(Z1-Z2)*(Z1-Z2)), 2.5) / (C1*C2);
    addValueToElement(m,ffi+0,fpi+0,((-2*X1*X1 + 4*X1*X2 - 2*X2*X2+ Y1*Y1 - 2*Y1*Y2 + Y2*Y2 + Z1*Z1 - 2*Z1*Z2 + Z2*Z2) / denom)); //1 dFx/dx1
    addValueToElement(m,ffi+1,fpi+0,((-3*(X1-X2)*(Y1-Y2)) / denom)); //2 dFy/dx1
    addValueToElement(m,ffi+2,fpi+0,((-3*(X1-X2)*(Z1-Z2)) / denom)); //3 dFz/dx1
    addValueToElement(m,ffi+0,fpi+1,((-3*(X1-X2)*(Y1-Y2)) / denom)); //4 dFx/dy1
    addValueToElement(m,ffi+1,fpi+1,((X1*X1 - 2*X1*X2 + X2*X2 - 2*Y1*Y1 + 4*Y1*Y2 - 2*Y2*Y2 + Z1*Z1 - 2*Z1*Z2 + Z2*Z2) / denom)); //5 dFy/dy1
    addValueToElement(m,ffi+2,fpi+1,((-3*(Y1-Y2)*(Z1-Z2)) / denom)); //6 dFz/dy1
    addValueToElement(m,ffi+0,fpi+2,((-3*(X1-X2)*(Z1-Z2)) / denom)); //7 dFx/dz1
    addValueToElement(m,ffi+1,fpi+2,((-3*(Y1-Y2)*(Z1-Z2)) / denom)); //8 dFy/dz1
    addValueToElement(m,ffi+2,fpi+2,((X1*X1 - 2*X1*X2 + X2*X2 + Y1*Y1 - 2*Y1*Y2 + Y2*Y2 -2*Z1*Z1 + 4*Z1*Z2 - 2*Z2*Z2) / denom)); //9 dFz/dz1
    addValueToElement(m,ffi+0,spi+0,(-((-2*X1*X1 + 4*X1*X2 - 2*X2*X2 + Y1*Y1 - 2*Y1*Y2 + Y2*Y2 + Z1*Z1 - 2*Z1*Z2 + Z2*Z2) / denom))); //10 dFx/dx2
    addValueToElement(m,ffi+1,spi+0,((3*(X1-X2)*(Y1-Y2)) / denom)); //11 dFy/dx2
    addValueToElement(m,ffi+2,spi+0,((3*(X1-X2)*(Z1-Z2)) / denom)); //12 dFz/dx2
    addValueToElement(m,ffi+0,spi+1,((3*(X1-X2)*(Y1-Y2)) / denom)); //13 dFx/dy2
    addValueToElement(m,ffi+1,spi+1,(-((X1*X1 - 2*X1*X2 + X2*X2 - 2*Y1*Y1 + 4*Y1*Y2 - 2*Y2*Y2 + Z1*Z1 - 2*Z1*Z2 + Z2*Z2) / denom))); //14 dFy/dy2
    addValueToElement(m,ffi+2,spi+1,((3*(Y1-Y2)*(Z1-Z2)) / denom)); //15 dFz/dy2
    addValueToElement(m,ffi+0,spi+2,((3*(X1-X2)*(Z1-Z2)) / denom)); //16 dFx/dz2
    addValueToElement(m,ffi+1,spi+2,((3*(Y1-Y2)*(Z1-Z2)) / denom)); //17 dFy/dz2
    addValueToElement(m,ffi+2,spi+2,(-((X1*X1 - 2*X1*X2 + X2*X2 + Y1*Y1 - 2*Y1*Y2 + Y2*Y2 - 2*Z1*Z1 + 4*Z1*Z2 - 2*Z2*Z2) / denom))); //18 dFz/dz2
  }
  if(mustDoSpring) {
    double common = sqrt(((X1-X2)*(X1-X2)+(Y1-Y2)*(Y1-Y2)+(Z1-Z2)*(Z1-Z2)));
    if (common < LR)
    addValueToElement(m,ffi+0,fpi+0,(K*(LR - ((X1-X2)*(X1-X2)/common) - common))); //1  dFx/dx1
    addValueToElement(m,ffi+1,fpi+0,(-(K*(X1-X2)*(Y1-Y2)) / common)); //2    dFy/dx1
    addValueToElement(m,ffi+2,fpi+0,(-(K*(X1-X2)*(Z1-Z2)) / common)); //3    dFz/dx1
    addValueToElement(m,ffi+0,fpi+1,(-(K*(X1-X2)*(Y1-Y2)) / common)); //4    dFx/dy1
    addValueToElement(m,ffi+1,fpi+1,(K*(LR - ((Y1-Y2)*(Y1-Y2)/common) - common))); //5  dFy/dy1
    addValueToElement(m,ffi+2,fpi+1,(-(K*(Y1-Y2)*(Z1-Z2)) / common)); //6    dFz/dy1
    addValueToElement(m,ffi+0,fpi+2,(-(K*(X1-X2)*(Z1-Z2)) / common)); //7    dFx/dz1
    addValueToElement(m,ffi+1,fpi+2,(-(K*(Y1-Y2)*(Z1-Z2)) / common)); //8    dFy/dz1
    addValueToElement(m,ffi+2,fpi+2,(K*(LR - common - ((Z1-Z2)*(Z1-Z2)/common)))); //9  dFz/dz1
    addValueToElement(m,ffi+0,spi+0,(K*(((X1-X2)*(X1-X2)/common) + common) - LR)); //10 dFx/dx2
    addValueToElement(m,ffi+1,spi+0,(K*(X1-X2)*(Y1-Y2) / common)); //11      dFy/dx2
    addValueToElement(m,ffi+2,spi+0,(K*(X1-X2)*(Z1-Z2) / common)); //12      dFz/dx2
    addValueToElement(m,ffi+0,spi+1,(K*(X1-X2)*(Y1-Y2) / common)); //13      dFx/dy2
    addValueToElement(m,ffi+1,spi+1,(K*(((Y1-Y2)*(Y1-Y2)/common) + common) - LR)); //14 dFy/dy2
    addValueToElement(m,ffi+2,spi+1,(K*(Y1-Y2)*(Z1-Z2) / common)); //15      dFz/dy2
    addValueToElement(m,ffi+0,spi+2,(K*(X1-X2)*(Z1-Z2) / common)); //16      dFx/dz2
    addValueToElement(m,ffi+1,spi+2,(K*(Y1-Y2)*(Z1-Z2) / common)); //17      dFy/dz2
    addValueToElement(m,ffi+2,spi+2,(K*(((Z1-Z2)*(Z1-Z2)/common) + common) - LR)); //18 dFz/dz2
  }
  if(mustDoDrag) {
    double vellen = sqrt((VX1*VX1)+(VY1*VY1)+(VZ1*VZ1));
    if (vellen > 0) {
      addValueToElement(m,ffi+0,fvi+0,-D*(VX1/vellen));
      addValueToElement(m,ffi+1,fvi+1,-D*(VY1/vellen));
      addValueToElement(m,ffi+2,fvi+2,-D*(VZ1/vellen));
    } else {
      addValueToElement(m,ffi+0,fvi+0,-D);
      addValueToElement(m,ffi+1,fvi+1,-D);
      addValueToElement(m,ffi+2,fvi+2,-D);
    }
    massScale = 1.0;
    addValueToElement(m,fpi+0,fvi+0,1);
    addValueToElement(m,fpi+1,fvi+1,1);
    addValueToElement(m,fpi+2,fvi+2,1);
  }
}
#if 0
static gsl_matrix *clCalculateJacobian(struct SBS3 *model, gsl_matrix *m, int fCharge, int fSpring, int fDrag)
{
  if (fCharge) {
    double denom = pow(((X1-X2)*(X1-X2)+(Y1-Y2)*(Y1-Y2)+(Z1-Z2)*(Z1-Z2)), 2.5);
    addValueToElement(m,0,0,((-2*X1*X1 + 4*X1*X2 - 2*X2*X2+ Y1*Y1 - 2*Y1*Y2 + Y2*Y2 + Z1*Z1 - 2*Z1*Z2 + Z2*Z2) / denom)); //1 dFx/dx1
    addValueToElement(m,0,1,((-3*(X1-X2)*(Y1-Y2)) / denom)); //2 dFy/dx1
    addValueToElement(m,0,2,((-3*(X1-X2)*(Z1-Z2)) / denom)); //3 dFz/dx1
    addValueToElement(m,0,3,((-3*(X1-X2)*(Y1-Y2)) / denom)); //4 dFx/dy1
    addValueToElement(m,0,4,((X1*X1 - 2*X1*X2 + X2*X2 - 2*Y1*Y1 + 4*Y1*Y2 - 2*Y2*Y2 + Z1*Z1 - 2*Z1*Z2 + Z2*Z2) / denom)); //5 dFy/dy1
    addValueToElement(m,0,5,((-3*(Y1-Y2)*(Z1-Z2)) / denom)); //6 dFz/dy1
    addValueToElement(m,0,6,((-3*(X1-X2)*(Z1-Z2)) / denom)); //7 dFx/dz1
    addValueToElement(m,0,7,((-3*(Y1-Y2)*(Z1-Z2)) / denom)); //8 dFy/dz1
    addValueToElement(m,0,8,((X1*X1 - 2*X1*X2 + X2*X2 + Y1*Y1 - 2*Y1*Y2 + Y2*Y2 -2*Z1*Z1 + 4*Z1*Z2 - 2*Z2*Z2) / denom)); //9 dFz/dz1
    addValueToElement(m,0,0,(-((-2*X1*X1 + 4*X1*X2 - 2*X2*X2 + Y1*Y1 - 2*Y1*Y2 + Y2*Y2 + Z1*Z1 - 2*Z1*Z2 + Z2*Z2) / denom))); //10 dFx/dx2
    addValueToElement(m,1,1,((3*(X1-X2)*(Y1-Y2)) / denom)); //11 dFy/dx2
    addValueToElement(m,1,2,((3*(X1-X2)*(Z1-Z2)) / denom)); //12 dFz/dx2
    addValueToElement(m,1,3,((3*(X1-X2)*(Y1-Y2)) / denom)); //13 dFx/dy2
    addValueToElement(m,1,4,(-((X1*X1 - 2*X1*X2 + X2*X2 - 2*Y1*Y1 + 4*Y1*Y2 - 2*Y2*Y2 + Z1*Z1 - 2*Z1*Z2 + Z2*Z2) / denom))); //14 dFy/dy2
    addValueToElement(m,1,5,((3*(Y1-Y2)*(Z1-Z2)) / denom)); //15 dFz/dy2
    addValueToElement(m,1,6,((3*(X1-X2)*(Z1-Z2)) / denom)); //16 dFx/dz2
    addValueToElement(m,1,7,((3*(Y1-Y2)*(Z1-Z2)) / denom)); //17 dFy/dz2
    addValueToElement(m,1,8,(-((X1*X1 - 2*X1*X2 + X2*X2 + Y1*Y1 - 2*Y1*Y2 + Y2*Y2 - 2*Z1*Z1 + 4*Z1*Z2 - 2*Z2*Z2) / denom))); //18 dFz/dz2
  }
  if(fSpring) {
    double common = sqrt(((X1-X2)*(X1-X2)+(Y1-Y2)*(Y1-Y2)+(Z1-Z2)*(Z1-Z2)));
    addValueToElement(m,0,0,(K*(LR - ((X1-X2)*(X1-X2)/common) - common))); //1  dFx/dx1
    addValueToElement(m,0,1,(-(K*(X1-X2)*(Y1-Y2)) / common)); //2    dFy/dx1
    addValueToElement(m,0,2,(-(K*(X1-X2)*(Z1-Z2)) / common)); //3    dFz/dx1
    addValueToElement(m,0,3,(-(K*(X1-X2)*(Y1-Y2)) / common)); //4    dFx/dy1
    addValueToElement(m,0,4,(K*(LR - ((Y1-Y2)*(X1-X2)/common) - common))); //5  dFy/dy1
    addValueToElement(m,0,5,(-(K*(Y1-Y2)*(Z1-Z2)) / common)); //6    dFz/dy1
    addValueToElement(m,0,6,(-(K*(X1-X2)*(Z1-Z2)) / common)); //7    dFx/dz1
    addValueToElement(m,0,7,(-(K*(Y1-Y2)*(Z1-Z2)) / common)); //8    dFy/dz1
    addValueToElement(m,0,8,(K*(LR - common - ((Z1-Z2)*(Z1-Z2)/common)))); //9  dFz/dz1
    addValueToElement(m,1,0,(K*(((X1-X2)*(X1-X2)/common) + common) - LR)); //10 dFx/dx2
    addValueToElement(m,1,1,(K*(X1-X2)*(Y1-Y2) / common)); //11      dFy/dx2
    addValueToElement(m,1,2,(K*(X1-X2)*(Z1-Z2) / common)); //12      dFz/dx2
    addValueToElement(m,1,3,(K*(X1-X2)*(Y1-Y2) / common)); //13      dFx/dy2
    addValueToElement(m,1,4,(K*(((Y1-Y2)*(Y1-Y2)/common) + common) - LR)); //14 dFy/dy2
    addValueToElement(m,1,5,(K*(Y1-Y2)*(Z1-Z2) / common)); //15      dFz/dy2
    addValueToElement(m,1,6,(K*(X1-X2)*(Z1-Z2) / common)); //16      dFx/dz2
    addValueToElement(m,1,7,(K*(Y1-Y2)*(Z1-Z2) / common)); //17      dFy/dz2
    addValueToElement(m,1,8,(K*(((Z1-Z2)*(Z1-Z2)/common) + common) - LR)); //18 dFz/dz2
  }
  if(fDrag) {
    addValueToElement(m,2,0,-(D*sqrt((VX1*VX1)+(VY1*VY1)+(VZ1*VZ1))));//dFx/dvx1
    addValueToElement(m,2,1,-(D*sqrt((VX1*VX1)+(VY1*VY1)+(VZ1*VZ1))));//dFy/dvy1
    addValueToElement(m,2,2,-(D*sqrt((VX1*VX1)+(VY1*VY1)+(VZ1*VZ1))));//dFz/dvz1
  }
  return m;
}
#endif

static void calculateForceOnBall1(struct SBS4 *sbs4, struct SBS3 *model, gsl_vector *result)
{
  double answer[3];
  int i;
  int mustDoDrag;
  int mustDoCharge = 0;
  int mustDoSpring = 0;
  assert(model != NULL);
  mustDoDrag = (model->j == 0);
  for (i = 0; i < 3; i += 1)
    answer[i] = gsl_vector_get(result, i);
  if (C1 != 0 && C2 != 0)
    mustDoCharge =1;
  if (K > 0) {
    mustDoSpring = 1;
  }
  if (mustDoCharge) {
        answer[0] += C1*C2*((X1-X2) / pow(((X1-X2)*(X1-X2) + (Y1-Y2)*(Y1-Y2) + (Z1-Z2)*(Z1-Z2)),1.5));
        answer[1] += C1*C2*((Y1-Y2) / pow(((X1-X2)*(X1-X2) + (Y1-Y2)*(Y1-Y2) + (Z1-Z2)*(Z1-Z2)),1.5));
        answer[2] += C1*C2*((Z1-Z2) / pow(((X1-X2)*(X1-X2) + (Y1-Y2)*(Y1-Y2) + (Z1-Z2)*(Z1-Z2)),1.5));
  }
  if (mustDoSpring) {
    answer[0] += K*(X2-X1)*(sqrt((X1-X2)*(X1-X2)+(Y1-Y2)*(Y1-Y2)+(Z1-Z2)*(Z1-Z2))-LR);
    answer[1] += K*(Y2-Y1)*(sqrt((X1-X2)*(X1-X2)+(Y1-Y2)*(Y1-Y2)+(Z1-Z2)*(Z1-Z2))-LR);
    answer[2] += K*(Z2-Z1)*(sqrt((X1-X2)*(X1-X2)+(Y1-Y2)*(Y1-Y2)+(Z1-Z2)*(Z1-Z2))-LR);
  }
  if (mustDoDrag) {
    answer[0] += -D*VX1*sqrt(VX1*VX1+VY1*VY1+VZ1*VZ1);
    if (sbs4->twodforce)
      answer[0] += -TWODFORCESTR*sqrt(X1*X1)/X1;
    answer[1] += -D*VY1*sqrt(VX1*VX1+VY1*VY1+VZ1*VZ1);
    answer[2] += -D*VZ1*sqrt(VX1*VX1+VY1*VY1+VZ1*VZ1);
  }
  for (i = 0; i < 3; i += 1)
      gsl_vector_set(result, i, answer[i]);
}

void clSbsFree(struct SpringBallSystem *sbs)
{
}

int clAclJac(double t, const double uy[], double *dfdy, double dfdt[], void *params)
{
  struct SBS4 *sbs4 = (struct SBS4 *) params;
  int d = sbs4->d;
  int ballnum = sbs4->smoothk->size1;
  int i, j;
  double smalld = 0.001;
  gsl_matrix_view bigclJac;
  memcpy(sbs4->statebackj, sbs4->statear, (sizeof(double)*2*d*ballnum));
  bigclJac = gsl_matrix_view_array(dfdy, 2*d*ballnum, 2*d*ballnum);
  for (i = 0; i < 2*d*ballnum; i += 1) {
    double yold;
    memcpy(sbs4->statear, uy, (sizeof(double)*2*d*ballnum));
    yold = sbs4->statear[i];
    sbs4->statear[i] -= 0.5*smalld;
    clFunc(t, sbs4->statear, sbs4->foballbase, params);
    sbs4->statear[i] += smalld;
    dfdt[i] = 0;
    clFunc(t, sbs4->statear, sbs4->foball, params);
    sbs4->statear[i] = yold;
    for (j = 0; j < 2*d*ballnum; j += 1) {
/*
      if (i == j) {
#if AJAC_PARITY
        gsl_matrix_set((gsl_matrix *) &bigclJac, i, j, 1.0);
#else
        gsl_matrix_set((gsl_matrix *) &bigclJac, j, i, 1.0);
#endif
        continue;
      }
*/
      double answer = (sbs4->foball[j] - sbs4->foballbase[j]) / smalld;
//      printf("Got clJacobian %f for entry with index %d, %d\n", answer, i, j);
#if AJAC_PARITY
        gsl_matrix_set((gsl_matrix *) &bigclJac, i, j, answer);
#else
        gsl_matrix_set((gsl_matrix *) &bigclJac, j, i, answer);
#endif
    }
  }
  memcpy(sbs4->statear, sbs4->statebackj, (sizeof(double)*2*sbs4->d*ballnum));
  return GSL_SUCCESS;
}

int clJac(double t, const double uy[], double *dfdy, double dfdt[], void *params)
{
  struct SBS4 *sbs4 = (struct SBS4 *) params;
  int i, j;
  int d = sbs4->d;
  int ballnum = sbs4->smoothk->size1;
  memcpy(sbs4->statebackj, sbs4->statear, (sizeof(double)*2*d*ballnum));
  memcpy(sbs4->statear, uy, (sizeof(double)*2*d*ballnum));
  gsl_matrix_view bigclJac;
  bigclJac = gsl_matrix_view_array(dfdy, 2*d*ballnum, 2*d*ballnum);
//  clFunc(t, uy, sbs4->foball, params);
  for (i = 0; i < ballnum; i += 1) {
    for (j = 0; j < d; j += 1) {
      gsl_matrix_set((gsl_matrix *) &bigclJac, 2*i*d + j, 2*i*d+d+j,1);
      gsl_matrix_set((gsl_matrix *) &bigclJac, 2*i*d + d + j, 2*i*d+d+j,1);
      dfdt[2*i*d + j] = 0;
      dfdt[2*i*d + j + d] = 0;
/*      dfdt[2*i*d + j] = uy[2*i*d + d + j];
      dfdt[2*i*d + j + d] = sbs4->foball[2*i*d + d + j]; */
    }
  }
  for (i = 0; i < 2*d*ballnum; i += 1) {
    for (j = 0; j < 2*d*ballnum; j += 1) {
      gsl_matrix_set((gsl_matrix *) &bigclJac, i, j, 0);
    }
  }
  for (i = 0; i < clDraSize(sbs4->subsys); i += 1) {
    struct SBS3 *sbs3 = clDraGetValueAt(sbs4->subsys, i).ptr;
    clCalculateJacobian(sbs3, (gsl_matrix *) &bigclJac); /* TODO: ad sbs4, fix me */
  }
  memcpy(sbs4->statear, sbs4->statebackj, (sizeof(double)*2*sbs4->d*ballnum));
  return GSL_SUCCESS;
}

int clRclJac(double t, const double uy[], double *dfdy, double dfdt[], void *params)
{
  struct SBS4 *sbs4 = (struct SBS4 *) params;
  //double *y = (double *) &uy[0];
  int i;
  int d = sbs4->d;
  int ballnum = sbs4->smoothk->size1;
  static double bigbuf[2][409600];
  static int havedone = 1;
  if (havedone == 0) {
    printf("About to wig out..\n");
    havedone = 1;
    printf("About to wig out..A\n");
    clAclJac(t, uy, &bigbuf[0][0], dfdt, params);
    printf("About to wig out..B\n");
    clJac(t, uy, &bigbuf[1][0], dfdt, params);
    printf("About to wig out..C\n");
    for (i = 0; i < (2*d*ballnum*2*d*ballnum); i += 1) {
      double absdist = fabs(bigbuf[0][i] - bigbuf[1][i]);
      int x = i % (2*d*ballnum);
      int y = (i-x)/(2*d*ballnum);
      printf("%d  ind %d: (%d,%d) [%d,%d]    a %f    j %f     %c (%3.3f)\n",(int) (absdist*1000), i, x, y, x%6, y%6, bigbuf[0][i], bigbuf[1][i], absdist > 1 ? '*' : ' ', absdist);
    }
  }
  return clAclJac(t, uy, dfdy, dfdt, params);
}

int clFunc(double t, const double uy[], double f[], void *params)
{
  struct SBS4 *sbs4 = (struct SBS4 *) params;
  //double *y = (double *) &uy[0];
  int i, j;
  int d = sbs4->d;
  int ballnum = sbs4->smoothk->size1;
  memcpy(sbs4->stateback, sbs4->statear, (sizeof(double)*2*sbs4->d*ballnum));
  memcpy(sbs4->statear, uy, (sizeof(double)*2*sbs4->d*ballnum));
  for (i = 0; i < 2 * d * ballnum; i += 1)
    f[i] = 0.0;
  for (i = 0; i < ballnum; i += 1)
    for (j = 0; j < 3; j += 1)
      f[6*i+j] = uy[6*i+3+j];
  //calculateForceOnBall1(sbs3, 1, f+3);
  for (i = 0; i < clDraSize(sbs4->subsys); i += 1) {
    struct SBS3 *sbs3 = clDraGetValueAt(sbs4->subsys, i).ptr;
    int objball = sbs3->i;
    double *forcetarget = f + (6*objball + 3);
    gsl_vector_view gv = gsl_vector_view_array(forcetarget, 3);
    calculateForceOnBall1(sbs4, sbs3, (gsl_vector *) &gv);
  }
  memcpy(sbs4->statear, sbs4->stateback, (sizeof(double)*2*sbs4->d*ballnum));
  return GSL_SUCCESS;
}

struct SpringBallSystem *clSbsNew(struct TreeAdaptor *ta) {
  struct SpringBallSystem *sbs;
  int howManyNodes = clTreeaNodeCount(ta);
  sbs = clCalloc(sizeof(*sbs), 1);

  sbs->sbs4 = clSbsNew4(clTreeaClone(ta));

  sbs->sys.function = clFunc;
  sbs->sys.jacobian = NULL;
  sbs->sys.dimension = howManyNodes * 2 * sbs->sbs4->d;
  sbs->sys.params = sbs->sbs4;

  sbs->modelSpeed = 1.0;

  sbs->t = sbs->modelSpeed * clDatetimeStaticTimer();

  sbs->T = (gsl_odeiv_step_type *) gsl_odeiv_step_rk4;
  //sbs->T = (gsl_odeiv_step_type *) gsl_odeiv_step_bsimp;
  sbs->s = gsl_odeiv_step_alloc(sbs->T, sbs->sys.dimension);
  sbs->c = gsl_odeiv_control_y_new(1e-1, 0.0);
  sbs->e = gsl_odeiv_evolve_alloc(sbs->sys.dimension);

  sbs->h = 0.1;

  return sbs;
}

void clSbsSetModelSpeed(struct SpringBallSystem *sbs, double modelSpeed)
{
  sbs->modelSpeed = modelSpeed;
  sbs->t = sbs->modelSpeed * clDatetimeStaticTimer();
}

void clPrintSBS(struct SpringBallSystem *sbs)
{
  int i;
  printf("t: %f, %d balls\n", (float) sbs->t, (int) sbs->sbs4->smoothk->size1);
  for (i = 0; i < sbs->sbs4->smoothk->size1; i += 1) {
    printf("%d: %f %f %f\n",i, sbs->sbs4->statear[i*6+0], sbs->sbs4->statear[i*6+1], sbs->sbs4->statear[i*6+2]);
  }
}


void clSbsEvolveForward(struct SpringBallSystem *sbs)
{
  double newt = sbs->modelSpeed * clDatetimeStaticTimer();
  if (newt <= sbs->t)
    return;
  clStepTowards(sbs->sbs4->smoothk, sbs->sbs4->targetk, newt - sbs->t, 0.1);
  while (sbs->t < newt) {
//    printf("t is now %f\n", sbs->t);
//    clPrintSBS(sbs);
    int status = gsl_odeiv_evolve_apply(sbs->e, sbs->c, sbs->s, &sbs->sys, &sbs->t, newt, &sbs->h, sbs->sbs4->statear);
    if (status != GSL_SUCCESS) {
      printf("Error evolving ODE..\n");
      exit(1);
    }
  }
  //updateMaxVal(clo);
}

void clDoSBS3Test(void)
{
  struct TreeAdaptor *ta = clTreeaLoadUnrooted(4);
  struct SpringBallSystem *sbs;
  int i;
  sbs = clSbsNew(ta);
//  printf("Got sbs %p\n", sbs);
  for (i = 0; i < 4; i += 1) {
    clSbsEvolveForward(sbs);
    sleep(1);
  }
}

double clSbsGetSpringSmooth(struct SpringBallSystem *sbs, int i, int j)
{
  return gsl_matrix_get(sbs->sbs4->smoothk, i, j);
}

int clSbsNodeCount(struct SpringBallSystem *sbs)
{
  return sbs->sbs4->smoothk->size1;
}

void clSbsSet2DForce(struct SpringBallSystem *sbs, int newval)
{
  sbs->sbs4->twodforce = newval;
}
