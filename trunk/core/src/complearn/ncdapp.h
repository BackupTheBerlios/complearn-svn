#ifndef __NCDAPP_H
#define __NCDAPP_H

/** \brief Specific configuration options to the ncd command
 *
 *  This structure holds all the configuration information specific to the
 *  ncd command. It is used in conjunction with the GeneralConfig
 *  structure.
*/
struct NCDConfig {
  int fUsingGoogle;           /*!< flag which indicates if using Google
                                   compressor */
  char *output_distmat_fname; /*!< name of distance matrix file output by
                                   ncd -b command */
  struct DBAccumulator da;    /*!< DBAccumulator */
};

/** \brief Loads the environment for the ncd command
 *
 *  The environment created by loadNCDEnvironment() will contain the same
 *  defaults created by loadDefaultEnvironment() but includes options specific
 *  only to the ncd command.
 *
 *  ncd options are set using command-line arguments or through the CompLearn
 *  configuration file.
 */
struct GeneralConfig *loadNCDEnvironment(void);

#endif
