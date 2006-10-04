#!/usr/bin/env ruby
require 'fileutils'

CL = "complearn-0.9.7"
BUILDDIR = "/home/alcruz/src/complearn/tmp/winbuild"
CLDIR = File.join(BUILDDIR,CL)
BINDIR = "/home/cilibrar/we"
DEPDIR = "/home/webuser/hosting/complearn/downloads"

FileUtils.mkdir_p CLDIR

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
