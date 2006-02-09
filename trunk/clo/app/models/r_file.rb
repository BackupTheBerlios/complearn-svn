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
