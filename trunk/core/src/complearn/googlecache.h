#ifndef __GOOGLECACHE_H
#define __GOOGLECACHE_H

#include <complearn/stringstack.h>

/*! \file googlecache.h */

/** \brief Cache of Google count samples over time
 *  \struct GoogleCache
 *
 *  A database or cache is kept of all Google search counts returned.  The
 *  count, date, and time are all retained.  This allows for programs to be
 *  adjusted and re-run without waiting for another query of the Google
 *  server, and also can cut down on total server requests substantially.
 */
struct GoogleCache;

struct GoogleCache *newGC(void);
void freeGC(struct GoogleCache *gc);

int fetchsample(struct GoogleCache *gc, const char *daystr, struct StringStack *terms, double *val, const char *gkey);
double fetchSampleSimple(struct StringStack *terms, const char *gkey, const char *daystr);

void deleteSavedGC(void);

#endif
