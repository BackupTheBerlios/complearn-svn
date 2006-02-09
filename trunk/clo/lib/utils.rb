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
