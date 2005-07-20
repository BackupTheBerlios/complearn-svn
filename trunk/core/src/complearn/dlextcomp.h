#ifndef __DLEXTCOMP_H
#define __DLEXTCOMP_H

#if DLFCN_RDY
#include <dlfcn.h>
#endif

#include <complearn/datablock.h>
#include <complearn/envmap.h>
#include <complearn/compadaptor.h>

/* These are the names of functions that a dynamic-library compression plugin
 * must define in order to be compatible with this system.
 */

#define FUNCNAMENCA "newCompAdaptor"

/** \brief Loads custom compressor module through dynamic library.
 *
 * \param libraryname name of dynamic library
 * \returns CompAdaptor new compressor interface
 */
struct CompAdaptor *loadDLCompAdaptor(const char *libraryname);

#endif
