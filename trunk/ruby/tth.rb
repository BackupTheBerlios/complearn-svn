require 'complearn4r'
require 'curses'
require 'date'
require 'matrix'

include CompLearn
include Curses

class TreeObserverPrinter < TreeObserver
  def initialize(tm)
    @tm = tm
    @li = 6
    @leftcolx = 2
    @midcolx = 35
    @rightcolx = 57
    @topy = 2
    @tstep = 0.01
    @treetopy = 9
    @lf1 = (rand(0) + 3) / 14.0
    @lf2 = (rand(0) + 5) / 11.0
  end
  def getTimeNow()
    Time.new
  end
  def formatTime(t)
    t.strftime("%H:%M:%S")
  end
  def mvaddstr(i,j,str)
    setpos(i,j)
    addstr(str)
  end
  def redrawTime()
    @curtime = getTimeNow
    if (@curtime.to_i != @starttime.to_i)
      treespeed = @tm.examinedcount / (@curtime.to_i - @starttime.to_i)
      mvaddstr(@topy + 3, @leftcolx, "Trees/sec   = #{nf(treespeed,@li)}") if @curtime.to_i != @starttime.to_i
    end
    mvaddstr(@topy + 0, @midcolx, "Time   now: #{formatTime(@curtime)}") if @starttime
    mvaddstr(@topy + 1, @midcolx, "Time start: #{formatTime(@starttime)}") if @starttime
    mvaddstr(@topy + 2, @midcolx, "Time  done: #{formatTime(@donetime)}") if @donetime
  end
  def nf(n, i)
    format("%#{i}d", n)
  end
  def redrawParams()
    mvaddstr(@topy + 0, @leftcolx, "         K  = #{nf(@tm.k,@li)}")
    mvaddstr(@topy + 4, @leftcolx, "Label count = #{nf(@tm.labelcount,@li)}")
    mvaddstr(@topy + 5, @leftcolx, "Node  count = #{nf(@tm.nodecount,@li)}")
  end
  def redrawStats()
  if @th
    mvaddstr(@topy + 1, @leftcolx, "       S(T) = #{format('%4.4f', @th.score)}")
    drawTree
  end
    mvaddstr(@topy + 2, @leftcolx, "Trees total = #{nf(@tm.examinedcount,@li)}")
  end
  def treeSearchStarted()
    init_screen
    @starttime = getTimeNow()
    redrawParams
    redrawTime
    redrawStats
    refresh
  end
  def treeRejected()
    redrawTime
    redrawStats
    refresh
  end
  def treeImproved(th)
    @th = th
    redrawTime
    redrawStats
    refresh
  end
  def treeDone(th)
    @th = th
    @donetime = getTimeNow()
    redrawTime
    drawDoneMessage
    refresh
  end
  def clearToBottom
    10.times { |i|
      setpos(i + @treetopy, 1)
      addch('\n')
    }
  end
  def drawIt(ch, inslo, inshi)
  instep = 0.05
  s = inslo
    while s < inshi
      theta1 = s * @lf1
      theta2 = s * @lf2
      yco = 6*Math.sin(theta1) + 16
      xco = 39 + 38*Math.cos(theta2)
      setpos(yco, xco)
      mvaddstr(yco, xco, ch)
      s += instep
    end
  end
  def drawTree()
    @curt = @tstep * @tm.examinedcount
    drawIt(' ', @curt, @curt + 1)
    drawIt('*', @curt + 5, @curt + 6)
  end
  def drawDoneMessage
    mvaddstr(@topy + 6, @leftcolx, "Tree complete.")
  end
end

t = TreeAdaptor.new(4)
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

m = TreeMaster.loadMatrix("distmatrix.clb")
th = TreeHolder.new(m,t)
tm = TreeMaster.new(m, 1)
toptreeoh = TreeObserverPrinter.new(tm)
#tm.setTreeObserver(toptreeoh)
f = tm.findTree
t = f.tree
puts "Got tree: #{t.to_dot}"

