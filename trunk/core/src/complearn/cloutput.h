#ifndef __CLOUTPUT_H
#define __CLOUTPUT_H


#include <math.h>
#include <complearn/datablockenum.h>
#include <complearn/uclconfig.h>

#if GSL_RDY
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>
#endif

/*! \file cloutput.h */

#if GSL_RDY
gsl_matrix *svdProject(gsl_matrix *a);
gsl_matrix *getNCDMatrix(struct DataBlockEnumeration *a, struct DataBlockEnumeration *b, struct GeneralConfig *cur);

#else
struct DoubleA *svdProjectDD(struct DoubleA *a);
struct DoubleA *getNCDMatrixDD(struct DataBlockEnumeration *a, struct DataBlockEnumeration *b, struct GeneralConfig *cur);
#endif

struct TreeMaster;

double xpremap(double inp, struct GeneralConfig *cur);

/** \brief Products results of NCD calculations
 *
 * \param a pointer to first DataBlockEnumeration
 * \param b pointer to second DataBlockEnumeration
 */
void printProduct(struct DataBlockEnumeration *a, struct DataBlockEnumeration *b, struct GeneralConfig *cur);

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
 * writeDataBlockToFile
 *
 * \param ub pointer to the UnrootedBinary to be path queried
 * \param labels pointer to a StringStack holding labels for each
 * column indicator in order
 * \return pointer to a new DataBlock holding the .dot-format tree
 */
struct DataBlock *convertTreeToDot(struct TreeAdaptor *ta, double score, struct StringStack *labels, struct CLNodeSet *flips, struct GeneralConfig *cur, struct TreeMaster *tm, gsl_matrix *dm);

FILE *clfopen(const char *fname, char *mode);
void clfclose(FILE *fp);

const char *getUsername(void);
int getPID(void);
const char *getHostname(void);
const char *getUTSName(void);

#endif
