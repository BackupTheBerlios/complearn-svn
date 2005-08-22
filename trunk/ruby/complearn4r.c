#include "clrbcon.h"
#include <gsl/gsl_odeiv.h>
#include <gsl/gsl_vector.h>

VALUE mCompLearn;

VALUE cAdjA;
VALUE cCompAdaptor;
VALUE cTreeAdaptor;
VALUE cTreeHolder;
VALUE cTreeMaster;
VALUE cTreeObserver;
VALUE cTreeMolder;
VALUE cTreeBlaster;
VALUE cTreeOrderObserver;

VALUE cMatrix;

static VALUE convertgslmatrixToRubyMatrix(gsl_matrix *dm)
{
  int i, j;
  volatile VALUE result;
  assert(dm);
//  printf("Trying to convert dm %p\n", dm);
//    printf("With size %d, %d\n", dm->size1, dm->size2);
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
//  printf("Aout to return..\n");
  gsl_matrix_free(dm);
  return result;
}


static dummySOAP(void)
{
  soap_client_init_args(1,NULL);
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

VALUE DoubleAOfIntsToRubyArray(struct DoubleA *da, unsigned int lev)
{
  int i;
  volatile VALUE result = rb_ary_new();
  for (i = 0; i < getSize(da); i += 1) {
    volatile VALUE cur;
    union pctypes p = getValueAt(da, i);
    if (lev > 0) {
      cur = DoubleAOfIntsToRubyArray(p.ar, lev-1);
    }
    else
      cur = INT2FIX(p.i);
    rb_ary_push(result, cur);
  }
  return result;
}

struct CLNodeSet *convertFromRubyArray(VALUE ar, int maxsz)
{
  struct CLNodeSet *clns = newCLNodeSet(maxsz);
  int i;
  if (ar != Qnil) {
    int sz = RARRAY(ar)->len;
//    printf("rb_arysize: %d\n", sz);
    for (i = 0; i < sz; i += 1) {
      volatile VALUE v;
      int ind;
      v = rb_ary_entry(ar, i);
      ind = NUM2INT(v);
      addNodeToSet(clns, ind);
    }
  }
  return clns;
}

struct TreeOrderObserverState {
  volatile VALUE obs;
};

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
    rb_raise(rb_eTypeError, "Error must have kind of TreeObserver in setTreeObserver");
  }
  tos->th = Qnil;
//  rb_gc_mark(tos->obs);
  to->ptr = tos;
  to->treesearchstarted = rbtmto_treesearchstarted;
  // TODO: fix me by remove next 3 comment indicators
  to->treeimproved = rbtmto_treeimproved;
  to->treedone = rbtmto_treedone;
  to->treerejected = rbtmto_treerejected;
  //rb_raise(rb_eTypeError, "Error setTreeObserver disabled.");
  setTreeObserver(tm, to);
}

#if 0
static VALUE rbtmo_tree(VALUE tree)
{
  struct TreeMolder *tmo;
  Data_Get_Struct(tree, struct TreeMolder, tmo);
  return secretrbtra_new(getCurTreeTM(tmo));
}

static VALUE rbtmo_score(VALUE self)
{
  struct TreeMolder *tmo;
  Data_Get_Struct(self, struct TreeMolder, tmo);
  return rb_float_new(getScoreScaledTM(tmo));
}

static VALUE rbtmo_init(VALUE self)
{
}

static VALUE secretrbtmo_new(struct TreeMolder *tomakeruby)
{
  volatile VALUE tdata = Data_Wrap_Struct(cTreeMolder, 0, freeTreeMolder, tomakeruby);
//  volatile VALUE tdata = Data_Wrap_Struct(cTreeMolder, 0, 0, tomakeruby);
  rb_obj_call_init(tdata, 0, 0);
  return tdata;
}

