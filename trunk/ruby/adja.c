#include "clrbcon.h"

#define MAXPATHLEN 1024

VALUE rbadja_secretnew(VALUE cl, struct AdjA *adja);

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

VALUE rbadja_new(VALUE cl, VALUE sz)
{
  struct AdjA *adja = loadAdaptorAL(NUM2INT(sz));
  return rbadja_secretnew(cl, adja);
}

void doInitAdja(void) {
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
}

/* used in tree.adja */
VALUE rbadja_secretnew(VALUE cl, struct AdjA *adja)
{
  assert(adja);
  volatile VALUE tdata = Data_Wrap_Struct(cl, 0, adja->adjafree, adja);
//  volatile VALUE tdata = Data_Wrap_Struct(cl, 0, 0, adja);
  rb_obj_call_init(tdata, 0, 0);
  return tdata;
}

