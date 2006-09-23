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
#ifndef __CLTYPES_H
#define __CLTYPES_H

#define PARAMSTRING 1
#define PARAMINT    2
#define PARAMDOUBLE 3

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>

#include <complearn/datablock.h>
#include <complearn/quartet.h>

/*! \file cltypes.h */

struct EnvMap;
struct CompAdaptor;
struct TransformAdaptor;
struct TreeAdaptor;
struct CLNodeSet;
struct TreeObserver;
struct TreeOrderObserver;
struct TreeHolder;
struct TreeMolder;
struct LabelPerm;
struct DataBlockEnumeration;
struct DataBlockEnumerationIterator;
struct DRA;
struct GeneralConfig;

typedef unsigned int t_tagtype;

typedef void (*t_emitclFunc)(char *str);
typedef void (*t_cldlinitcl)(void);
typedef double (*t_compclFunc)(struct CompAdaptor *ca, struct DataBlock *src);
typedef void (*t_freecompclFunc)(struct CompAdaptor *ca);
typedef char *(*t_shortname)(void);
typedef char *(*t_longname)(void);
typedef char *(*t_compparam)(struct CompAdaptor *ca);
typedef int (*t_apiver)(void);
typedef struct ParamList *(*t_getparams)(void);

typedef void (*t_doublefreeclFunc)(struct DRA *da, void *udata);

typedef int (*t_predicate)(struct DataBlock *db);
typedef void (*t_transfree)(struct TransformAdaptor *ta);

typedef struct DataBlock *(*t_transform)(struct DataBlock *db);
typedef struct DataBlockEnumerationIterator *(*t_newenumiter)(struct DataBlockEnumeration *);
typedef void (*t_iterfree)(struct DataBlockEnumerationIterator *);
typedef void (*t_enumfree)(struct DataBlockEnumeration *);
typedef struct DataBlock *(*t_istar)(struct DataBlockEnumeration *, struct DataBlockEnumerationIterator *);
typedef void (*t_istep)(struct DataBlockEnumeration *, struct DataBlockEnumerationIterator *);
typedef char * (*t_ilabel)(struct DataBlockEnumeration *, struct DataBlockEnumerationIterator *);
typedef struct TransformAdaptor *(*t_searchclFunc)(void *container, void *element);

struct QuartetObserver;
struct Quartet;

typedef int (*t_quartetvisitor)(struct QuartetObserver *qo, struct Quartet q, int qphase);

struct AdjAdaptor;
typedef void (*t_adjaprint)(struct AdjAdaptor *a);
typedef int (*t_adjasize)(struct AdjAdaptor *a);
typedef void (*t_adjafree)(struct AdjAdaptor *a);
typedef struct AdjAdaptor *(*t_adclJaclone)(struct AdjAdaptor *a);
typedef int (*t_adjagetconstate)(struct AdjAdaptor *a, int i, int j);
typedef void (*t_adjasetconstate)(struct AdjAdaptor *a, int i, int j, int which);
typedef int (*t_adjagetneighborcount)(struct AdjAdaptor *a, int i);
typedef int (*t_adjagetneighbors)(struct AdjAdaptor *a, int i, int *nbuf, int *nsize);
typedef struct DRA *(*t_adjaspmmap)(struct AdjAdaptor *a);

typedef void (*t_treemutate)(struct TreeAdaptor *tra);
typedef void (*t_treefree)(struct TreeAdaptor *tra);
typedef struct TreeAdaptor *(*t_treeclone)(struct TreeAdaptor *tra);
typedef struct LabelPerm *(*t_treegetlabelperm)(struct TreeAdaptor *tra);
typedef void (*t_treelpsetat)(struct TreeAdaptor *tra, int j, int i);
typedef struct AdjAdaptor *(*t_treegetadja)(struct TreeAdaptor *tra);
typedef int (*t_treenodepred)(struct TreeAdaptor *tra, int which);
typedef int (*t_treemutecount)(struct TreeAdaptor *tra);
typedef struct DRA *(*t_treeperimpairs)(struct TreeAdaptor *tra, struct CLNodeSet *flips);

typedef void (*t_treesearchstarted)(struct TreeObserver *tob);
typedef void (*t_treeimproved)(struct TreeObserver *tob, struct TreeHolder *th);
typedef void (*t_treerejected)(struct TreeObserver *tob);
typedef void (*t_treedone)(struct TreeObserver *tob, struct TreeHolder *th);

typedef void (*t_treeordersearchstarted)(struct TreeOrderObserver *tob);
typedef void (*t_treeorderimproved)(struct TreeOrderObserver *tob, struct TreeMolder *th, struct CLNodeSet *flips);
typedef void (*t_treeorderrejected)(struct TreeOrderObserver *tob);
typedef void (*t_treeorderdone)(struct TreeOrderObserver *tob, struct TreeMolder *th, struct CLNodeSet *flips);

typedef void (*t_freeappconfig)(struct GeneralConfig *cur);
typedef void (*t_updateappemtoconfig)(struct GeneralConfig *cur);
typedef void (*t_updateappconfigtoem)(struct GeneralConfig *cur);
typedef void (*t_printapphelp)(struct GeneralConfig *cur);
typedef void (*t_printappenvironment)(struct GeneralConfig *cur);

#endif
