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
    redrawStats
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

t = CompLearn::TreeAdaptor.new(4)
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

m = CompLearn::TreeMaster.loadMatrix("distmatrix.clb")
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
