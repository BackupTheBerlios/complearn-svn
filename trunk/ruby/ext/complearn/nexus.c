
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

static VALUE rbnexus_isnexus( VALUE cl, VALUE rstr ) {
  int result;
  struct DataBlock *db = convertRubyStringToDataBlock(rstr);
  result = clIsNexusFile( db );
  clDatablockFreePtr( db );
  return ( result ? Qtrue : Qnil );
}

void doInitNexus(void) {
  cNexus = rb_define_class_under(mCompLearn, "Nexus", rb_cObject);
  rb_define_singleton_method(cNexus, "isNexus", rbnexus_isnexus, 1);
}
