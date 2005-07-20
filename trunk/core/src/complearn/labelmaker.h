#ifndef __DMLABELS_H
#define __DMLABELS_H

struct DataBlock dumpDMLabels(struct StringStack *ss);
struct StringStack *loadDMLabels(struct DataBlock db, int fmustbe);
struct StringStack *get_labels_from_clb(char *fname);
struct StringStack *get_labels_from_txt(char *fname);
struct DataBlock dumpCommands(struct StringStack *ss);
struct StringStack *get_clcmds_from_clb(char *fname);

#endif
