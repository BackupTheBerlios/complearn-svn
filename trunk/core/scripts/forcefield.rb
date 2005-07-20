rsq = "((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)+(z2-z1)*(z2-z1))"
r = "Sqrt[#{rsq}]"
dragvel = "Sqrt[vx*vx+vy*vy+vz*vz]"
def abs(x) "Sqrt[#{x}*#{x}]" end
def ass(x) "TeXForm[Assuming[{Element[vx1, Reals], Element[vy1, Reals], Element[vz1, Reals], Element[x1, Reals], Element[x2, Reals] Element[y1, Reals], Element[y2, Reals], Element[z1, Reals], Element[z2, Reals], Element[drag, Reals], Element[k, Reals], Element[rl, Reals]}, Simplify[#{x}]]]" end
r12 = "{x2-x1,y2-y1,z2-z1}"
normrad = "((1/#{r})*#{r12})"
vars = "x1 y1 z1 x2 y2 z2 vx1 vy1 vz1".split
puts <<EOF
<<Calculus`VectorAnalysis`
(* first trying to understand function definition *)
SetCoordinates[Cartesian[x,y,z]]
(* case 1: assume sphere1 at origin, sphere2 at location x,y,z and sphere1,
with velocity v, a scalar *)
eforce[x1_, y1_, z1_, x2_,y2_,z2_,vx1_,vy1_,vz1_] := (-1/#{rsq}) * #{normrad}
sforce[x1_, y1_, z1_, x2_,y2_,z2_,vx1_,vy1_,vz1_] := ((#{r}-rl)*k) * #{r12}
dforce[x1_, y1_, z1_, x2_,y2_,z2_,vx1_,vy1_,vz1_] := (-#{dragvel} * drag) * {vx1,vy1,vz1}
tforce[x1_, y1_, z1_, x2_,y2_,z2_,vx1_,vy1_,vz1_] :=
eforce[x1, y1, z1, x2,y2,z2,vx1,vy1,vz1] +
sforce[x1, y1, z1, x2,y2,z2,vx1,vy1,vz1] +
dforce[x1, y1, z1, x2,y2,z2,vx1,vy1,vz1]
(*Assuming[{Element[vx1, Reals], Element[vy1, Reals], Element[vz1, Reals], Element[x1, Reals], Element[x2, Reals] Element[y1, Reals], Element[y2, Reals], Element[z1, Reals], Element[z2, Reals], Element[drag, Reals], Element[k, Reals], Element[rl, Reals]}, Simplify[tforce[x1,y1,z1,x2,y2,z2,vx1,vy1,vz1].{1,0,0}]] *)
(*#{ass('tforce[x1,y1,z1,x2,y2,z2,vx1,vy1,vz1].{1,0,0}')} *)
EOF
forces = ['eforce','sforce','dforce']
forces.each { |fo|
    puts "Print[\"Label: force for #{fo}\"]"
vecs = ['{1,0,0}','{0,1,0}','{0,0,1}']
vecs.each { |ve|
    puts "Print[\"Label: force for #{ve}\"]"
puts ass("(*force for #{ve} *) #{fo}[x1,y1,z1,x2,y2,z2,vx1,vy1,vz1].#{ve}")
}
vars.each { |i|
vecs.each { |ve|
    puts "Print[\"Label: d#{ve}/d#{i} jac\"]"
    puts ass("(* d#{ve}/d#{i} jac *) D[#{fo}[x1,y1,z1,x2,y2,z2,vx1,vy1,vz1].#{ve},#{i}]")
  }
}
}

<<STR
#(*Simplify[D[tforce[x1, y1, z1, x2,y2,z2,vx1,vy1,vz1].{1,0,0}, y2]] *)
tx1 = { _xx, _xy, _xz }
tx2 = { _xx, _xy, _xz }
FullSimplify[{
D[epot[tx1],xx],
D[epot[{xx,xy,xz}],xy],
D[epot[{xx,xy,xz}],xz]
}]
(*
JacobianMatrix[f_List,x_List] := Outer[D,f,x]
normp[x_] = Norm[x, 2]
normsq[x_] = Times[Norm[x,2] , Norm[x,2]]
radius[x1_,x2_] = x2 - x1            (* radius from x1 towards x2 *)
normrad[x1_,x2_] = Times[(1.0/normp[radius[x1,x2]]), radius[x1,x2]]
invsq[x1_,x2_] = 1.0/normsq[x2-x1]     (* inverse square law *)
(* LR := 0.75 *)
(* LM := 1 *)
(* B = 0.1 *)
Element[LM, Reals]
invsqsafe[x1_,x2_] = 1/(Times[normp[x2-x1]+LM,normp[x2-x1]+LM])
coulsafe[x1_,x2_] = Times[invsqsafe[x1,x2], normrad[x1,x2]]
viscous[v_] = Times[B , v]
spring[x1_,x2_] = Times[(LR - normp[radius[x1,x2]]), normrad[x1,x2]] 
F[x1_, x2_, v1_] = viscous[v1] + coulsafe[x1,x2] + ISSPRING*spring[x1,x2]

dfdx1[x1_, x2_, v1_] := D[F[x1,x2,v1],x1]
dfdx1[x1,x2,v1]

(* TeXForm[FullSimplify[JacobianMatrix[{F[x1,x2,v]},{x1,x2,v}]]] *)
dfdx2[x1_, x2_, v1_] := D[F[x1,x2,v1],x2]
dfdx2[x1,x2,v1]

dfdv1[x1_, x2_, v1_] := Simplify[D[F[x1,x2,v1],v1], x1 != x2 ]
dfdv1[x1,x2,v1]
(* dfdv1[x1,x2,v1] *)

D[viscous[v1],v1]

TeXForm[Assuming[{x1=={a,b,c},x2=={a2,b2,c2}}, FullSimplify[D[normrad[x1,x2]*Times[1.0/(Norm[x2-x1,2]+LM),1.0/(Norm[x2-x1,2]+LM)],x1]]]]
(* Assuming[z1.z2 == z2.z1, FullSimplify[D[coulsafe[x1,x2],x1]]] *)
(* FullSimplify[D[coulsafe[x2,x1],x1]] *)
(* FullSimplify[D[coulsafe[x1,x2],x2]] *)
*)
STR
