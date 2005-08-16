#ifndef __ENVIRONMENT_H
#define __ENVIRONMENT_H

#include <complearn/cltypes.h>
#include <complearn/uclconfig.h>
#include <stdio.h>

/*! \file environment.h */

#include <complearn/datablockenum.h>
#if GETOPT_RDY
#include <getopt.h>
#endif

/** \brief Where configuration details are stored.
 *
 *  \struct GeneralConfig
 *
 *  \sa environment.h
 */
struct GeneralConfig {
  int fVerbose;
  int fAscii;
  int fBinary;
  int fDoExponentiate;
  int fJustSize;
  int fSVD;
  int fHTML;
  int fAddNL, fAddNLAtString;
  int fUsingFilenames;
  int fSuppressVisibleDetails;
  int fShowLabels;
  int fTextToStdout;
  int fDoBothOutputs;

  double M, multiplier;

  char *output_tree_fname;
  char *config_filename;
  struct EnvMap *em;
  struct StringStack *cmdKeeper;
  char *compressor_name;
  struct CompAdaptor *ca;
  void *vptr;
  t_freeappconfig freeappcfg;
  t_updateappemtoconfig upappcfg;
  t_updateappconfigtoem upappem;
  t_printapphelp printapphelp;
  t_printappenvironment printappenv;
};

#if GETOPT_RDY
int complearn_getopt_long(int argc,  char * const argv[], const char *optstring,
                const struct option *longopts, int *longindex,
                struct GeneralConfig *cfg);
#endif

struct GeneralConfig *loadDefaultEnvironment(void);
struct EnvMap *getEnvMap(struct GeneralConfig *g);
void printActiveEnvironment(void);
void freeDefaultEnvironment(struct GeneralConfig *g);
void printOptionHelp(void);

/** \brief Adds a newline to an input string
 *
 *   Allocates a new block of memory to which the contents of the input string
 *   are copied and a newline added to the end.
 *  \param inp string
 *  \return pointer to new string
 */
char *addNL(const char *inp);
struct StringStack *loadDefaultDotParams(void);
void updateConfigToEM(struct GeneralConfig *env);
void updateEMToConfig(struct GeneralConfig *env);

#endif
