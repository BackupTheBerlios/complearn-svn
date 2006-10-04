#!/usr/bin/env ruby
require 'fileutils'

CL = "complearn-0.9.7"
BUILDDIR = "/home/alcruz/src/complearn/tmp/winbuild"
CLDIR = File.join(BUILDDIR,CL)
BINDIR = "/home/cilibrar/we"
DEPDIR = "/home/webuser/hosting/complearn/downloads"

unless File.exists?(CLDIR)
  puts "Error: you will need the directory:\n
        #{CLDIR} which should contain " +
       "cygwin1.dll and readme.txt"
  exit(0)
end

Dir.chdir(BUILDDIR) do
  `scp -rv rainbow:#{BINDIR}/ncd.exe #{CLDIR}/`
  `scp -rv rainbow:#{BINDIR}/maketree.exe #{CLDIR}/`
  zipname = "#{CL}-win-bin.zip"
  if File.exists?(oldfile = File.join(BUILDDIR,zipname))
    FileUtils.rm(oldfile)
  end
  `zip -r #{zipname} #{CL}`
  `scp -rv #{zipname} webuser@alcruz.com:#{DEPDIR}`
end
