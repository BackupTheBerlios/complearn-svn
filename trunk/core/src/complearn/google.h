#ifndef __GOOGLE_H
#define __GOOGLE_H

#include <complearn/stringstack.h>

/*! \file google.h */

/* returns -1 on error, or pagecount, which may be 0 or greater */
double clGetPageCount(struct StringStack *terms, const char *gkey);
const char *clMakeQueryString(struct StringStack *terms);
void clNormalizeSearchTerms(struct StringStack *terms);
double clCalculateM(const char *daystr, const char *gkey);
double clCalculateMbase(const char *daystr, const char *gkey);

#endif
