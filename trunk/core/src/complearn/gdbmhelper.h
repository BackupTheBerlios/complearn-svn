#ifndef __GDBMHELPER_H
#define __GDBMHELPER_H

#include <complearn/datablock.h>
/*! \file gdbmhelper.h */

/** \brief a simple encapsulation of the GDBM database system
 * \struct GDBMHelper
 *
 * This structure allows for the convenient reading and writing of
 * GDBM files wihtin the complearn system.  All such files are referenced
 * relative to a hidden directory in $HOME/.complearn.  Basic exact-match
 * fetching and indexed storing of data are supported.  Both keys and
 * values are a simple DataBlock.
 */
struct GDBMHelper;

struct GDBMHelper *cldbopen(const char *filename);
void cldbunlink(const char *filename);
struct DataBlock *cldbfetch(struct GDBMHelper *gh, struct DataBlock key);
void cldbstore(struct GDBMHelper *gh, struct DataBlock key, struct DataBlock val);
int cldbclose(struct GDBMHelper *gh);

#endif
