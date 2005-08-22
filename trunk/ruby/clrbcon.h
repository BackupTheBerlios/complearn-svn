#ifndef __CLRBCON_H
#define __CLRBCON_H

#include <ruby.h>
#include <complearn/complearn.h>
#include "conversions.h"

#include <gsl/gsl_odeiv.h>
#include <gsl/gsl_vector.h>

extern VALUE mCompLearn;

extern VALUE cAdjA;
extern VALUE cCompAdaptor;
extern VALUE cTreeAdaptor;
extern VALUE cTreeHolder;
extern VALUE cTreeMaster;
extern VALUE cTreeObserver;
extern VALUE cTreeMolder;
extern VALUE cTreeBlaster;
extern VALUE cTreeOrderObserver;

extern VALUE cMatrix;
extern VALUE cTime;

struct CLNodeSet *convertFromRubyArray(VALUE ar, int maxsz);
VALUE DoubleAOfIntsToRubyArray(struct DoubleA *da, unsigned int lev);
gsl_matrix *convertRubyMatrixTogsl_matrix(VALUE rbm);
VALUE secretrbth_new(struct TreeHolder *tomakeruby);
VALUE convertgslmatrixToRubyMatrix(gsl_matrix *dm);
#endif
