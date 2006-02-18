/*
* Copyright (c) 2006 Rudi Cilibrasi, Rulers of the RHouse
* All rights reserved.     cilibrar@cilibrar.com
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the RHouse nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE RULERS AND CONTRIBUTORS "AS IS" AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE RULERS AND CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
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
  int fVerbose; /*!< flag indicating verbosity (1) or silence (0) */
  int fAscii;   /*!< flag indicating ASCII output (1) or not (0) */
  int fBinary;  /*!< flag indicating Binary output (1) or not (0) */
  int fDoExponentiate; /*!< flag indicating exponentiation for sizes (1) */
  int fJustSize; /*!< flag indicating just do sizes and nothing else (1) */
  int fSVD; /*!< flag indicating Singular Value Decomposition (1) */
  int fHTML; /*!< flag indicating HTML output format (1) */
  int fAddNL; /*!< flag indicating that newlines should be added on input (1) */
  int fAddNLAtString; /*!< flag indicating add newlines inside strings (1) */
  int fUsingFilenames; /*!< flag indicating that lines indicate filenames (1) */
  int fSuppressVisibleDetails; /*!< flag to suppress visible details in .dot (1) */
  int fShowLabels; /*!< flag to show labels in distance matrix output (1) */
  int fTextToStdout; /*!< flag to write text to stdout(1) or a file (0) */
  int fDoBothOutputs; /*!< flag to do both output to stdout and file (1) */

  double M;    /*!< M is for normalizing the Normalized Google Distance */
  double multiplier; /*!< this is used for exponentiation output mode */

  char *output_tree_fname; /*!< filename to use to output tree .dot file */
  char *config_filename;  /*!< filename to read config from with -f option */
  struct EnvMap *em; /*!< EnvMap storing serialized version of this GeneralConfig */
  struct StringStack *cmdKeeper; /*!< Growing list of commands in this experiment */
  char *compressor_name; /*!< name of desired compressor */
  struct CompAdaptor *ca; /*!< loaded CompAdaptor to use */
  void *ptr;              /*!< pointer to application specific configuration */
  t_freeappconfig freeappcfg; /*!< pointer to function which frees application
                                   specific config */
  t_updateappemtoconfig upappcfg; /*!< pointer to function which moves the
                                       EnvMap environment to application
                                       configuration*/
  t_updateappconfigtoem upappem;  /*!< pointer to function which moves
                                       application configure to EnvMap */
  t_printapphelp printapphelp;    /*!< pointer to function which prints
                                       application specific help */
  t_printappenvironment printappenv; /*!< pointer to function which prints
                                          environment specific to an
                                          application */
};

#if GETOPT_RDY
/** \brief provides a getopt_long() (GNU) like interface to command line
 * option parsing with a CompLearn specific configuration block.
 *
 * This function behaves exactly as getopt_long with the addition of a
 * single parameter at the end.  This is a GeneralConfig struct pointer
 * that will be updated by the complearn option processing system each time
 * an option is unhandled by the user option processing and is handled
 * by the complearn generic option processing subsystem.  This means that
 * at the end of option processing cfg will contain the results of all
 * standard option settings.  For more details on this function see
 * the getopt(3) and getopt_long(3) manpages.
 *
 * \param cfg pointer to a GeneralConfig struct to be updated
 */
int clComplearn_getopt_long(int argc,  char * const argv[], const char *optstring,
                const struct option *longopts, int *longindex,
                struct GeneralConfig *cfg);
#endif

/** \brief loads a default configuration environment based on the standard files
 *
 * This function is the normal interface to the configuration, parameter,
 * and environment settings.  It loads the systemwide and user-specific
 * settings in $HOME/.complearn/config.yml and sets the GeneralConfig
 * fields accordingly.  These values will also be stored in the EnvMap.
 *
 * \return pointer to a GeneralConfig struct holding the loaded environment
 */
struct GeneralConfig *clLoadDefaultEnvironment(void);

/** \brief this is a wrapper function to isolate access to the EnvMap
 *
 * This function retrieves the EnvMap associated with a GeneralConfig.
 * This EnvMap will be used in saving or loading operations involving the
 * GeneralConfig g.
 *
 * \param g pointer to a GeneralConfig struct
 * \return pointer to the EnvMap that contains the information
 */
struct EnvMap *clGetEnvMap(struct GeneralConfig *g);

/** \brief loads the default environment and prints it
 *
 * There are no parameters nor return values for this function.
 */
void clPrintActiveEnvironment(void);

/** \brief deallocates memory for this GeneralConfig
 *
 * There is no return value.
 *
 * \param g pointer to a GeneralConfig struct
 */
void clFreeDefaultEnvironment(struct GeneralConfig *g);

/** \brief print generic option help string
 *
 * This function prints a generic help string.
 * There are no parameters nor return values.
 */
void clPrintOptionHelp(void);

/** \brief Adds a newline to an input string
 *
 *   Allocates a new block of memory to which the contents of the input string
 *   are copied and a newline added to the end.
 *  \param inp string to copy
 *  \return pointer to new string with newline appended
 */
char *clAddNL(const char *inp);

/** \brief moves the configuration information in a GeneralConfig into the em
 *
 * This function serializes the configuration information and stores it in
 * a pointer to an EnvMap called em within the GeneralConfig struct.
 * There is no return value as the results go directly into the EnvMap
 * referenced by env->em.
 *
 * \param env pointer to the GeneralConfig struct to dump
 */
void clUpdateConfigToEM(struct GeneralConfig *env);

/** \brief deserializes the EnvMap at env->em into live config information
 *
 * This function deserializes the configuration information stored in em and
 * updates the other fields in env (a GeneralConfig pointer) accordingly.
 * There is no return value as the results go directly into the GeneralConfig.
 *
 * \param env pointer to the GeneralConfig struct to load
 */
void clUpdateEMToConfig(struct GeneralConfig *env);

#endif
