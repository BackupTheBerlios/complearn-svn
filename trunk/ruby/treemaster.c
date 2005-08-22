#include "clrbcon.h"

static VALUE rbtm_findtree(VALUE self)
{
  struct TreeMaster *tm;
  struct TreeHolder *th;
  Data_Get_Struct(self, struct TreeMaster, tm);
  th = cloneTreeHolder(findTree(tm));
  return secretrbth_new(th);
}

static VALUE rbtm_examinedcount(VALUE self)
{
  struct TreeMaster *tm;
  Data_Get_Struct(self, struct TreeMaster, tm);
  return INT2NUM(totalTreesExamined(tm));
}

static VALUE rbtm_k(VALUE self)
{
  struct TreeMaster *tm;
  Data_Get_Struct(self, struct TreeMaster, tm);
  return INT2FIX(getKTM(tm));
}

static VALUE rbtm_nodecount(VALUE self)
{
  struct TreeMaster *tm;
  Data_Get_Struct(self, struct TreeMaster, tm);
  return INT2FIX(getNodeCountTM(tm));
}

static VALUE rbtm_labelcount(VALUE self)
{
  struct TreeMaster *tm;
  Data_Get_Struct(self, struct TreeMaster, tm);
  return INT2FIX(getLabelCountTM(tm));
}

void doInitTreeMaster(void) {
  cTreeMaster = rb_define_class_under(mCompLearn,"TreeMaster", rb_cObject);
  rb_define_method(cTreeMaster, "findTree", rbtm_findtree, 0);
  rb_define_method(cTreeMaster, "examinedcount", rbtm_examinedcount, 0);
  rb_define_method(cTreeMaster, "k", rbtm_k, 0);
  rb_define_method(cTreeMaster, "labelcount", rbtm_labelcount, 0);
  rb_define_method(cTreeMaster, "nodecount", rbtm_nodecount, 0);
}
