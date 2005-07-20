#ifndef __NCDAPP_H
#define __NCDAPP_H

struct NCDConfig {
  int fUsingGoogle;
  char *output_distmat_fname;
  struct DBAccumulator da;
};

struct GeneralConfig *loadNCDEnvironment(void);

#endif
