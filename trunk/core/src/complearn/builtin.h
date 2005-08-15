#ifndef __BUILTIN_H
#define __BUILTIN_H

#include <complearn/uclconfig.h>
#include <complearn/environment.h>

/*! \file builtin.h */

/** \brief Returns pointer to a builtin compressor interface.
 *
 * The loadBuiltinCA() function takes a char *name and matches it against a
 * list of builtin compressor interfaces which are available.  Availability of
 * an interface is dependent on whether its corresponding compression library
 * is installed during compile time.  Currently, CompLearn has 2 builtin
 * compressor adaptors: "bzip" and "zlib".
 *
 * If name matches an available compressor interface, a pointer to that 
 * CompAdaptor interface is returned. If name is not found in the list, NULL is
 * returned.
 * \param name name of compressor interface
 * \returns pointer to CompAdaptor or NULL
 */
struct CompAdaptor *loadBuiltinCA(const char *name);

/** \brief Returns pointer to StringStack of available builtin compressors.
 *
 */
struct StringStack *listBuiltinCA(void);

/** \brief Returns pointer to a real compressor interface.
 * 
 *  The real compressor interface requires a parameter cmd in the EnvMap for
 *  newcompinst. cmd specifies the filename of a command to execute to compress
 *  data. This executable is expected to read the input data from STDIN and
 *  output compressed data to STDOUT. The compressed bytes will be counted and
 *  converted to bits. 
 * \returns pointer to CompAdaptor
 */
struct CompAdaptor *loadRealComp(const char *cmd);

/** \brief Returns pointer to a virtual compressor interface.
 *
 * The virtual compressor interface requires a parameter cmd in the EnvMap
 * for newcompinst. cmd specifies the filename of a command to execute to
 * compress data. This executable is expected to read the input data from STDIN
 * and output the floating point number representing the compressed size, in
 * bits, to STDOUT. This number should be printed in standard ASCII-format as a
 * decimal. 
 * \returns pointer to CompAdaptor
 */
struct CompAdaptor *loadVirtComp(const char *cmd);

/** \brief Returns pointer to a SOAP compressor server interface.
 *
 *  The SOAP compressor server interace requires a url of the form
 *  "http://localhost:2000/" and urn of the form "urn:hws". The CompLearn
 *  system will connect to the specified SOAP server and call a single function
 *  called compfunc and pass it one string. The compression server is expected
 *  to return a double precision floating-point value indicating the compressed
 *  size, in bits, of the string. 
 * \returns pointer to CompAdaptor
 */
struct CompAdaptor *loadSOAPAdaptor(const char *url, const char *urn);

/** \brief Prints a list of builtin compressors.
 *
 *  printBuiltinCompressors() prints to stdout a list of builtin compressors
 *  supported by CompLearn.  This list is dependent on the compression
 *  libraries detected during source compilcation.
 */
void printBuiltinCompressors(void);

/* TODO: move this function to a more appropriate location */

#endif
