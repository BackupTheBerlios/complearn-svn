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

require 'complearn4r'
require 'matrix'

include CompLearn
puts "it is #{CompLearn.to_s}"
puts "and it is #{AdjAdaptor.inspect}"

def printAdjAdaptor(ad)
  sz = ad.size
  sz.times  { |i|
    sz.times { |j|
      print ad.getconstate(i,j) ? '*' : '.'
  }
  puts
}
end

ad = AdjAdaptor.new(11)
ad.setconstate(0,1,1)
m = Marshal.dump(ad)
backobj = Marshal.load(m)
fail "bad adja class marshalling" unless backobj.class == ad.class
fail "bad marshalling of AdjA" unless backobj.getconstate(0,1)
ad = ad.clone  # quick test clone
puts "First it is "
printAdjAdaptor(ad)
ad.setconstate(2,3,1)
ad.setconstate(1,3,1)

puts "Now it is"
printAdjAdaptor(ad)


input = "hello world\n"

CompressionBase.names.sort.each { |compname|
bzca = CompressionBase.new(compname)
s1 = 'block'
s2 = 'flock'
ln = bzca.longname
puts "Testing #{compname}: #{bzca.compfunc(input)} (#{ln})"
puts "Version: #{bzca.apiver}, NCD(#{s1},#{s2})=#{bzca.ncd(s1,s2)}"
}
#zca = CompressionBase.new("zlib")
#puts "#{zca.shortname}, #{zca.longname}: #{zca.compfunc(input)}"
#puts "Version: #{zca.apiver}"
#
#zca = CompressionBase.new("blocksort")
#puts "#{zca.shortname}, #{zca.longname}: #{zca.compfunc(input)}"
#puts "Version: #{zca.apiver}, NCD: #{zca.ncd('block', 'flock')}"

# TODO: find and fix the memory double free / corruption bug commented out here
gca = CompressionBase.new("zlib")
puts "#{gca.shortname}, #{gca.longname}: #{gca.compfunc(input)}"
puts "Version: #{gca.apiver}"

puts "Making new tree."
t = TreeAdaptor.new(6)
puts "Made new AdjAdaptor"
tadja = t.adja
puts "Tree Adja:"
printAdjAdaptor(tadja)
nodes = t.nodes
descs = [ ]
nodes.each { |i|
j = format("%3.3s", i.to_s)
if t.isFlippable(i)
  j = "#{j}F"
else
  j = "#{j} "
end
j = "#{j}Q[#{t.nodeToColumn(i)}]" if t.isQuartetable(i)
descs << j
}
puts "Nodes:"
descs.map! { |i| format("%-9.9s", i) }
puts descs.join(',')
def testTree(tr)
  10.times {
  i = rand(tr.adja.size);
  j = rand(tr.adja.size);
  p = tr.adja.path(i,j);
  puts "Path from #{i} to #{j}: #{p.join(',')}"
puts "About to call pp"
clns = [ 2 ]
pp = tr.perimeterPairs(clns)
puts "Perim pairs: #{pp.inspect}"
printAdjAdaptor(tr.adja)
puts "About to mutate.."
tr.mutate
puts "Mutated.."
m = tr.adja.to_matrix
puts "Got matrix: #{m.inspect}"
  }
  puts "Outside Perim pairs: #{tr.perimeterPairs(nil).inspect}"
end

puts "About to test tree..."
testTree(t)
puts "Done testing tree."

#d = Marshal.dump(t.adja)
#puts "Making SpringBallSystem"
#m = SpringBallSystem.new(t)
#puts "Done."
#3.times {
#  m.size.times { |i|
#    puts "ball #{i}: #{m[i]}"
#  }
#  sleep(1)
#  m.evolve
#}

bzca = CompressionBase.new("bzip2")
bzca.params.each do |k,v|
  puts "#{k} => #{v}"
end

puts "Here's the CompressionBase: #{bzca.inspect}"
cdump = Marshal.dump(bzca)
cloaded = Marshal.load(cdump)
puts cloaded.class
cloaded.params.each do |k,v|
  puts "#{k} => #{v}"
end

exit 0
