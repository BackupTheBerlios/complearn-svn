require 'fileutils'
include FileUtils

class PPMDRunner
@@cmd = '/usr/bin/ppmd'

def rtmpdir()
  "/tmp/cl#{$$}dir"
end

def dodecompppmd(*args)
  mkdir_p(rtmpdir) unless File.exists?(rtmpdir)
  result = nil
  blockres = nil
  chdir(rtmpdir) {
    bigcmd = [@@cmd, args].flatten.join(' ')
    puts bigcmd
    result = `#{bigcmd}`
    File.open(File.join(rtmpdir,"inp"), "r") { |f| blockres = f.read }
  }
  blockres
end
def docompppmd(*args)
  mkdir_p(rtmpdir) unless File.exists?(rtmpdir)
  result = nil
  blockres = nil
  chdir(rtmpdir) {
    bigcmd = [@@cmd, args].flatten.join(' ')
    puts bigcmd
    File.open(File.join(rtmpdir,"inp"), "w") { |f| f.puts("fun") }
    result = `#{bigcmd}`
    File.open(File.join(rtmpdir,"comp"), "r") { |f| blockres = f.read }
  }
  blockres
end

def dotest()
 compressed =   docompppmd('e', '-fcomp', 'inp')
  compname = File.join(rtmpdir,"comp")
  impname = File.join(rtmpdir,"comp")
  rm(impname) if File.exists?(impname)
  raise "cannot remove file" if File.exists?(impname)
  decom = dodecompppmd('d', 'comp')
  puts decom
end
end

p = PPMDRunner.new
p.dotest
