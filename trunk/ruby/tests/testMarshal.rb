require 'complearn4r'

dmk = CompLearn::IncrementalDistMatrix.dmk
dbk = CompLearn::IncrementalDistMatrix.dbk
ssk = CompLearn::IncrementalDistMatrix.ssk
cak = CompLearn::IncrementalDistMatrix.cak

ca = CompLearn::CompAdaptor.new("zlib")

idm = CompLearn::IncrementalDistMatrix.new(ca)
#idm.addDataBlock("hello there")
idm.addString("hi")
idm.addString("bye")
m = Marshal.dump(idm)
nidm = Marshal.load(m)
puts idm.distmatrix if idm.distmatrix != nidm.distmatrix
