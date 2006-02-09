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

static VALUE rbth_tree(VALUE tree)
{
  struct TreeHolder *th;
  struct TreeAdaptor *ta;
  Data_Get_Struct(tree, struct TreeHolder, th);
  ta = clTreehTreeAdaptor(th);
  if (ta)
    return secretrbtra_new(ta);
  else
    return Qnil;
}

static VALUE rbth_score(VALUE self)
{
  struct TreeHolder *th;
  Data_Get_Struct(self, struct TreeHolder, th);
  return rb_float_new(clTreehScore(th));
}

static VALUE rbth_scramble(VALUE self)
{
  struct TreeHolder *th;
  Data_Get_Struct(self, struct TreeHolder, th);
  clTreehScramble(th);
  return Qnil;
}

static VALUE rbth_improve(VALUE self)
{
  struct TreeHolder *th;
  Data_Get_Struct(self, struct TreeHolder, th);
  return clTreehImprove(th) ? Qtrue : Qfalse;
}

static VALUE rbth_failcount(VALUE self)
{
  struct TreeHolder *th;
  Data_Get_Struct(self, struct TreeHolder, th);
  return INT2FIX(clTreehFailCount(th));
}

static VALUE rbth_treecount(VALUE self)
{
  struct TreeHolder *th;
  Data_Get_Struct(self, struct TreeHolder, th);
  return INT2FIX(clTreehTreeCount(th));
}

static VALUE rbth_clone(VALUE self)
{
  struct TreeHolder *th;
  Data_Get_Struct(self, struct TreeHolder, th);
  return secretrbth_new(clTreehClone(th));
}

static VALUE rbth_distmatrix(VALUE self)
{
  struct TreeHolder *th;
  Data_Get_Struct(self, struct TreeHolder, th);
  return convertgslmatrixToRubyMatrix(clTreehDistMatrix(th));
}

static VALUE rbth_init(VALUE self)
{
  if (0) // avoid link errors
    clTreehFree(NULL);
}

VALUE secretrbth_new(struct TreeHolder *tomakeruby)
{
  volatile VALUE tdata;
  tdata= Data_Wrap_Struct(cTreeHolder, 0, clTreehFree, tomakeruby);
  rb_obj_call_init(tdata, 0, 0);
  return tdata;
}

VALUE rbth_new(VALUE cl, VALUE rbm, VALUE isrooted)
{
  struct TreeHolder *th;
  struct TreeAdaptor *ta;
  gsl_matrix *gslm;
  if (cMatrix == rb_class_of(rbm)) {
    gslm = convertRubyMatrixTogsl_matrix(rbm);
  } else {
    printf("About to throw..\n");
    rb_raise(rb_eTypeError, "Error, must have Matrix to make TreeHolder");
    printf("thrown...\n");
  }
  assert(rb_class_of(rbm) == cMatrix);
//  gslm = convertRubyMatrixTogsl_matrix(rbm);
  if (isrooted == Qnil || isrooted == Qfalse)
    ta = clTreeaLoadUnrooted(gslm->size1);
  else
    ta = clTreeaLoadRootedBinary(gslm->size1);
  th = clTreehNew(gslm,ta);
  clTreehScramble(th);
  return secretrbth_new(th);
}

void doInitTH(void) {
  cTreeHolder = rb_define_class_under(mCompLearn,"TreeHolder", rb_cObject);
  rb_define_singleton_method(cTreeHolder, "new", rbth_new, 2);
  rb_define_method(cTreeHolder, "initialize", rbth_init, 0);
  rb_define_method(cTreeHolder, "tree", rbth_tree, 0);
  rb_define_method(cTreeHolder, "score", rbth_score, 0);
  rb_define_method(cTreeHolder, "scramble", rbth_scramble, 0);
  rb_define_method(cTreeHolder, "distMatrix", rbth_distmatrix, 0);
  rb_define_method(cTreeHolder, "improve", rbth_improve, 0);
  rb_define_method(cTreeHolder, "clone", rbth_clone, 0);
  rb_define_method(cTreeHolder, "treeCount", rbth_treecount, 0);
  rb_define_method(cTreeHolder, "failCount", rbth_failcount, 0);
}
