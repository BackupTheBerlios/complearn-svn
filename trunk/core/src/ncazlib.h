#ifndef __NCAZLIB_H
#define __NCAZLIB_H

struct ZlibDynamicAdaptorCB {
  int (*compress2)(unsigned char *dbuff,unsigned long *p,
    unsigned char *src, unsigned long sz, int level);
  int (*uncompress)(unsigned char *dbuff,unsigned long *p, unsigned char *src, unsigned long sz);
};

struct ZlibDynamicAdaptorCB *clGrabZlibDACB(void);

#endif
