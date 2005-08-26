#ifndef __CLTYPES_H
#define __CLTYPES_H

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
struct DoubleA;
struct GeneralConfig;

typedef unsigned int t_tagtype;

typedef struct CompAdaptor *(*t_clnewca)(void);
typedef double (*t_compfunc)(struct CompAdaptor *ca, struct DataBlock src);
typedef void (*t_freecompfunc)(struct CompAdaptor *ca);
typedef char *(*t_shortname)(void);
typedef char *(*t_longname)(void);
typedef char *(*t_compparam)(struct CompAdaptor *ca);
typedef int (*t_apiver)(void);

typedef void (*t_doublefreefunc)(struct DoubleA *da, void *udata);

typedef int (*t_predicate)(struct DataBlock db);
typedef void (*t_transfree)(struct TransformAdaptor *ta);

typedef struct DataBlock (*t_transform)(struct DataBlock db);
typedef struct DataBlockEnumerationIterator *(*t_newenumiter)(struct DataBlockEnumeration *);
typedef void (*t_iterfree)(struct DataBlockEnumerationIterator *);
typedef void (*t_enumfree)(struct DataBlockEnumeration *);
typedef struct DataBlock *(*t_istar)(struct DataBlockEnumeration *, struct DataBlockEnumerationIterator *);
typedef void (*t_istep)(struct DataBlockEnumeration *, struct DataBlockEnumerationIterator *);
typedef char * (*t_ilabel)(struct DataBlockEnumeration *, struct DataBlockEnumerationIterator *);
typedef struct TransformAdaptor *(*t_searchfunc)(void *container, void *element);

struct QuartetObserver;
struct Quartet;

typedef int (*t_quartetvisitor)(struct QuartetObserver *qo, struct Quartet q, int qphase);

struct AdjAdaptor;
typedef void (*t_adjaprint)(struct AdjAdaptor *a);
typedef int (*t_adjasize)(struct AdjAdaptor *a);
typedef void (*t_adjafree)(struct AdjAdaptor *a);
typedef struct AdjAdaptor *(*t_adjaclone)(struct AdjAdaptor *a);
typedef int (*t_adjagetconstate)(struct AdjAdaptor *a, int i, int j);
typedef void (*t_adjasetconstate)(struct AdjAdaptor *a, int i, int j, int which);
typedef int (*t_adjagetneighborcount)(struct AdjAdaptor *a, int i);
typedef int (*t_adjagetneighbors)(struct AdjAdaptor *a, int i, int *nbuf, int *nsize);
typedef struct DoubleA *(*t_adjaspmmap)(struct AdjAdaptor *a);

typedef void (*t_treemutate)(struct TreeAdaptor *tra);
typedef void (*t_treefree)(struct TreeAdaptor *tra);
typedef struct TreeAdaptor *(*t_treeclone)(struct TreeAdaptor *tra);
typedef struct LabelPerm *(*t_treegetlabelperm)(struct TreeAdaptor *tra);
typedef struct AdjAdaptor *(*t_treegetadja)(struct TreeAdaptor *tra);
typedef int (*t_treenodepred)(struct TreeAdaptor *tra, int which);
typedef int (*t_treemutecount)(struct TreeAdaptor *tra);
typedef struct DoubleA *(*t_treeperimpairs)(struct TreeAdaptor *tra, struct CLNodeSet *flips);

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
