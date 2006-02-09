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

#!/usr/bin/ruby

homedir = "/home/alcruz"

srcdir = "#{homedir}/src/shared/complearn"

confscript = "#{srcdir}/configure"

morecmds = [ "make clean", "make" ]

opts = ["--with-bzip2=#{homedir}/mySoftware/bzip2",
        "--with-xml2=#{homedir}/mySoftware/xml2",
        "--with-nanohttp=#{homedir}/mySoftware/csoap",
        "--with-csoap=#{homedir}/mySoftware/csoap",
        "--with-zlib=#{homedir}/mySoftware/zlib",
        "--with-gsl=#{homedir}/mySoftware/gsl",
        "--with-gdbm=#{homedir}/mySoftware/gdbm"
        ]; 

def makeHalfCopy(inp)
	result = [ ]
  inp.each { |i| result << i if (rand(0) < 0.5) }
	result
end

trialnum = 0
cmdstart = "cd #{srcdir} ; rm output.log ; touch outputlog"
retval = system(cmdstart)
while true
trialnum += 1
puts "\nTRIAL NUMBER #{trialnum} ****************************\n"
curopts = makeHalfCopy(opts)
cmd = "cd #{srcdir} ; #{confscript} #{curopts.join(' ')} >> output.log 2>error.log"
puts "ATTEMPTING #{cmd}"
retval = system(cmd)
puts "GOT RETURN #{retval} for #{cmd}"
exit unless retval
morecmds.each { |c| 
cm = "cd #{srcdir} ; #{c}"
retval = system(cm)
puts "GOT CMDRETURN: #{retval} for #{cm}"
exit unless retval
}
end
