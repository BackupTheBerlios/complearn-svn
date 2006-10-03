/*
* Copyright (c) 2006 Rudi Cilibrasi, Rulers of the RHouse
* All rights reserved.     cilibrar@cilibrar.com
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the RHouse nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE RULERS AND CONTRIBUTORS "AS IS" AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE RULERS AND CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <complearn/complearn.h>
#include "clrbcon.h"

static VALUE rbcompa_params(VALUE self) {
  struct CompressionBase *ca;
  int i;
  struct ParamList *pl;
  char *key, *val;
  VALUE hash;
  Data_Get_Struct(self, struct CompressionBase, ca);
  assert(ca);
  pl = clGetParameterListCB(ca);
  assert(pl);

  hash = rb_hash_new();
  rb_hash_aset(hash, rb_str_new2("compressor"),rb_str_new2(clShortNameCB(ca)));
  for (i = 0; i < pl->size ; i += 1) {
    key = pl->fields[i]->key; val = pl->fields[i]->value;
    rb_hash_aset(hash, rb_str_new2(key), rb_str_new2(val));;
  }
  assert(pl);
  clParamlistFree(pl);
  return hash;
}

static VALUE rbcompa_dump(VALUE self) {
  return rb_funcall(cMarshal, rb_intern("dump"), 1, rbcompa_params(self));
}

static VALUE rbcompa_load(VALUE cl, VALUE rdata)
{
  struct CompressionBase *ca;
  int i;
  struct EnvMap *em = clEnvmapNew();
  char *key;
  char *val;
  volatile VALUE tdata;
  VALUE cname, rkeys, rsize;

  rdata = rb_funcall(cMarshal, rb_intern("load"), 1, rdata);
  rkeys = rb_funcall(rdata, rb_intern("keys"), 0);
  rsize = rb_funcall(rdata, rb_intern("size"), 0);
  cname = rb_hash_aref(rdata, rb_str_new2("compressor"));
  for ( i = 0; i < NUM2INT(rsize) ; i += 1) {
    key = STR2CSTR(rb_ary_entry(rkeys, i));
    val = STR2CSTR(rb_hash_aref(rdata, rb_ary_entry(rkeys, i)));
    clEnvmapSetKeyVal(em, key, val);
  }
  clEnvmapMerge(clLoadDefaultEnvironment()->em, em);
  ca = clNewCompressorCB(STR2CSTR(cname));
  assert(ca);

  tdata = Data_Wrap_Struct(cCompressionBase, 0, clFreeCB, ca);
  rb_obj_call_init(tdata, 0, 0);
  return tdata;
}

static VALUE rbcompa_shortname(VALUE self)
{
  struct CompressionBase *ca;
  Data_Get_Struct(self, struct CompressionBase, ca);
  return rb_str_new2(clShortNameCB(ca));
}

static VALUE rbcompa_longname(VALUE self)
{
  struct CompressionBase *ca;
  Data_Get_Struct(self, struct CompressionBase, ca);
  return rb_str_new2(clLongNameCB(ca));
}

static VALUE rbcompa_apiver(VALUE self)
{
  struct CompressionBase *ca;
  Data_Get_Struct(self, struct CompressionBase, ca);
  return INT2NUM(APIVER_CLCOMP10);
}

static VALUE rbcompa_compfunc(VALUE self, VALUE str)
{
  struct CompressionBase *ca;
  struct DataBlock *db = clStringToDataBlockPtr(STR2CSTR(str));
  double result;
  Data_Get_Struct(self, struct CompressionBase, ca);

  result = clCompressCB(ca, db);
  clDatablockFreePtr(db);
  return rb_float_new(result);
}

static VALUE rbcompa_ncd(VALUE self, VALUE stra, VALUE strb)
{
  struct CompressionBase *ca;
  double result;
  struct DataBlock *dba = clStringToDataBlockPtr(STR2CSTR(stra));
  struct DataBlock *dbb = clStringToDataBlockPtr(STR2CSTR(strb));

  Data_Get_Struct(self, struct CompressionBase, ca);
  result = clNcdFuncCB(ca, dba, dbb);
  clDatablockFreePtr(dba);
  clDatablockFreePtr(dbb);
  return rb_float_new(result);
}

static VALUE rbcompa_names(VALUE kl)
{
  VALUE result;
  struct StringStack *ss = clListBuiltinsCB(1);
  result = convertStringStackToRubyArray(ss);
  clStringstackFree(ss);
  return result;
}

static VALUE rbcompa_init(VALUE self)
{
  return self;
}

VALUE rbcompa_new(VALUE cl, VALUE comp)
{
  struct CompressionBase *ca = clNewCompressorCB(STR2CSTR(comp));
  volatile VALUE tdata = Data_Wrap_Struct(cl, 0, clFreeCB, ca);
//  volatile VALUE tdata = Data_Wrap_Struct(cl, 0, 0, ca);
  rb_obj_call_init(tdata, 0, 0);
  return tdata;
}

void doInitCompa(void) {
  cCompressionBase = rb_define_class_under(mCompLearn,"CompressionBase", rb_cObject);
  rb_define_method(cCompressionBase, "initialize", rbcompa_init, 0);
  rb_define_singleton_method(cCompressionBase, "new", rbcompa_new, 1);
  rb_define_singleton_method(cCompressionBase, "loadBuiltin", rbcompa_new, 1);
  rb_define_singleton_method(cCompressionBase, "names", rbcompa_names, 0);
  rb_define_singleton_method(cCompressionBase, "listBuiltin", rbcompa_names, 0);
  rb_define_method(cCompressionBase, "compfunc", rbcompa_compfunc, 1);
  rb_define_method(cCompressionBase, "shortname", rbcompa_shortname, 0);
  rb_define_method(cCompressionBase, "longname", rbcompa_longname, 0);
  rb_define_method(cCompressionBase, "apiver", rbcompa_apiver, 0);
  rb_define_method(cCompressionBase, "params", rbcompa_params, 0);
  rb_define_method(cCompressionBase, "ncd", rbcompa_ncd, 2);
  rb_define_method(cCompressionBase, "_dump", rbcompa_dump, 1);
  rb_define_singleton_method(cCompressionBase, "_load", rbcompa_load, 1);
}
