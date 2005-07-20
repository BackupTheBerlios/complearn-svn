require 'soap/rpc/standaloneServer'
require 'zlib'

class HelloWorldServer < SOAP::RPC::StandaloneServer
  def on_init
    @log.level = Logger::Severity::DEBUG
    add_method(self, 'compfunc', 'str')
    add_method(self, 'shortname')
    add_method(self, 'longname')
    add_method(self, 'apiver')
  end

  def hello_world(from)
    "Hello World, from #{ from }"
  end

  def compfunc(str)
    Zlib::Deflate.deflate(str).size*8.0
  end

  def shortname() "SOAP zlib Ruby" end

  def longname() "SOAP zlib Ruby (Lempel-Ziv)" end

  def apiver() 2 end

end

if $0 == __FILE__
  server = HelloWorldServer.new('hws', 'urn:hws', '0.0.0.0', 2000)
  server.start
end
