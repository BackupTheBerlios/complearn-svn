#ifndef __COMPADAPTOR_H
#define __COMPADAPTOR_H

#include <complearn/envmap.h>
#include <complearn/datablock.h>
#include <complearn/cltypes.h>

/*! \file compadaptor.h */

/** \brief Compressor interface to CompLearn.
 *
 *  \struct CompAdaptor
 *
 *  CompAdaptor contains pointers to 5 clFunctions which are defined in an
 *  external compression module.  There is also a user opaque pointer for
 *  custom extensions.
 *
 *  \sa compadaptor.h
 */
struct CompAdaptor {
  void *cptr;
	t_compclFunc cf;      /*!< Pointer to clFunction which returns the compressed
												   size of a DataBlock in bits*/
	t_freecompclFunc fcf; /*!< Pointer to clFunction which frees a compressor
												   instance from memory */
	t_shortname sn;     /*!< Pointer to clFunction which returns string shortname of
										       CompAdaptor object */
	t_longname ln;      /*!< Pointer to clFunction which returns string longname of
											     CompAdaptor object */
  t_apiver apiv;      /*!< Pointer to clFunction which returns int api version
												   of CompAdaptor interface */
  t_getparams params; /*!< Pointer to clFunction which returns a paramlist
												   of CompAdaptor interface */
  struct ParamList *pl;
};

/** When using this interface, you must return APIVER_V1.  Future versions
 * will change this number as the interface specification changes.
 */
#define APIVER_V1 1

double clCompaCompress(struct CompAdaptor *ca, struct DataBlock *src);
void clCompaFree(struct CompAdaptor *ca);
char *clCompaShortName(struct CompAdaptor *ca);
char *clCompaLongName(struct CompAdaptor *ca);

/** \brief Returns API version of CompAdaptor interface
 *
 *  \param ca pointer to CompAdaptor
 *  \return int API version number
 */
int clCompaAPIVer(struct CompAdaptor *ca);

/** \brief Returns the NCD of a pair of DataBlocks.
 *
 *  The ncd formula is as follows:
 *
 *    NCD(a,b) = (C(a,b) - min(C(a),C(b))) / max(C(a),C(b))
 *
 *    where
 *
 *    C(a,b) is the compressed size in bits of the concatenation of a and b.
 *    C(a)   is the compressed size of a in bits
 *    C(b)   is the compressed size of b in bits
 *
 *  clCompaNCD() will return a double precision floating-point value 0 <= x < 1.1
 *  representing  how different the two files are.  Smaller numbers represent
 *  more similar files.  The largest number is somewhere near 1.  It is not
 *  exactly 1 due to imperfections in compression techniques, but for most
 *  standard compression algorithms you are unlikely to see a number above 1.1
 *  in any case.
 *
 *  \param a pointer to DataBlock
 *  \param b pointer to another DataBlock
 *  \returns ncd a double precision floating-point value
 */
double clCompaNCD(struct CompAdaptor *comp, struct DataBlock *a, struct DataBlock *b);
struct ParamList *clCompaParameters(struct CompAdaptor *comp);
void clCompaInitParameters(struct CompAdaptor *ca);
void clCompaPushParameter(struct CompAdaptor *ca, const char *key, const char *value, int type);
void clCompaSetValueForKey(struct CompAdaptor *ca, const char *key, void *dest);

#endif

