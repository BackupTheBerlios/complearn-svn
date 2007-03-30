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
#ifndef __NCDAPP_H
#define __NCDAPP_H

/** \brief Specific configuration options to the ncd command
 *
 *  This structure holds all the configuration information specific to the
 *  ncd command. It is used in conjunction with the GeneralConfig
 *  structure.
*/
struct NCDConfig {
  int fUsingGoogle;           /*!< indicates if using Google compressor */
  int fNexusFormat;           /* Nexus output format */
  char *output_distmat_fname; /*!< name of distance matrix file output by
                                   ncd -b or ncd -n command */
  struct DBAccumulator da;    /*!< DBAccumulator */
};

/** \brief Loads the environment for the ncd command
 *
 *  The environment created by loadNCDEnvironment() will contain the same
 *  defaults created by clLoadDefaultEnvironment() but includes options specific
 *  only to the ncd command.
 *
 *  ncd options are set using command-line arguments or through the CompLearn
 *  configuration file.
 */
struct GeneralConfig *loadNCDEnvironment(void);

#endif
