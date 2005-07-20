filelist = "filelist.txt"
outfile = "treestats.txt"
File.delete(outfile) if File.exist?(outfile)

startleaves = 4
endleaves = 24
trials = 200

compdir = "#{ENV['HOME']}/src/shared/complearn"

ncdcmd = "#{compdir}/src/ncd -b -p #{filelist} #{filelist}"
trialcmd = "#{compdir}/src/clbinfo distmatrix.clb #{trials}"

(startleaves..endleaves).each do |i|
  listcmd = "ls #{compdir}/src/*.c |head -#{i} > #{filelist}"
  system(listcmd)
  system(ncdcmd)
  system(trialcmd)
end
