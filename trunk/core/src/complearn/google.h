#ifndef __GOOGLE_H
#define __GOOGLE_H

#include <complearn/stringstack.h>

/*! \file google.h */

/* returns -1 on error, or pagecount, which may be 0 or greater */
double getPageCount(struct StringStack *terms, const char *gkey);
const char *makeQueryString(struct StringStack *terms);
void normalizeSearchTerms(struct StringStack *terms);
double calculateM(const char *daystr, const char *gkey);
double calculateMbase(const char *daystr, const char *gkey);

#endif
