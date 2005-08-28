#include "clrbcon.h"


static VALUE rbth_tree(VALUE tree)
{
  struct TreeHolder *th;
  struct TreeAdaptor *ta;
  Data_Get_Struct(tree, struct TreeHolder, th);
  ta = treehTreeAdaptor(th);
  if (ta)
    return secretrbtra_new(ta);
  else
    return Qnil;
}

static VALUE rbth_score(VALUE self)
{
  struct TreeHolder *th;
  Data_Get_Struct(self, struct TreeHolder, th);
  return rb_float_new(treehScore(th));
}

static VALUE rbth_scramble(VALUE self)
{
  struct TreeHolder *th;
  Data_Get_Struct(self, struct TreeHolder, th);
  treehScramble(th);
  return Qnil;
}

static VALUE rbth_improve(VALUE self)
{
  struct TreeHolder *th;
  Data_Get_Struct(self, struct TreeHolder, th);
  return treehImprove(th) ? Qtrue : Qfalse;
}

static VALUE rbth_failcount(VALUE self)
{
  struct TreeHolder *th;
  Data_Get_Struct(self, struct TreeHolder, th);
  return INT2FIX(treehFailCount(th));
}

static VALUE rbth_treecount(VALUE self)
{
  struct TreeHolder *th;
  Data_Get_Struct(self, struct TreeHolder, th);
  return INT2FIX(treehTreeCount(th));
}

static VALUE rbth_clone(VALUE self)
{
  struct TreeHolder *th;
  Data_Get_Struct(self, struct TreeHolder, th);
  return secretrbth_new(treehClone(th));
}

static VALUE rbth_distmatrix(VALUE self)
{
  struct TreeHolder *th;
  Data_Get_Struct(self, struct TreeHolder, th);
  return convertgslmatrixToRubyMatrix(treehDistMatrix(th));
}

static VALUE rbth_init(VALUE self)
{
}

VALUE secretrbth_new(struct TreeHolder *tomakeruby)
{
  volatile VALUE tdata;
/* TODO: figure out how come this segfaults with ruby tth.rb when treehFree is used */
//  TODO: Ara Howard: can you help us?
//  tdata= Data_Wrap_Struct(cTreeHolder, 0, treehFree, tomakeruby);
  tdata = Data_Wrap_Struct(cTreeHolder, 0, 0, tomakeruby);
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
    ta = treeaLoadUnrooted(gslm->size1);
  else
    ta = treeaLoadRootedBinary(gslm->size1);
  th = treehNew(gslm,ta);
  treehScramble(th);
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
