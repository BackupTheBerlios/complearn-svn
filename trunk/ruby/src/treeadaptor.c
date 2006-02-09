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

static VALUE rbtra_treemutate(VALUE self)
{
  struct TreeAdaptor *ta;
  Data_Get_Struct(self, struct TreeAdaptor, ta);
  clTreeaMutate(ta);
  return Qnil;
}

static VALUE rbtra_init(VALUE self)
{
}

VALUE secretrbtra_new(struct TreeAdaptor *tra)
{
//  volatile VALUE tdata = Data_Wrap_Struct(cTreeAdaptor, 0, 0, tra);
  volatile VALUE tdata = Data_Wrap_Struct(cTreeAdaptor, 0, clTreeaFree, tra);
  rb_obj_call_init(tdata, 0, 0);
  return tdata;
}

static VALUE rbtra_new(VALUE cl, VALUE sz)
{
  struct TreeAdaptor *tra = clTreeaLoadUnrooted(NUM2INT(sz));
  return secretrbtra_new(tra);
}

static VALUE rbtra_diffscore(VALUE self, VALUE vtra2)
{
  struct TreeAdaptor *tra1, *tra2;
  Data_Get_Struct(self, struct TreeAdaptor, tra1);
  Data_Get_Struct(vtra2, struct TreeAdaptor, tra2);
  return rb_float_new(clTreeaDifferenceScore(tra1, tra2));
}

static VALUE rbtra_perimpairs(VALUE self, VALUE flips)
{
  volatile VALUE result = Qnil;
  struct TreeAdaptor *ta;
  struct CLNodeSet *clns;
  struct DRA *pairs;
  int i;
  clns = convertFromRubyArray(flips, 1024);
  Data_Get_Struct(self, struct TreeAdaptor, ta);
  /* TODO: fix this hard limit */

  pairs = clTreeaPerimPairs(ta, clns);

  if (pairs) {
    result = rb_ary_new();
    for (i = 0; i < clDraSize(pairs); i += 1) {

      volatile VALUE gi = rb_ary_new();

      union PCTypes p = clDraGetValueAt(pairs, i);

      rb_ary_push(gi, INT2NUM(p.ip.x));
      rb_ary_push(gi, INT2NUM(p.ip.y));

      rb_ary_push(result, gi);
    }
  clDraFree(pairs);
  pairs = NULL;
  }
  clnodesetFree(clns);

  return result;
}

static VALUE rbtra_mutationcount(VALUE self)
{
  struct TreeAdaptor *ta;
  int n;
  Data_Get_Struct(self, struct TreeAdaptor, ta);
  clTreeaMutate(ta);
  return INT2FIX(clTreeaMutationCount(ta));
}

static VALUE rbtra_mutate(VALUE self)
{
  struct TreeAdaptor *ta;
  int n;
  Data_Get_Struct(self, struct TreeAdaptor, ta);
  clTreeaMutate(ta);
  return Qnil;
}

static VALUE rbtra_nodetocol(VALUE self, VALUE vn)
{
  struct LabelPerm *lp;
  struct TreeAdaptor *ta;
  int n;
  Data_Get_Struct(self, struct TreeAdaptor, ta);
  n = NUM2INT(vn);
  lp = clTreeaLabelPerm(ta);
  return INT2FIX(clLabelpermColIndexForNodeID(lp, n));
}

static VALUE rbtra_coltonode(VALUE self, VALUE vn)
{
  struct LabelPerm *lp;
  struct TreeAdaptor *ta;
  int n;
  Data_Get_Struct(self, struct TreeAdaptor, ta);
  n = NUM2INT(vn);
  lp = clTreeaLabelPerm(ta);
  return INT2FIX(clLabelpermNodeIDForColIndex(lp, n));
}

static VALUE rbtra_isflippable(VALUE self, VALUE vn)
{
  struct TreeAdaptor *ta;
  int n;
  Data_Get_Struct(self, struct TreeAdaptor, ta);
  n = NUM2INT(vn);
  return clTreeaIsFlippable(ta, n) ? Qtrue : Qfalse;
}

