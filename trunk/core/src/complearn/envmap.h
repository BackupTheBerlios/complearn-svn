#ifndef __ENVMAP_H
#define __ENVMAP_H

#include <complearn/datablock.h>

/*! \file envmap.h */

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
 *
 * \sa envmap.h
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

/** \brief Adds key-value pair.
 *
 *  If setKeyValEM() encounters a duplicate key, the function will overwrite
 *  the existing value in the EnvMap with the new val.
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

/** \brief Dumps an EnvMap into a serialized format and returns a DataBlock
 *
 *  dumpEnvMap() a DataBlock which then can be written to a file using the
 *  function datablockWriteToFile().  This resulting DataBlock is also
 *  appropriate when using the function package_DataBlocks().
 *
 *  To convert the resulting DataBlock back into an EnvMap, use loadEnvMap()
 *  function.
 *  \param em pointer to EnvMap
 *  \return DataBlock which can be written to file
 */
struct DataBlock dumpEnvMap(struct EnvMap *em);

/** \brief Converts a DataBlock created by dumpEnvMap() back into an EnvMap
 *
 *  loadEnvMap() will take as an argument a DataBlock, which was
 *  most likely created by the dumpEnvMap() function, and convert the DataBlock
 *  into an EnvMap. A pointer to the EnvMap is returned.
 *
 *  An option to loadEnvMap() is the fmustbe flag, which, if set to 1, forces
 *  the function to exit if the check for the special EnvMap tag created by
 *  dumpEnvMap() fails.  If the tag is not found, an error message is printed to
 *  stdout. Set fmustbe to 0 to return NULL instead.
 *
 *  \param db DataBlock
 *  \param fmustbe 1 if the DataBlock must contain the identifying EnvMap flag;
 *  0 if not
 *  \return pointer to new EnvMap
 */
struct EnvMap *loadEnvMap(struct DataBlock db, int fmustbe);

/** \brief Retrieves a key value pair at a given index of an EnvMap
 *  The key-value pair returned by getKeyValAt() is in the form of a PCTypes
 *  StringPair object.
 *  \param em pointer to EnvMap
 *  \param index
 *  \return PCTypes key-value StringPair
 */
union PCTypes getKeyValAt(struct EnvMap *em, int where);

/** \brief Returns the index of an EnvMap given a character string key
 *  The index returned by findIndexForKey(), upon success, will be an integer
 *  between and including 0 and sizeEM(). If an input key does not match any
 *  keys stored in the EnvMap, -1 is returned.
 *  \param em pointer to EnvMap
 *  \param key pointer to character string key
 *  \return integer index where key found, or -1 if not found
 */
int findIndexForKey(struct EnvMap *em, const char *key);

/** \brief Marks a key in an EnvMap as "private"
 *
 *  A convenience function for users. A key set as "private" by setPrivateEM()
 *  indicates its corresponding value contains "sensitive" data.  A GoogleKey,
 *  for example, can be such a key.
 *
 *  \param em pointer to EnvMap
 *  \param key pointer to character string key
 */
void setKeyPrivateEM(struct EnvMap *em, const char *key);

/** \brief Sets a key in an EnvMap as "marked"
 *
 *  A key set as "marked" by setKeyMarkedEM() indicates that its corresponding
 *  value has been retrieved through the readValForEM() function.  CompLearn
 *  keeps track of this information so that a user knows which EnvMap keys have
 *  been used by their program.
 *  \param em pointer to EnvMap
 *  \param key pointer to character string key
 */
void setKeyMarkedEM(struct EnvMap *em, const char *key);

/** \brief Returns true value if key at index is set "marked"
 *  \param em pointer to EnvMap
 *  \param where index
 *  \return true if "private", return false if not
 */
int isMarkedAtEM(struct EnvMap *em, int where);

/** \brief Returns true value if key at index is set "private"
 *  \param em pointer to EnvMap
 *  \param where index
 *  \return true if "private", return false if not
 */
int isPrivateAtEM(struct EnvMap *em, int where);

/** \brief Merges the contents of two EnvMaps
 *
 *  mergeEM() merges key-value pairs from the src EnvMap into the dest EnvMap,
 *  overwriting values for already existing keys, and pushing on key-value
 *  pairs for new ones.
 *  \param dest pointer to dest EnvMap
 *  \param src pointer to src EnvMap
 *  \return CL_OK upon success
 */
int mergeEM(struct EnvMap *dest, struct EnvMap *src);

/** \brief Retrieves an EnvMap from a CompLearn binary file
 *
 *  get_clem_from_clb() is a high level function which reads a CompLearn binary
 *  (clb) file, as generated by the ncd -b command, and returns the embedded
 *  EnvMap which contains information about the creation of the clb.
 *  \param fname path of CompLearn binary file
 *  \return EnvMap
 */
struct EnvMap *get_clem_from_clb(char *fname);
#endif
