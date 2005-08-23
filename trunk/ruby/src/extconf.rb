# ruby extconf.rb --with-complearn-lib=$HOME/tmp/complearntest/lib --with-complearn-include=$HOME/tmp/complearntest/include/complearn-1.0
#

require 'mkmf'
dir_config 'complearn'
dir_config 'gsl'
have_library('gslcblas', 'cblas_sspr') || find_library('gslcblas', 'cblas_sspr', '/lib' , '/usr/lib', '/usr/local/lib')
have_library('gsl', 'gsl_matrix_get') || find_library('gsl', 'gsl_matrix_get', '/lib' , '/usr/lib', '/usr/local/lib')
have_library('bz2', 'BZ2_bzBuffToBuffCompress') || find_library('bz2', 'BZ2_bzBuffToBuffCompress', '/lib' , '/usr/lib', '/usr/local/lib')
have_library('m', 'main') || find_library('m', 'main', '/lib' , '/usr/lib', '/usr/local/lib')
have_library('z', 'compress2') || find_library('z', 'compress2', '/lib' , '/usr/lib', '/usr/local/lib')
have_library('xml2', 'xmlXIncludeProcess') || find_library('xml2', 'xmlXIncludeProcess', '/lib' , '/usr/lib', '/usr/local/lib')
have_library('nanohttp-1.0', 'log_error') || find_library('nanohttp-1.0', 'log_error', '/lib' , '/usr/lib', '/usr/local/lib', '/home/webuser/mySoftware/csoap-1.0.3/lib')
have_library('csoap-1.0', 'soap_client_init_args') || find_library('csoap-1.0', 'soap_client_init_args', '/lib' , '/usr/lib', '/usr/local/lib', '/home/webuser/mySoftware/csoap-1.0.3/lib')
have_library('gdbm', 'gdbm_open') || find_library('gdbm', 'gdbm_open', '/lib' , '/usr/lib', '/usr/local/lib')
have_library('complearn', 'getSize') || find_library('complearn', 'getSize', '.', './src', '../src', '..', '../../src', '../../..', ENV['HOME']+'/tmp/complearntest/lib')
have_header('complearn/complearn.h')

create_makefile 'complearn4r'
