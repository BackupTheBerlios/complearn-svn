#ifndef __YAMLPARSER_H
#define __YAMLPARSER_H

#include <complearn/stringstack.h>
#include <complearn/envmap.h>

/*! \file clparser.h */

struct StringStack *clGetDefaultFileList(void);

/* These clFunctions assume you have already created your own EnvMap to
 * hold accumulated configuration parameters.
 */
int clReadDefaultConfig(struct EnvMap *dest);

/* Returns CL_OK or CL_ERRBADFILE if it cannot read the given file. */
int clReadSpecificFile(struct EnvMap *dest, const char *fname);
void clHandleLine(struct EnvMap *dest, const char *uline);

#endif
