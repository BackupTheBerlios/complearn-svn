#!/home/cilibrar/bin/ruby

# run this from the top level like this:
# ruby scripts/docserver.rb
#
# then go to the url at
#
# http://localhost:4347/
#
require 'webrick'
include WEBrick

s = HTTPServer.new(
  :Port            => 4347,
  :DocumentRoot    => Dir::pwd + "/doc/html"
)

trap("INT"){ s.shutdown }
s.start
