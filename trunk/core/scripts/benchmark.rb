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
