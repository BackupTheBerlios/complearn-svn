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
