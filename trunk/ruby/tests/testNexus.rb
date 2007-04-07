require 'complearn'

include CompLearn

puts Nexus.isNexus( "hello world" )
puts Nexus.isNexus( File.read("distmatrix.nex") )
