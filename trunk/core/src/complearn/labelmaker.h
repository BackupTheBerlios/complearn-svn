#ifndef __DMLABELS_H
#define __DMLABELS_H

/*! \file labelmaker.h */

/** \brief Serializes a StringStack of distance matrix labels into a DataBlock
 *
 *  labelsDump() returns to DataBlock which then can be written to
 *  a file using the function datablockWriteToFile().  This resulting DataBlock
 *  is also appropriate when using the function package_DataBlocks().
 *
 *  To convert the resulting DataBlock back into an StringStack, use the
 *  labelsLoad() function.
 *  \param ss pointer to StringStack
 *  \return serialized DataBlock
 */
struct DataBlock *labelsDump(struct StringStack *ss);

/** \brief Converts DataBlock created by labelsDump() back into a StringStack
 *
 *  labelsLoad() will take as an argument a DataBlock, which was
 *  most likely created by the labelsDump() function, and convert the
 *  DataBlock into a StringStack. A pointer to the StringStack is returned.
 *
 *  An option to labelsLoad() is the fmustbe flag, which, if set to 1, forces
 *  the function to exit if the check for the special DMLabels tag created by
 *  labelsDump() fails.  If the tag is not found, an error message is printed
 *  to stdout. Set fmustbe to 0 to return NULL instead.
 *
 *  \param db pointer to a DataBlock
 *  \param fmustbe 1 if the DataBlock must contain the identifying DMLabels
 *  flag; 0 if not
 *  \return pointer to new StringStack
 */
struct StringStack *labelsLoad(struct DataBlock *db, int fmustbe);

/** \brief Returns StringStack of distance matrix labels from CompLearn binary
 *
 *  clbLabels() is a high level function which reads a CompLearn
 *  binary(clb) file, generated by the ncd -b command and returns a StringStack
 *  of labels for the distance matrix saved in the clb file.
 *  \param fname path to CompLearn binary file
 *  \return StringStack of distance matrix labels
 */
struct StringStack *clbLabels(char *fname);
struct StringStack *clbDBLabels(struct DataBlock *db);

struct DataBlock *clbLabelsDataBlock(char *fname);
struct StringStack *clbLabelsLoad(struct DataBlock *db);

/** \brief Returns StringStack of distance matrix labels from a ASCII text file
 *
 *  cltxtLabels() is a high level function which reads a text file,
 *  formatted in the same way as output from the ncd command, and returns a
 *  StringStack of labels for the distance matrix.
 *  \param fname path to text file
 *  \return StringStack of distance matrix labels
 */
struct StringStack *cltxtLabels(char *fname);

/** \brief Dumps a StringStack of commands into a serialized DataBlock
 *
 *  Used to capture commands when using the ncd and maketree commands.
 *  \param ss pointer to StringStack of commands
 *  \return DataBlock
 */
struct DataBlock *commandsDump(struct StringStack *ss);

/** \brief Returns StringStack of commands from a CompLearn binary file
 *
 *  get_clcmds() is a high level function which reads a CompLearn binary (clb)
 *  file, generated from the ncd -b command, and returns a StringStack of
 *  commands.
 *  \param fname path CompLearn binary file
 *  \return StringStack of commands
 */
struct StringStack *clbCommands(char *fname);

#endif
