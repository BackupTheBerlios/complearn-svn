#include "clrbcon.h"

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

void doInitCompa(void) {
  cCompAdaptor = rb_define_class_under(mCompLearn,"CompAdaptor", rb_cObject);
  rb_define_method(cCompAdaptor, "initialize", rbcompa_init, 0);
  rb_define_singleton_method(cCompAdaptor, "new", rbcompa_new, 1);
  rb_define_method(cCompAdaptor, "compfunc", rbcompa_compfunc, 1);
  rb_define_method(cCompAdaptor, "shortname", rbcompa_shortname, 0);
  rb_define_method(cCompAdaptor, "longname", rbcompa_longname, 0);
  rb_define_method(cCompAdaptor, "apiver", rbcompa_apiver, 0);
}
