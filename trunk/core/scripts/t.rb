def doTest(cmd)
  `#{cmd} --help >o 2>&1 </dev/null`
  lines = nil
  File.open('o', 'r') { |l| lines = l.readlines }
  scores = { }
  lines.each_with_index { |l, i|
    s = 0
    s += 15 unless l =~ /opyri/i
    s += 5 unless (l =~ /199\d/i || l =~ /200\d/i)
    s -= 30 if l.size < 4
    s += (10-i) if i < 10
    scores[l.strip] = s
  }
  goodlines = scores.keys.sort { |a,b| (scores[a] <=> scores[b]) }.reverse
  goodlines = goodlines[0,4] if goodlines.size > 4
  puts goodlines
end

commands = [ 'gzip', 'bzip2', 'ppmd', 'lzop' ]
commands.each { |i| doTest(i) }
