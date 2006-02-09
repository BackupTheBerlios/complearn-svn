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
#ifndef __CLRBCON_H
#define __CLRBCON_H

#include <ruby.h>
#undef PACKAGE_VERSION
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_BUGREPORT
#include <complearn/complearn.h>
#include "conversions.h"

#include <gsl/gsl_odeiv.h>
#include <gsl/gsl_vector.h>

extern VALUE mCompLearn;

extern VALUE cAdjAdaptor;
extern VALUE cCompAdaptor;
extern VALUE cTreeAdaptor;
extern VALUE cTreeHolder;
extern VALUE cTreeMaster;
extern VALUE cTreeObserver;
extern VALUE cTreeMolder;
extern VALUE cTreeBlaster;
extern VALUE cTreeOrderObserver;
extern VALUE cSpringBallSystem;
extern VALUE cIncrementalDistMatrix;

extern VALUE cMatrix;
extern VALUE cVector;
extern VALUE cTime;
extern VALUE cMarshal;

VALUE convertgslmatrixToRubyMatrix(gsl_matrix *dm);
#endif
