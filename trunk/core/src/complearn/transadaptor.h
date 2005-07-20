#ifndef __TRANSAPDAPTOR_H
#define __TRANSAPDAPTOR_H

#include <complearn/complearn.h>

/** \brief Transform interface.
 *  \struct TransAdaptor
 *
 *  TransAdaptor contains pointers to 3 functions which are defined in an
 *  external transform module.
 *
 *  CompLearn comes with 3 builtin tranform adaptors:
 *
 *    unbzip: uncompresses DataBlocks in the bzip compression format
 *    unzlib: uncompresses DataBlocks in the zlib compression format
 *    ungz: uncompresses DataBlocks in the gzip file format
 *
 */
struct TransAdaptor {
  t_shortname sn;    /*!< Pointer to function which returns string shortname of
                          TransAdaptor object */
  t_transfree tfree; /*!  Pointer to function frees the TransAdaptor object
                          from memory */
  t_predicate pf;    /*!< Pointer to function which evaluates a DataBlock and
                          returns a true value if DataBlock needs to be
                          transformed */
  t_transform tf;    /*!< Pointer to function receives a DataBlock and returns
                          a transformed DataBlock */
  void *tptr;        /*!< Pointer which can be used to save the state of a
                          TransAdaptor instance */
};

#endif
