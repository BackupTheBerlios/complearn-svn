require 'ruby/complearn4r'
require 'matrix'

include CompLearn

ca = CompressionBase.new("blocksort")

authors = []
datafile = "/usr/share/doc/gnumeric/AUTHORS"
unless File.exists?(datafile)
  puts "Error: #{datafile}, no such file on your system."
end

File.open(datafile, "r") do |f|
  while line = f.gets
    authors << line.strip.upcase
  end
end

authors.uniq!

while true
  autncds = { }
  print "Please enter a name: "
  name = gets.strip.upcase

  authres = ca.closematch(name, authors)

  # prints top 10 results
  puts authres[0..10].collect { |a,d| "#{a}:#{d}" }
end
