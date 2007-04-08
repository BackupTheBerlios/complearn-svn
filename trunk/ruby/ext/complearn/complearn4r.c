
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

#include "clrbcon.h"

VALUE mCompLearn;

VALUE cAdjAdaptor;
VALUE cCompressionBase;
VALUE cTreeAdaptor;
VALUE cTreeHolder;
VALUE cTreeMaster;
VALUE cTreeObserver;
VALUE cTreeMolder;
VALUE cTreeBlaster;
VALUE cTreeOrderObserver;
VALUE cSpringBallSystem;
VALUE cIncrementalDistMatrix;
VALUE cNexus;

VALUE cMatrix;
VALUE cVector;
VALUE cHash;
VALUE cTime;
VALUE cMarshal;

static int dummySOAP(void)
{
  soap_client_init_args(1,NULL);
  if (0)
    dummySOAP(); // avoid warning
}

struct TreeOrderObserverState {
  volatile VALUE obs;
};

VALUE rbcl_version(VALUE m) {
  return rb_str_new2( clLibVersionString() );
}

void
Init_complearn4r(void)
{
  mCompLearn = rb_define_module("CompLearn");

  rb_require("matrix");
  rb_define_singleton_method(mCompLearn, "version", rbcl_version, 0);
  rb_define_method(mCompLearn, "version", rbcl_version, 0);
  cMatrix = rb_const_get(rb_cObject, rb_intern("Matrix"));
  cVector = rb_const_get(rb_cObject, rb_intern("Vector"));
  cHash = rb_const_get(rb_cObject, rb_intern("Hash"));

  cTime = rb_const_get(rb_cObject, rb_intern("Time"));
  cMarshal = rb_const_get(rb_cObject, rb_intern("Marshal"));

  doInitAdja();
  doInitCompa();
  doInitTRA();
  doInitTH();
  doInitTreeMaster();
  doInitSpringBallSystem();
  doInitIncrDistMatrix();
  doInitNexus();
}
