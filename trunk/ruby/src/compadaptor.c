#include "clrbcon.h"

static VALUE rbcompa_shortname(VALUE self)
{
  struct CompAdaptor *ca;
  Data_Get_Struct(self, struct CompAdaptor, ca);
  return rb_str_new2(shortNameCA(ca));
}

static VALUE rbcompa_longname(VALUE self)
{
  struct CompAdaptor *ca;
  Data_Get_Struct(self, struct CompAdaptor, ca);
  return rb_str_new2(longNameCA(ca));
}

static VALUE rbcompa_apiver(VALUE self)
{
  struct CompAdaptor *ca;
  Data_Get_Struct(self, struct CompAdaptor, ca);
  return INT2NUM(apiverCA(ca));
}

static VALUE rbcompa_compfunc(VALUE self, VALUE str)
{
  struct CompAdaptor *ca;
  struct DataBlock db = convertStringToDataBlock(STR2CSTR(str));
  double result;
  Data_Get_Struct(self, struct CompAdaptor, ca);

  result = compfuncCA(ca, db);
  freeDataBlock(db);
  return rb_float_new(result);
}

static VALUE rbcompa_ncd(VALUE self, VALUE stra, VALUE strb)
{
  struct CompAdaptor *ca;
  double result;
  struct DataBlock dba = convertStringToDataBlock(STR2CSTR(stra));
  struct DataBlock dbb = convertStringToDataBlock(STR2CSTR(strb));

  Data_Get_Struct(self, struct CompAdaptor, ca);
  result = ncdCA(ca, dba, dbb);
  freeDataBlock(dba);
  freeDataBlock(dbb);
  return rb_float_new(result);
}

static VALUE rbcompa_names(VALUE kl)
{
  VALUE result;
  struct StringStack *ss = compaListBuiltin();
  result = convertStringStackToRubyArray(ss);
  freeSS(ss);
  return result;
}

static VALUE rbcompa_init(VALUE self)
{
}

VALUE rbcompa_new(VALUE cl, VALUE comp)
{
  struct CompAdaptor *ca = compaLoadBuiltin(STR2CSTR(comp));
  volatile VALUE tdata = Data_Wrap_Struct(cl, 0, freeCA, ca);
//  volatile VALUE tdata = Data_Wrap_Struct(cl, 0, 0, ca);
  rb_obj_call_init(tdata, 0, 0);
  return tdata;
}

void doInitCompa(void) {
  cCompAdaptor = rb_define_class_under(mCompLearn,"CompAdaptor", rb_cObject);
  rb_define_method(cCompAdaptor, "initialize", rbcompa_init, 0);
  rb_define_singleton_method(cCompAdaptor, "new", rbcompa_new, 1);
  rb_define_singleton_method(cCompAdaptor, "loadBuiltin", rbcompa_new, 1);
  rb_define_singleton_method(cCompAdaptor, "names", rbcompa_names, 0);
  rb_define_singleton_method(cCompAdaptor, "listBuiltin", rbcompa_names, 0);
  rb_define_method(cCompAdaptor, "compfunc", rbcompa_compfunc, 1);
  rb_define_method(cCompAdaptor, "shortname", rbcompa_shortname, 0);
  rb_define_method(cCompAdaptor, "longname", rbcompa_longname, 0);
  rb_define_method(cCompAdaptor, "apiver", rbcompa_apiver, 0);
  rb_define_method(cCompAdaptor, "ncd", rbcompa_ncd, 2);
}
