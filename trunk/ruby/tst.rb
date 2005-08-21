require 'complearn4r'
require 'matrix'

include CompLearn
puts "it is #{CompLearn.to_s}"
puts "and it is #{AdjA.inspect}"

def printAdjA(ad)
  sz = ad.size
  sz.times  { |i|
    sz.times { |j|
print ad.getconstate(i,j) ? '*' : '.'
}
puts
}
end

ad = AdjA.new(11)

puts "First it is "
printAdjA(ad)
ad.setconstate(2,3,1)
ad.setconstate(1,3,1)

puts "Now it is"
printAdjA(ad)


input = "hello world\n"

bzca = CompAdaptor.new("bzip")
puts "#{bzca.shortname}, #{bzca.longname}: #{bzca.compfunc(input)}"
puts "Version: #{bzca.apiver}"

zca = CompAdaptor.new("zlib")
puts "#{zca.shortname}, #{zca.longname}: #{zca.compfunc(input)}"
puts "Version: #{zca.apiver}"

# TODO: find and fix the memory double free / corruption bug commented out here
#gca = CompAdaptor.new("google")
#puts "#{gca.shortname}, #{gca.longname}: #{gca.compfunc(input)}"
#puts "Version: #{gca.apiver}"

t = TreeAdaptor.new(6)
tadja = t.adja
puts "Tree Adja:"
printAdjA(tadja)
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
puts "Perim pairs: #{pp}"
printAdjA(tr.adja)
puts "About to mutate.."
tr.mutate
puts "Mutated.."
  }
  puts "Outside Perim pairs: #{tr.perimeterPairs(nil)}"
end

puts "About to test tree..."
testTree(t)
puts "Done testing tree."
exit 0
