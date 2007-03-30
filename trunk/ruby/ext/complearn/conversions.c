
/*
 * This file is part of the libcomplearn-ruby project.
 *
 * Copyright (C) 2006-2007 Rudi Cilibrasi <cilibrar@cilibrar.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
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
