# Copyright (c) 2006 Rudi Cilibrasi, Rulers of the RHouse
# All rights reserved.     cilibrar@cilibrar.com
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#     # Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     # Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     # Neither the name of the RHouse nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE RULERS AND CONTRIBUTORS "AS IS" AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE RULERS AND CONTRIBUTORS BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# ruby extconf.rb --with-complearn-lib=$HOME/tmp/complearntest/lib --with-complearn-include=$HOME/tmp/complearntest/include/complearn-1.0
#

require 'mkmf'
$CFLAGS += " #{ENV['CFLAGS']}" || ''
$CPPFLAGS += " #{ENV['CPPFLAGS']}" || ''
$LDFLAGS += " #{ENV['LDFLAGS']}" || ''
dir_config 'complearn'
dir_config 'gsl'
pkg_config('complearn')
have_library('gslcblas', 'cblas_sspr') || find_library('gslcblas', 'cblas_sspr', '/lib' , '/usr/lib', '/usr/local/lib')
have_library('gsl', 'gsl_matrix_get') || find_library('gsl', 'gsl_matrix_get', '/lib' , '/usr/lib', '/usr/local/lib')
have_library('bz2', 'BZ2_bzBuffToBuffCompress') || find_library('bz2', 'BZ2_bzBuffToBuffCompress', '/lib' , '/usr/lib', '/usr/local/lib')
have_library('m', 'main') || find_library('m', 'main', '/lib' , '/usr/lib', '/usr/local/lib')
have_library('z', 'compress2') || find_library('z', 'compress2', '/lib' , '/usr/lib', '/usr/local/lib')
have_library('xml2', 'xmlXIncludeProcess') || find_library('xml2', 'xmlXIncludeProcess', '/lib' , '/usr/lib', '/usr/local/lib')
have_library('nanohttp-1.0', 'log_error') || find_library('nanohttp-1.0', 'log_error', '/lib' , '/usr/lib', '/usr/local/lib')
have_library('csoap-1.0', 'soap_client_init_args') || find_library('csoap-1.0', 'soap_client_init_args', '/lib' , '/usr/lib', '/usr/local/lib')
have_library('gdbm', 'gdbm_open') || find_library('gdbm', 'gdbm_open', '/lib' , '/usr/lib', '/usr/local/lib')
have_library('complearn', 'clDraSize') || find_library('complearn', 'clDraSize', '.', './src', '../src', '..', '../../src', '../../..', ENV['HOME']+'/tmp/complearntest/lib')

have_header('complearn/complearn.h')

create_makefile 'complearn4r'
