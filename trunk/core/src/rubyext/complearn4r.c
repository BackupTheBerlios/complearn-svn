#include <ruby.h>
#include <complearn/complearn.h>
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

static VALUE secretrbth_new(struct TreeHolder *tomakeruby);

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

struct TreeObserver toom;
static dummySOAP(void)
{
  soap_client_init_args(1,NULL);
}

static gsl_matrix *convertRubyMatrixTogsl_matrix(VALUE rbm)
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

static VALUE DoubleAOfIntsToRubyArray(struct DoubleA *da, unsigned int lev)
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

#define MAXPATHLEN 1024
static VALUE rbadja_path(VALUE self, VALUE vsrc, VALUE vdest)
{
  static int pbuf[MAXPATHLEN];
  int plen;
  int presult;
  struct AdjA *adja;
  volatile VALUE result = Qnil;
  int src, dest;
  result = Qnil;
  Data_Get_Struct(self, struct AdjA, adja);
  src = NUM2INT(vsrc);
  dest = NUM2INT(vdest);
  plen = MAXPATHLEN;
  presult = pathFinder(adja, src, dest, pbuf, &plen);
  if (presult == CL_OK) {
    int i;
    result = rb_ary_new();
    for (i = 0; i < plen; i += 1)
      rb_ary_push(result, INT2NUM(pbuf[i]));
  }
  return result;
}

static VALUE rbadja_size(VALUE self)
{
  struct AdjA *adja;
  Data_Get_Struct(self, struct AdjA, adja);
  return INT2FIX(adja->adjasize(adja));
}

static VALUE rbadja_getconstate(VALUE self, VALUE vi, VALUE vj)
{
  struct AdjA *adja;
  int i, j;
  Data_Get_Struct(self, struct AdjA, adja);
  i = NUM2INT(vi);
  j = NUM2INT(vj);
  return adja->adjagetconstate(adja, i, j)?Qtrue:Qnil;
}

static VALUE rbadja_setconstate(VALUE self, VALUE vi, VALUE vj, VALUE g)
{
  struct AdjA *adja;
  int i, j;
  Data_Get_Struct(self, struct AdjA, adja);
  i = NUM2INT(vi);
  j = NUM2INT(vj);
  adja->adjasetconstate(adja, i, j, (g == INT2FIX(0) || g == Qnil || g == Qfalse) ? 0 : 1);
}

static VALUE rbadja_getneighborcount(VALUE self, VALUE vi)
{
  struct AdjA *adja;
  int i;
  Data_Get_Struct(self, struct AdjA, adja);
  i = NUM2INT(vi);
  return INT2FIX(adja->adjagetneighborcount(adja, i));
}

static VALUE rbadja_spmmap(VALUE self)
{
  struct AdjA *adja;
  struct DoubleA *da = NULL;
  volatile VALUE result;
  Data_Get_Struct(self, struct AdjA, adja);
  if (adja->adjaspmmap)
    da = adja->adjaspmmap(adja);
  if (da) {
    result = DoubleAOfIntsToRubyArray(da, 1);
  } else {
    result = Qnil;
  }
//  freeSPMMap(da);
  return result;
}

static VALUE rbadja_getneighbors(VALUE self, VALUE vwhich)
{
  struct AdjA *adja;
  volatile VALUE result = rb_ary_new();
  int which;
  int nc, i;
  Data_Get_Struct(self, struct AdjA, adja);
  which = NUM2INT(vwhich);
  nc = adja->adjagetneighborcount(adja, which);

  if (nc > 0) {
    int bufsize = sizeof(int) * nc;
    int *nbuf = malloc(bufsize);
    int retval;
    retval = adja->adjagetneighbors(adja,which,nbuf, &bufsize);
    assert(retval == CL_OK);
    for (i = 0; i < nc; i += 1)
      rb_ary_push(result, INT2FIX(nbuf[i]));
    free(nbuf);
  }
  return result;
}

static VALUE rbadja_init(VALUE self)
{
}

static VALUE rbadja_secretnew(VALUE cl, struct AdjA *adja)
{
  assert(adja);
  volatile VALUE tdata = Data_Wrap_Struct(cl, 0, adja->adjafree, adja);
//  volatile VALUE tdata = Data_Wrap_Struct(cl, 0, 0, adja);
  rb_obj_call_init(tdata, 0, 0);
  return tdata;
}

VALUE rbadja_new(VALUE cl, VALUE sz)
{
  struct AdjA *adja = loadAdaptorAL(NUM2INT(sz));
  return rbadja_secretnew(cl, adja);
}

static VALUE rbcompa_shortname(VALUE self)
{
  struct CompAdaptor *ca;
  Data_Get_Struct(self, struct CompAdaptor, ca);
  return rb_str_new2(ca->sn());
}