static VALUE rbtra_isquartetable(VALUE self, VALUE vn)
{
  struct TreeAdaptor *ta;
  int n;
  Data_Get_Struct(self, struct TreeAdaptor, ta);
  n = NUM2INT(vn);
  return clTreeaIsQuartettable(ta, n) ? Qtrue : Qfalse;
}

static VALUE rbtra_getnodes(VALUE self)
{
  struct TreeAdaptor *ta;
  struct AdjAdaptor *adja;
  volatile VALUE nodes = rb_ary_new();
  int i;
  Data_Get_Struct(self, struct TreeAdaptor, ta);
  adja = clTreeaAdjAdaptor(ta);
  for (i = 0; i < clAdjaSize(adja); i += 1)
    rb_ary_push(nodes, INT2FIX(i));
  return nodes;
}

static VALUE rbtra_to_dot(VALUE self)
{
  struct TreeAdaptor *ta;
  struct DataBlock *dotdb;
  volatile VALUE result;
  Data_Get_Struct(self, struct TreeAdaptor, ta);
  dotdb = clConvertTreeToDot(ta, 0.0, NULL, NULL, NULL, NULL, NULL);
//  clDatablockWriteToFile(dotdb, maketreecfg->output_tree_fname);
  result = rb_str_new((char *) clDatablockData(dotdb), clDatablockSize(dotdb));
  clDatablockFreePtr(dotdb);
  return result;
}

static VALUE rbtra_getadja(VALUE self)
{
  struct AdjAdaptor *adja;
  struct TreeAdaptor *ta;
  Data_Get_Struct(self, struct TreeAdaptor, ta);
  adja = clTreeaAdjAdaptor(ta);
  return rbadja_secretnew(cAdjAdaptor, clAdjaClone(adja));
}

static VALUE rbtra_nodecount(VALUE self)
{
  struct TreeAdaptor *ta;
  Data_Get_Struct(self, struct TreeAdaptor, ta);
  return INT2FIX(clTreeaNodeCount(ta));
}

static VALUE rbtra_isroot(VALUE self, VALUE vn)
{
  struct TreeAdaptor *ta;
  int n;
  Data_Get_Struct(self, struct TreeAdaptor, ta);
  n = NUM2INT(vn);
  return clTreeaIsRoot(ta, n) ? Qtrue : Qfalse;
}

static VALUE rbtra_clone(VALUE self)
{
  struct TreeAdaptor *ta;
  Data_Get_Struct(self, struct TreeAdaptor, ta);
  return secretrbtra_new(clTreeaClone(ta));
}

void doInitTRA(void) {
  cTreeAdaptor = rb_define_class_under(mCompLearn,"TreeAdaptor", rb_cObject);
  rb_define_singleton_method(cTreeAdaptor, "new", rbtra_new, 1);
  rb_define_method(cTreeAdaptor, "initialize", rbtra_init, 0);
  rb_define_method(cTreeAdaptor, "adja", rbtra_getadja, 0);
  rb_define_method(cTreeAdaptor, "nodes", rbtra_getnodes, 0);
  rb_define_method(cTreeAdaptor, "isQuartetable", rbtra_isquartetable, 1);
  rb_define_method(cTreeAdaptor, "isFlippable", rbtra_isflippable, 1);
  rb_define_method(cTreeAdaptor, "columnToNode", rbtra_coltonode, 1);
  rb_define_method(cTreeAdaptor, "nodeToColumn", rbtra_nodetocol, 1);
  rb_define_method(cTreeAdaptor, "to_dot", rbtra_to_dot, 0);
  rb_define_method(cTreeAdaptor, "mutate", rbtra_mutate, 0);
  rb_define_method(cTreeAdaptor, "mutationCount", rbtra_mutationcount, 0);
  rb_define_method(cTreeAdaptor, "perimeterPairs", rbtra_perimpairs, 1);
  rb_define_method(cTreeAdaptor, "treeDifferenceScore", rbtra_diffscore, 1);
  rb_define_method(cTreeAdaptor, "nodeCount", rbtra_nodecount, 0);
  rb_define_method(cTreeAdaptor, "isRoot", rbtra_isroot, 1);
  rb_define_method(cTreeAdaptor, "clone", rbtra_clone, 0);
}
