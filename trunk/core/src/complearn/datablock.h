#ifndef __DATABLOCK_H
#define __DATABLOCK_H

#include <stdio.h>

/*! \file datablock.h */

/** \brief The basic data object of CompLearn.
 *  \struct DataBlock
 *
 *  For each data object to be compressed or transformed in CompLearn,  there
 *  must exist a DataBlock. A DataBlock consists of two elements:
 *  a const char *ptr which points to a data buffer, and a int size to
 *  indicate length of the buffer in bytes.
 */
#ifndef __DATABLOCK_ //TODO: leave only prototype when complete with dbptr conv
struct DataBlock {
  unsigned char *ptr; /*!< Pointer to data buffer */
	int size;           /*!< Size of data buffer in bytes */
};
#else
struct DataBlock;
#endif


/** \brief Converts C string to CompLearn DataBlock.
 *
 *  The stringToDataBlockPtr() function takes a char *s string and
 *  copies it to a new memory area without the terminating nul ('\\0'). A new
 *  DataBlock is returned.  \param s character string
 *  \return pointer to a new DataBlock
 */
struct DataBlock *stringToDataBlockPtr(const char *s);

/** \brief Converts file to a CompLearn DataBlock.
 *
 *  The fileToDataBlockPtr() function reads a file at the specified
 *  path and returns a new DataBlock.
 *  \param path path to a file
 *  \return pointer to a new DataBlock
 */
struct DataBlock *fileToDataBlockPtr(const char *path);

struct DataBlock *filePtrToDataBlockPtr(FILE *fp);

struct DataBlock *datablockClonePtr(struct DataBlock *db);

/** \brief Writes contents of DataBlock ptr if exists to stdout.
 *  \param db DataBlock
 */
void datablockPrintPtr(struct DataBlock *db);

/** \brief Takes DataBlock db and returns string, including '\\0'.
 *  \param db pointer to DataBlock
 */
char *datablockToString(struct DataBlock *db);

/** \brief Writes contents of DataBlock ptr to file path.
 *  \param db DataBlock
 *  \param path path to file
 *  \return ptr to string
 */
void datablockWriteToFile(struct DataBlock *db, const char *path);

/** \brief Concatenates DataBlock b to end of DataBlock a and returns new
 * DataBlock.
 *
 * Creates new DataBlock, concatenating contents b ptr to DataBlock a ptr, and
 * recalculating size (a.size + b.size).  Returns new DataBlock.
 * \param a DataBlock to be concatenated to
 * \param b DataBlock to be concatenated
 * \returns new DataBlock
 */
struct DataBlock *datablockCatPtr(struct DataBlock *a, struct DataBlock *b);

/** \brief Creates a new DataBlock given a pointer to an address and a
 * size in bytes.
 *
 * \param ptr a pointer to the base of the memory block to convert
 * \param size number of bytes of space to convert to the DataBlock
 * \return pointer to a new DataBlock containing the data in the block you gave
 */
struct DataBlock *datablockNewFromBlock(const void *ptr, unsigned int size);

void datablockFreePtr(struct DataBlock *db);
int datablockSize(struct DataBlock *db);
unsigned char *datablockData(struct DataBlock *db);
#endif