static VALUE rbcompa_longname(VALUE self)
{
  struct CompAdaptor *ca;
  Data_Get_Struct(self, struct CompAdaptor, ca);
  return rb_str_new2(ca->ln());
}

static VALUE rbcompa_compparam(VALUE self)
{
  struct CompAdaptor *ca;
  Data_Get_Struct(self, struct CompAdaptor, ca);
  return rb_str_new2(ca->cp(ca));
}

static VALUE rbcompa_apiver(VALUE self)
{
  struct CompAdaptor *ca;
  Data_Get_Struct(self, struct CompAdaptor, ca);
  return INT2NUM(ca->apiv());
}

static VALUE rbcompa_compfunc(VALUE self, VALUE str)
{
  struct CompAdaptor *ca;
  struct DataBlock db = convertStringToDataBlock(STR2CSTR(str));
  double result;
  Data_Get_Struct(self, struct CompAdaptor, ca);

  result = ca->cf(ca, db);
  freeDataBlock(db);
  return rb_float_new(result);
}

static VALUE rbcompa_init(VALUE self)
{
}

VALUE rbcompa_new(VALUE cl, VALUE comp)
{
  struct CompAdaptor *ca = loadBuiltinCA(STR2CSTR(comp));
  volatile VALUE tdata = Data_Wrap_Struct(cl, 0, ca->fcf, ca);
//  volatile VALUE tdata = Data_Wrap_Struct(cl, 0, 0, ca);
  rb_obj_call_init(tdata, 0, 0);
  return tdata;
}

static VALUE rbtra_treemutate(VALUE self)
{
  struct TreeAdaptor *ta;
  Data_Get_Struct(self, struct TreeAdaptor, ta);
  treemutateTRA(ta);
  return Qnil;
}

static VALUE rbtra_init(VALUE self)
{
}

static VALUE secretrbtra_new(struct TreeAdaptor *tra)
{
//  volatile VALUE tdata = Data_Wrap_Struct(cTreeAdaptor, 0, 0, tra);
  volatile VALUE tdata = Data_Wrap_Struct(cTreeAdaptor, 0, tra->treefree, tra);
  rb_obj_call_init(tdata, 0, 0);
  return tdata;
}

static VALUE rbtra_new(VALUE cl, VALUE sz)
{
  struct TreeAdaptor *tra = loadNewRootedTRA(NUM2INT(sz));
  return secretrbtra_new(tra);
}

static struct CLNodeSet *convertFromRubyArray(VALUE ar, int maxsz)
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

static VALUE rbtra_diffscore(VALUE self, VALUE vtra2)
{
  struct TreeAdaptor *tra1, *tra2;
  Data_Get_Struct(self, struct TreeAdaptor, tra1);
  Data_Get_Struct(vtra2, struct TreeAdaptor, tra2);
  return rb_float_new(getTreeDifferenceScore(tra1, tra2));
}

static VALUE rbtra_perimpairs(VALUE self, VALUE flips)
{
  volatile VALUE result = Qnil;
  struct TreeAdaptor *ta;
  struct CLNodeSet *clns;
  struct DoubleA *pairs;
  int i;
  clns = convertFromRubyArray(flips, 1024);
  Data_Get_Struct(self, struct TreeAdaptor, ta);
  /* TODO: fix this hard limit */

  pairs = treeperimpairsTRA(ta, clns);

  if (pairs) {
    result = rb_ary_new();
    for (i = 0; i < getSize(pairs); i += 1) {

      volatile VALUE gi = rb_ary_new();

      union pctypes p = getValueAt(pairs, i);

      rb_ary_push(gi, INT2NUM(p.ip.x));
      rb_ary_push(gi, INT2NUM(p.ip.y));

      rb_ary_push(result, gi);
    }
  freeDoubleDoubler(pairs);
  pairs = NULL;
  }
  freeCLNodeSet(clns);

  return result;
}

static VALUE rbtra_mutationcount(VALUE self)
{
  struct TreeAdaptor *ta;
  int n;
  Data_Get_Struct(self, struct TreeAdaptor, ta);
  treemutateTRA(ta);
  return INT2FIX(treegetmutecountTRA(ta));
}

static VALUE rbtra_mutate(VALUE self)
{
  struct TreeAdaptor *ta;
  int n;
  Data_Get_Struct(self, struct TreeAdaptor, ta);
  treemutateTRA(ta);
  return Qnil;
}

static VALUE rbtra_nodetocol(VALUE self, VALUE vn)
{
  struct LabelPerm *lp;
  struct TreeAdaptor *ta;
  int n;
  Data_Get_Struct(self, struct TreeAdaptor, ta);
  n = NUM2INT(vn);
  lp = treegetlabelpermTRA(ta);
  return INT2FIX(getColumnIndexForNodeIDLP(lp, n));
}

