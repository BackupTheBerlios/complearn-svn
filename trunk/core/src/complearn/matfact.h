#ifndef __MATFACT_H
#define __MATFACT_H

#include <complearn/uclconfig.h>

#if GSL_RDY
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>
#endif

/*! \file matfact.h */

#if GSL_RDY
/** \brief Clones a given gsl_matrix to make an independent copy.
 *
 *  cloneGSLMatrix() creates a copy of a given gsl_matrix into a newly
 *  allocated memory space.  This copy must be freed independent of the
 *  original gsl_matrix.
 *
 *  \param a pointer to the gsl_matrix to be copied.
 *  \return pointer to a newly allocated gsl_matrix.
 */
gsl_matrix *cloneGSLMatrix(const gsl_matrix *a);

/** \brief Dumps a gsl_matrix into a serialized format and returns a DataBlock.
 *
 * This function saves or serializes a gsl_matrix, preparing it to be saved
 * within a file, transmitted over the network, etc.
 *
 * \param a pointer to a gsl_matrix
 * \return newly created DataBlock
 */
struct DataBlock dumpGSLMatrix(const gsl_matrix *a);

/** \brief Loads a gsl_matrix from a serialized format inside a DataBlock
 *
 * Allocates memory and returns a pointer to a deserialized gsl_matrix
 * instance.  The DataBlock passed in to this function should have been made
 * using a dumpGSLMatrix() call earlier. The second parameter to
 * loadGSLMatrix() * indicates what the function will do in the case the
 * serialized format is not recognized.  0 indicates NULL should be returned.
 * 1 indicates the function will output an error message to stderr and 
 * terminate the program.
 *
 * \param d DataBlock
 * \param fmustbe 0 to indicate the function returns NULL upon not recognizing
 * the serialized format, or 1 to indicate the program must terminate.
 * \return pointer to a deserialized gsl_matrix, or NULL if loadGSLMatrix()
 * does not recognize the serialized format.
 */
gsl_matrix *loadGSLMatrix(const struct DataBlock d, int fmustbe);

/** \brief Frees a gsl_matrix object from memory.
 *
 *  The freeGSLMatrix() function is a name wrapper to the gsl_matrix_free
 *  function.  It has been added here for convenience.
 *
 *  \param m pointer to gsl_matrix to be freed.
 */
void freeGSLMatrix(gsl_matrix *m);

/** \brief Loads a gsl_matrix from a serialized CompLearn Distance Matrix
 * format inside a DataBlock
 *
 * Allocates memory and returns a pointer to a deserialized gsl_matrix
 * instance.  The DataBlock passed in to this function should have been made
 * using a dumpCLDistMatrix() call earlier. The second parameter to
 * loadCLDistMatrix() * indicates what the function will do in the case the
 * serialized format is not recognized.  0 indicates NULL should be returned.
 * 1 indicates the function will output an error message to stderr and
 * terminate the program.
 *
 * \param d DataBlock to be deserialized
 * \param fmustbe 0 to indicate the function returns NULL upon not recognizing
 * the serialized format, or 1 to indicate the program must terminate.
 * \return pointer to a deserialized gsl_matrix, or NULL if loadCLDistMatrix()
 * does not recognize the serialized format.
 *
 */
gsl_matrix *loadCLDistMatrix(const struct DataBlock d, int fmustbe);
struct DataBlock dumpCLDistMatrix(gsl_matrix *m);
gsl_matrix *get_cldm_from_clb(char *fname);
gsl_matrix *get_dm_from_txt(char *fname);
int get_row_size_from_txt(char *fname);
int get_col_size_from_txt(char *fname);
void print_gsl_matrix(gsl_matrix *m, char *delim);
#else

struct DoubleA *cloneDDMatrix(struct DoubleA *a);
struct DataBlock dumpDDMatrix(struct DoubleA *a);
struct DoubleA *loadCLDDDistMatrix(const struct DataBlock d, int fmustbe);
struct DataBlock dumpCLDDDistMatrix(struct DoubleA *m);
#endif

#endif
