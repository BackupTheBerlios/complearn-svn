#include "clrbcon.h"

static VALUE rbcompa_shortname(VALUE self)
{
  struct CompAdaptor *ca;
  Data_Get_Struct(self, struct CompAdaptor, ca);
  return rb_str_new2(compaShortName(ca));
}

static VALUE rbcompa_longname(VALUE self)
{
  struct CompAdaptor *ca;
  Data_Get_Struct(self, struct CompAdaptor, ca);
  return rb_str_new2(compaLongName(ca));
}

static VALUE rbcompa_apiver(VALUE self)
{
  struct CompAdaptor *ca;
  Data_Get_Struct(self, struct CompAdaptor, ca);
  return INT2NUM(compaAPIVer(ca));
}

static VALUE rbcompa_compfunc(VALUE self, VALUE str)
{
  struct CompAdaptor *ca;
  struct DataBlock db = stringToDataBlock(STR2CSTR(str));
  double result;
  Data_Get_Struct(self, struct CompAdaptor, ca);

  result = compaCompress(ca, db);
  datablockFree(db);
  return rb_float_new(result);
}

static VALUE rbcompa_ncd(VALUE self, VALUE stra, VALUE strb)
{
  struct CompAdaptor *ca;
  double result;
  struct DataBlock dba = stringToDataBlock(STR2CSTR(stra));
  struct DataBlock dbb = stringToDataBlock(STR2CSTR(strb));

  Data_Get_Struct(self, struct CompAdaptor, ca);
  result = compaNCD(ca, dba, dbb);
  datablockFree(dba);
  datablockFree(dbb);
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
  volatile VALUE tdata = Data_Wrap_Struct(cl, 0, compaFree, ca);
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
