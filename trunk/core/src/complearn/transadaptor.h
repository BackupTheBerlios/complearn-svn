#ifndef __TRANSAPDAPTOR_H
#define __TRANSAPDAPTOR_H

#include <complearn/complearn.h>

/*! \file transadaptor.h */

/** \brief Transform interface.
 *  \struct TransformAdaptor
 *
 *  TransformAdaptor contains pointers to 3 clFunctions which are defined in an
 *  external transform module.
 *
 *  CompLearn comes with 3 builtin tranform adaptors:
 *
 *    unbzip: uncompresses DataBlocks in the bzip compression format
 *    unzlib: uncompresses DataBlocks in the zlib compression format
 *    ungz: uncompresses DataBlocks in the gzip file format
 *
 */
struct TransformAdaptor {
  t_shortname sn;    /*!< Pointer to clFunction which returns string shortname of
                          TransformAdaptor object */
  t_transfree tfree; /*!  Pointer to clFunction frees the TransformAdaptor object
                          from memory */
  t_predicate pf;    /*!< Pointer to clFunction which evaluates a DataBlock and
                          returns a true value if DataBlock needs to be
                          transformed */
  t_transform tf;    /*!< Pointer to clFunction receives a DataBlock and returns
                          a transformed DataBlock */
  void *tptr;        /*!< Pointer which can be used to save the state of a
                          TransformAdaptor instance */
};

struct TransformAdaptor *clBuiltin_UNBZIP(void);
struct TransformAdaptor *clBuiltin_UNGZ(void);
struct TransformAdaptor *clBuiltin_UNZLIB(void);

#endif
