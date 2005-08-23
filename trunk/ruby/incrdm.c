#include "clrbcon.h"

static VALUE rbincrdm_init(VALUE self)
{
}

VALUE rbincrdm_new(VALUE cl, VALUE ca)
{
  struct CompAdaptor *incrdmca;
  struct IncrementalDistMatrix *idm;
  volatile VALUE tdata;
  Data_Get_Struct(ca, struct CompAdaptor, incrdmca);
  idm = newIDM(incrdmca);
  tdata = Data_Wrap_Struct(cIncrementalDistMatrix, 0, freeIncrementalDistMatrix, idm);
  rb_obj_call_init(tdata, 0, 0);
  return tdata;
}

void doInitIncrDistMatrix(void) {
  cIncrementalDistMatrix = rb_define_class_under(mCompLearn,"IncrementalDistMatrix", rb_cObject);
  rb_define_singleton_method(cIncrementalDistMatrix, "new", rbincrdm_new, 2);
  rb_define_method(cIncrementalDistMatrix, "initialize", rbincrdm_init, 0);
}
