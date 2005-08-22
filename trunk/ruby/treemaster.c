#include "clrbcon.h"

static VALUE rbtm_init(VALUE self)
{
}

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

void markTreeMaster(void *ptr)
{
  struct TreeMaster *tm = (struct TreeMaster *) ptr;
#if 0
  struct TreeObserver *obs = getTreeObserver(tm);
  //printf("Marking in TreeMaster...\n");
  if (obs) {
    struct TreeObserverState *tos = (struct TreeObserverState *) obs->ptr;
    if (tos->obs != Qnil) {
      rb_gc_mark(tos->obs);
      if (tos->th != Qnil)
        rb_gc_mark(tos->th);
    }
  }
//  printf("Done.\n");
#endif
}

VALUE rbtm_new(VALUE cl, VALUE dm, VALUE isRooted)
{
  struct TreeMaster *tm;
  gsl_matrix *gdm = NULL;
  int fIsRooted = 1;
  volatile VALUE tdata;
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
  tm = newTreeMaster(gdm, fIsRooted);
  tdata = Data_Wrap_Struct(cl, markTreeMaster, freeTreeMaster, tm);
//  tdata = Data_Wrap_Struct(cl, 0, 0, tm);
  setUserDataTM(tm, (void *) tdata);
  rb_obj_call_init(tdata, 0, 0);
  return tdata;
}

static VALUE rbto_searchstarted(VALUE self)
{
  return Qnil;
}

static VALUE rbto_rejected(VALUE self)
{
  return Qnil;
}

static VALUE rbto_improved(VALUE self, VALUE improvement)
{
  return Qnil;
}

static VALUE rbto_done(VALUE self, VALUE doneth) /* the tree holder is done */
{
  return Qnil;
}

void doInitTreeMaster(void) {
  cTreeMaster = rb_define_class_under(mCompLearn,"TreeMaster", rb_cObject);
  rb_define_singleton_method(cTreeMaster, "new", rbtm_new, 2);
  rb_define_method(cTreeMaster, "initialize", rbtm_init, 0);
  rb_define_method(cTreeMaster, "findTree", rbtm_findtree, 0);
  rb_define_method(cTreeMaster, "examinedcount", rbtm_examinedcount, 0);
  rb_define_method(cTreeMaster, "k", rbtm_k, 0);
  rb_define_method(cTreeMaster, "labelcount", rbtm_labelcount, 0);
  rb_define_method(cTreeMaster, "size", rbtm_labelcount, 0);
  rb_define_method(cTreeMaster, "nodecount", rbtm_nodecount, 0);

  cTreeObserver = rb_define_class_under(mCompLearn,"TreeObserver", rb_cObject);
  rb_define_method(cTreeObserver, "treeImproved", rbto_improved, 1);
  rb_define_method(cTreeObserver, "treeRejected", rbto_rejected, 0);
  rb_define_method(cTreeObserver, "treeDone", rbto_done, 1);
}