VALUE rbtmo_new(VALUE cl, VALUE rbm, VALUE isrooted)
{
  struct TreeMolder *tmo;
  struct TreeAdaptor *ta;
  gsl_matrix *gslm;
  if (cMatrix == rb_class_of(rbm)) {
    gslm = convertRubyMatrixTogsl_matrix(rbm);
  } else {
    printf("About to throw..\n");
    rb_raise(rb_eTypeError, "Error, must have Matrix to make TreeMolder");
    printf("thrown...\n");
  }
  assert(rb_class_of(rbm) == cMatrix);
//  gslm = convertRubyMatrixTogsl_matrix(rbm);
  if (isrooted == Qnil || isrooted == Qfalse)
    ta = loadNewUnrootedTRA(gslm->size1);
  else
    ta = loadNewRootedTRA(gslm->size1);
  tmo = newTreeMolder(gslm,ta);
  scrambleTreeMolder(tmo);
  return secretrbtmo_new(tmo);
}

static VALUE rbtbl_init(VALUE self)
{
}

static VALUE rbtbl_k(VALUE self)
{
  struct TreeBlaster *tbl;
  Data_Get_Struct(self, struct TreeBlaster, tbl);
  return INT2FIX(getKTB(tbl));
}

static VALUE rbtbl_nodecount(VALUE self)
{
  struct TreeBlaster *tbl;
  Data_Get_Struct(self, struct TreeBlaster, tbl);
  return INT2FIX(getNodeCountTB(tbl));
}

static VALUE rbtbl_labelcount(VALUE self)
{
  struct TreeBlaster *tbl;
  Data_Get_Struct(self, struct TreeBlaster, tbl);
  return INT2FIX(getLabelCountTB(tbl));
}

static VALUE rbtbl_findtreeorder(VALUE self)
{
  struct TreeBlaster *tbl;
  struct CLNodeSet *clns;
  double score;
  volatile VALUE result;
  Data_Get_Struct(self, struct TreeBlaster, tbl);
  clns = findTreeOrder(tbl, &score);
  result = DoubleAOfIntsToRubyArray(CLNodeSetToDoubleA(clns), 0);
  return result;
}
static void rbtmtoo_treeordersearchstarted(struct TreeOrderObserver *tob)
{
  struct TreeOrderObserverState *tos = (struct TreeOrderObserverState *) tob->ptr;
  rb_funcall(tos->obs, rb_intern("treeOrderSearchStarted"), 0);
}

static void rbtmtoo_treeorderrejected(struct TreeOrderObserver *tob)
{
  struct TreeOrderObserverState *tos = (struct TreeOrderObserverState *) tob->ptr;
  rb_funcall(tos->obs, rb_intern("treeRejected"), 0);
}

static void rbtmtoo_treeorderimproved(struct TreeOrderObserver *tob, struct TreeMolder *tmo, struct CLNodeSet *flips)
{
  struct TreeOrderObserverState *tos = (struct TreeOrderObserverState *) tob->ptr;
  rb_funcall(tos->obs, rb_intern("treeOrderImproved"), 1, secretrbtmo_new(tmo));
}

static void rbtmtoo_treeorderdone(struct TreeOrderObserver *tob, struct TreeMolder *tmo, struct CLNodeSet *flips)
{
  struct TreeObserverState *tos = (struct TreeObserverState *) tob->ptr;
  rb_funcall(tos->obs, rb_intern("treeOrderDone"), 1, secretrbtmo_new(tmo));
}

static VALUE rbtbl_settreeorderobserver(VALUE self, VALUE obs)
{
  struct TreeOrderObserver *too;
  struct TreeOrderObserverState *tos;
  struct TreeBlaster *tbl;
  Data_Get_Struct(self, struct TreeBlaster, tbl);
  too = gcalloc(sizeof(*too), 1);
  tos = gcalloc(sizeof(*tos), 1);
  tos->obs = obs;
  rb_gc_mark(tos->obs);
  too->ptr = tos;
  too->treeordersearchstarted = rbtmtoo_treeordersearchstarted;
  too->treeorderimproved = rbtmtoo_treeorderimproved;
  too->treeorderdone = rbtmtoo_treeorderdone;
  setTreeOrderObserver(tbl, too);
}

