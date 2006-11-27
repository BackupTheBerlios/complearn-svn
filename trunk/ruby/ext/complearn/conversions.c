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
#include "clrbcon.h"

VALUE convertCLDateTimeToTime(struct CLDateTime *cldt)
{
  if (cldt) {
    unsigned long i = clDatetimeToInt(cldt);
    return rb_funcall(cTime, rb_intern("at"), 1, UINT2NUM(i));
  }
  else
    return Qnil;
}

VALUE convertgslvectorToRubyVector(gsl_vector *v)
{
  VALUE result;
  VALUE *coords;
  int i, size;
  size = v->size;
  coords = clCalloc(size, sizeof(VALUE));
  for (i = 0; i < v->size; i += 1)
    coords[i] = rb_float_new(gsl_vector_get(v, i));
  result = rb_funcall3(cVector, rb_intern("[]"), size, coords);
  clFreeandclear(coords);
  return result;
}

VALUE convertgslmatrixToRubyMatrix(gsl_matrix *dm)
{
  int i, j;
  volatile VALUE result;
  assert(dm);
  volatile VALUE rows = rb_ary_new();
  for (i = 0; i < dm->size1; i += 1) {
    volatile VALUE currow = rb_ary_new();
    rb_ary_push(rows, currow);
    for (j = 0; j < dm->size2; j += 1) {
      double val = gsl_matrix_get(dm, i, j);
      volatile VALUE rval = rb_float_new(val);
      rb_ary_push(currow, rval);
    }
  }
  result = rb_funcall(cMatrix, rb_intern("rows"), 2, rows, Qnil);
  return result;
}

gsl_matrix *convertRubyMatrixTogsl_matrix(VALUE rbm)
{
  gsl_matrix *gslm;
  int i, j;
  int size1, size2;
  size1 = NUM2INT(rb_funcall(rbm, rb_intern("row_size"), 0));
  size2 = NUM2INT(rb_funcall(rbm, rb_intern("column_size"), 0));
  assert(size1==size2);
  gslm = gsl_matrix_alloc(size1, size2);
  for (i = 0; i < size1 ; i += 1) {
    for (j = 0; j < size2 ; j += 1) {
      double value = NUM2DBL(rb_funcall(rbm, rb_intern("[]"), 2, INT2FIX(i), INT2FIX(j)));
      gsl_matrix_set(gslm, i, j, value);
    }
  }
  return gslm;
}

VALUE DRAOfIntsToRubyArray(struct DRA *da, unsigned int lev)
{
  int i;
  volatile VALUE result = rb_ary_new();
  for (i = 0; i < clDraSize(da); i += 1) {
    volatile VALUE cur;
    union PCTypes p = clDraGetValueAt(da, i);
    if (lev > 0) {
      cur = DRAOfIntsToRubyArray(p.ar, lev-1);
    }
    else
      cur = INT2FIX(p.i);
    rb_ary_push(result, cur);
  }
  return result;
}

struct CLNodeSet *convertFromRubyArray(VALUE ar, int maxsz)
{
  struct CLNodeSet *clns = clNodesetNew(maxsz);
  int i;
  if (ar != Qnil) {
    int sz = RARRAY(ar)->len;
//    printf("rb_arysize: %d\n", sz);
    for (i = 0; i < sz; i += 1) {
      volatile VALUE v;
      int ind;
      v = rb_ary_entry(ar, i);
      ind = NUM2INT(v);
      clNodesetAddNode(clns, ind);
    }
  }
  return clns;
}

VALUE convertStringStackToRubyArray(struct StringStack *ss)
{
  VALUE result = rb_ary_new();
  int i;
  for (i = 0; i < clStringstackSize(ss); i += 1)
    rb_ary_push(result, rb_str_new2(clStringstackReadAt(ss, i)));
  return result;
}

VALUE convertDataBlockToRubyString(struct DataBlock *db)
{
  return rb_str_new((char *) clDatablockData(db), clDatablockSize(db));
}

struct DataBlock *convertRubyStringToDataBlock(VALUE rstr)
{
  long length;
  struct DataBlock *db;
  char *cstr = rb_str2cstr(rstr, &length);
  db = clDatablockNewFromBlock(cstr, length);
  return db;
}
