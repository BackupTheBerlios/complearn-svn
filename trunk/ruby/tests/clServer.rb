require 'complearn'
require 'clcommon'
include CompLearn

while line = gets
  m = Marshal.load(line.strip.unpack('m*')[0])
  result = nil
  if m.kind_of?(BCommand) || m.kind_of?(CSCommand)
    result = m.execute if m.respond_to? :execute
  end
  puts [Marshal.dump(result)].pack('m*')
end
