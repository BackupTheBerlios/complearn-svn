#ifndef __NCDCALC_H
#define __NCDCALC_H

#include <complearn/environment.h>

/*! \file ncd-calc.h */

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
 *  ncdPair() will return a double precision floating-point value 0 <= x < 1.1
 *  representing  how different the two files are.  Smaller numbers represent
 *  more similar files.  The largest number is somewhere near 1.  It is not
 *  exactly 1 due to imperfections in compression techniques, but for most
 *  standard compression algorithms you are unlikely to see a number above 1.1
 *  in any case.
 *
 *  \param a DataBlock
 *  \param b DataBlock
 *  \returns ncd a double precision floating-point value
 */
double ncdPair(struct CompAdaptor *comp, struct DataBlock a, struct DataBlock b );

/* should this be static? */
double mndf(double ca, double cb, double cab, double cba);
double ncdfunc(struct DataBlock *a, struct DataBlock *b, struct GeneralConfig *cur);

#endif
