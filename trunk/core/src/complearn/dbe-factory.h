#ifndef __DBE_FACTORY_H
#define __DBE_FACTORY_H

struct DataBlockEnumeration *buildQuotedDBE(const char *str);
struct DataBlockEnumeration *buildFileDBE(const char *str);

/** \brief Determines the mode in which a new DataBlockEnumeration is created.
 *  \struct DBEFactory
 *
 *  A DBEFactory can be set to one of 5 different modes:
 *
 *    quoted - for converting a string into a DataBlock
 *    file - for converting a file into a DataBlock
 *    filelist - for converting a text list of files into a set of DataBlocks
 *    directory - for converting a directory of files into a set of DataBlocks
 *    windowed - for converting a single DataBlock into a set of DataBlocks
 *
 *  By default, the factory is set to file mode.
 */
struct DBEFactory;

#define DBF_MODE_QUOTED     1
#define DBF_MODE_FILE       2
#define DBF_MODE_FILELIST   3
#define DBF_MODE_DIRECTORY  4
#define DBF_MODE_WINDOWED   5
#define DBF_MODE_STRINGLIST   6

#define DBF_MODE_MAX          6

/** \brief Returns a pointer to a new DBEFactory instance.
 *  \return pointer to a new DBEFactory instance
 */
struct DBEFactory *newDBEFactory(void);

/** \brief Frees DBEFactory instance from memory.
 *  \param dbf DBEFactory instance
 */
void freeDBEFactory(struct DBEFactory *dbf);

/** \brief Sets a new mode for a DBEFactory.
 *  \param dbf pointer to a DBEFactory instance
 *  \param dbf newMode
 *  \return 0 on success
 */
int dbef_setMode(struct DBEFactory *dbf, int newMode);

/** \brief Returns mode of DBEFactory instance.
 *  \param dbf pointer to a DBEFactory instance
 *  \return value between 1 and 5 inclusive. 1 = quoted, 2 = file, 3 =
 *   filelist, 4 = directory, 5 = windowed
 */
int dbef_getMode(struct DBEFactory *dbf);

/** \brief Returns string which describes mode of DBEFactory instance.
 *  \param dbf pinter to a DBEFactory instance
 *  \return pointer to string of one of the following values: quoted, file,
 *   filelist, directory, windowed
 */
const char *dbef_getModeStr(struct DBEFactory *dbf);

/** \brief Given a DBEFactory instance and string argument, returns a pointer
 *  to a new DataBlockEnumeration.
 *
 *  The DBEFactory instance which is passed in indicates the mode in which
 *  a DataBlockEnumeration must be created.  str is an argument to that mode,
 *  and may either be a quoted string, filename, filename which contains a list
 *  of filenames, directory name, or filename to be "windowed."
 *  \param dbf pointer to a DBEFactory instance
 *  \param str string
 *  \return pointer to newly created DataBlockEnumeration
 */
struct DataBlockEnumeration *dbef_convertStr(struct DBEFactory *dbf, const char *str);
#endif
