require 'complearn'
include CompLearn

cb = CompressionBase.newnew('bzip2', { 'verbosity' => '0', 'xfactor' => '10000',
                            'blocksize' => '8', 'workfactor' => '250' } )
puts "Before ncd call: #{cb.params.inspect}"
puts cb.ncd("hello", "world")
puts " After ncd call: #{cb.params.inspect}"
