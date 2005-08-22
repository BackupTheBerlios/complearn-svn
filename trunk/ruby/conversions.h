#ifndef __CONVERSIONS_H
#define __CONVERSIONS_H

VALUE convertgslmatrixToRubyMatrix(gsl_matrix *dm);
gsl_matrix *convertRubyMatrixTogsl_matrix(VALUE rbm);
VALUE DoubleAOfIntsToRubyArray(struct DoubleA *da, unsigned int lev);
VALUE convertStringStackToRubyArray(struct StringStack *ss);
struct CLNodeSet *convertFromRubyArray(VALUE ar, int maxsz);

#endif

