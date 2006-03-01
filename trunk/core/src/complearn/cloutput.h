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
#ifndef __CLOUTPUT_H
#define __CLOUTPUT_H


#include <math.h>
#include <complearn/datablockenum.h>
#include <complearn/uclconfig.h>

#include <gsl/gsl_blas.h>

/*! \file cloutput.h */

gsl_matrix *clSvdProject(gsl_matrix *a);
gsl_matrix *clGetNCDMatrix(struct DataBlockEnumeration *a, struct DataBlockEnumeration *b, struct GeneralConfig *cur);

struct TreeMaster;

double clXpremap(double inp, struct GeneralConfig *cur);

/** \brief Products results of NCD calculations
 *
 * \param a pointer to first DataBlockEnumeration
 * \param b pointer to second DataBlockEnumeration
 */
void clPrintProduct(struct DataBlockEnumeration *a, struct DataBlockEnumeration *b, struct GeneralConfig *cur);

/** \brief outputs a tree into a .dot format DataBlock
 *
 * This function writes a DataBlock representation of the given tree in
 * the .dot file format.  This is the tree format used by dot and neato
 * of the popular AT&T graphviz package.  This is often used before
 * laying out a tree for visual representation by another program.
 *
 * To use this function, you must first have a tree.  You may optionally
 * pass in a StringStack holding an ordered list of leaf-labels.
 * This list must be in the same order that columns were given for the
 * distance matrix that led to this tree.  If NULL is passed in
 * for labels, then the node identifier integers will be used as labels
 * themselves.
 *
 * In order to export a file to another program you must call
 * clDatablockWriteToFile
 *
 * \param ub pointer to the UnrootedBinary to be path queried
 * \param labels pointer to a StringStack holding labels for each
 * column indicator in order
 * \return pointer to a new DataBlock holding the .dot-format tree
 */
struct DataBlock *clConvertTreeToDot(struct TreeAdaptor *ta, double score, struct StringStack *labels, struct CLNodeSet *flips, struct GeneralConfig *cur, struct TreeMaster *tm, gsl_matrix *dm);

const char *clGetUsername(void);
int clGetPID(void);
const char *clGetHostname(void);
const char *clGetUTSName(void);
struct DataBlock *makeCLBDistMatrix(gsl_matrix *gres, struct StringStack *labels, struct StringStack *cmds, struct EnvMap *em);

#endif
