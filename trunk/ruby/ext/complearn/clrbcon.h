
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

#ifndef __CLRBCON_H
#define __CLRBCON_H

#ifdef PACKAGE_VERSION
#undef PACKAGE_VERSION
#endif
#ifdef PACKAGE_NAME
#undef PACKAGE_NAME
#endif
#ifdef PACKAGE_STRING
#undef PACKAGE_STRING
#endif
#ifdef PACKAGE_TARNAME
#undef PACKAGE_TARNAME
#endif
#ifdef PACKAGE_BUGREPORT
#undef PACKAGE_BUGREPORT
#endif
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
extern VALUE cCompressionBase;
extern VALUE cTreeAdaptor;
extern VALUE cTreeHolder;
extern VALUE cTreeMaster;
extern VALUE cTreeObserver;
extern VALUE cTreeMolder;
extern VALUE cTreeBlaster;
extern VALUE cTreeOrderObserver;
extern VALUE cSpringBallSystem;
extern VALUE cIncrementalDistMatrix;
extern VALUE cNexus;

extern VALUE cMatrix;
extern VALUE cVector;
extern VALUE cHash;
extern VALUE cTime;
extern VALUE cMarshal;

VALUE convertgslmatrixToRubyMatrix(gsl_matrix *dm);
#endif
