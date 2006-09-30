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
#include <stdio.h>
#include <stdlib.h>

#include <complearn/complearn.h>

/*! \mainpage CompLearn API Documentation
 *
 *  Click links at top to explore docs.
 *
 *  \section sect_comp Five Different Ways to Interface External Compressors with CompLearn
 *
 *  \subsection sub_builtin Builtin Compressors: bzip or zlib
 *  requires installation of bzip2 and/or zlib libraries
 *  For bzip, the parameters blocksize, workfactor, and bzverbosity are honored.
 *  For zlib, the parameter zliblevel is used.
 *  For bzip as well as zlib, all parameters are optional and defaults can be
 *  used instead.
 *
 *  \subsection sub_real Real Compressor
 *  The real compressor interface requires a parameter cmd in the envmap for
 *  newcompinst. cmd specifies the filename of a command to execute to compress
 *  data. This executable is expected to read the input data from STDIN and
 *  output compressed data to STDOUT. The compressed bytes will be counted and
 *  converted to bits.
 *
 *  \subsection sub_virt Virtual Compressor
 *  The virtual compressor interface requires a parameter cmd in the envmap
 *  for newcompinst. cmd specifies the filename of a command to execute to
 *  compress data. This executable is expected to read the input data from
 *  STDIN and output the floating point number representing the compressed
 *  size, in bits, to STDOUT. This number should be printed in standard
 *  ASCII-format as a decimal.
 *
 *  \subsection sub_soap SOAP Compressor Server
 *  requires a url of the form http://localhost:2000/ and urn of the form
 *  urn:hws. The complearn system will connect to the specified SOAP server
 *  and call a single function called compfunc and pass it one string. The
 *  compression server is expected to return a double precision floating-point
 *  value indicating the compressed size, in bits, of the string.  Custom
 *  compressor module through dynamic library
 *  a custom compression module defines 7 C functions in a dynamic library with
 *  the following names: newcompinst, compfunc, freecompfunc, shortname,
 *  longname, compparam, apiver
 *
 */

int isRooted = 0;

int main(int argc, char *argv[])
{
  exit(0);
}
