#ifndef __YAMLPARSER_H
#define __YAMLPARSER_H

#include <complearn/stringstack.h>
#include <complearn/envmap.h>

/*! \file yamlparser.h */

struct StringStack *getDefaultFileList(void);

/* These functions assume you have already created your own EnvMap to
 * hold accumulated configuration parameters.
 */
int readDefaultConfig(struct EnvMap *dest);

/* Returns CL_OK or CL_ERRBADFILE if it cannot read the given file. */
int readSpecificFile(struct EnvMap *dest, const char *fname);
void handleLine(struct EnvMap *dest, const char *uline);

#endif
