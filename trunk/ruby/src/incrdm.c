#include "clrbcon.h"

static VALUE rbincrdm_init(VALUE self)
{
}

VALUE rbincrdm_new(int argc, VALUE *argv, VALUE cl)
{
  struct CompAdaptor *incrdmca = NULL;
  struct IncrementalDistMatrix *idm;
  volatile VALUE tdata;
  if (argc > 0) {
    Data_Get_Struct(argv[0], struct CompAdaptor, incrdmca);
  }
  idm = newIDM(incrdmca);
  tdata = Data_Wrap_Struct(cl, 0, freeIncrementalDistMatrix, idm);
  rb_obj_call_init(tdata, 0, 0);
  return tdata;
}

static VALUE rbincrdm_addstring(VALUE self, VALUE rstr)
{
  struct IncrementalDistMatrix *idm;
  struct DataBlock *db = convertRubyStringToDataBlock(rstr);
  Data_Get_Struct(self, struct IncrementalDistMatrix, idm);
  addDataBlock(idm,db);
  return Qnil;
}

static VALUE rbincrdm_distmatrix(VALUE self)
{
  struct IncrementalDistMatrix *idm;
  Data_Get_Struct(self, struct IncrementalDistMatrix, idm);
  return convertgslmatrixToRubyMatrix(getDistMatrixIDM(idm));
}

static VALUE rbincrdm_size(VALUE self)
{
  struct IncrementalDistMatrix *idm;
  Data_Get_Struct(self, struct IncrementalDistMatrix, idm);
  return INT2FIX(doubleaSizeIDM(idm));
}

void doInitIncrDistMatrix(void) {
  cIncrementalDistMatrix = rb_define_class_under(mCompLearn,"IncrementalDistMatrix", rb_cObject);
  rb_define_singleton_method(cIncrementalDistMatrix, "new", rbincrdm_new, -1);
  rb_define_method(cIncrementalDistMatrix, "initialize", rbincrdm_init, 0);
  rb_define_method(cIncrementalDistMatrix, "addString", rbincrdm_addstring, 1);
  rb_define_method(cIncrementalDistMatrix, "addString", rbincrdm_addstring, 1);
  rb_define_method(cIncrementalDistMatrix, "distmatrix", rbincrdm_distmatrix, 0);
  rb_define_method(cIncrementalDistMatrix, "size", rbincrdm_size, 0);
}
