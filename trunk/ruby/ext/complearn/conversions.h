
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

#ifndef __CONVERSIONS_H
#define __CONVERSIONS_H

struct CLNodeSet *convertFromRubyArray(VALUE ar, int maxsz);
VALUE DRAOfIntsToRubyArray(struct DRA *da, unsigned int lev);
gsl_matrix *convertRubyMatrixTogsl_matrix(VALUE rbm);
VALUE secretrbth_new(struct TreeHolder *tomakeruby);
VALUE convertStringStackToRubyArray(struct StringStack *ss);
VALUE convertCLDateTimeToTime(struct CLDateTime *cldt);
VALUE convertgslvectorToRubyVector(gsl_vector *v);
struct DataBlock *convertRubyStringToDataBlock(VALUE rstr);
struct EnvMap *convertRubyHashToEnvMap(VALUE rem);
VALUE convertgslmatrixToRubyMatrix(gsl_matrix *dm);
VALUE convertDataBlockToRubyString(struct DataBlock *db);
#endif
