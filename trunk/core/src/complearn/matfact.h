#ifndef __MATFACT_H
#define __MATFACT_H

#include <complearn/uclconfig.h>

/*! \file matfact.h */

/** \brief Tag added to a "dump" of a gsl_matrix.
 *  \struct DRAHdr
 *
 *  This structure is embedded within the resulting DataBlock returned by the
 *  function dumpDLDistMatrix(), which is used to serial a gsl_matrix so that
 *  it may be written to file, sent over a network, etc. GSLMHdr contains
 *  information necessary for the conversion of a "dumped" DataBlock, using
 *  distmatrixLoad, back into a gsl_matrix.
 */
struct GSLMHdr;

/** \brief Clones a given gsl_matrix to make an independent copy.
 *
 *  gslmatrixClone() creates a copy of a given gsl_matrix into a newly
 *  allocated memory space.  This copy must be freed independent of the
 *  original gsl_matrix.
 *
 *  \param a pointer to the gsl_matrix to be copied.
 *  \return pointer to a newly allocated gsl_matrix.
 */
gsl_matrix *gslmatrixClone(const gsl_matrix *a);

/** \brief Dumps a gsl_matrix into a serialized format and returns a DataBlock.
 *
 * This function saves or serializes a gsl_matrix, preparing it to be saved
 * within a file, transmitted over the network, etc.
 *
 * \param a pointer to a gsl_matrix
 * \return pointer to a newly created DataBlock
 */
struct DataBlock *gslmatrixDump(const gsl_matrix *a);

/** \brief Loads a gsl_matrix from a serialized format inside a DataBlock
 *
 * Allocates memory and returns a pointer to a deserialized gsl_matrix
 * instance.  The DataBlock passed in to this function should have been made
 * using a gslmatrixDump() call earlier. The second parameter to
 * gslmatrixLoad() * indicates what the function will do in the case the
 * serialized format is not recognized.  0 indicates NULL should be returned.
 * 1 indicates the function will output an error message to stderr and
 * terminate the program.
 *
 * \param d DataBlock
 * \param fmustbe 0 to indicate the function returns NULL upon not recognizing
 * the serialized format, or 1 to indicate the program must terminate.
 * \return pointer to a deserialized gsl_matrix, or NULL if gslmatrixLoad()
 * does not recognize the serialized format.
 */
gsl_matrix *gslmatrixLoad(struct DataBlock *d, int fmustbe);

/** \brief Frees a gsl_matrix object from memory.
 *
 *  The gslmatrixFree() function is a name wrapper to the gsl_matrix_free
 *  function.  It has been added here for convenience.
 *
 *  \param m pointer to gsl_matrix to be freed.
 */
void gslmatrixFree(gsl_matrix *m);

/** \brief Loads a gsl_matrix from a serialized CompLearn Distance Matrix
 * format inside a DataBlock
 *
 * Allocates memory and returns a pointer to a deserialized gsl_matrix
 * instance.  The DataBlock passed in to this function should have been made
 * using a distmatrixDump() call earlier. The second parameter to
 * distmatrixLoad() * indicates what the function will do in the case the
 * serialized format is not recognized.  0 indicates NULL should be returned.
 * 1 indicates the function will output an error message to stderr and
 * terminate the program.
 *
 * \param d DataBlock to be deserialized
 * \param fmustbe 0 to indicate the function returns NULL upon not recognizing
 * the serialized format, or 1 to indicate the program must terminate.
 * \return pointer to a deserialized gsl_matrix, or NULL if distmatrixLoad()
 * does not recognize the serialized format.
 *
 */
gsl_matrix *distmatrixLoad(struct DataBlock *d, int fmustbe);

/** \brief Dumps an gsl_matrix into a serialized format and returns a DataBlock
 *
 *  envmapDump() a DataBlock which then can be written to a file using the
 *  function datablockWriteToFile().  This resulting DataBlock is also
 *  appropriate when using the function package_DataBlocks().
 *
 *  To convert the resulting DataBlock back into a gsl_matrix, use the
 *  distmatrixLoad() function.
 *  \param m pointer to gsl_matrix
 *  \return DataBlock which can be written to file
 */
struct DataBlock *distmatrixDump(gsl_matrix *m);

/** \brief Retrieves a gsl_matrix from a CompLearn binary file
 *
 *  clbDistMatrix() is a high level function which reads a CompLearn binary
 *  (clb) file, as generated by the ncd -b command, and returns the enclosed
 *  distance matrix in the gsl_matrix format.
 *  \param fname path of CompLearn binary file
 *  \return gsl_matrix
 */
gsl_matrix *clbDistMatrix(char *fname);

/** \brief Retrieves a gsl_matrix from a CompLearn DataBlock "package"
 *
 *  clbDBDistMatrix() takes as input, a DataBlock "package" as created by
 *  package_DataBlocks().
 *  \param pointer to DataBlock "package"
 *  \param pointer to gsl_matrix
 */
gsl_matrix *clbDBDistMatrix(struct DataBlock *db);

/** \brief Retrieves a gsl_matrix from a serialized gsl_matrix
 *
 *  clbDBDistMatrix() takes as input a serialized gsl_matrix DataBlock as
 *  created by gslmatrixDump() and returns a pointer to a gsl_matrix. Same as
 *  gslmatrixLoad(), but without the mustbe parameter.
 *  \param pointer to DataBlock
 *  \return pointer to gsl_matrix
 */
gsl_matrix *clbDistMatrixLoad(struct DataBlock *db);

/** \brief Retrieves a "serialized" gsl_matrix
 *
 * clbDMDataBlock() takes as input the path of a CompLearn Binary file (.clb)
 * and returns a serialized gsl_matrix as created by gslmatrixDump(). Use
 * clbDistMatrixLoad() to retrieve the gsl_matrix.
 * \param pathname of Complearn Binary file
 * \return pointer to serialized gsl_matrix DataBlock
 */
struct DataBlock *clbDMDataBlock(char *fname);

/** \brief Retrieves gsl_matrix from a ASCII text file
 *
 *  cltxtDistMatrix() is a high level function which reads a text file,
 *  formatted in the same way as output from the ncd command, and returns a
 *  a distance matrix in the gsl_matrix format.
 *  \param fname path to text file
 *  \return gsl_matrix
 */
gsl_matrix *cltxtDistMatrix(char *fname);

/** \brief Retrieves the row size of a matrix stored in an ASCII text file
 *
 *  The row size of a valie ASCII text file containing a distance matrix that
 *  can be read by CompLearn is always 1 more the integer returned by
 *  cltxtColSize(), where the "extra" row signifies the labels for the
 *  square distance matrix.
 *  \param fname path to text file
 *  \return int row size
 */
int cltxtRowSize(char *fname);

/** \brief Retrieves the column size of a matrix stored in an ASCII text file
 *
 *  The column size of a valid ASCII text file containing a distance matrix
 *  that can be read by CompLearn is always 1 less the integer returned by
 *  cltxtRowSize(), where the "extra" row signifies the labels for the
 *  square distance matrix.
 *  \param fname path to text file
 *  \return int column size
 */
int cltxtColSize(char *fname);

/** \brief Prints to stdout the given gsl_matrix
 *  \param m pointer to gsl_matrix
 *  \return char character string used to separate the values
 */
void gslmatrixPrint(gsl_matrix *m, char *delim);

/** \brief converts ASCII distance matrix into a CompLearn Binary
 *
 *  NOT READY FOR PRIMETIME YET
 */
int cltxtToCLB(char *source, char *dest);

#endif