static VALUE rbtra_coltonode(VALUE self, VALUE vn)
{
  struct LabelPerm *lp;
  struct TreeAdaptor *ta;
  int n;
  Data_Get_Struct(self, struct TreeAdaptor, ta);
  n = NUM2INT(vn);
  lp = treegetlabelpermTRA(ta);
  return INT2FIX(getNodeIDForColumnIndexLP(lp, n));
}

static VALUE rbtra_isflippable(VALUE self, VALUE vn)
{
  struct TreeAdaptor *ta;
  int n;
  Data_Get_Struct(self, struct TreeAdaptor, ta);
  n = NUM2INT(vn);
  if (treeIsFlippable(ta, n))
    return Qtrue;
  else
    return Qnil;

}

static VALUE rbtra_isquartetable(VALUE self, VALUE vn)
{
  struct TreeAdaptor *ta;
  int n;
  Data_Get_Struct(self, struct TreeAdaptor, ta);
  n = NUM2INT(vn);
  if (treeIsQuartettable(ta, n))
    return Qtrue;
  else
    return Qnil;
}

static VALUE rbtra_getnodes(VALUE self)
{
  struct TreeAdaptor *ta;
  struct AdjA *adja;
  volatile VALUE nodes = rb_ary_new();
  int i;
  Data_Get_Struct(self, struct TreeAdaptor, ta);
  adja = treegetadjaTRA(ta);
  for (i = 0; i < adja->adjasize(adja); i += 1)
    rb_ary_push(nodes, INT2FIX(i));
  return nodes;
}

static VALUE rbtra_getadja(VALUE self)
{
  struct AdjA *adja;
  struct TreeAdaptor *ta;
  Data_Get_Struct(self, struct TreeAdaptor, ta);
  adja = treegetadjaTRA(ta);
  return rbadja_secretnew(cAdjA, adja->adjaclone(adja));
}

