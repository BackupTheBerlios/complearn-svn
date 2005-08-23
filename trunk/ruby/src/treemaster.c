#include "clrbcon.h"

static VALUE rbtm_loadMatrix(VALUE cl, VALUE rfname)
{
  char *fname = STR2CSTR(rfname);
  struct DataBlock db = convertFileToDataBlock(fname);
  struct DataBlock dbdm;
  struct DoubleA *dd;
  volatile VALUE result;
  gsl_matrix *dm;
  dd = load_DataBlock_package(db);
  dbdm = scanForTag(dd, TAGNUM_CLDISTMATRIX);
  dm = loadCLDistMatrix(dbdm, 1);
  assert(dm);
  result = convertgslmatrixToRubyMatrix(dm);
  gsl_matrix_free(dm);
  return result;
}

struct TreeObserverState {
  volatile VALUE obs;
  volatile VALUE th;
};

static void rbtmto_treesearchstarted(struct TreeObserver *tob)
{
  struct TreeObserverState *tos = (struct TreeObserverState *) tob->ptr;
  if (tos->obs != Qnil) {
    if (rb_obj_is_kind_of(tos->obs, cTreeObserver)) {
      rb_funcall(tos->obs, rb_intern("treeSearchStarted"), 0);
    }
    else
      fprintf(stderr, "Some kind of error, tos->obs is invalid.\n");
  }
}

static void rbtmto_treerejected(struct TreeObserver *tob)
{
  struct TreeObserverState *tos = (struct TreeObserverState *) tob->ptr;
  if (tos->obs != Qnil)
    rb_funcall(tos->obs, rb_intern("treeRejected"), 0);
}

static void rbtmto_treeimproved(struct TreeObserver *tob, struct TreeHolder *th)
{
  struct TreeObserverState *tos = (struct TreeObserverState *) tob->ptr;
  volatile VALUE vth = secretrbth_new(cloneTreeHolder(th));
  if (tos->obs != Qnil) {
    tos->th = vth;
    rb_funcall(tos->obs, rb_intern("treeImproved"), 1, vth);
    tos->th = Qnil;
  }
}

static void rbtmto_treedone(struct TreeObserver *tob, struct TreeHolder *th)
{
  struct TreeObserverState *tos = (struct TreeObserverState *) tob->ptr;
  volatile VALUE vth = secretrbth_new(cloneTreeHolder(th));
  if (tos->obs != Qnil) {
    tos->th = vth;
    rb_funcall(tos->obs, rb_intern("treeDone"), 1, vth);
    tos->th = Qnil;
  }
}

static VALUE rbtm_settreeobserver(VALUE self, VALUE obs)
{
  struct TreeObserver *to;
  struct TreeObserverState *tos;
  struct TreeMaster *tm;
  Data_Get_Struct(self, struct TreeMaster, tm);
  to = gcalloc(sizeof(*to), 1);
  tos = gcalloc(sizeof(*tos), 1);
  if (rb_obj_is_kind_of(obs, cTreeObserver))
    tos->obs = obs;
  else {
    tos->obs = Qnil;
    rb_raise(rb_eTypeError, "Error must have kind of TreeObserver in setTreeObserverTM");
  }
  tos->th = Qnil;
//  rb_gc_mark(tos->obs);
  to->ptr = tos;
  to->treesearchstarted = rbtmto_treesearchstarted;
  // TODO: fix me by remove next 3 comment indicators
  to->treeimproved = rbtmto_treeimproved;
  to->treedone = rbtmto_treedone;
  to->treerejected = rbtmto_treerejected;
  //rb_raise(rb_eTypeError, "Error setTreeObserverTM disabled.");
  setTreeObserverTM(tm, to);
}

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
  struct TreeObserver *obs = getTreeObserverTM(tm);
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

static VALUE rbtm_getstarttime(VALUE self)
{
  struct TreeMaster *tm;
  Data_Get_Struct(self, struct TreeMaster, tm);
  return convertCLDateTimeToTime(getStartTimeTM(tm));
}

static VALUE rbtm_getendtime(VALUE self)
{
  struct TreeMaster *tm;
  Data_Get_Struct(self, struct TreeMaster, tm);
  return convertCLDateTimeToTime(getEndTimeTM(tm));
}

static VALUE rbtm_aborttreesearch(VALUE self)
{
  struct TreeMaster *tm;
  Data_Get_Struct(self, struct TreeMaster, tm);
  abortTreeSearchTM(tm);
}

void doInitTreeMaster(void) {
  cTreeMaster = rb_define_class_under(mCompLearn,"TreeMaster", rb_cObject);
  rb_define_method(cTreeMaster, "setTreeObserver", rbtm_settreeobserver, 1);
  rb_define_method(cTreeMaster, "observer=", rbtm_settreeobserver, 1);
  rb_define_singleton_method(cTreeMaster, "loadMatrix", rbtm_loadMatrix, 1);
  rb_define_singleton_method(cTreeMaster, "new", rbtm_new, 2);
  rb_define_method(cTreeMaster, "initialize", rbtm_init, 0);
  rb_define_method(cTreeMaster, "findTree", rbtm_findtree, 0);
  rb_define_method(cTreeMaster, "startTime", rbtm_getstarttime, 0);
  rb_define_method(cTreeMaster, "endTime", rbtm_getendtime, 0);
  rb_define_method(cTreeMaster, "examinedcount", rbtm_examinedcount, 0);
  rb_define_method(cTreeMaster, "k", rbtm_k, 0);
  rb_define_method(cTreeMaster, "labelcount", rbtm_labelcount, 0);
  rb_define_method(cTreeMaster, "size", rbtm_labelcount, 0);
  rb_define_method(cTreeMaster, "abortTreeSearch", rbtm_aborttreesearch, 0);

  cTreeObserver = rb_define_class_under(mCompLearn,"TreeObserver", rb_cObject);
  rb_define_method(cTreeObserver, "treeImproved", rbto_improved, 1);
  rb_define_method(cTreeObserver, "treeRejected", rbto_rejected, 0);
  rb_define_method(cTreeObserver, "treeDone", rbto_done, 1);
}
