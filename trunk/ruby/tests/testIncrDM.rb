require 'complearn4r'
require 'matrix'

include CompLearn

ca = CompAdaptor.new("blocksort")
idm = IncrementalDistMatrix.new(ca)
idm.addString("hello")
idm.addString("world")
puts idm.distmatrix
idm.addString("alien")
puts idm.distmatrix

class CompAdaptor
  def ncdMatrix(ardb)
    incrdm = IncrementalDistMatrix.new(self)
    ardb.each { |d| incrdm.addString(d) }
    incrdm.distmatrix
  end
end

puts ca.ncdMatrix(['alphabet', 'soup', 'fabulous', 'febo'])
