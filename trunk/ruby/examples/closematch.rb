#!/usr/bin/ruby
# A simple approximate string search using data compression.
# Demonstrating use of blocksort, NCD, CompLearn by Rudi Cilibrasi
#
#     Sample output:
#
#| Enter a random name or word: turin
#|                                   Alan Turing:0.480           |
#|                                      Internet:0.564           |
#|                                    Peter Naur:0.595           |
#|                               Juris Hartmanis:0.597           |
#|                                Linus Torvalds:0.603           |
#|                                     Vint Cerf:0.613           |
#|                               Jonathan Rumion:0.615           |
#|                                    Alan Burns:0.620           |
#|                                  Donald Knuth:0.640           |
#|                       Tosiyasu Laurence Kunii:0.650           |
#|                               Geoffrey Hinton:0.651           |
#
require 'ruby/complearn4r'
require 'net/http'
require 'uri'

include CompLearn

def grabPeople()
  url = 'http://en.wikipedia.org/wiki/List_of_computer_scientists'
  lines = Net::HTTP.get( URI.parse( url ) ).split(/\n/)
  names = [ ]
  lines.each { |l| names << $1 if l =~ /title="([a-zA-Z ]*)"/ }
  names
end

ppl = grabPeople
ca = CompressionBase.new("blocksort")

while true
  print "|\n| Enter a random name or word: "
  n = gets
  break unless n
  res = ca.closematch(n.strip, ppl)
  res[0..10].each { |s,d| puts format("|%46s:%-16s|", s, format("%3.3f", d)) }
end

