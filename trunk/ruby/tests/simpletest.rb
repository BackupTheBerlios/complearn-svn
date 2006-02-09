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

@data = [
[ 0.162804878, 0.568292683, 0.879878049, 0.857748184, 0.849390244, 0.877439024, 0.856097561 ],
[ 0.568292683, 0.163120567, 0.842617153, 0.873486683, 0.849569251, 0.861402095, 0.868181818 ],
[ 0.879878049, 0.842617153, 0.151193634, 0.805690073, 0.768058317, 0.797743755, 0.815584416 ],
[ 0.857748184, 0.873486683, 0.805690073, 0.177966102, 0.696125908, 0.564769976, 0.679176755],
[ 0.849390244, 0.849569251, 0.768058317, 0.696125908, 0.168986083, 0.716368456, 0.674025974],
[ 0.877439024, 0.861402095, 0.797743755, 0.564769976, 0.716368456, 0.195004029,  0.692857143],
[ 0.856097561, 0.868181818, 0.815584416, 0.679176755, 0.674025974, 0.692857143,  0.173376623] ]

@isRooted = nil
maxtrials = 1000
matched = 0.0
startleaves = 4
maxleaves = 7

def get_distmatrix(howbig)
  rows = [ ]
  howbig.times do |i|
    tmp = [ ]
    howbig.times do |j|
     tmp << @data[i][j]
    end
    rows[i] = tmp
  end
  Matrix.rows(rows)
end

curleaves = startleaves

def get_tree_score(m)
  tm = TreeMaster.new(m, @isRooted)
  tr = tm.findTree #TreeHolder is returned here
  tr.score
end

begin
  matrix = get_distmatrix curleaves
  matched = 0.0
  puts "starting trials for #{curleaves}-leaf tree"
  maxtrials.times do |i|
    score1 = get_tree_score matrix
    score2 = get_tree_score matrix
    matched += 1 if score1 == score2
  end
  puts "#{matched/maxtrials*100.0}% correctness for #{curleaves}-leaf tree"
  curleaves +=1
end while maxleaves >= curleaves
