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

class CloController < ApplicationController
  layout 'main'

  def fun
    @title = 'Complearn Online Demo'
    @onload = 'updateStatus()'
    loadAll
    @maxterms = @@MAXTERMS
  end
  def status
    loadAll
    render_partial "status"
  end

  def getrandexp
    @title = 'Random experiment'
    l = Lab.find_all
    unless l.empty?
      l = l[rand(l.size)]
      redirect_to :action => 'showexpnum', :id => l.codenum
    else
      flash[:notice] = 'No experiments in Lab'
      redirect_to :action => 'listmonths'
    end
  end

  def showmonth
    @title = 'Monthly Experiments Summary'
    ti = Time.now.to_i.to_f
    curi = (params['when'] || params['id'] || Time.now).to_i
    curtime = Time.at(curi)
    @starttime = curtime
    @experiments = Lab.find(:all, :conditions => ['created_at > ? and created_at < ?', curtime, Time.at(curtime.to_i + 3600*24*31)])
#    @experiments = @experiments.sort_by { |i| i.created_at }
    @experiments = @experiments.sort_by { |i| -i.showcount / Math.exp((ti-i.created_at.to_f)/(3600*24*20)) }   # 20 days for 1/e decay
    @metakeywords = @experiments.map { |i| i.title }.compact.sort.join(',')
    @metadescription = @experiments.map { |i| i.termlist[-1] }.sort.join(',')
  end

  def listmonths
    @title = 'CompLearn Online - All Experiments'
    @months = [ ]
    curtime = Time.gm(*[1, 0, 0, 1, 10, 2005, 0, 317, false, "CET"])
    now = Time.new
    while curtime < now
      @months << curtime
      a = curtime.to_a
      a[4] += 1
      if a[4] == 13
        a[4] = 1
        a[5] += 1
      end
      curtime = Time.gm(*a)
    end
  end

  def listdays
    @title = 'Experiments by day'
    l = Lab.find_all.sort_by { |i| i.created_at }
    days = l.map { |i| i = i.created_at
                   i.strftime "#{Date::MONTHNAMES[i.mon]}-%d" }.uniq
    @daystr = days.join("<br>\n")
  end

  def grabfile
    num=params["id"]
    filename=params["filename"] || 'notafile'
    path = "./timedir/#{num}/#{filename}"
    f = RFile.grabFile(path)
    if f
      @headers["Content-Type"] = f.mimetype if f.mimetype
      @contents = f.contents
    else
      @contents = "(file #{path} missing)"
    end
  end

  def delexpnow
    ipnum = request.env["REMOTE_ADDR"] || request.env["REMOTE_HOST"]
    admin = (ipnum.to_s == "213.84.35.186")
    num=params["id"]
    if admin && num
      lab = Lab.find(:first, :conditions => [ "codenum = ?", num ])
      if lab
        lab.destroy
        @msg = "#{num} successfully deleted."
        redirect_to :controller => 'clo', :action => 'listmonths'
      else
        @msg = "Error, cannot delete experiment codenum #{num}"
      end
    end
  end

  def addcomment
    @ipnum = request.env["REMOTE_ADDR"] || request.env["REMOTE_HOST"]
    id = params["id"] || 0
    str = params["str"]
    lab = Lab.find(id)
    if lab
      com = Comment.new(:username => @ipnum, :str => str)
      lab.comments << com
      lab.save
      redirect_to :controller => 'clo', :action => 'showexpnum', :id => lab.codenum
    else
      render_text "Error, cannot accept parameters."
    end
  end

  def settitle
    num=params["id"]
    title=params["title"] || ''
    lab = Lab.find(:first, :conditions => [ "codenum = ?", num ])
    lab.title = title
    lab.save
    redirect_to :controller => 'clo', :action => 'showexpnum', :id => lab.codenum
  end

  def showexpnum
    @num=params["id"]
    @title = 'CompLearn Online Experimental Results page for ' + @num.to_s
    @comments = [ ]
    @expid = 0
    @exptime = "(unknown)"
    @expipnum = "(unknown)"
    @choices = [ ]
    @title = "(unknown)"
    if @num
      lab = Lab.find(:first, :conditions => [ "codenum = ?", @num ])
      if lab
        @expid = lab.id
        @expipnum = lab.ipnum
        @exptime = lab.created_at
        @starttime = @exptime
        @title = lab.title
        @comments = lab.comments.sort_by { |i| i.created_at }
        lab.showcount ||= 0
        lab.showcount = lab.showcount + 1
        lab.save
      end
    end
    @ipnum = request.env["REMOTE_ADDR"] || request.env["REMOTE_HOST"]
    @admin = (@ipnum.to_s == "213.84.35.186")
    @namechoice = params["namechoice"] || ''
    @namechoice = '(unknown)' if @namechoice.size < 3
    @deltime=(params["deltime"] || "0").to_i || 0
    @deltime += 3
    @distmatrix = 'Please wait for your distance matrix to become available.'
    whichnum=@num
    imgname = File.join("./timedir/#{whichnum}", "treefile.png")
    f = RFile.grabFile(imgname)
    @omitrefresh = f && (f.contents.size > 1024)
    if whichnum =~ /^[0-9]*$/
      path = File.join("./timedir/#{whichnum}", 'distmatrix.html')
      f = RFile.grabFile(path)
      if f
        @distmatrix = f.contents
        @distmatrix.gsub!(/(\d\.\d\d\d)\d*/, "\\1")
      else
        @distmatrix = "<distance matrix invalid>"
      end
      path = File.join("./timedir/#{whichnum}", 'termlist.txt')
      f = RFile.grabFile(path)
      if f
        f.contents.split(/\n/).each { |n| @choices << n }
      end
    end
  end

  def gettreepng
    @headers["Content-Type"] = "image/png"
    whichnum=params["num"]
    if whichnum =~ /^[0-9]*$/
      filename = File.join(TimeDirArea.getDir(whichnum), 'treefile.png')
      begin
      File.open(filename, 'r') { |f| render_text f.read }
      rescue
      render_text "Sorry, no tree for #{whichnum} yet, please refresh"
      end
    end
  end

  def getdistmatrix
    @headers["Content-Type"] = "text/html"
    whichnum=params["num"]
    if whichnum =~ /^[0-9]*$/
      filename = File.join(TimeDirArea.getDir(whichnum), 'distmatrix.html')
      begin
      File.open(filename, 'r') { |f| render_text f.read }
      rescue
      render_text "Sorry, no matrix yet for #{whichnum} yet, please refresh"
      end
    end
  end

  def del
    loadAll
    whichnum=params["num"]
    if whichnum
      whichnum = whichnum.to_i - 1  # HTML numbering starts at 1 not 0
      if whichnum >= 0 && whichnum < @termlist.size
        @termlist.delete_at(whichnum)
        @statuspane.addMsg("Deleted term ##{whichnum}")
      else
        @statuspane.addMsg("Invalid index #{whichnum}")
      end
      saveAll
    end
    @maxterms = @@MAXTERMS
    render_partial "list"
  end
  @@MINTERMS = 4
  @@MAXTERMS = 25
  def doexperiment
    loadAll
    ipnum = request.env["REMOTE_ADDR"] || request.env["REMOTE_HOST"]
