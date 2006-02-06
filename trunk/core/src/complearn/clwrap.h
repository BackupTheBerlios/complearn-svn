#ifndef __CLWRAP_H
#define __CLWRAP_H

/*! \file clwrap.h */

FILE *clFopen(const char *fname, char *mode);
const char *getLikelyTmpPrefix();
void clFclose(FILE *fp);

#endif
