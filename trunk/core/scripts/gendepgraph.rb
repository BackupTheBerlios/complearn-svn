neatoname = "/usr/bin/dot"
neatoopts = '-Gsize=12,12 -Grotate=90 -Tps'
dname = '/home/cilibrar/src/shared/complearn/src'
tmpname = "/tmp/deps.dot"
neatopipe = File.open(tmpname, "w")
#/IO.popen("#{neatoname} #{neatoopts}", "w")
d = Dir.new(dname)
resolvedBy = { }
edgeTo = { }
nodeNames = { }
d.each { |ent|
next unless ent =~ /.o$/
edgeTo[ent] = { }
fname = "#{dname}/#{ent}"
syms = `nm #{fname}`
  syms.split(/\n/).each { |line|
  line.chomp!
  if line =~ /([TDU]) (\w+);?$/
    typ, nam = $1, $2
    nodeNames[nam] = true
    if typ == 'T' || typ == 'D'
      resolvedBy[nam] = ent
    end
  end
  }
}
d = Dir.new(dname)
d.each { |ent|
next unless ent =~ /.o$/
fname = "#{dname}/#{ent}"
syms = `nm #{fname}`
  syms.split(/\n/).each { |line|
  line.chomp!
  if line =~ /([TDU]) (\w+);?$/
    typ, nam = $1, $2
    if typ == 'U'
      if resolvedBy[nam]
        edgeTo[ent][resolvedBy[nam]] = true
      end
    end
  end
  }
}
neatopipe.puts "digraph prog {"
edgeTo.each_key { |n|
edgeTo[n].each_key { |k|
neatopipe.puts <<EOF
"#{n}" -> "#{k}";
EOF
}
}
neatopipe.puts "}"
neatopipe.close
puts `#{neatoname} #{neatoopts} #{tmpname}`
