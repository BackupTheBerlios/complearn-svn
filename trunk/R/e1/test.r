dyn.load("rcomplearn.so")
dyn.load("/usr/lib/libz.so")
.Call("ncd", "rudi", "there")
