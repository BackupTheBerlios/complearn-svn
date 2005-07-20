#ifndef __MAKETREEAPP_H
#define __MAKETREEAPP_H

struct MakeTreeConfig {
  char *output_tree_fname;
};

struct GeneralConfig *loadMakeTreeEnvironment(void);

#endif
