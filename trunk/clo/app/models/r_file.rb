# Copyright (c) 2006 Rudi Cilibrasi, Rulers of the RHouse
# All rights reserved.     cilibrar@cilibrar.com
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#     # Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     # Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     # Neither the name of the RHouse nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE RULERS AND CONTRIBUTORS "AS IS" AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE RULERS AND CONTRIBUTORS BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

class RFile < ActiveRecord::Base
  # Use like this:  ruby script/runner RFile.moveToDB('/path/to/add')
  # For more advanced users, this hash can be used:
  # RFile.moveToDB(:path => '/path/to/add', :devicename => 'v3')
  # Files and directories are supported.  They are handled recursively.
  MT = { 'html' => 'text/html', 'txt' => 'text/plain', 'png' => 'image/png' }
  def self.moveToDB(obj)
    obj = { :path => obj } if obj.kind_of? String
    path = obj[:path]
    ext = nil
    ext = $1 if path =~ /\.(.*)$/
    fail "Must have :path set" unless path
#    puts "moveToDB: #{path} moving..."
    devicename = obj[:devicename] || '';
    if FileTest.file?(path)
      contents = nil
      File.open(path, 'r') { |f| contents = f.read }
      f = RFile.new(:contents => contents, :fullpath => path, :devicename => devicename, :mimetype => MT[ext])
      f.save
    end
    if FileTest.directory?(path)
      Dir.entries(path).entries.each { |i|
        moveToDB(File.join(path, i)) unless (i == '.' || i == '..')
      }
    end
  end
  def self.grabFile(obj)
    obj = { :path => obj } if obj.kind_of? String
    path = obj[:path]
    fail "Must have :path set" unless path
    devicename = obj[:devicename] || '';
    p = self.find(:first, :conditions => ['fullpath = ? AND devicename = ?', path, devicename])
    p
  end
end
