# The filters added to this controller will be run for all controllers in the application.
# Likewise will all the methods added be available for all controllers.
require 'utils'

class ApplicationController < ActionController::Base
  def getSessionName(cl) cl.to_s.downcase end
  def loadSessionInstance(cl)
    sessname = getSessionName(cl)
    eval "@#{sessname} = @session['#{sessname}'] || #{cl}.new"
  end
  def saveSessionInstance(cl)
    sessname = getSessionName(cl)
    eval "@session['#{sessname}'] = @#{sessname}"
  end
  @@SESSELEMS = [ TermList, StatusPane ]
  def loadAll()
    @@SESSELEMS.each { |c| loadSessionInstance(c) }
  end
  def saveAll()
    @@SESSELEMS.each { |c| saveSessionInstance(c) }
  end
end
