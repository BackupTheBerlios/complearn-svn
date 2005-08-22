#include "clrbcon.h"

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

static dummySOAP(void)
{
  soap_client_init_args(1,NULL);
}

struct TreeOrderObserverState {
  volatile VALUE obs;
};

void Init_complearn4r(void)
{
  mCompLearn = rb_define_module("CompLearn");

  rb_require("matrix");
  cMatrix = rb_const_get(rb_cObject, rb_intern("Matrix"));

  doInitAdja();
  doInitCompa();
  doInitTRA();
  doInitTH();
  doInitTreeMaster();

}
