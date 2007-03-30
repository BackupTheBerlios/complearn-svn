
# This file is part of the libcomplearn-ruby project.
#
# Copyright (C) 2006-2007 Rudi Cilibrasi <cilibrar@cilibrar.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
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
