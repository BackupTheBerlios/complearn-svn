#ifndef __MAKETREEAPP_H
#define __MAKETREEAPP_H

/** \brief Specific configuration options to the maketree command
 *
 *  This structure holds all the configuration information specific to the
 *  maketree command. It is used in conjunction with the GeneralConfig
 *  structure.
 */
struct MakeTreeConfig {
  char *output_tree_fname; /*!< name of file output by maketree command */
};

/** \brief Loads the environment for the maketree command
 *
 *  The environment created by loadMakeTreeEnvironment() will contain the same
 *  defaults created by loadDefaultEnvironment() but includes options specific
 *  only to the maketree command.
 *
 *  maketree options are set using command-line arguments.
 */
struct GeneralConfig *loadMakeTreeEnvironment(void);

#endif
