#ifndef __BUILTIN_H
#define __BUILTIN_H

#include <complearn/uclconfig.h>
#include <complearn/environment.h>

/*! \file builtin.h */

/** \brief Returns pointer to a builtin compressor interface.
 *
 * The compaLoadBuiltin() function takes a char *name and matches it against a
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
struct CompAdaptor *compaLoadBuiltin(const char *name);

/** \brief Returns pointer to StringStack of available builtin compressors.
 *
 */
struct StringStack *compaListBuiltin(void);

/** \brief Returns pointer to builtin bzip2 compressor interface.
 *
 *  Equivalent to calling load BuiltinCA("bzip").
 * \returns pointer to CompAdaptor or NULL
 */
struct CompAdaptor *compaLoadBzip2(void);

/** \brief Returns pointer to builtin blocksort compressor interface.
 *
 *  Equivalent to calling load BuiltinCA("blocksort").
 * \returns pointer to CompAdaptor
 */
struct CompAdaptor *compaLoadBlockSort(void);

/** \brief Returns pointer to builtin google compressor interface.
 *
 *  Equivalent to calling load BuiltinCA("google").
 * \returns pointer to CompAdaptor or NULL
 */
struct CompAdaptor *compaLoadGoogle(void);

/** \brief Returns pointer to builtin zlib compressor interface.
 *
 *  Equivalent to calling load BuiltinCA("zlib").
 * \returns pointer to CompAdaptor or NULL
 */
struct CompAdaptor *compaLoadZlib(void);

/** \brief Prints a list of builtin compressors.
 *
 *  compaPrintBuiltin() prints to stdout a list of builtin compressors
 *  supported by CompLearn.  This list is dependent on the compression
 *  libraries detected during source compilation.
 */
void compaPrintBuiltin(void);

/* TODO:  Following functions belong somewhere else.  They are not true
 * builtin compressors.  Must also rename the files builtinca-realcomp.c,
 * builtinca-virtcomp.c, and builtinca-soap.c */

/** \brief Returns pointer to a real compressor interface.
 *
 *  The real compressor interface requires a parameter cmd in the EnvMap for
 *  newcompinst. cmd specifies the filename of a command to execute to compress
 *  data. This executable is expected to read the input data from STDIN and
 *  output compressed data to STDOUT. The compressed bytes will be counted and
 *  converted to bits.
 * \returns pointer to CompAdaptor
 */
struct CompAdaptor *compaLoadReal(const char *cmd);

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
struct CompAdaptor *compaLoadVirtual(const char *cmd);

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
struct CompAdaptor *compaLoadSOAP(const char *url, const char *urn);

/** Dynamic Adaptors to support dual-mode loading */
struct BZ2DynamicAdaptor {
  int (*buftobufcompress)(char *dbuff,unsigned int *p,
    char *src, unsigned int sz, int blocksize, int verblevel, int workfactor);
  int (*buftobufdecompress)(char *dbuff,unsigned int *p,
    char *src, unsigned int sz, int small, int verblevel);
};

struct BZ2DynamicAdaptor *grabBZ2DA(void);

struct ZlibDynamicAdaptor {
  int (*compress2)(unsigned char *dbuff,unsigned long *p,
    unsigned char *src, unsigned long sz, int level);
  int (*uncompress)(unsigned char *dbuff,unsigned long *p, unsigned char *src, unsigned long sz);
};

struct ZlibDynamicAdaptor *grabZlibDA(void);

#endif
