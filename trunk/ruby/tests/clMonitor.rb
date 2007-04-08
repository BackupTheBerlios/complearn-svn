require 'complearn'
require 'clcommon'
include CompLearn

while line = gets
  m = Marshal.load(line.strip.unpack('m*')[0])
  puts m.class.to_s + ":" + m.inspect
end
