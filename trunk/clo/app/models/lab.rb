class Lab < ActiveRecord::Base
  has_many :comments
  def maketitle
    dms = RFile.grabFile("./timedir/#{codenum}/distmatrix.txt").contents.split(/\n/)
    dms.map! { |i| i.split(/[\s]*-?[01]\./) }
    terms = termlist
    if dms.size == terms.size
      terms.each_with_index { |t, i|
        d = dms[i]
        puts "Got #{t} and #{i} and #{d}"
      }
    end
  end
  def termlist()
    RFile.grabFile("./timedir/#{codenum}/termlist.txt").contents.split(/\n/)
  end
end