static VALUE rbtm_init(VALUE self)
{
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

static VALUE rbtm_examinedcount(VALUE self)
{
  struct TreeMaster *tm;
  Data_Get_Struct(self, struct TreeMaster, tm);
  return INT2NUM(totalTreesExamined(tm));
}

static VALUE rbtm_findtree(VALUE self)
{
  struct TreeMaster *tm;
  struct TreeHolder *th;
  Data_Get_Struct(self, struct TreeMaster, tm);
  th = cloneTreeHolder(findTree(tm));
  return secretrbth_new(th);
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
  if (tos->obs != Qnil)
    rb_funcall(tos->obs, rb_intern("treeSearchStarted"), 0);
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
  volatile VALUE vth = secretrbth_new(th);
  if (tos->obs != Qnil) {
    tos->th = vth;
    rb_funcall(tos->obs, rb_intern("treeImproved"), 1, vth);
    tos->th = Qnil;
  }
}

static void rbtmto_treedone(struct TreeObserver *tob, struct TreeHolder *th)
{
  struct TreeObserverState *tos = (struct TreeObserverState *) tob->ptr;
  volatile VALUE vth = secretrbth_new(th);
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
  tos->obs = obs;
  tos->th = Qnil;
//  rb_gc_mark(tos->obs);
  to->ptr = tos;
  to->treesearchstarted = rbtmto_treesearchstarted;
  to->treeimproved = rbtmto_treeimproved;
  to->treedone = rbtmto_treedone;
  to->treerejected = rbtmto_treerejected;
  //rb_raise(rb_eTypeError, "Error setTreeObserver disabled.");
  setTreeObserver(tm, to);
}

void markTreeMaster(void *ptr)
{
  struct TreeMaster *tm = (struct TreeMaster *) ptr;
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

static VALUE rbth_tree(VALUE tree)
{
  struct TreeHolder *th;
  struct TreeAdaptor *ta;
  Data_Get_Struct(tree, struct TreeHolder, th);
  ta = getCurTree(th);
  if (ta)
    return secretrbtra_new(ta);
  else
    return Qnil;
}

static VALUE rbth_score(VALUE self)
{
  struct TreeHolder *th;
  Data_Get_Struct(self, struct TreeHolder, th);
  return rb_float_new(getCurScore(th));
}

static VALUE rbth_init(VALUE self)
{
}

static VALUE secretrbth_new(struct TreeHolder *tomakeruby)
{
  volatile VALUE tdata;
/* TODO: figure out how come this segfaults with ruby tth.rb when freeTreeHolder is used */
//  tdata= Data_Wrap_Struct(cTreeHolder, 0, freeTreeHolder, tomakeruby);
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
    ta = loadNewUnrootedTRA(gslm->size1);
  else
    ta = loadNewRootedTRA(gslm->size1);
  th = newTreeHolder(gslm,ta);
  scrambleTreeHolder(th);
  return secretrbth_new(th);
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

void Init_complearn4r(void)
{
  mCompLearn = rb_define_module("CompLearn");

  rb_require("matrix");
  cMatrix = rb_const_get(rb_cObject, rb_intern("Matrix"));

  cAdjA = rb_define_class_under(mCompLearn, "AdjA", rb_cObject);
  rb_define_method(cAdjA, "initialize", rbadja_init, 0);
  rb_define_singleton_method(cAdjA, "new", rbadja_new, 1);
  rb_define_method(cAdjA, "initialize", rbadja_init, 0);
  rb_define_method(cAdjA, "getneighbors", rbadja_getneighbors, 1);
  rb_define_method(cAdjA, "getneighborcount", rbadja_getneighborcount, 1);
  rb_define_method(cAdjA, "getconstate", rbadja_getconstate, 2);
  rb_define_method(cAdjA, "setconstate", rbadja_setconstate, 3);
  rb_define_method(cAdjA, "size", rbadja_size, 0);
  rb_define_method(cAdjA, "spmmap", rbadja_spmmap, 0);
  rb_define_method(cAdjA, "path", rbadja_path, 2);
  cCompAdaptor = rb_define_class_under(mCompLearn,"CompAdaptor", rb_cObject);
  rb_define_method(cCompAdaptor, "initialize", rbcompa_init, 0);
  rb_define_singleton_method(cCompAdaptor, "new", rbcompa_new, 1);
  rb_define_method(cCompAdaptor, "compfunc", rbcompa_compfunc, 1);
  rb_define_method(cCompAdaptor, "shortname", rbcompa_shortname, 0);
  rb_define_method(cCompAdaptor, "longname", rbcompa_longname, 0);
  rb_define_method(cCompAdaptor, "apiver", rbcompa_apiver, 0);
  rb_define_method(cCompAdaptor, "compparam", rbcompa_compparam, 0);
  cTreeAdaptor = rb_define_class_under(mCompLearn,"TreeAdaptor", rb_cObject);
  rb_define_singleton_method(cTreeAdaptor, "new", rbtra_new, 1);
  rb_define_method(cTreeAdaptor, "initialize", rbtra_init, 0);
  rb_define_method(cTreeAdaptor, "adja", rbtra_getadja, 0);
  rb_define_method(cTreeAdaptor, "nodes", rbtra_getnodes, 0);
  rb_define_method(cTreeAdaptor, "isQuartetable", rbtra_isquartetable, 1);
  rb_define_method(cTreeAdaptor, "isFlippable", rbtra_isflippable, 1);
  rb_define_method(cTreeAdaptor, "columnToNode", rbtra_coltonode, 1);
  rb_define_method(cTreeAdaptor, "nodeToColumn", rbtra_nodetocol, 1);
  rb_define_method(cTreeAdaptor, "mutate", rbtra_mutate, 0);
  rb_define_method(cTreeAdaptor, "mutationCount", rbtra_mutationcount, 0);
  rb_define_method(cTreeAdaptor, "perimeterPairs", rbtra_perimpairs, 1);
  rb_define_method(cTreeAdaptor, "treeDifferenceScore", rbtra_diffscore, 1);
  cTreeHolder = rb_define_class_under(mCompLearn,"TreeHolder", rb_cObject);
  rb_define_singleton_method(cTreeHolder, "new", rbth_new, 2);
  rb_define_method(cTreeHolder, "initialize", rbth_init, 0);
  rb_define_method(cTreeHolder, "tree", rbth_tree, 0);
  rb_define_method(cTreeHolder, "score", rbth_score, 0);

  cTreeMaster = rb_define_class_under(mCompLearn,"TreeMaster", rb_cObject);

  rb_define_singleton_method(cTreeMaster, "new", rbtm_new, 2);
  rb_define_singleton_method(cTreeMaster, "loadMatrix", rbtm_loadMatrix, 1);

  rb_define_method(cTreeMaster, "initialize", rbtm_init, 0);
  rb_define_method(cTreeMaster, "findTree", rbtm_findtree, 0);
  rb_define_method(cTreeMaster, "k", rbtm_k, 0);
  rb_define_method(cTreeMaster, "labelcount", rbtm_labelcount, 0);
  rb_define_method(cTreeMaster, "nodecount", rbtm_nodecount, 0);
  rb_define_method(cTreeMaster, "examinedcount", rbtm_examinedcount, 0);
  rb_define_method(cTreeMaster, "setTreeObserver", rbtm_settreeobserver, 1);

  cTreeObserver = rb_define_class_under(mCompLearn,"TreeObserver", rb_cObject);
  rb_define_method(cTreeObserver, "treeSearchStarted", rbto_searchstarted, 0);
  rb_define_method(cTreeObserver, "treeImproved", rbto_improved, 1);
  rb_define_method(cTreeObserver, "treeRejected", rbto_rejected, 0);
  rb_define_method(cTreeObserver, "treeDone", rbto_done, 1);

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
}
