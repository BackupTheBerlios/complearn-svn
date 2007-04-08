require 'complearn'
include CompLearn

puts version

cb = CompressionBase.new('bzip2')
puts "Before ncd: #{cb.params.inspect}"
cb.ncd("hello", "world")
puts "Before ncd: #{cb.params.inspect}"