#    @termlist = @termlist.select { |i| i =~ /[a-zA-Z0-9]/ }
    if @termlist.size >= @@MINTERMS && @termlist.size <= @@MAXTERMS
      exp = { }
      exp['termlist'] = @termlist
      n = Time.now
      exp['now'] = n
      exp['ipnum'] = ipnum
      exp['codenum'] = n.to_i
      wd = TimeDirArea.getDir(n.to_i)
      FileUtils.mkdir wd, :mode => 0777
      File.open(File.join(wd, 'distmatrix.html'), 'w') { |f| f.write("<i>...loading experiment...</i>") }
      Experiment.create("todo"=>Marshal.dump(exp))
      @statuspane.addMsg("Queued experiment ##{n.to_i}.")
      @statuspane.addMsg("Clearing termlist for next experiment.")
      @termlist = TermList.new
    else
      @statuspane.addMsg("Error, must have at least #{@@MINTERMS} and at most #{@@MAXTERMS} items.")
    end
    saveAll
    redirect_to :controller => 'clo', :action => 'showexpnum', :id => n.to_i
  end
  def add
    loadAll
    term=params["term"]
    if term
      @termlist.addTerm(term.gsub(/"/,''))
      @statuspane.addMsg("Added term '#{term}'")
      saveAll
    end
    @maxterms = @@MAXTERMS
    render_partial "list"
  end
end
