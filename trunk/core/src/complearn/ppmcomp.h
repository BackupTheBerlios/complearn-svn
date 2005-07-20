#ifndef __PPMCOMP_H
#define __PPMCOMP_H

struct PPMComp;

struct PPMComp *newPPMComp(void);
double getPPMCompSize(struct PPMComp *ppmc);
void encodeSymbolArray(struct PPMComp *ppmc, unsigned char *syms, unsigned 
int len);
void freePPMComp(struct PPMComp *ptr);

#endif
