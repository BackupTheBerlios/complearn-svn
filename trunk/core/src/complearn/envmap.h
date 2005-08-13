#ifndef __ENVSTACK_H
#define __ENVSTACK_H

#include <complearn/datablock.h>

/** \brief Container of key-value pairs which set the compression environment.
 *  \struct EnvMap
 *
 *  EnvMap contains a stack of key-value pairs which are used to set the
 *  options for a compressor instance.
 *
 *  A compressor instance is created by passing an EnvMap to the newcompinst
 *  function of a CompAdaptor interface, which then returns a pointer to a new
 *  compressor instance.
 *
 *  Each CompAdaptor has its own set of acceptable keys and values.  e.g., the
 *  builtin bzip CompAdaptor accepts the key "blocksize" and a value between 1
 *  and 9 inclusive, but will ignore the key "zliblevel" and any value
 *  associated with it.
 *
 *  EnvMap is a multiple associate container, meaning that there is no limit
 *  to the number of values with the same key.  However, a compression instance
 *  may only have one value per unique key. By default, the builtin
 *  CompAdaptor interfaces give precedence to the top-most key-value pair in an
 *  EnvMap.
 */
struct EnvMap;

/** \brief Returns pointer to a new EnvMap.
 *
 *  Allocates memory and returns pointer to new EnvMap object, which contains
 *  0 elements.
 */
struct EnvMap *newEnvMap();

/** \brief Frees EnvMap object from memory.
 *  \param em EnvMap
 *  \return CL_OK on success
 */
int freeEM(struct EnvMap *em);

/** \brief Adds key-value pair to top of EnvMap.
 *  \param em EnvMap
 *  \param key key
 *  \param val value
 *  \return CL_OK on success
 */
int setKeyValEM(struct EnvMap *em, char *key, char *val);

/** \brief Returns number of key-value pairs in EnvMap.
 *  \param em EnvMap
 *  \return size
 */
int sizeEM(struct EnvMap *em);

/** \brief Returns true value if EnvMap is empty
 *  \param em EnvMap
 *  \return true if EnvMap contains 0 elements 
 */
int isEmptyEM(struct EnvMap *em);

/** \brief Duplicates EnvMap and returns a pointer to a new EnvMap.
 *  \param em EnvMap to be duplicated
 *  \return pointer to new EnvMap
 */
struct EnvMap *cloneEM(struct EnvMap *em);

/** \brief Prints an EnvMap to stdout
 *  \param em EnvMap to be duplicated
 *  \return nothing
 */
void printEM(struct EnvMap *uem);

/** \brief Returns corresponding value for passed in key.
 *
 *  readValForES() will return the top-most value in a given EnvMap for a
 *  given key.
 *  \param em EnvMap
 *  \param key
 *  \return string on success; NULL if no match
 */
char *readValForEM(struct EnvMap *em, const char *key);

/** \brief Converts an EnvMap to a file-writable DataBlock
 *
 *  dumpEnvMap() returns a pointer to a DataBlock which then can be written to
 *  a file using the function writeDataBlockToFile().  This resulting DataBlock
 *  is also appropriate for when using the function package_DataBlocks().
 *
 *  To convert the resulting DataBlock back into an EnvMap, use loadEnvMap()
 *  function.
 *  \param em pointer to EnvMap
 *  \return pointer to DataBlock which can be written to file
 *
 */
struct DataBlock dumpEnvMap(struct EnvMap *em);

/** \brief Converts a DataBlock created by dumpEnvMap() back into an EnvMap
 *
 *  loadEnvMap() will take as an argument a pointer to a DataBlock, which was
 *  most likely created by the dumpEnvMap() function, and convert the DataBlock
 *  into an EnvMap. A pointer to the EnvMap is returned.
 *
 *  An option to loadEnvMap() is the fmustbe flag, which, if set to 1, forces
 *  the function to check for the special EnvMap tag created by dumpEnvMap().
 *  If the tag is not found, an error message is printed to stdout and the
 *  program will exit.  Set fmustbe to 0 to ignore the tag check.
 *
 *  \param db pointer to DataBlock
 *  \param fmustbe 1 if the DataBlock must contain the identifying EnvMap flag;
 *  0 if not
 *  \return pointer to new EnvMap
 */
struct EnvMap *loadEnvMap(struct DataBlock db, int fmustbe);

union pctypes getKeyValAt(struct EnvMap *em, int where);
int findIndexForKey(struct EnvMap *em, const char *key);
void setKeyPrivateEM(struct EnvMap *em, const char *key);
void setKeyMarkedEM(struct EnvMap *em, const char *key);
int isMarkedAtEM(struct EnvMap *em, int where);
int isPrivateAtEM(struct EnvMap *em, int where);
int mergeEM(struct EnvMap *dest, struct EnvMap *src);
struct EnvMap *get_clem_from_clb(char *fname);
#endif
