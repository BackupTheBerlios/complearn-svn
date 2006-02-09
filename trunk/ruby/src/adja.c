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

#define MAXPATHLEN 1024

VALUE rbadja_secretnew(VALUE cl, struct AdjAdaptor *adja);
static VALUE rbadja_tomatrix(VALUE self);

static VALUE
rbadja_path(VALUE self, VALUE vsrc, VALUE vdest)
{
  static int pbuf[MAXPATHLEN];
  int plen;
  int presult;
  struct AdjAdaptor *adja;
  volatile VALUE result = Qnil;
  int src, dest;
  result = Qnil;
  Data_Get_Struct(self, struct AdjAdaptor, adja);
  src = NUM2INT(vsrc);
  dest = NUM2INT(vdest);
  plen = MAXPATHLEN;
  presult = clPathFinder(adja, src, dest, pbuf, &plen);
  if (presult == CL_OK) {
    int i;
    result = rb_ary_new();
    for (i = 0; i < plen; i += 1)
      rb_ary_push(result, INT2NUM(pbuf[i]));
  }
  return result;
}

static VALUE
rbadja_size(VALUE self)
{
  struct AdjAdaptor *adja;
  Data_Get_Struct(self, struct AdjAdaptor, adja);
  return INT2FIX(clAdjaSize(adja));
}

static VALUE
rbadja_getconstate(VALUE self, VALUE vi, VALUE vj)
{
  struct AdjAdaptor *adja;
  int i, j;
  Data_Get_Struct(self, struct AdjAdaptor, adja);
  i = NUM2INT(vi);
  j = NUM2INT(vj);
  return clAdjaGetConState(adja, i, j)?Qtrue:Qnil;
}

static VALUE
rbadja_setconstate(VALUE self, VALUE vi, VALUE vj, VALUE g)
{
  struct AdjAdaptor *adja;
  int i, j;
  Data_Get_Struct(self, struct AdjAdaptor, adja);
  i = NUM2INT(vi);
  j = NUM2INT(vj);
  clAdjaSetConState(adja, i, j, (g == INT2FIX(0) || g == Qnil || g == Qfalse) ? 0 : 1);
}

static VALUE
rbadja_getneighborcount(VALUE self, VALUE vi)
{
  struct AdjAdaptor *adja;
  int i;
  Data_Get_Struct(self, struct AdjAdaptor, adja);
  i = NUM2INT(vi);
  return INT2FIX(adjaGetNeighborCountAt(adja, i));
}

static VALUE
rbadja_spmmap(VALUE self)
{
  struct AdjAdaptor *adja;
  struct DRA *da = NULL;
  volatile VALUE result;
  Data_Get_Struct(self, struct AdjAdaptor, adja);
  da = clAdjaSPMMap(adja);
  if (da) {
    result = DRAOfIntsToRubyArray(da, 1);
  } else {
    result = Qnil;
  }
//  clFreeSPMMap(da);
  return result;
}

static VALUE
rbadja_getneighbors(VALUE self, VALUE vwhich)
{
  struct AdjAdaptor *adja;
  volatile VALUE result = rb_ary_new();
  int which;
  int nc, i;
  Data_Get_Struct(self, struct AdjAdaptor, adja);
  which = NUM2INT(vwhich);
  nc = clAdjaNeighborCount(adja, which);

  if (nc > 0) {
    int bufsize = sizeof(int) * nc;
    int *nbuf = malloc(bufsize);
    int retval;
    retval = clAdjaNeighbors(adja,which,nbuf, &bufsize);
    assert(retval == CL_OK);
    for (i = 0; i < nc; i += 1)
      rb_ary_push(result, INT2FIX(nbuf[i]));
    free(nbuf);
  }
  return result;
}

static VALUE
rbadja_init(VALUE self)
{
}

VALUE
rbadja_new(VALUE cl, VALUE sz)
{
  struct AdjAdaptor *adja = clAdjaLoadAdjList(NUM2INT(sz));
  return rbadja_secretnew(cl, adja);
}

static VALUE
rbadja_clone(VALUE self)
{
  struct AdjAdaptor *adja;
  Data_Get_Struct(self, struct AdjAdaptor, adja);
  adja = clAdjaClone(adja);
  return rbadja_secretnew(cAdjAdaptor, adja);
}

static VALUE
rbadja_dump(VALUE self, VALUE depth) {
  VALUE obj = rbadja_tomatrix(self);
  return rb_funcall(cMarshal, rb_intern("dump"), 1, obj);
}

static VALUE
rbadja_load(VALUE kl, VALUE mat)
{
  int i, j;
  VALUE self;
  struct AdjAdaptor *adja;
  mat = rb_funcall(cMarshal, rb_intern("load"), 1, mat);
  gsl_matrix *gslm = convertRubyMatrixTogsl_matrix(mat);
  adja = clAdjaLoadAdjList(gslm->size1);
  self = rbadja_secretnew(cAdjAdaptor, adja);
  //adja = rbadja_new((VALUE) cAdjAdaptor, (VALUE) INT2FIX(gslm->size1));
  for (i = 0; i < gslm->size1; i += 1)
    for (j = i + 1; j < gslm->size2; j += 1)
        clAdjaSetConState(adja, i, j, (gsl_matrix_get(gslm, i, j) == 0) ? 0:1);
  gsl_matrix_free(gslm);
  return self;
}

static VALUE
rbadja_tomatrix(VALUE self)
{
  struct AdjAdaptor *adja;
  gsl_matrix *mat;
  VALUE result;
  Data_Get_Struct(self, struct AdjAdaptor, adja);
  mat = clAdjaToGSLMatrix(adja);
  result = convertgslmatrixToRubyMatrix(mat);
  gsl_matrix_free(mat);
  return result;
}

void
doInitAdja(void) {
  cAdjAdaptor = rb_define_class_under(mCompLearn, "AdjAdaptor", rb_cObject);
  rb_define_method(cAdjAdaptor, "initialize", rbadja_init, 0);
  rb_define_singleton_method(cAdjAdaptor, "new", rbadja_new, 1);
  rb_define_method(cAdjAdaptor, "initialize", rbadja_init, 0);
  rb_define_method(cAdjAdaptor, "clone", rbadja_clone, 0);
  rb_define_method(cAdjAdaptor, "getneighbors", rbadja_getneighbors, 1);
  rb_define_method(cAdjAdaptor, "getneighborcount", rbadja_getneighborcount, 1);
  rb_define_method(cAdjAdaptor, "getconstate", rbadja_getconstate, 2);
  rb_define_method(cAdjAdaptor, "setconstate", rbadja_setconstate, 3);
  rb_define_method(cAdjAdaptor, "size", rbadja_size, 0);
  rb_define_method(cAdjAdaptor, "spmmap", rbadja_spmmap, 0);
  rb_define_method(cAdjAdaptor, "path", rbadja_path, 2);
  rb_define_method(cAdjAdaptor, "to_matrix", rbadja_tomatrix, 0);
  rb_define_method(cAdjAdaptor, "_dump", rbadja_dump, 1);
  rb_define_singleton_method(cAdjAdaptor, "_load", rbadja_load, 1);
}

/* used in tree.adja */
VALUE
rbadja_secretnew(VALUE cl, struct AdjAdaptor *adja)
{
  assert(adja);
  volatile VALUE tdata = Data_Wrap_Struct(cl, 0, clAdjaFree, adja);
//  volatile VALUE tdata = Data_Wrap_Struct(cl, 0, 0, adja);
  rb_obj_call_init(tdata, 0, 0);
  return tdata;
}

