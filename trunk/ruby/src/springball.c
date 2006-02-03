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
