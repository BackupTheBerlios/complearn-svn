#ifndef __PPMENCODER_H
#define __PPMENCODER_H

/** Encoder class. For now only calculates code size */
struct PPMEncoder;

/** Allocate a new PPMEncoder */
struct PPMEncoder *newPPMEncoder(void);
void freePPMEncoder(struct PPMEncoder *ppmenc);
/** Encode a symbol by its probability.  There is no return value.
 * @param c Prob. of symbol
 * @param ppmenc pointer to a PPMEncoder made with newPPMEncoder
 */
void PPMencode(struct PPMEncoder *ppmenc, double c);
double PPMlen(struct PPMEncoder *ppmenc);

#endif
