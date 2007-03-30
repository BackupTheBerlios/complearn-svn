
/*
 * This file is part of the libcomplearn-ruby project.
 *
 * Copyright (C) 2006-2007 Rudi Cilibrasi <cilibrar@cilibrar.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */


#include <complearn/complearn.h>
#include "clrbcon.h"

static VALUE rbincrdm_init(VALUE self)
{
  return self;
}

VALUE rbincrdm_new(int argc, VALUE *argv, VALUE cl)
{
  struct CompressionBase *incrdmca = NULL;
  struct IncrementalDistMatrix *idm;
  volatile VALUE tdata;
  if (argc > 0) {
    Data_Get_Struct(argv[0], struct CompressionBase, incrdmca);
  }
  idm = clIncrdmNew(incrdmca);
  tdata = Data_Wrap_Struct(cl, 0, clIncrdmFree, idm);
  rb_obj_call_init(tdata, 0, 0);
  if (argc > 0)
    rb_ivar_set(tdata, rb_intern("ca"), argv[0]);
  return tdata;
}

static VALUE rbincrdm_addstring(VALUE self, VALUE rstr)
{
  struct IncrementalDistMatrix *idm;
  struct DataBlock *db = convertRubyStringToDataBlock(rstr);
  Data_Get_Struct(self, struct IncrementalDistMatrix, idm);
  clIncrdmAddDataBlock(idm,db);
  return Qnil;
}

static VALUE rbincrdm_distmatrix(VALUE self)
{
  struct IncrementalDistMatrix *idm;
  Data_Get_Struct(self, struct IncrementalDistMatrix, idm);
  return convertgslmatrixToRubyMatrix(clIncrdmDistMatrix(idm));
}

static VALUE ksDMK;
static VALUE ksDBK;
static VALUE ksSSK;
static VALUE ksCBK;

static VALUE rbincrdm_dmk(VALUE self)
{
//  struct IncrementalDistMatrix *idm;
// Data_Get_Struct(self, struct IncrementalDistMatrix, idm);
  return ksDMK;
}

static VALUE rbincrdm_dbk(VALUE self)
{
  return ksDBK;
}

static VALUE rbincrdm_ssk(VALUE self)
{
  return ksSSK;
}

static VALUE rbincrdm_cbk(VALUE self)
{
  return ksCBK;
}

static VALUE rbincrdm_size(VALUE self)
{
  struct IncrementalDistMatrix *idm;
  Data_Get_Struct(self, struct IncrementalDistMatrix, idm);
  return INT2FIX(clIncrdmSize(idm));
}

static VALUE ksDMK;
static VALUE ksDBK;
static VALUE ksSSK;
static VALUE ksCBK;
static VALUE rbincrdm_dump(VALUE self, VALUE depth) {
  struct IncrementalDistMatrix *idm;
  Data_Get_Struct(self, struct IncrementalDistMatrix, idm);
  VALUE obj = rb_hash_new();
  VALUE dba = rb_ary_new();
  VALUE ssa = rb_ary_new();
  int i;
  for (i = 0; i < idm->dbcount; i += 1) {
    rb_ary_push(dba, convertDataBlockToRubyString(idm->db[i]));
    rb_ary_push(ssa, INT2NUM(idm->singlesize[i]));
  }
  if (idm->dbcount) {
    VALUE rmm = convertgslmatrixToRubyMatrix((gsl_matrix *) &idm->result);
    rb_hash_aset(obj, ksDMK, rmm);
  }
  rb_hash_aset(obj, ksDBK, dba);
  rb_hash_aset(obj, ksSSK, ssa);
  rb_hash_aset(obj, ksCBK, rb_ivar_get(self, rb_intern("ca")));
  return rb_funcall(cMarshal, rb_intern("dump"), 1, obj);
}

