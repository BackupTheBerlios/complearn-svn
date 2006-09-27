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

class TermList
  def initialize() @t = [ ] end
  def terms() @t.clone end
  def delete_at(num) @t.delete_at(num) end
  def addTerm(str)
    @t << str #if str =~ /[a-zA-Z0-9]/
    @t.sort!
  end
  def size() @t.size end
end

class StatusPane
  @@MAXSTATUSMSG = 3
  def initialize()
    @status = [ ]
    addMsg("CompLearn Online Demo initialized")
  end
  def addMsg(str)
    @status << [str, Time.now]
    @status.shift if @status.size > @@MAXSTATUSMSG
  end
  def getStatusLines() @status.clone end
end

class TimeDirArea
  @@OPTPREF = "/mnt/cjhttpd"
  @@MAINDIR = "/var/clod/timedir"
  if File.exist?(@@MAINDIR)
    @@WD = @@MAINDIR
  else
    @@WD = File.join(@@OPTPREF, @@MAINDIR)
  end
  def self.getWD() @@WD end
  def self.getDir(num) File.join(@@WD, num.to_s) end
end
