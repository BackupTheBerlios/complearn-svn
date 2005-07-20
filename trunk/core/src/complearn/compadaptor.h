#ifndef __COMPADAPTOR_H
#define __COMPADAPTOR_H

#include <complearn/envmap.h>
#include <complearn/datablock.h>
#include <complearn/cltypes.h>

/** \brief Compressor interface to CompLearn.
 *  \struct CompAdaptor
 *
 *  CompAdaptor contains pointers to 5 functions which are defined in an
 *  external compression module.
 */
struct CompAdaptor {
  void *cptr;
	t_compfunc cf;      /*!< Pointer to function which returns the compressed
												   size of a DataBlock in bits*/
	t_freecompfunc fcf; /*!< Pointer to function which frees a compressor
												   instance from memory */
	t_shortname sn;     /*!< Pointer to function which returns string shortname of
										       CompAdaptor object */
	t_longname ln;      /*!< Pointer to function which returns string longname of
											     CompAdaptor object */
  t_apiver apiv;      /*!< Pointer to function which returns int api version
												   of CompAdaptor interface */
												  
};

/** When using this interface, you must return APIVER_V1.  Future versions
 * will change this number as the interface specification changes.
 */
#define APIVER_V1 1

double compfuncCA(struct CompAdaptor *ca, struct DataBlock src);
void freeCA(struct CompAdaptor *ca);
char *shortNameCA(struct CompAdaptor *ca);
char *longNameCA(struct CompAdaptor *ca);
int apiverCA(struct CompAdaptor *ca);

#endif

