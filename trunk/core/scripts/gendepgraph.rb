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

neatoname = "/usr/bin/dot"
neatoopts = '-Gsize=12,12 -Grotate=90 -Tps'
dname = '/home/cilibrar/src/shared/complearn/src'
tmpname = "/tmp/deps.dot"
neatopipe = File.open(tmpname, "w")
#/IO.popen("#{neatoname} #{neatoopts}", "w")
d = Dir.new(dname)
resolvedBy = { }
edgeTo = { }
nodeNames = { }
d.each { |ent|
next unless ent =~ /.o$/
edgeTo[ent] = { }
fname = "#{dname}/#{ent}"
syms = `nm #{fname}`
  syms.split(/\n/).each { |line|
  line.chomp!
  if line =~ /([TDU]) (\w+);?$/
    typ, nam = $1, $2
    nodeNames[nam] = true
    if typ == 'T' || typ == 'D'
      resolvedBy[nam] = ent
    end
  end
  }
}
d = Dir.new(dname)
d.each { |ent|
next unless ent =~ /.o$/
fname = "#{dname}/#{ent}"
syms = `nm #{fname}`
  syms.split(/\n/).each { |line|
  line.chomp!
  if line =~ /([TDU]) (\w+);?$/
    typ, nam = $1, $2
    if typ == 'U'
      if resolvedBy[nam]
        edgeTo[ent][resolvedBy[nam]] = true
      end
    end
  end
  }
}
neatopipe.puts "digraph prog {"
edgeTo.each_key { |n|
edgeTo[n].each_key { |k|
neatopipe.puts <<EOF
"#{n}" -> "#{k}";
EOF
}
}
neatopipe.puts "}"
neatopipe.close
puts `#{neatoname} #{neatoopts} #{tmpname}`
