def printheader()
puts <<'EOF'
\documentclass[10pt]{article}
\setlength{\oddsidemargin}{0in}
\begin{document}
\def\Mvariable#1{{#1}}
\def\Mfunction#1{{\bf #1}}
\def\Muserfunction#1{{\bf #1}}
\title{A Formula for the Jacobian}
\author{Rudi Cilibrasi}
EOF
end

def printfooter()
puts <<'EOF'
\end{document}
EOF
end

#filename = "datafile.txt"
#src = IO.readlines(filename)

printheader
inlatex = false
formula = "$$"
while line = gets do
  if inlatex and line == "\n"
    inlatex = false
    formula = formula+"$$"
    formula.gsub!(/vx1/,"v_{x1}")
    formula.gsub!(/vy1/,"v_{y1}")
    formula.gsub!(/vz1/,"v_{z1}")
    formula.gsub!(/x1/,"x_1")
    formula.gsub!(/y1/,"y_1")
    formula.gsub!(/z1/,"z_1")
    formula.gsub!(/x2/,"x_2")
    formula.gsub!(/y2/,"y_2")
    formula.gsub!(/z2/,"z_2")
    formula.gsub!(/drag/,"d")
    formula.gsub!(/rl/,"L_r")
    puts formula
    formula = "$$"
  end
  if line =~ /Label: (.+)$/
    curlabel = $1.to_s
    puts curlabel
  end
  if inlatex
    formula = formula + line
  end
  if line =~ /TeXForm= (.*)/m
    formula = formula + $1
    inlatex = true
  end
end

printfooter
