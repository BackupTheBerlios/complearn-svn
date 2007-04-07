require "complearn"
require "clcommon"
include CompLearn

puts [Marshal.dump(CSCommand.new)].pack('m*')
