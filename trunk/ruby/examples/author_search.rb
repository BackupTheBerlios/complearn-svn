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
  authors.each do |a|
    autncds[a] = 99999999.0 + a.size
    parts = a.split(/[^\w]+/).reject { |p| p.size < 2 }
    min = 1.0
    if parts.size > 0 and parts.size < 10 and a.size < 80
      min = nil
      parts.each do |p|
        n = ca.ncd(name, p)
        min = n if min.nil? || n < min
      end
      autncds[a] = 0.25*ca.ncd(name,a) + 0.75*min
    end
  end
  authors.sort! { |a,b| autncds[a] <=> autncds[b] }
  # prints top 10 results
  puts authors[0..10].collect { |a| "#{a}:#{autncds[a]}" }
end
