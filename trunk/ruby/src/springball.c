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

static VALUE rbsbs_retarget(VALUE self, VALUE tree)
{
  struct SpringBallSystem *sbs;
  struct TreeAdaptor *ta;
  Data_Get_Struct(tree, struct TreeAdaptor, ta);
  Data_Get_Struct(self, struct SpringBallSystem, sbs);
  clSbsChangeTargetTree(sbs, ta);
  return Qnil;
}

static VALUE rbsbs_springsmooth(VALUE self, VALUE ri, VALUE rj)
{
  struct SpringBallSystem *sbs;
  int i= NUM2INT(ri), j = NUM2INT(rj);
  Data_Get_Struct(self, struct SpringBallSystem, sbs);
  return rb_float_new(clSbsGetSpringSmooth(sbs, i, j));
}

static VALUE rbsbs_pos(VALUE self, VALUE which)
{
  struct SpringBallSystem *sbs;
  int ind = NUM2INT(which);
  Data_Get_Struct(self, struct SpringBallSystem, sbs);
  return convertgslvectorToRubyVector(clSbsBallPosition(sbs, ind));
}

static VALUE rbsbs_evolve(VALUE self)
{
  struct SpringBallSystem *sbs;
  Data_Get_Struct(self, struct SpringBallSystem, sbs);
  clSbsEvolveForward(sbs);
  return Qnil;
}

static VALUE rbsbs_size(VALUE self)
{
  struct SpringBallSystem *sbs;
  Data_Get_Struct(self, struct SpringBallSystem, sbs);
  return INT2FIX(clSbsNodeCount(sbs));
}

static VALUE rbsbs_init(VALUE self)
{
}

VALUE rbsbs_new(VALUE cl, VALUE tree, VALUE isRooted)
{
  struct SpringBallSystem *sbs;
  struct TreeAdaptor *ta;
  volatile VALUE tdata;
  Data_Get_Struct(tree, struct TreeAdaptor, ta);
  sbs = clSbsNew(ta);
  clSbsSetModelSpeed(sbs, 5);
  tdata = Data_Wrap_Struct(cl, NULL, NULL, sbs);
  rb_obj_call_init(tdata, 0, 0);
  return tdata;
}

#if 0
//  int fIsRooted = 1;
  //Check_Type(dm, T_OBJECT);
  if (cMatrix == rb_class_of(dm)) {
    gdm = convertRubyMatrixTogsl_matrix(dm);
  } else {
    printf("About to throw..\n");
    rb_raise(rb_eTypeError, "Error, must have matrix to make TreeMaster!");
    printf("thrown...\n");
  }
  if (isRooted == Qnil || isRooted == Qfalse)
    fIsRooted = 0;
  tm = clTreemasterNew(gdm, fIsRooted);
  tdata = Data_Wrap_Struct(cl, markTreeMaster, clTreemasterFree, tm);
//  tdata = Data_Wrap_Struct(cl, 0, 0, tm);
  clTreemasterSetUserData(tm, (void *) tdata);
  rb_obj_call_init(tdata, 0, 0);
  return tdata;
}
#endif

void doInitSpringBallSystem(void) {
  cSpringBallSystem = rb_define_class_under(mCompLearn,"SpringBallSystem", rb_cObject);
  rb_define_singleton_method(cSpringBallSystem, "new", rbsbs_new, 1);
  rb_define_method(cSpringBallSystem, "initialize", rbsbs_init, 0);
  rb_define_method(cSpringBallSystem, "size", rbsbs_size, 0);
  rb_define_method(cSpringBallSystem, "evolve", rbsbs_evolve, 0);
  rb_define_method(cSpringBallSystem, "[]", rbsbs_pos, 1);
  rb_define_method(cSpringBallSystem, "pos", rbsbs_pos, 1);
  rb_define_method(cSpringBallSystem, "clSbsBallPosition", rbsbs_pos, 1);
  rb_define_method(cSpringBallSystem, "retarget", rbsbs_retarget, 1);
  rb_define_method(cSpringBallSystem, "springSmooth", rbsbs_springsmooth, 2);
}
