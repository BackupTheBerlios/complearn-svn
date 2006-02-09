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
