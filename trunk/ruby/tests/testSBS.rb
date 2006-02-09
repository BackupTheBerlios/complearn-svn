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
require 'curses'
require 'date'
require 'matrix'

#include CompLearn

class TreeObserverPrinter < CompLearn::TreeObserver
  def initialize(tm)
    @tm = tm
    @li = 6
    @leftcolx = 2
    @midcolx = 35
    @rightcolx = 57
    @topy = 2
    @tstep = 0.01
    @treetopy = 9
  end
  def getTimeNow()
    Time.new
  end
  def formatTime(t)
    t.strftime("%H:%M:%S")
  end
  def mvaddstr(i,j,str)
    Curses::setpos(i,j)
    Curses::addstr(str)
  end
  def redrawTime()
    starttime = @tm.startTime
    @curtime = getTimeNow
    if (starttime && @curtime.to_i != starttime.to_i)
      treespeed = @tm.examinedcount / (@curtime.to_i - starttime.to_i)
      mvaddstr(@topy + 3, @leftcolx, "Trees/sec   = #{nf(treespeed,@li)}") if @curtime.to_i != starttime.to_i
    end
    mvaddstr(@topy + 0, @midcolx, "Time   now: #{formatTime(@curtime)}")
    mvaddstr(@topy + 1, @midcolx, "Time start: #{formatTime(starttime)}") if starttime
    mvaddstr(@topy + 2, @midcolx, "Time  done: #{formatTime(@tm.endTime)}") if @tm.endTime
  end
  def nf(n, i)
    format("%#{i}d", n)
  end
  def redrawParams()
    mvaddstr(@topy + 0, @leftcolx, "         K  = #{nf(@tm.k,@li)}")
    mvaddstr(@topy + 4, @leftcolx, "Label count = #{nf(@tm.size,@li)}")
#    mvaddstr(@topy + 5, @leftcolx, "Node  count = #{nf(@tm.nodecount,@li)}")
  end
  def redrawStats()
  if @th
    mvaddstr(@topy + 1, @leftcolx, "       S(T) = #{format('%4.4f', @th.score)}")
    drawTree
  end
    mvaddstr(@topy + 2, @leftcolx, "Trees total = #{nf(@tm.examinedcount,@li)}")
  end
  def treeSearchStarted()
    redrawParams
    redrawTime
    redrawStats
    Curses::refresh
  end
  def treeRejected()
    redrawTime
    redrawStats if rand(0) < 0.05  # how come this segfaults
    Curses::refresh
  end
  def treeImproved(th)
    @th = th
    @shownsbs.retarget(th.tree) if @shownsbs
    redrawTime
    redrawStats
    Curses::refresh
  end
  def treeDone(th)
    @th = th
    @donetime = getTimeNow()
    @shownsbs.retarget(th.tree) if @shownsbs
    redrawTime
    drawDoneMessage
    Curses::refresh
  end
  def clearToBottom
    10.times { |i|
      setpos(i + @treetopy, 1)
      addch('\n')
    }
  end
  def ballToScreen(ballco)
    yco = 10 + ballco[1]
    xco = 40 + 3 * ballco[2]
    [yco, xco]
  end
  def findPoint(c1, c2, t)
    res = [ ]
    c1.size.times { |i| res[i] = (1.0-t) * c1[i] + (c2[i]*t) }
    res.map { |i| i.to_i }
  end
  def drawLine(c1, c2)
    steps=7
    steps.times { |st|
      t = st.to_f / (steps.to_f-1.0)
      pt = findPoint(c1, c2, t)
      mvaddstr(pt[0], pt[1], '+')
    }
  end
  def drawIt()
    Curses.clear
    if @shownsbs
      @shownsbs.size.times { |i|
        @shownsbs.size.times { |j|
          if i < j && @shownsbs.springSmooth(i,j) > 0.3
            drawLine(ballToScreen(@shownsbs[i]), ballToScreen(@shownsbs[j]))
          end
        }
      }
      @shownsbs.size.times { |i|
        ballco = @shownsbs[i]
        yco, xco = ballToScreen(ballco)
        ch = " #{i} "
        mvaddstr(yco, xco, ch)
      }
    end
    Curses.refresh()
  end
  def drawTree()
    if @th && !@shownsbs
      @shownsbs = CompLearn::SpringBallSystem.new(@th.tree)
    end
    @shownsbs.evolve if @shownsbs
    @curt = @tstep * @tm.examinedcount
    drawIt()
  end
  def drawDoneMessage
    mvaddstr(@topy + 6, @leftcolx, "Tree complete.")
  end
end

t = CompLearn::TreeAdaptor.new(8)
tadja = t.adja
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
def makeNewMat(howbig)
  rows = []
  howbig.times { |i|
    currow = []
    rows << currow
    howbig.times { |j|
      val = rand(0)
      currow << val
    }
  }
  Matrix.rows(rows)
end

m = Matrix[ [ 0.203530033,0.936534248,0.934325947,0.931391616,0.925598542,0.981752635,0.984941131,1.001326958,1.002637789 ],
[ 0.936534248,0.156838829,0.931333210,0.854574376,0.849464548,0.975991751,0.978365846,1.005251399,1.005128904 ],
[ 0.934325947,0.931333210,0.257166731,0.962143169,0.960984780,0.881373074,0.974741314,0.997135499,0.997202037 ],
[ 0.931391616,0.854574376,0.962143169,0.158378346,0.391691509,0.983229623,1.001940072,1.011636725,1.013533980 ],
[ 0.925598542,0.849464548,0.960984780,0.391691509,0.154265975,0.986098830,0.995477472,1.010284249,1.013215754 ],
[ 0.981752635,0.975991751,0.881373074,0.983229623,0.986098830,0.256816506,0.964394738,0.979598468,0.978435595 ],
[ 0.984941131,0.978365846,0.974741314,1.001940072,0.995477472,0.964394738,0.145978358,0.964626728,0.979637143 ],
[ 1.001326958,1.005251399,0.997135499,1.011636725,1.010284249,0.979598468,0.964626728,0.141921559,0.303451993 ],
[ 1.002637789,1.005128904,0.997202037,1.013533980,1.013215754,0.978435595,0.979637143,0.303451993,0.145661851 ]
]
th = CompLearn::TreeHolder.new(m,t)

# simple method calls to see if errors are generated
th.scramble
th.improve
th.treeCount
th.failCount
tclone = th.clone

unless m == tclone.distMatrix && th.treeCount == tclone.treeCount && th.failCount == tclone.failCount
  puts "cloned treeholder and original do not match"
  exit(1)
end

tm = CompLearn::TreeMaster.new(m, 1)
toptreeoh = TreeObserverPrinter.new(tm)
tm.setTreeObserver(toptreeoh)
f = tm.findTree
Curses::init_screen
Curses::close_screen

#tests/testSBS.rb:88:in `+': method `coerce' called on terminated object (0xb7e1f
#06c) (NotImplementedError)
#        from tests/testSBS.rb:88:in `ballToScreen'
#        from tests/testSBS.rb:117:in `drawIt'
#        from tests/testSBS.rb:115:in `times'
#        from tests/testSBS.rb:115:in `drawIt'
#        from tests/testSBS.rb:130:in `drawTree'
#        from tests/testSBS.rb:51:in `redrawStats'
#        from tests/testSBS.rb:63:in `treeRejected'
#        from tests/testSBS.rb:194:in `treemasterFindTree'
#        from tests/testSBS.rb:194