static VALUE rbincrdm_load(VALUE kl, VALUE str)
{
  int i, j;
  VALUE obj;
  gsl_matrix *gslm;
  struct IncrementalDistMatrix *idm;
  VALUE rca, dba, ssa, tdata, dm;
  struct CompressionBase *ca = NULL;

  obj = rb_funcall(cMarshal, rb_intern("load"), 1, str);
  rca = rb_hash_aref(obj, ksCBK);
  dba = rb_hash_aref(obj, ksDBK);
  ssa = rb_hash_aref(obj, ksSSK);
  dm =  rb_hash_aref(obj, ksDMK);
  if (rca != Qnil) {
    Data_Get_Struct(rca, struct CompressionBase, ca);
  }

  idm = clIncrdmNew(ca);
  idm->dbcount = RARRAY(dba)->len;

  if (idm->dbcount) {
    gslm = convertRubyMatrixTogsl_matrix(dm);
    idm->result = gsl_matrix_submatrix(idm->curmat, 0, 0, idm->dbcount, idm->dbcount);

    assert(gslm->size1 == gslm->size2);
    assert(gslm->size1 == idm->dbcount);
    assert(RARRAY(ssa)->len == idm->dbcount);

    for (i = 0; i < gslm->size1; i += 1) {
      idm->singlesize[i] = NUM2INT(rb_ary_entry(ssa, i));
      idm->db[i] = convertRubyStringToDataBlock(rb_ary_entry(dba, i));
      for (j = 0; j < gslm->size2; j += 1) {
        gsl_matrix_set(idm->curmat, i, j, gsl_matrix_get(gslm, i, j));
      }
    }

    gsl_matrix_free(gslm);
  }

  tdata = Data_Wrap_Struct(cIncrementalDistMatrix, 0, clIncrdmFree, idm);
  rb_ivar_set(tdata, rb_intern("ca"), rca);
  rb_obj_call_init(tdata, 0, 0);
  return tdata;
}

void doInitIncrDistMatrix(void) {
  cIncrementalDistMatrix = rb_define_class_under(mCompLearn,"IncrementalDistMatrix", rb_cObject);
  rb_define_singleton_method(cIncrementalDistMatrix, "new", rbincrdm_new, -1);
  rb_define_method(cIncrementalDistMatrix, "initialize", rbincrdm_init, 0);
  rb_define_method(cIncrementalDistMatrix, "addString", rbincrdm_addstring, 1);
  rb_define_method(cIncrementalDistMatrix, "addDataBlock", rbincrdm_addstring, 1);
  rb_define_method(cIncrementalDistMatrix, "distmatrix", rbincrdm_distmatrix, 0);
  rb_define_method(cIncrementalDistMatrix, "size", rbincrdm_size, 0);

  rb_define_singleton_method(cIncrementalDistMatrix, "dmk", rbincrdm_dmk, 0);
  rb_define_singleton_method(cIncrementalDistMatrix, "dbk", rbincrdm_dbk, 0);
  rb_define_singleton_method(cIncrementalDistMatrix, "ssk", rbincrdm_ssk, 0);
  rb_define_singleton_method(cIncrementalDistMatrix, "cbk", rbincrdm_cbk, 0);

  rb_define_method(cIncrementalDistMatrix, "_dump", rbincrdm_dump, 1);

  rb_define_singleton_method(cIncrementalDistMatrix, "_load", rbincrdm_load, 1);

  rb_define_const(cIncrementalDistMatrix, "DistMatrixKey", rb_str_new2("dmk"));
  rb_define_const(cIncrementalDistMatrix, "DataBlockKey", rb_str_new2("dbk"));
  rb_define_const(cIncrementalDistMatrix, "SingleSizeKey", rb_str_new2("ssk"));
  rb_define_const(cIncrementalDistMatrix, "CompressionBaseKey", rb_str_new2("cbk"));

  ksDMK = rb_const_get(cIncrementalDistMatrix, rb_intern("DistMatrixKey"));
  ksDBK = rb_const_get(cIncrementalDistMatrix, rb_intern("DataBlockKey"));
  ksSSK = rb_const_get(cIncrementalDistMatrix, rb_intern("SingleSizeKey"));
  ksCBK = rb_const_get(cIncrementalDistMatrix, rb_intern("CompressionBaseKey"));

}
