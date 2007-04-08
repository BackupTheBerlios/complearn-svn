require 'complearn'
include CompLearn

puts CompLearn.version

cb = CompressionBase.newnew('bzip2', 'blocksize' => '8', 'workfactor' => '2' )
puts "Before ncd: #{cb.params.inspect}"
cb.ncd("hello", "world")
puts "Before ncd: #{cb.params.inspect}"
