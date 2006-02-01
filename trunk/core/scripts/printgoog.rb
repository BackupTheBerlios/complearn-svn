#!/usr/local/bin/ruby

require 'gdbm'

gdbm = GDBM.open("#{ENV['HOME']}/.complearn/gdbm/gsamp", 0666)

k = gdbm.keys.sort

k.each { |i|
  sz = gdbm[i].size
  desc = gdbm[i].to_s
  if sz == 204
    desc = "full sample"
  end
  puts "#{i}: #{gdbm[i].size} #{desc}"
}
gdbm.close();
