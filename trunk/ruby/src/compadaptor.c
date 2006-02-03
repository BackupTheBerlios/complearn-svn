#include "clrbcon.h"


static VALUE rbcompa_params(VALUE self) {
  struct CompAdaptor *ca;
  int i;
  struct ParamList *pl;
  char *key, *val;
  VALUE hash;
  Data_Get_Struct(self, struct CompAdaptor, ca);
  assert(ca);
  pl = clCompaParameters(ca);
  assert(pl);

  hash = rb_hash_new();
  rb_hash_aset(hash, rb_str_new2("compressor"),rb_str_new2(clCompaShortName(ca)));
  for (i = 0; i < pl->size ; i += 1) {
    key = pl->fields[i]->key; val = pl->fields[i]->value;
    rb_hash_aset(hash, rb_str_new2(key), rb_str_new2(val));;
  }
  assert(pl);
  clParamlistFree(pl);
  return hash;
}

static VALUE rbcompa_dump(VALUE self) {
  return rb_clFuncall(cMarshal, rb_intern("dump"), 1, rbcompa_params(self));
}

static VALUE rbcompa_load(VALUE cl, VALUE rdata)
{
  struct CompAdaptor *ca;
  int i;
  struct EnvMap *em = clEnvmapNew();
  char *key;
  char *val;
  volatile VALUE tdata;
  VALUE cname, rkeys, rsize;

  rdata = rb_clFuncall(cMarshal, rb_intern("load"), 1, rdata);
  rkeys = rb_clFuncall(rdata, rb_intern("keys"), 0);
  rsize = rb_clFuncall(rdata, rb_intern("size"), 0);
  cname = rb_hash_aref(rdata, rb_str_new2("compressor"));
  for ( i = 0; i < NUM2INT(rsize) ; i += 1) {
    key = STR2CSTR(rb_ary_entry(rkeys, i));
    val = STR2CSTR(rb_hash_aref(rdata, rb_ary_entry(rkeys, i)));
    clEnvmapSetKeyVal(em, key, val);
  }
  clEnvmapMerge(clLoadDefaultEnvironment()->em, em);
  ca = clCompaLoadBuiltin(STR2CSTR(cname));
  assert(ca);

  tdata = Data_Wrap_Struct(cCompAdaptor, 0, clCompaFree, ca);
  rb_obj_call_init(tdata, 0, 0);
  return tdata;
}

static VALUE rbcompa_shortname(VALUE self)
{
  struct CompAdaptor *ca;
  Data_Get_Struct(self, struct CompAdaptor, ca);
  return rb_str_new2(clCompaShortName(ca));
}

static VALUE rbcompa_longname(VALUE self)
{
  struct CompAdaptor *ca;
  Data_Get_Struct(self, struct CompAdaptor, ca);
  return rb_str_new2(clCompaLongName(ca));
}

static VALUE rbcompa_apiver(VALUE self)
{
  struct CompAdaptor *ca;
  Data_Get_Struct(self, struct CompAdaptor, ca);
  return INT2NUM(clCompaAPIVer(ca));
}

static VALUE rbcompa_compclFunc(VALUE self, VALUE str)
{
  struct CompAdaptor *ca;
  struct DataBlock *db = clStringToDataBlockPtr(STR2CSTR(str));
  double result;
  Data_Get_Struct(self, struct CompAdaptor, ca);

  result = clCompaCompress(ca, db);
  clDatablockFreePtr(db);
  return rb_float_new(result);
}

static VALUE rbcompa_ncd(VALUE self, VALUE stra, VALUE strb)
{
  struct CompAdaptor *ca;
  double result;
  struct DataBlock *dba = clStringToDataBlockPtr(STR2CSTR(stra));
  struct DataBlock *dbb = clStringToDataBlockPtr(STR2CSTR(strb));

  Data_Get_Struct(self, struct CompAdaptor, ca);
  result = clCompaNCD(ca, dba, dbb);
  clDatablockFreePtr(dba);
  clDatablockFreePtr(dbb);
  return rb_float_new(result);
}

static VALUE rbcompa_names(VALUE kl)
{
  VALUE result;
  struct StringStack *ss = clCompaListBuiltin();
  result = convertStringStackToRubyArray(ss);
  clStringstackFree(ss);
  return result;
}

static VALUE rbcompa_init(VALUE self)
{
}

VALUE rbcompa_new(VALUE cl, VALUE comp)
{
  struct CompAdaptor *ca = clCompaLoadBuiltin(STR2CSTR(comp));
  volatile VALUE tdata = Data_Wrap_Struct(cl, 0, clCompaFree, ca);
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
  rb_define_method(cCompAdaptor, "compclFunc", rbcompa_compclFunc, 1);
  rb_define_method(cCompAdaptor, "shortname", rbcompa_shortname, 0);
  rb_define_method(cCompAdaptor, "longname", rbcompa_longname, 0);
  rb_define_method(cCompAdaptor, "apiver", rbcompa_apiver, 0);
  rb_define_method(cCompAdaptor, "params", rbcompa_params, 0);
  rb_define_method(cCompAdaptor, "ncd", rbcompa_ncd, 2);
  rb_define_method(cCompAdaptor, "_dump", rbcompa_dump, 1);
  rb_define_singleton_method(cCompAdaptor, "_load", rbcompa_load, 1);
}
