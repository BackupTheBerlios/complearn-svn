#include "clrbcon.h"

#define MAXPATHLEN 1024

VALUE rbadja_secretnew(VALUE cl, struct AdjAdaptor *adja);

static VALUE rbadja_path(VALUE self, VALUE vsrc, VALUE vdest)
{
  static int pbuf[MAXPATHLEN];
  int plen;
  int presult;
  struct AdjAdaptor *adja;
  volatile VALUE result = Qnil;
  int src, dest;
  result = Qnil;
  Data_Get_Struct(self, struct AdjAdaptor, adja);
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
  struct AdjAdaptor *adja;
  Data_Get_Struct(self, struct AdjAdaptor, adja);
  return INT2FIX(adja->adjasize(adja));
}

static VALUE rbadja_getconstate(VALUE self, VALUE vi, VALUE vj)
{
  struct AdjAdaptor *adja;
  int i, j;
  Data_Get_Struct(self, struct AdjAdaptor, adja);
  i = NUM2INT(vi);
  j = NUM2INT(vj);
  return adjaGetConState(adja, i, j)?Qtrue:Qnil;
}

static VALUE rbadja_setconstate(VALUE self, VALUE vi, VALUE vj, VALUE g)
{
  struct AdjAdaptor *adja;
  int i, j;
  Data_Get_Struct(self, struct AdjAdaptor, adja);
  i = NUM2INT(vi);
  j = NUM2INT(vj);
  adjaSetConState(adja, i, j, (g == INT2FIX(0) || g == Qnil || g == Qfalse) ? 0 : 1);
}

static VALUE rbadja_getneighborcount(VALUE self, VALUE vi)
{
  struct AdjAdaptor *adja;
  int i;
  Data_Get_Struct(self, struct AdjAdaptor, adja);
  i = NUM2INT(vi);
  return INT2FIX(adja->adjagetneighborcount(adja, i));
}

static VALUE rbadja_spmmap(VALUE self)
{
  struct AdjAdaptor *adja;
  struct DoubleA *da = NULL;
  volatile VALUE result;
  Data_Get_Struct(self, struct AdjAdaptor, adja);
  if (adja->adjaspmmap)
    da = adjaSPMMap(adja);
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
  struct AdjAdaptor *adja;
  volatile VALUE result = rb_ary_new();
  int which;
  int nc, i;
  Data_Get_Struct(self, struct AdjAdaptor, adja);
  which = NUM2INT(vwhich);
  nc = adjaNeighborCount(adja, which);

  if (nc > 0) {
    int bufsize = sizeof(int) * nc;
    int *nbuf = malloc(bufsize);
    int retval;
    retval = adjaNeighbors(adja,which,nbuf, &bufsize);
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

VALUE rbadja_new(VALUE cl, VALUE sz)
{
  struct AdjAdaptor *adja = adjaLoadAdjList(NUM2INT(sz));
  return rbadja_secretnew(cl, adja);
}

static VALUE rbadja_clone(VALUE self)
{
  struct AdjAdaptor *adja;
  Data_Get_Struct(self, struct AdjAdaptor, adja);
  adja = adjaClone(adja);
  return rbadja_secretnew(cAdjAdaptor, adja);
}

static VALUE rbadja_tomatrix(VALUE self)
{
  struct AdjAdaptor *adja;
  gsl_matrix *mat;
  VALUE result;
  Data_Get_Struct(self, struct AdjAdaptor, adja);
  mat = convertAdjAdaptorToGSLMatrix(adja);
  result = convertgslmatrixToRubyMatrix(mat);
  gsl_matrix_free(mat);
  return result;
}

void doInitAdja(void) {
  cAdjAdaptor = rb_define_class_under(mCompLearn, "AdjAdaptor", rb_cObject);
  rb_define_method(cAdjAdaptor, "initialize", rbadja_init, 0);
  rb_define_singleton_method(cAdjAdaptor, "new", rbadja_new, 1);
  rb_define_method(cAdjAdaptor, "initialize", rbadja_init, 0);
  rb_define_method(cAdjAdaptor, "clone", rbadja_clone, 0);
  rb_define_method(cAdjAdaptor, "getneighbors", rbadja_getneighbors, 1);
  rb_define_method(cAdjAdaptor, "getneighborcount", rbadja_getneighborcount, 1);
  rb_define_method(cAdjAdaptor, "getconstate", rbadja_getconstate, 2);
  rb_define_method(cAdjAdaptor, "setconstate", rbadja_setconstate, 3);
  rb_define_method(cAdjAdaptor, "size", rbadja_size, 0);
  rb_define_method(cAdjAdaptor, "spmmap", rbadja_spmmap, 0);
  rb_define_method(cAdjAdaptor, "path", rbadja_path, 2);
  rb_define_method(cAdjAdaptor, "to_matrix", rbadja_tomatrix, 0);
}

/* used in tree.adja */
VALUE rbadja_secretnew(VALUE cl, struct AdjAdaptor *adja)
{
  assert(adja);
  volatile VALUE tdata = Data_Wrap_Struct(cl, 0, adja->adjafree, adja);
//  volatile VALUE tdata = Data_Wrap_Struct(cl, 0, 0, adja);
  rb_obj_call_init(tdata, 0, 0);
  return tdata;
}

