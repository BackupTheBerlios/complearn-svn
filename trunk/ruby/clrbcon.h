#ifndef __CLRBCON_H
#define __CLRBCON_H

#include <ruby.h>
#include <complearn/complearn.h>

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

struct CLNodeSet *convertFromRubyArray(VALUE ar, int maxsz);
VALUE DoubleAOfIntsToRubyArray(struct DoubleA *da, unsigned int lev);
#endif
