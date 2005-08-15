#ifndef __DMLABELS_H
#define __DMLABELS_H

/** \brief Serializes a StringStack of labels into a DataBlock
 *
 *  dumpDMLabels() returns to a DataBlock which then can be written to
 *  a file using the function writeDataBlockToFile().  This resulting DataBlock
 *  is also appropriate for when using the function package_DataBlocks().
 *
 *  To convert the resulting DataBlock back into an EnvMap, use loadEnvMap()
 *  function.
 *  \param em pointer to EnvMap
 *  \return DataBlock which can be written to file
 */
struct DataBlock dumpDMLabels(struct StringStack *ss);
struct StringStack *loadDMLabels(struct DataBlock db, int fmustbe);
struct StringStack *get_labels_from_clb(char *fname);
struct StringStack *get_labels_from_txt(char *fname);
struct DataBlock dumpCommands(struct StringStack *ss);
struct StringStack *get_clcmds_from_clb(char *fname);

#endif
