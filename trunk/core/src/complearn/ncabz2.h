#ifndef __NCABZ2_H
#define __NCABZ2_H

struct BZ2DynamicAdaptorCB {
  int (*buftobufcompress)(char *dbuff,unsigned int *p,
    char *src, unsigned int sz, int blocksize, int verblevel, int workfactor);
  int (*buftobufdecompress)(char *dbuff,unsigned int *p,
    char *src, unsigned int sz, int small, int verblevel);
};

struct BZ2DynamicAdaptorCB *clGrabBZ2DACB(void);

#endif
