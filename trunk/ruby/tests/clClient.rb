require "complearn"
require "clcommon"
include CompLearn

puts [Marshal.dump(BVersion.new)].pack('m*')