VALUE rbtbl_new(VALUE cl, VALUE dm, VALUE rtra)
{
  struct TreeBlaster *tbl;
  struct TreeAdaptor *tra;
  gsl_matrix *gdm = NULL;
  volatile VALUE tdata;
  Data_Get_Struct(rtra, struct TreeAdaptor, tra);
  //Check_Type(dm, T_OBJECT);
  if (cMatrix == rb_class_of(dm)) {
    gdm = convertRubyMatrixTogsl_matrix(dm);
  } else {
    printf("About to throw..\n");
    rb_raise(rb_eTypeError, "Error, must have matrix to make TreeMaster!");
    printf("thrown...\n");
  }
  rb_raise(rb_eTypeError, "Not implemented yet.");
  tbl = newTreeBlaster(gdm, tra);
  tdata = Data_Wrap_Struct(cl, 0, freeTreeBlaster, tbl);
//  tdata = Data_Wrap_Struct(cl, 0, 0, tbl);
  rb_obj_call_init(tdata, 0, 0);
  return tdata;
}

static VALUE rbtoo_searchstarted(VALUE self)
{
  return Qnil;
}

static VALUE rbtoo_improved(VALUE self, VALUE improvement)
{
  return Qnil;
}

static VALUE rbtoo_done(VALUE self, VALUE donetm) /* the tree molder is done */
{
  return Qnil;
}
#endif

void Init_complearn4r(void)
{
  mCompLearn = rb_define_module("CompLearn");
  //mCompLearn = rb_const_get(rb_cObject, rb_intern("CompLearn"));

  rb_require("matrix");
  cMatrix = rb_const_get(rb_cObject, rb_intern("Matrix"));

  doInitAdja();
  doInitCompa();
  doInitTRA();
  doInitTH();
  doInitTreeObserver();
  doInitTreeMaster();

  rb_define_method(cTreeMaster, "setTreeObserver", rbtm_settreeobserver, 1);
  rb_define_singleton_method(cTreeMaster, "loadMatrix", rbtm_loadMatrix, 1);

  //cTreeObserver = rb_define_class_under(mCompLearn,"TreeObserver", rb_cObject);
//  cTreeObserver = rb_const_get(mCompLearn, rb_intern("TreeObserver"));
//  rb_define_method(cTreeObserver, "treeSearchStarted", rbto_searchstarted, 0);

#if 0
  cTreeMolder = rb_define_class_under(mCompLearn,"TreeMolder", rb_cObject);
  rb_define_singleton_method(cTreeMolder, "new", rbtmo_new, 2);
  rb_define_method(cTreeMolder, "initialize", rbtmo_init, 0);
  rb_define_method(cTreeMolder, "tree", rbtmo_tree, 0);
  rb_define_method(cTreeMolder, "score", rbtmo_score, 0);

  cTreeBlaster = rb_define_class_under(mCompLearn,"TreeBlaster", rb_cObject);
  rb_define_singleton_method(cTreeBlaster, "new", rbtbl_new, 2);
  rb_define_method(cTreeBlaster, "initialize", rbtbl_init, 0);
  rb_define_method(cTreeBlaster, "findTreeOrder", rbtbl_findtreeorder, 0);
  rb_define_method(cTreeBlaster, "k", rbtbl_k, 0);
  rb_define_method(cTreeBlaster, "nodecount", rbtbl_nodecount, 0);
  rb_define_method(cTreeBlaster, "labelcount", rbtbl_labelcount, 0);
  rb_define_method(cTreeBlaster, "setTreeOrderObserver", rbtbl_settreeorderobserver, 1);

  cTreeOrderObserver = rb_define_class_under(mCompLearn,"TreeOrderObserver", rb_cObject);
  rb_define_method(cTreeOrderObserver, "treeOrderSearchStarted", rbtoo_searchstarted, 0);
  rb_define_method(cTreeOrderObserver, "treeOrderImproved", rbtoo_improved, 1);
  rb_define_method(cTreeOrderObserver, "treeOrderDone", rbtoo_done, 1);
#endif
}
