# To use this script, make sure there is a distmatrix.clb file already in
# the top-level dir.  It should be about size 11.

require "optparse"
require "benchmark"
include Benchmark

OPTIONS = {
  :revision    => nil,
  :trials      => 10,
  :leaves      => 15,
  :optimize    => nil
}

cdcmd = "cd #{ENV['HOME']}/src/shared/complearn"
puts cdcmd
system(cdcmd)
svninfo = `svn info`
existingrev = nil
svninfo.each_with_index { |line,i|
  if line =~ /Revision: (\d+)/
    existingrev = $1.to_i
  end
}
OPTIONS[:revision] ||= existingrev


ARGV.options do |opts|
  opts.on("-r", "--revision=revision", Integer,
          "Updates repository to specific revision number",
          "{Default: #{OPTIONS[:revision]}" ) { |OPTIONS[:revision]| }
  opts.on("-t", "--trials=trials", Integer,
          "Updates number of trials to be executed",
          "{Default: #{OPTIONS[:trials]}" ) { |OPTIONS[:trials]| }
  opts.on("-l", "--leaves=leafnodes", Integer,
          "Updates number of trials to be executed",
          "{Default: #{OPTIONS[:leaves]}" ) { |OPTIONS[:leaves]| }
  opts.on("-o", "--optimize", "Enabling performance optimizations") { |OPTIONS[:optimize]| }
  opts.separator ""
  opts.parse!
end

svncmd = "svn up -r#{OPTIONS[:revision]}"
puts "Performing svn update to revision #{OPTIONS[:revision]}"
system(svncmd)

reconfcmd = "./scripts/reconf.zsh #{ OPTIONS[:optimize] ? '--enable-optimize' : ''} ; make clean ; make"
puts reconfcmd
system(reconfcmd)


def testfunc()
system("./src/maketree ./distmatrix.clb")
end

fname = "filelist.txt"
system("ls src/*.c |head -#{OPTIONS[:leaves]} > #{fname}")
system("./src/ncd -b -p #{fname} #{fname}")

puts "Doing #{OPTIONS[:trials]} trials with #{OPTIONS[:leaves]} leaves"

     benchmark(" "*7 + CAPTION, 7, FMTSTR, ">total:", ">avg:") do |x|
       tf = x.report("for:")  {for i in 1..OPTIONS[:trials]; puts "Trial #{i}:" ; testfunc(); a = "1"; end}
       [tf, tf / OPTIONS[:trials].to_f]
     end

system("svn up")

#   Benchmark.bm do |x|
#      x.report { testfunc(); }
#L    end
