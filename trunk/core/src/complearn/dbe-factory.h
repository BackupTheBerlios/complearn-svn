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
#ifndef __DBE_FACTORY_H
#define __DBE_FACTORY_H

/*! \file dbe-factory.h */

/** \brief Determines the mode in which a new DataBlockEnumeration is created.
 *
 *  A DBEFactory can be set to one of 5 different modes:
 *
 *    quoted - for converting a string into a DataBlock
 *    file - for converting a file into a DataBlock
 *    filelist - for converting a text list of files into a set of DataBlocks
 *    directory - for converting a directory of files into a set of DataBlocks
 *    windowed - for converting a single DataBlock into a set of DataBlocks
 *
 *  By default, the factory is set to file mode.
 *
 *  \struct DBEFactory
 */
struct DBEFactory;

#define DBF_MODE_QUOTED     1
#define DBF_MODE_FILE       2
#define DBF_MODE_FILELIST   3
#define DBF_MODE_DIRECTORY  4
#define DBF_MODE_WINDOWED   5
#define DBF_MODE_STRINGLIST   6

#define DBF_MODE_MAX          6

/** \brief Returns a pointer to a new DBEFactory instance.
 *  \return pointer to a new DBEFactory instance
 */
struct DBEFactory *clDbefactoryNew(void);

/** \brief Frees DBEFactory instance from memory.
 *  \param dbf DBEFactory instance
 */
void clDbefactoryFree(struct DBEFactory *dbf);

/** \brief Sets a new mode for a DBEFactory.
 *  \param dbf pointer to a DBEFactory instance
 *  \param dbf newMode
 *  \return 0 on success
 */
int clDbefactorySetMode(struct DBEFactory *dbf, int newMode);

/** \brief Returns mode of DBEFactory instance.
 *  \param dbf pointer to a DBEFactory instance
 *  \return value between 1 and 5 inclusive. 1 = quoted, 2 = file, 3 =
 *   filelist, 4 = directory, 5 = windowed
 */
int clDbefactoryGetMode(struct DBEFactory *dbf);

/** \brief Returns string which describes mode of DBEFactory instance.
 *  \param dbf pinter to a DBEFactory instance
 *  \return pointer to string of one of the following values: quoted, file,
 *   filelist, directory, windowed
 */
const char *clDbefactoryModeString(struct DBEFactory *dbf);

/** \brief Given a DBEFactory instance and string argument, returns a pointer
 *  to a new DataBlockEnumeration.
 *
 *  The DBEFactory instance which is passed in indicates the mode in which
 *  a DataBlockEnumeration must be created.  str is an argument to that mode,
 *  and may either be a quoted string, filename, filename which contains a list
 *  of filenames, directory name, or filename to be "windowed."
 *  \param dbf pointer to a DBEFactory instance
 *  \param str string
 *  \return pointer to newly created DataBlockEnumeration
 */
struct DataBlockEnumeration *clDbefactoryNewDBE(struct DBEFactory *dbf, const char *str);
#endif
