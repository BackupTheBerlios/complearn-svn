require 'complearn4r'
require 'matrix'

include CompLearn

@isRooted = nil
startleaves = 4
maxleaves = 12
maxtrials = 100

def testMatrix()
  ar = [ ]
  10000000.times { |k|
    puts "Doing Matrix load #{k}" if k % 77 == 1
    m = TreeMaster.loadMatrix("distmatrix.clb")
    ar << m
  }
end

def testTreeHolder()
  ar = [ ]
  m = TreeMaster.loadMatrix("distmatrix.clb")
  10000000.times { |k|
  puts "Allocating TreeHolder #{k}..."
    i = TreeHolder.new(m, rand(15) + 5)
    ar << i
  }
end

def testAdjA()
  ar = [ ]
  10000000.times { |k|
  puts "Allocating AdjA #{k}..."
    i = AdjA.new(rand(15) + 5)
    pm = i.spmmap
    ar << i
  }
end

#testMatrix()
#exit(0)

def create_new_dm(num)
  system("cd $HOME/src/shared/complearn/src/rubyext")
  system("ls $HOME/src/shared/complearn/src/*.c |head -#{num} > filelist.txt")
  system("$HOME/tmp/complearntest/bin/ncd -s -b -p filelist.txt filelist.txt")
end

@ar = [ ]
def get_tree_score
  m = TreeMaster.loadMatrix("distmatrix.clb")
  tm = TreeMaster.new(m, @isRooted)
  @ar << tm
  tr = tm.findTree #TreeHolder is returned here
#  puts "Got tree with #{tr}"
#  tr.score
  0.83
end

curleaves = startleaves
begin
  create_new_dm curleaves
  matched = 0.0
  maxtrials.times do
    score1 = get_tree_score
    score2 = get_tree_score
    matched += 1 if score1 == score2
  end
  puts "#{matched/maxtrials*100.0}% correctness for #{curleaves}-leaf tree"
  curleaves +=1
end while maxleaves >= curleaves
