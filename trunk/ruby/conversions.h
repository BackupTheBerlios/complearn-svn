#ifndef __CONVERSIONS_H
#define __CONVERSIONS_H

struct CLNodeSet *convertFromRubyArray(VALUE ar, int maxsz);
VALUE DoubleAOfIntsToRubyArray(struct DoubleA *da, unsigned int lev);
gsl_matrix *convertRubyMatrixTogsl_matrix(VALUE rbm);
VALUE secretrbth_new(struct TreeHolder *tomakeruby);
VALUE convertStringStackToRubyArray(struct StringStack *ss);
VALUE convertCLDateTimeToTime(struct CLDateTime *cldt);
VALUE convertgslvectorToRubyVector(gsl_vector *v);
#endif

